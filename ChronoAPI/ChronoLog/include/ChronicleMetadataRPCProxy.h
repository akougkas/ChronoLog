//
// Created by kfeng on 4/4/22.
//

#ifndef CHRONOLOG_CHRONICLEMETADATARPCPROXY_H
#define CHRONOLOG_CHRONICLEMETADATARPCPROXY_H

#include <iostream>
#include <unordered_map>
#include <functional>
#include <thallium.hpp>
#include <sys/types.h>
#include <unistd.h>
#include "macro.h"
#include "RPCFactory.h"
#include "ChronicleMetaDirectory.h"
#include "ClientRegistryInfo.h"
#include "ClientRegistryManager.h"

class ChronicleMetadataRPCProxy {
public:
    ChronicleMetadataRPCProxy() : rpc(ChronoLog::Singleton<ChronoLogRPCFactory>::GetInstance()->
                                      GetRPC(CHRONOLOG_CONF->RPC_BASE_SERVER_PORT)) {
        func_prefix = "ChronoLog";
        switch (CHRONOLOG_CONF->RPC_IMPLEMENTATION) {
            case CHRONOLOG_THALLIUM_SOCKETS:
            case CHRONOLOG_THALLIUM_TCP:
            case CHRONOLOG_THALLIUM_ROCE:
                func_prefix += "Thallium";
                break;
        }
        if (CHRONOLOG_CONF->IS_SERVER) {
            chronicleMetaDirectory = ChronoLog::Singleton<ChronicleMetaDirectory>::GetInstance();
        }
        LOGD("%s constructor finishes, object created@%p in thread PID=%d", typeid(*this).name(), this, getpid());
    }

    ~ChronicleMetadataRPCProxy() = default;

    bool LocalCreateChronicle(std::string &name, const std::unordered_map<std::string, std::string> &attrs) {
        LOGD("%s is called in PID=%d, with args: name=%s, attrs=", __FUNCTION__, getpid(), name.c_str());
        for (auto iter = attrs.begin(); iter != attrs.end(); ++iter) {
            LOGD("%s=%s", iter->first.c_str(), iter->second.c_str());
        }
        extern std::shared_ptr<ChronicleMetaDirectory> g_chronicleMetaDirectory;
        return g_chronicleMetaDirectory->create_chronicle(name);
    }

    bool CreateChronicle(std::string &name, const std::unordered_map<std::string, std::string> &attrs) {
        LOGD("%s is called in PID=%d, with args: name=%s, attrs=", __FUNCTION__, getpid(), name.c_str());
        for (auto iter = attrs.begin(); iter != attrs.end(); ++iter) {
            LOGD("%s=%s", iter->first.c_str(), iter->second.c_str());
        }
        return CHRONOLOG_RPC_CALL_WRAPPER("CreateChronicle", 0, bool, name, attrs);
    }

    bool LocalDestroyChronicle(std::string &name, const int &flags) {
        LOGD("%s is called in PID=%d, with args: name=%s, flags=%d", __FUNCTION__, getpid(), name.c_str(), flags);
        extern std::shared_ptr<ChronicleMetaDirectory> g_chronicleMetaDirectory;
        return g_chronicleMetaDirectory->destroy_chronicle(name, flags);
    }

    bool DestroyChronicle(std::string &name, const int &flags) {
        LOGD("%s is called in PID=%d, with args: name=%s, flags=%d", __FUNCTION__, getpid(), name.c_str(), flags);
        return CHRONOLOG_RPC_CALL_WRAPPER("DestroyChronicle", 0, bool, name, flags);
    }

    bool LocalAcquireChronicle(std::string &name, const int &flags) {
        LOGD("%s is called in PID=%d, with args: name=%s, flags=%d", __FUNCTION__, getpid(), name.c_str(), flags);
        extern std::shared_ptr<ChronicleMetaDirectory> g_chronicleMetaDirectory;
        return g_chronicleMetaDirectory->acquire_chronicle(name, flags);
    }

    bool AcquireChronicle(std::string &name, const int &flags) {
        LOGD("%s is called in PID=%d, with args: name=%s, flags=%d", __FUNCTION__, getpid(), name.c_str(), flags);
        return CHRONOLOG_RPC_CALL_WRAPPER("AcquireChronicle", 0, bool, name, flags);
    }

    bool LocalReleaseChronicle(std::string &name, const int &flags) {
        LOGD("%s is called in PID=%d, with args: name=%s, flags=%d", __FUNCTION__, getpid(), name.c_str(), flags);
        extern std::shared_ptr<ChronicleMetaDirectory> g_chronicleMetaDirectory;
        return g_chronicleMetaDirectory->release_chronicle(name, flags);
    }

    bool ReleaseChronicle(std::string &name, const int &flags) {
        LOGD("%s is called in PID=%d, with args: name=%s, flags=%d", __FUNCTION__, getpid(), name.c_str(), flags);
        return CHRONOLOG_RPC_CALL_WRAPPER("ReleaseChronicle", 0, bool, name, flags);
    }

    bool LocalCreateStory(std::string &chronicle_name, std::string &story_name,
                          const std::unordered_map<std::string, std::string> &attrs) {
        LOGD("%s is called in PID=%d, with args: chronicle_name=%s, story_name=%s, ,attrs=",
             __FUNCTION__, getpid(), chronicle_name.c_str(), story_name.c_str());
        for (auto iter = attrs.begin(); iter != attrs.end(); ++iter) {
            LOGD("%s=%s", iter->first.c_str(), iter->second.c_str());
        }
        extern std::shared_ptr<ChronicleMetaDirectory> g_chronicleMetaDirectory;
        return g_chronicleMetaDirectory->create_story(chronicle_name, story_name, attrs);
    }

    bool CreateStory(std::string &chronicle_name, std::string &story_name,
                     const std::unordered_map<std::string, std::string> &attrs) {
        LOGD("%s is called in PID=%d, with args: chronicle_name=%s, story_name=%s, ,attrs=",
             __FUNCTION__, getpid(), chronicle_name.c_str(), story_name.c_str());
        for (auto iter = attrs.begin(); iter != attrs.end(); ++iter) {
            LOGD("%s=%s", iter->first.c_str(), iter->second.c_str());
        }
        return CHRONOLOG_RPC_CALL_WRAPPER("CreateStory", 0, bool, chronicle_name, story_name, attrs);
    }

    bool LocalDestroyStory(std::string &chronicle_name, std::string &story_name, const int &flags) {
        LOGD("%s is called in PID=%d, with args: chronicle_name=%s, story_name=%s, flags=%d",
             __FUNCTION__, getpid(), chronicle_name.c_str(), story_name.c_str(), flags);
        extern std::shared_ptr<ChronicleMetaDirectory> g_chronicleMetaDirectory;
        return g_chronicleMetaDirectory->destroy_story(chronicle_name, story_name, flags);
    }

    bool DestroyStory(std::string &chronicle_name, std::string &story_name, const int &flags) {
        LOGD("%s is called in PID=%d, with args: chronicle_name=%s, story_name=%s, flags=%d",
             __FUNCTION__, getpid(), chronicle_name.c_str(), story_name.c_str(), flags);
        return CHRONOLOG_RPC_CALL_WRAPPER("DestroyStory", 0, bool, chronicle_name, story_name, flags);
    }

    bool LocalAcquireStory(std::string &chronicle_name, std::string &story_name, const int &flags) {
        LOGD("%s is called in PID=%d, with args: chronicle_name=%s, story_name=%s, flags=%d",
             __FUNCTION__, getpid(), chronicle_name.c_str(), story_name.c_str(), flags);
        extern std::shared_ptr<ChronicleMetaDirectory> g_chronicleMetaDirectory;
        return g_chronicleMetaDirectory->acquire_story(chronicle_name, story_name, flags);
    }

    bool AcquireStory(std::string &chronicle_name, std::string &story_name, const int &flags) {
        LOGD("%s is called in PID=%d, with args: chronicle_name=%s, story_name=%s, flags=%d",
             __FUNCTION__, getpid(), chronicle_name.c_str(), story_name.c_str(), flags);
        return CHRONOLOG_RPC_CALL_WRAPPER("AcquireStory", 0, bool, chronicle_name, story_name, flags);
    }

    bool LocalReleaseStory(std::string &chronicle_name, std::string &story_name, const int &flags) {
        LOGD("%s is called in PID=%d, with args: chronicle_name=%s, story_name=%s, flags=%d",
             __FUNCTION__, getpid(), chronicle_name.c_str(), story_name.c_str(), flags);
        extern std::shared_ptr<ChronicleMetaDirectory> g_chronicleMetaDirectory;
        return g_chronicleMetaDirectory->release_story(chronicle_name, story_name, flags);
    }

    bool ReleaseStory(std::string &chronicle_name, std::string &story_name, const int &flags) {
        LOGD("%s is called in PID=%d, with args: chronicle_name=%s, story_name=%s, flags=%d",
             __FUNCTION__, getpid(), chronicle_name.c_str(), story_name.c_str(), flags);
        return CHRONOLOG_RPC_CALL_WRAPPER("ReleaseStory", 0, bool, chronicle_name, story_name, flags);
    }

    std::string LocalGetChronicleAttr(std::string &name, const std::string &key) {
        LOGD("%s is called in PID=%d, with args: name=%s, key=%s", __FUNCTION__, getpid(), name.c_str(), key.c_str());
        extern std::shared_ptr<ChronicleMetaDirectory> g_chronicleMetaDirectory;
        return g_chronicleMetaDirectory->get_chronicle_attr(name, key);
    }

    std::string GetChronicleAttr(std::string &name, const std::string &key) {
        LOGD("%s is called in PID=%d, with args: name=%s, key=%s", __FUNCTION__, getpid(), name.c_str(), key.c_str());
        return CHRONOLOG_RPC_CALL_WRAPPER("GetChronicleAttr", 0, std::string, name, key);
    }

    bool LocalEditChronicleAttr(std::string &name, const std::string &key, const std::string &value) {
        LOGD("%s is called in PID=%d, with args: name=%s, key=%s, value=%s",
             __FUNCTION__, getpid(), name.c_str(), key.c_str(), value.c_str());
        extern std::shared_ptr<ChronicleMetaDirectory> g_chronicleMetaDirectory;
        return g_chronicleMetaDirectory->edit_chronicle_attr(name, key, value);
    }

    bool EditChronicleAttr(std::string &name, const std::string &key, const std::string &value) {
        LOGD("%s is called in PID=%d, with args: name=%s, key=%s, value=%s",
             __FUNCTION__, getpid(), name.c_str(), key.c_str(), value.c_str());
        return CHRONOLOG_RPC_CALL_WRAPPER("EditChronicleAttr", 0, bool, name, key, value);
    }
    
    void bind_functions() {
        switch (CHRONOLOG_CONF->RPC_IMPLEMENTATION) {
            case CHRONOLOG_THALLIUM_SOCKETS:
            case CHRONOLOG_THALLIUM_TCP:
            case CHRONOLOG_THALLIUM_ROCE: {
                std::function<void(const tl::request &,
                                   std::string &,
                                   const std::unordered_map<std::string, std::string> &)> createChronicleFunc(
                        std::bind(&ChronicleMetadataRPCProxy::ThalliumLocalCreateChronicle,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  std::placeholders::_3)
                );
                std::function<void(const tl::request &,
                                   std::string &,
                                   const int &)> destroyChronicleFunc(
                        std::bind(&ChronicleMetadataRPCProxy::ThalliumLocalDestroyChronicle,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  std::placeholders::_3)
                );
                std::function<void(const tl::request &,
                                   std::string &,
                                   const int &)> acquireChronicleFunc(
                        std::bind(&ChronicleMetadataRPCProxy::ThalliumLocalAcquireChronicle,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  std::placeholders::_3)
                );
                std::function<void(const tl::request &,
                                   std::string &,
                                   const int &)> releaseChronicleFunc(
                        std::bind(&ChronicleMetadataRPCProxy::ThalliumLocalReleaseChronicle,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  std::placeholders::_3)
                );

                std::function<void(const tl::request &,
                                   std::string &,
                                   std::string &,
                                   const std::unordered_map<std::string, std::string> &)> createStoryFunc(
                        std::bind(&ChronicleMetadataRPCProxy::ThalliumLocalCreateStory,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  std::placeholders::_3,
                                  std::placeholders::_4)
                );
                std::function<void(const tl::request &,
                                   std::string &,
                                   std::string &,
                                   const int &)> destroyStoryFunc(
                        std::bind(&ChronicleMetadataRPCProxy::ThalliumLocalDestroyStory,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  std::placeholders::_3,
                                  std::placeholders::_4)
                );
                std::function<void(const tl::request &,
                                   std::string &,
                                   std::string &,
                                   const int &)> acquireStoryFunc(
                        std::bind(&ChronicleMetadataRPCProxy::ThalliumLocalAcquireStory,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  std::placeholders::_3,
                                  std::placeholders::_4)
                );
                std::function<void(const tl::request &,
                                   std::string &,
                                   std::string &,
                                   const int &)> releaseStoryFunc(
                        std::bind(&ChronicleMetadataRPCProxy::ThalliumLocalReleaseStory,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  std::placeholders::_3,
                                  std::placeholders::_4)
                );

                std::function<void(const tl::request &,
                                   std::string &name,
                                   const std::string &key)> getChronicleAttrFunc(
                        std::bind(&ChronicleMetadataRPCProxy::ThalliumLocalGetChronicleAttr,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  std::placeholders::_3)
                );
                std::function<void(const tl::request &,
                                   std::string &name,
                                   const std::string &key,
                                   const std::string &value)> editChronicleAttrFunc(
                        std::bind(&ChronicleMetadataRPCProxy::ThalliumLocalEditChronicleAttr,
                                  this,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  std::placeholders::_3,
                                  std::placeholders::_4)
                );

                rpc->bind("ChronoLogThalliumCreateChronicle", createChronicleFunc);
                rpc->bind("ChronoLogThalliumDestroyChronicle", destroyChronicleFunc);
                rpc->bind("ChronoLogThalliumAcquireChronicle", acquireChronicleFunc);
                rpc->bind("ChronoLogThalliumReleaseChronicle", releaseChronicleFunc);

                rpc->bind("ChronoLogThalliumCreateStory", createStoryFunc);
                rpc->bind("ChronoLogThalliumDestroyStory", destroyStoryFunc);
                rpc->bind("ChronoLogThalliumAcquireStory", acquireStoryFunc);
                rpc->bind("ChronoLogThalliumReleaseStory", releaseStoryFunc);

                rpc->bind("ChronoLogThalliumGetChronicleAttr", getChronicleAttrFunc);
                rpc->bind("ChronoLogThalliumEditChronicleAttr", editChronicleAttrFunc);
            }
        }
    }

    CHRONOLOG_THALLIUM_DEFINE(LocalCreateChronicle, (name, attrs),
                    std::string &name, const std::unordered_map<std::string, std::string> &attrs)
    CHRONOLOG_THALLIUM_DEFINE(LocalDestroyChronicle, (name, flags), std::string &name, const int &flags)
    CHRONOLOG_THALLIUM_DEFINE(LocalAcquireChronicle, (name, flags), std::string &name, const int &flags)
    CHRONOLOG_THALLIUM_DEFINE(LocalReleaseChronicle, (name, flags), std::string &name, const int &flags)

    CHRONOLOG_THALLIUM_DEFINE(LocalCreateStory, (chronicle_name, story_name, attrs),
                    std::string &chronicle_name, std::string &story_name,
                    const std::unordered_map<std::string, std::string> &attrs)
    CHRONOLOG_THALLIUM_DEFINE(LocalDestroyStory, (chronicle_name, story_name, flags),
                    std::string &chronicle_name, std::string &story_name, const int &flags)
    CHRONOLOG_THALLIUM_DEFINE(LocalAcquireStory, (chronicle_name, story_name, flags),
                    std::string &chronicle_name, std::string &story_name, const int &flags)
    CHRONOLOG_THALLIUM_DEFINE(LocalReleaseStory, (chronicle_name, story_name, flags),
                    std::string &chronicle_name, std::string &story_name, const int &flags)

    CHRONOLOG_THALLIUM_DEFINE(LocalGetChronicleAttr, (name, key), std::string &name, const std::string &key)
    CHRONOLOG_THALLIUM_DEFINE(LocalEditChronicleAttr, (name, key, value),
                    std::string &name, const std::string &key, const std::string &value)

//private:
    std::shared_ptr<ChronicleMetaDirectory> chronicleMetaDirectory;
    ChronoLogCharStruct func_prefix;
    std::shared_ptr<ChronoLogRPC> rpc;
};

#endif //CHRONOLOG_CHRONICLEMETADATARPCPROXY_H