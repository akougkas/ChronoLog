//
// Created by kfeng on 7/18/22.
//
#include <client.h>
#include <cassert>
#include <common.h>
#include <thread>
#define STORY_NAME_LEN 32

struct thread_arg
{
  int tid;
};

ChronoLogClient *client;

void thread_body(struct thread_arg *t)
{

<<<<<<< HEAD
    std::string server_ip = CHRONOLOG_CONF->RPC_CONF.CLIENT_VISOR_CONF.VISOR_END_CONF.VISOR_IP.string();
    int base_port = CHRONOLOG_CONF->RPC_CONF.CLIENT_VISOR_CONF.VISOR_END_CONF.VISOR_BASE_PORT;
=======
    std::cout << "Start Thread tid="<<t->tid<< std::endl;
    //std::string server_ip = CHRONOLOG_CONF->RPC_CONF.CLIENT_VISOR_CONF.VISOR_END_CONF.VISOR_IP.string();
    //int base_port = CHRONOLOG_CONF->RPC_CONF.CLIENT_VISOR_CONF.VISOR_END_CONF.VISOR_BASE_PORT;
>>>>>>> 105df6b6cebb07312cf919ade4bb90c617fa51f4
    int flags = 0;
    uint64_t offset;
    int ret;
    std::string chronicle_name;
<<<<<<< HEAD
    if(t->tid%2==0) chronicle_name = "gscs5er9TcdJ9mOgUDteDVBcI0oQjozK";
    else chronicle_name = "6RPkwqX2IOpR41dVCqmWauX9RfXIuTAp";
=======
    if (t->tid % 2 == 0) chronicle_name = "Chronicle_2";
    else chronicle_name = "Chronicle_1";
>>>>>>> 105df6b6cebb07312cf919ade4bb90c617fa51f4
    std::unordered_map<std::string, std::string> chronicle_attrs;
    chronicle_attrs.emplace("Priority", "High");
    chronicle_attrs.emplace("IndexGranularity", "Millisecond");
    chronicle_attrs.emplace("TieringPolicy", "Hot");
    ret = client->CreateChronicle(chronicle_name, chronicle_attrs, flags);
    std::cout << "tid="<<t->tid<<" CreateChronicle {"<<chronicle_name<<"} ret: " << ret<< std::endl;
    flags = 1;
    std::string story_name = gen_random(STORY_NAME_LEN);
    std::unordered_map<std::string, std::string> story_attrs;
    story_attrs.emplace("Priority", "High");
    story_attrs.emplace("IndexGranularity", "Millisecond");
    story_attrs.emplace("TieringPolicy", "Hot");
    flags = 2;
<<<<<<< HEAD
    ret = client->AcquireStory(chronicle_name, story_name, story_attrs, flags);
    ret = client->ReleaseStory(chronicle_name,story_name,flags);
    ret = client->DestroyStory(chronicle_name,story_name,flags);
    ret = client->DestroyChronicle(chronicle_name,flags);
=======
    auto acquire_ret = client->AcquireStory(chronicle_name, story_name, story_attrs, flags);
    std::cout << "tid="<<t->tid<<" AcquireStory {"<<chronicle_name<<":"<<story_name<<"} ret: " << acquire_ret.first << std::endl;
    assert(acquire_ret.first == CL_SUCCESS || acquire_ret.first == CL_ERR_NOT_EXIST);
    ret = client->DestroyStory(chronicle_name, story_name);//, flags);
    std::cout << "tid="<<t->tid<<" DestroyStory {"<<chronicle_name<<":"<<story_name<<"} ret: " << ret<< std::endl;
    assert(ret == CL_ERR_ACQUIRED || ret==CL_SUCCESS || ret==CL_ERR_NOT_EXIST);
    ret = client->Disconnect(); //t->client_id, flags);
    assert(ret == CL_ERR_ACQUIRED || ret==CL_SUCCESS);
    ret = client->ReleaseStory(chronicle_name, story_name);//, flags);
    std::cout << "tid="<<t->tid<<" ReleaseStory {"<<chronicle_name<<":"<<story_name<<"} ret: " << ret<< std::endl;
    assert(ret == CL_SUCCESS || ret == CL_ERR_NO_CONNECTION);
    ret = client->DestroyStory(chronicle_name, story_name);//, flags);
    std::cout << "tid="<<t->tid<<" DestroyStory {"<<chronicle_name<<":"<<story_name<<"} ret: " << ret<< std::endl;
    assert(ret == CL_SUCCESS || ret == CL_ERR_NOT_EXIST || ret == CL_ERR_ACQUIRED || ret ==CL_ERR_NO_CONNECTION);

    
    ret = client->DestroyChronicle(chronicle_name);//, flags);
    assert(ret == CL_SUCCESS || ret == CL_ERR_NOT_EXIST || ret == CL_ERR_ACQUIRED || ret==CL_ERR_NO_CONNECTION);
    std::cout << "tid="<<t->tid<<" DestroyChronicle{"<<chronicle_name<<"} ret: " << ret<< std::endl;
    std::cout << "Stop Thread tid="<<t->tid<< std::endl;
>>>>>>> 105df6b6cebb07312cf919ade4bb90c617fa51f4
}

int main(int argc,char **argv) {


    int provided;
    std::string client_id = gen_random(8);

    int num_threads = 4;

    std::vector<struct thread_arg> t_args(num_threads);
    std::vector<std::thread> workers(num_threads);

    ChronoLogRPCImplementation protocol = CHRONOLOG_THALLIUM_SOCKETS;
    std::string server_ip = CHRONOLOG_CONF->RPC_CONF.CLIENT_VISOR_CONF.VISOR_END_CONF.VISOR_IP.string();
    int base_port = CHRONOLOG_CONF->RPC_CONF.CLIENT_VISOR_CONF.VISOR_END_CONF.VISOR_BASE_PORT;
    client = new ChronoLogClient(protocol, server_ip, base_port);

    std::string server_uri = CHRONOLOG_CONF->RPC_CONF.CLIENT_VISOR_CONF.PROTO_CONF.string();
    server_uri += "://"+server_ip+":"+std::to_string(base_port);

    int flags = 0;
    uint64_t offset;
    int ret = client->Connect(server_uri,client_id,flags,offset);

    for(int i=0;i<num_threads;i++)
    {
	t_args[i].tid = i;
	std::thread t{thread_body,&t_args[i]};
	workers[i] = std::move(t);
    }

    for(int i=0;i<num_threads;i++)
	    workers[i].join();

    ret = client->Disconnect(client_id,flags);
    delete client;

    return 0;
}
