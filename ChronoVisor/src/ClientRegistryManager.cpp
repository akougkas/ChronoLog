//
// Created by kfeng on 7/11/22.
//

#include <unistd.h>
#include <mutex>
#include "ClientRegistryManager.h"
#include "errcode.h"
#include "log.h"
#include <ChronicleMetaDirectory.h>

ClientRegistryManager::ClientRegistryManager() {
    LOGD("%s constructor is called, object created@%p in thread PID=%d",
         typeid(*this).name(), this, getpid());

    clientRegistry_ = new std::unordered_map<std::string, ClientInfo>();

    LOGD("clientRegistry_@%p has %ld entries", clientRegistry_, clientRegistry_->size());
}

ClientRegistryManager::~ClientRegistryManager() {
    delete clientRegistry_;
}

ClientInfo& ClientRegistryManager::get_client_info(const std::string &client_id) {
    std::lock_guard<std::mutex> clientRegistryLock(g_clientRegistryMutex_);
    auto clientRegistryRecord = clientRegistry_->find(client_id);
    if (clientRegistryRecord != clientRegistry_->end()) {
        return clientRegistryRecord->second;
    }
}

int ClientRegistryManager::add_story_acquisition(const std::string &client_id, uint64_t &sid, Story *pStory) {
    std::lock_guard<std::mutex> clientRegistryLock(g_clientRegistryMutex_);
    auto clientRegistryRecord = clientRegistry_->find(client_id);
    if (clientRegistryRecord != clientRegistry_->end()) {
        auto clientInfo = clientRegistryRecord->second;
        if (clientInfo.acquiredStoryList_.find(sid) != clientInfo.acquiredStoryList_.end()) {
            LOGD("client_id=%s has already acquired StoryID=%lu", client_id.c_str(), sid);
            return CL_ERR_ACQUIRED;
        } else {
            clientInfo.acquiredStoryList_.emplace(sid, pStory);
            auto res = clientRegistry_->insert_or_assign(client_id, clientInfo);
            if (res.second) {
                LOGD("added a new entry for client_id=%s acquiring StoryID=%lu", client_id.c_str(), sid);
                return CL_SUCCESS;
            } else {
                LOGD("updated an existing entry for client_id=%s acquiring StoryID=%lu", client_id.c_str(), sid);
                return CL_ERR_UNKNOWN;
            }
        }
    } else {
        LOGD("client_id=%s does not exist", client_id.c_str());
        return CL_ERR_UNKNOWN;
    }
}

int ClientRegistryManager::remove_story_acquisition(const std::string &client_id, uint64_t &sid) {
    std::lock_guard<std::mutex> clientRegistryLock(g_clientRegistryMutex_);
    auto clientRegistryRecord = clientRegistry_->find(client_id);
    if (clientRegistryRecord != clientRegistry_->end()) {
        if (clientRegistryRecord->second.acquiredStoryList_.find(sid) !=
            clientRegistryRecord->second.acquiredStoryList_.end()) {
            auto nErased = clientRegistryRecord->second.acquiredStoryList_.erase(sid);
            if (nErased == 1) {
                LOGD("removed StoryID=%lu from acquiredStoryList for client_id=%s", sid, client_id.c_str());
                LOGD("acquiredStoryList of client_id=%s has %zu entries left", client_id.c_str(),
                     clientRegistryRecord->second.acquiredStoryList_.size());
                return CL_SUCCESS;
            } else {
                LOGD("failed to remove StoryID=%lu from acquiredStoryList for client_id=%s", sid, client_id.c_str());
                return CL_ERR_UNKNOWN;
            }
        } else {
            LOGD("StoryID=%lu is not acquired by client_id=%s, cannot remove from acquiredStoryList for it",
                 sid, client_id.c_str());
            return CL_ERR_NOT_ACQUIRED;
        }
    } else {
        LOGD("client_id=%s does not exist", client_id.c_str());
        return CL_ERR_UNKNOWN;
    }
}

int ClientRegistryManager::add_client_record(const std::string &client_id, const ClientInfo &record) {
    LOGD("%s in ClientRegistryManager@%p", __FUNCTION__, this);
    LOGD("clientRegistry_@%p has %ld entries stored", clientRegistry_, clientRegistry_->size());
    std::lock_guard<std::mutex> lock(g_clientRegistryMutex_);
    if (clientRegistry_->insert_or_assign(client_id, record).second) {
        LOGD("a new entry has been added to clientRegistry_@%p", clientRegistry_);
        return CL_SUCCESS;
    } else {
        LOGE("Fail to add entry to clientRegistry_");
        return CL_ERR_UNKNOWN;
    }
}

int ClientRegistryManager::remove_client_record(const std::string &client_id) {
    LOGD("%s in ClientRegistryManager@%p", __FUNCTION__, this);
    LOGD("clientRegistry_@%p has %ld entries", clientRegistry_, clientRegistry_->size());
    std::lock_guard<std::mutex> lock(g_clientRegistryMutex_);
    auto clientRegistryRecord = clientRegistry_->find(client_id);
    if (clientRegistryRecord != clientRegistry_->end()) {
        auto clientInfo = clientRegistryRecord->second;
        if (!clientInfo.acquiredStoryList_.empty()) {
            LOGD("ClientID=%s still has %zu stories acquired, entry cannot be removed",
                 client_id.c_str(), clientInfo.acquiredStoryList_.size());
            return CL_ERR_ACQUIRED;
        }
    } else {
        LOGD("client_id=%s does not exist", client_id.c_str());
        return CL_ERR_UNKNOWN;
    }
    if (clientRegistry_->erase(client_id)) {
        LOGD("an entry has been removed from clientRegistry_@%p", clientRegistry_);
        return CL_SUCCESS;
    } else {
        LOGE("Fail to remove entry from clientRegistry_");
        return CL_ERR_UNKNOWN;
    }
}
