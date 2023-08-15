
#include <arpa/inet.h>

#include <signal.h>

#include "chrono_common/KeeperIdCard.h"
#include "chrono_common/KeeperStatsMsg.h"
#include "KeeperRecordingService.h"
#include "KeeperRegClient.h"
#include "IngestionQueue.h"
#include "StoryChunkExtractionQueue.h"
#include "StoryChunkExtractor.h"
#include "KeeperDataStore.h"
#include "DataStoreAdminService.h"
#include "ConfigurationManager.h"
#include "StoryChunkExtractor.h"
#include "CSVFileChunkExtractor.h"

#define KEEPER_GROUP_ID 7

    // we will be using a combination of the uint32_t representation of the service IP address 
    // and uint16_t representation of the port number
int service_endpoint_from_dotted_string(std::string const & ip_string, int port, std::pair<uint32_t,uint16_t> & endpoint)
{	
    // we will be using a combination of the uint32_t representation of the service IP address 
    // and uint16_t representation of the port number 
    // NOTE: both IP and port values in the KeeperCard are in the host byte order, not the network order)
    // to identfy the ChronoKeeper process

   struct sockaddr_in sa;
   // translate the recording service dotted IP string into 32bit network byte order representation
   int inet_pton_return = inet_pton(AF_INET, ip_string.c_str(), &sa.sin_addr.s_addr); //returns 1 on success
    if(1 != inet_pton_return)
    { std::cout<< "invalid ip address"<<std::endl;
	    return (-1);
    }

    // translate 32bit ip from network into the host byte order
    uint32_t ntoh_ip_addr = ntohl(sa.sin_addr.s_addr); 
    uint16_t ntoh_port = port;
    endpoint = std::pair<uint32_t,uint16_t>(ntoh_ip_addr, ntoh_port);

return 1;
}	

volatile sig_atomic_t keep_running = true;

void sigterm_handler (int)
{
    std::cout << "Received SIGTERM, starrt shutting down "<<std::endl;

    keep_running = false;
    return;
}


///////////////////////////////////////////////

int main(int argc, char** argv) {

  int exit_code = 0;

    signal(SIGTERM, sigterm_handler);

  //INNA: TODO: pass the config file path on the command line & load the parameters inot a ConfigurationObject

    ChronoLog::ConfigurationManager confManager("./default_conf.json");
    std::string keeper_csv_files_directory= confManager.KEEPER_CONF.STORY_FILES_DIR;
    uint64_t keeper_group_id = KEEPER_GROUP_ID;
   
    // instantiate DataStoreAdminService
    
    std::string datastore_service_ip =  confManager.RPC_CONF.VISOR_KEEPER_CONF.KEEPER_END_CONF.KEEPER_IP.string();
    int datastore_service_port = confManager.RPC_CONF.VISOR_KEEPER_CONF.KEEPER_END_CONF.KEEPER_PORT;
    std::string KEEPER_DATASTORE_SERVICE_NA_STRING = 
        confManager.RPC_CONF.VISOR_KEEPER_CONF.PROTO_CONF.string()
        +"://" + datastore_service_ip
        +":" + std::to_string( datastore_service_port);

    uint16_t datastore_service_provider_id =
            confManager.RPC_CONF.VISOR_KEEPER_CONF.KEEPER_END_CONF.SERVICE_PROVIDER_ID;            
    
    chronolog::service_endpoint datastore_endpoint;
    // validate ip address, instantiate DataAdminService and create ServiceId to be included in KeeperRegistrationMsg

    if( -1 == service_endpoint_from_dotted_string( datastore_service_ip, datastore_service_port, datastore_endpoint) )
    {
    	    std::cout<<"invalid DataStoreAdmin service address"<<std::endl;
	    return (-1);
    }	  

    chronolog::ServiceId collectionServiceId(datastore_endpoint.first,datastore_endpoint.second, datastore_service_provider_id);

    // Instantiate KeeperRecordingService 

    std::string KEEPER_RECORDING_SERVICE_PROTOCOL = confManager.RPC_CONF.CLIENT_KEEPER_CONF.PROTO_CONF.string();
    std::string KEEPER_RECORDING_SERVICE_IP  = confManager.RPC_CONF.CLIENT_KEEPER_CONF.KEEPER_END_CONF.KEEPER_IP.string();
    uint16_t KEEPER_RECORDING_SERVICE_PORT  = confManager.RPC_CONF.CLIENT_KEEPER_CONF.KEEPER_END_CONF.KEEPER_PORT;
    uint16_t recording_service_provider_id  = confManager.RPC_CONF.CLIENT_KEEPER_CONF.KEEPER_END_CONF.SERVICE_PROVIDER_ID;

    std::string KEEPER_RECORDING_SERVICE_NA_STRING = std::string(KEEPER_RECORDING_SERVICE_PROTOCOL)
	                                           +"://"+std::string(KEEPER_RECORDING_SERVICE_IP)
						   +":"+std::to_string(KEEPER_RECORDING_SERVICE_PORT);

    // validate ip address, instantiate Recording Service and create KeeperIdCard

    chronolog::service_endpoint recording_endpoint;
    if( -1 == service_endpoint_from_dotted_string( KEEPER_RECORDING_SERVICE_IP, KEEPER_RECORDING_SERVICE_PORT, recording_endpoint) )
    {
    	std::cout<<"invalid KeeperRecordingService  address"<<std::endl;
	    return (-1);
    }
	  
    // create KeeperIdCard to identify this Keeper process in ChronoVisor's KeeperRegistry
    chronolog::KeeperIdCard keeperIdCard( keeper_group_id, recording_endpoint.first, recording_endpoint.second, recording_service_provider_id);
    std::cout << keeperIdCard<<std::endl;

    // Instantiate ChronoKeeper MemoryDataStore & ExtractorModule
    chronolog::IngestionQueue ingestionQueue;
    chronolog::CSVFileStoryChunkExtractor storyExtractor( keeperIdCard, keeper_csv_files_directory); 
    chronolog::KeeperDataStore theDataStore(ingestionQueue, storyExtractor.getExtractionQueue());

    margo_instance_id collection_margo_id=margo_init( KEEPER_DATASTORE_SERVICE_NA_STRING.c_str(),MARGO_SERVER_MODE, 1, 1);

    if(MARGO_INSTANCE_NULL == collection_margo_id)
    {
      std::cout<<"FAiled to initialise collection_margo_instance"<<std::endl;
      return 1;
    }
    std::cout<<"collection_margo_instance initialized"<<std::endl;

    tl::engine collectionEngine(collection_margo_id);
 
    std::cout << "ChronoKeeperInstance group_id {"<<keeper_group_id<<"} starting DataStoreAdminService at address {" << collectionEngine.self()
        << "} with provider_id {" << datastore_service_provider_id <<"}"<< std::endl;
    
    chronolog::DataStoreAdminService * keeperDataAdminService = 
	    chronolog::DataStoreAdminService::CreateDataStoreAdminService(collectionEngine, datastore_service_provider_id, theDataStore);


    // Instantiate KeeperRecordingService 
    margo_instance_id margo_id=margo_init( KEEPER_RECORDING_SERVICE_NA_STRING.c_str(),MARGO_SERVER_MODE, 1, 1);

    if(MARGO_INSTANCE_NULL == margo_id)
    {
      std::cout<<"FAiled to initialise margo_instance"<<std::endl;
      return 1;
    }
    std::cout<<"margo_instance initialized"<<std::endl;

    tl::engine recordingEngine(margo_id);
 
    std::cout << "ChronoKeeperInstance group_id {"<<keeper_group_id<<"} starting KeeperRecordingService at address {" << recordingEngine.self()
        << "} with provider_id {" << recording_service_provider_id <<"}"<< std::endl;


    // Instantiate KeeperRecordingService 

    chronolog::KeeperRecordingService *  keeperRecordingService=
	   chronolog::KeeperRecordingService::CreateKeeperRecordingService(recordingEngine, recording_service_provider_id, ingestionQueue);


    // create KeeperRegistryClient and register the new KeeperRecording service with the KeeperRegistry 
    std::string KEEPER_REGISTRY_SERVICE_NA_STRING=
        confManager.RPC_CONF.VISOR_KEEPER_CONF.PROTO_CONF.string()
        +"://" + confManager.RPC_CONF.VISOR_KEEPER_CONF.VISOR_END_CONF.VISOR_IP.string()
        +":" + std::to_string(confManager.RPC_CONF.VISOR_KEEPER_CONF.VISOR_END_CONF.VISOR_BASE_PORT);

    uint16_t KEEPER_REGISTRY_SERVICE_PROVIDER_ID = confManager.RPC_CONF.VISOR_KEEPER_CONF.VISOR_END_CONF.SERVICE_PROVIDER_ID;

    chronolog::KeeperRegistryClient * keeperRegistryClient = chronolog::KeeperRegistryClient::CreateKeeperRegistryClient(
		     collectionEngine, KEEPER_REGISTRY_SERVICE_NA_STRING, KEEPER_REGISTRY_SERVICE_PROVIDER_ID);

    keeperRegistryClient->send_register_msg(chronolog::KeeperRegistrationMsg(keeperIdCard,collectionServiceId));


    tl::abt scope;

    theDataStore.startDataCollection(3);

    // start extraction streams & threads
    storyExtractor.startExtractionThreads(2);

   // now we are ready to ingest records coming from the storyteller clients ....
    // main thread would be sending stats message until keeper process receives
    // sigterm signal
    chronolog::KeeperStatsMsg keeperStatsMsg(keeperIdCard);
    while( keep_running)
    {
        keeperRegistryClient->send_stats_msg(keeperStatsMsg);
        sleep(30);
    }

    //unregister from the chronoVisor so that no new story requests would be coming 
    keeperRegistryClient->send_unregister_msg(keeperIdCard);
    delete keeperRegistryClient;

    //stop recording events
    delete keeperRecordingService;
    delete keeperDataAdminService;

    //shutdown the Data Collection
    //INNA: move ingestionQueue and sequencing Xstream pool into the dataStore class later 
   // ingestionQueue.shutdown();
    theDataStore.shutdownDataCollection();

    // shutdown extraction module
    // drain extractionQueue and stop extraction xStreams 
    storyExtractor.shutdownExtractionThreads();

// these are not probably needed as thalium handles the engine finalization...
//    recordingEngine.finalize();
  //  collectionEngine.finalize();

return exit_code;
}

