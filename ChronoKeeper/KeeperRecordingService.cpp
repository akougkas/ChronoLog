#include <iostream>
#include <margo.h>
#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>
#include "KeeperIdCard.h"

namespace tl = thallium;


//INNA :  for now assume all identifiers are uint64_t , revisit later on

namespace chronolog
{

//#INNA :  for now assume all identifiers are uint64_t , revisit later 
typedef uint64_t StoryId;
typedef uint64_t StorytellerId;

struct Event { uint64_t time; std::string record; };

class KeeperRecordingService : public tl::provider<KeeperRecordingService> 
{

    private:

    void hello(const std::string& name) {
        std::cout << "Hello, " << name << std::endl;
    }

    void record_event(tl::request const& request, 
                      //const StorytellerId& teller_id, const  StoryId& story_id, const std::string& record) 
                       StorytellerId teller_id,  StoryId story_id, const std::string& record) 
    {
        std::cout << "recording {"<< teller_id<<":"<<story_id<<":"<< record <<"}"<< std::endl;
        request.respond( static_cast<int>(record.size()) );
    }

    int record_int_values( int teller_id, int story_id) //, int teller_id)
    { 
        std::cout << "recording int by value  {"<<teller_id <<":"<<story_id<<"}"<< std::endl;
	return story_id;
    }
    public:

    KeeperRecordingService(tl::engine& tl_engine, uint16_t service_provider_id=1)
    : tl::provider<KeeperRecordingService>(tl_engine, service_provider_id) {
        define("hello", &KeeperRecordingService::hello, tl::ignore_return_value());
        define("record_event", &KeeperRecordingService::record_event);
        define("record_int_values", &KeeperRecordingService::record_int_values );
    }

    ~KeeperRecordingService() {
        std::cout<<"KeeperRecordingService::destructor"<<std::endl;
        get_engine().wait_for_finalize();
    }
};


}// namespace chronolog

// uncomment to test this class as standalone process
int main(int argc, char** argv) {

    uint16_t provider_id = 22;

    margo_instance_id margo_id=margo_init("ofi+sockets://localhost:5555",MARGO_SERVER_MODE, 1, 0);

    if(MARGO_INSTANCE_NULL == margo_id)
    {
      std::cout<<"FAiled to initialise margo_instance"<<std::endl;
      return 1;
    }
    std::cout<<"margo_instance initialized"<<std::endl;

   tl::engine keeperEngine(margo_id);
 
    std::cout << "Starting KeeperRecordingService  at address " << keeperEngine.self()
        << " with provider id " << provider_id << std::endl;

    chronolog::KeeperRecordingService keeperRecordingService(keeperEngine, provider_id);
   
    return 0;
}

