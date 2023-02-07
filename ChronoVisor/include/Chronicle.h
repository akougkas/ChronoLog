/*BSD 2-Clause License

Copyright (c) 2022, Scalable Computing Software Laboratory
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
//
// Created by kfeng on 3/6/22.
//

#ifndef CHRONOLOG_CHRONICLE_H
#define CHRONOLOG_CHRONICLE_H

#include <unordered_map>
#include <ostream>
#include <atomic>
#include <Story.h>
#include <Archive.h>
#include "city.h"
#include <log.h>
#include <errcode.h>
#include <mutex>

#define MAX_CHRONICLE_PROPERTY_LIST_SIZE 16
#define MAX_CHRONICLE_METADATA_MAP_SIZE 16
#define MAX_STORY_MAP_SIZE 1024
#define MAX_ARCHIVE_MAP_SIZE 1024

enum ChronicleIndexingGranularity {
    chronicle_gran_ns = 0,
    chronicle_gran_us = 1,
    chronicle_gran_ms = 2,
    chronicle_gran_sec = 3
};

enum ChronicleType {
    chronicle_type_standard = 0,
    chronicle_type_priority = 1
};

enum ChronicleTieringPolicy {
    chronicle_tiering_normal = 0,
    chronicle_tiering_hot = 1,
    chronicle_tiering_cold = 2
};

typedef struct ChronicleAttrs_ {
    uint64_t size;
    enum ChronicleIndexingGranularity indexing_granularity;
    enum ChronicleType type;
    enum ChronicleTieringPolicy tiering_policy;
    uint16_t access_permission;
} ChronicleAttrs;

typedef struct ChronicleStats_ {
    uint64_t count;
} ChronicleStats;

class Chronicle {
public:
    Chronicle() {
        propertyList_ = std::unordered_map<std::string, std::string>(MAX_CHRONICLE_PROPERTY_LIST_SIZE);
        metadataMap_ = std::unordered_map<std::string, std::string>(MAX_CHRONICLE_METADATA_MAP_SIZE);
        storyMap_ = std::unordered_map<uint64_t, Story *>(MAX_STORY_MAP_SIZE);
        archiveMap_ = std::unordered_map<uint64_t, Archive *>(MAX_ARCHIVE_MAP_SIZE);
        attrs_.size = 0;
        attrs_.indexing_granularity = chronicle_gran_ms;
        attrs_.type = chronicle_type_standard;
        attrs_.tiering_policy = chronicle_tiering_normal;
        attrs_.access_permission = 0;
        stats_.count = 0;
    }

    void setName(const std::string &name) { name_ = name; }
    void setCid(const uint64_t &cid) { cid_ = cid; }
    void setStats(const ChronicleStats &stats) { stats_ = stats; }
    void setProperty(const std::unordered_map<std::string, std::string>& attrs) {
        for (auto const& entry : attrs) {
            propertyList_.emplace(entry.first, entry.second);
        }
    }

    const std::string &getName() const { return name_; }
    const uint64_t &getCid() const { return cid_; }
    const ChronicleStats &getStats() const { return stats_; }
    std::unordered_map<std::string, std::string> &getPropertyList() { return propertyList_; }
    const std::unordered_map<std::string, std::string> &getMetadataMap() const { return metadataMap_; }
    std::unordered_map<uint64_t, Story *> &getStoryMap() { return storyMap_; }
    const std::unordered_map<uint64_t, Archive *> &getArchiveMap() const { return archiveMap_; }

    friend std::ostream& operator<<(std::ostream& os, const Chronicle& chronicle);

    int addProperty(const std::string& name, const std::string& value) {
        if (propertyList_.size() <= MAX_CHRONICLE_PROPERTY_LIST_SIZE) {
            auto res = propertyList_.insert_or_assign(name, value);
            if (res.second) return CL_SUCCESS;
            else return CL_ERR_UNKNOWN;
        } else {
            return CL_ERR_CHRONICLE_PROPERTY_FULL;
        }
    }

    int addMetadata(const std::string& name, const std::string& value) {
        if (metadataMap_.size() <= MAX_CHRONICLE_METADATA_MAP_SIZE) {
            auto res = metadataMap_.insert_or_assign(name, value);
            if (res.second) return CL_SUCCESS;
            else return CL_ERR_UNKNOWN;
        } else {
            return CL_ERR_CHRONICLE_METADATA_FULL;
        }
    }

    int addStory(std::string &chronicle_name, const std::string& story_name,
                 const std::unordered_map<std::string, std::string>& attrs) {
        // add cid to name before hash to allow same story name across chronicles
        std::string story_name_for_hash = chronicle_name + story_name;
        uint64_t cid = CityHash64(chronicle_name.c_str(), chronicle_name.size());
        uint64_t sid = CityHash64(story_name_for_hash.c_str(), story_name_for_hash.size());
        if(storyMap_.find(sid) != storyMap_.end()) return CL_ERR_STORY_EXISTS;
        auto *pStory = new Story();
        pStory->setName(story_name);
        pStory->setProperty(attrs);
        pStory->setSid(sid);
        pStory->setCid(cid);
        LOGD("adding to storyMap@%p with %lu entries in Chronicle@%p",
             &storyMap_, storyMap_.size(), this);
        auto res = storyMap_.emplace(sid, pStory);
        if (res.second) return CL_SUCCESS;
        else return CL_ERR_UNKNOWN;
    }

    int removeStory(std::string &chronicle_name, const std::string& story_name, int flags) {
        // add cid to name before hash to allow same story name across chronicles
        std::string story_name_for_hash = chronicle_name + story_name;
        uint64_t sid = CityHash64(story_name_for_hash.c_str(), story_name_for_hash.size());
        auto storyRecord = storyMap_.find(sid);
        if (storyRecord != storyMap_.end()) {
            Story *pStory = storyRecord->second;
            if (pStory->getAcquisitionCount() != 0) {
                return CL_ERR_ACQUIRED;
            }
            delete pStory;
            LOGD("removing from storyMap@%p with %lu entries in Chronicle@%p",
                 &storyMap_, storyMap_.size(), this);
            auto nErased = storyMap_.erase(sid);
            if (nErased == 1) return CL_SUCCESS;
            else return CL_ERR_UNKNOWN;
        }
        return CL_ERR_NOT_EXIST;
    }


    int addArchive(uint64_t cid, const std::string& name, const std::unordered_map<std::string, std::string>& attrs) {
        // add cid to name before hash to allow same archive name across chronicles
        std::string archive_name_for_hash = std::to_string(cid) + name;
        uint64_t aid = CityHash64(archive_name_for_hash.c_str(), archive_name_for_hash.size());
        if (archiveMap_.find(aid) != archiveMap_.end()) return false;
        auto *pArchive = new Archive();
        pArchive->setName(name);
        pArchive->setProperty(attrs);
        pArchive->setAid(aid);
        pArchive->setCid(cid);
        LOGD("adding to archiveMap@%p with %lu entries in Chronicle@%p",
             &archiveMap_, archiveMap_.size(), this);
        auto res = archiveMap_.emplace(aid, pArchive);
        if (res.second) return CL_SUCCESS;
        else return CL_ERR_UNKNOWN;
    }

    int removeArchive(uint64_t cid, const std::string& name, int flags) {
        // add cid to name before hash to allow same archive name across chronicles
        std::string archive_name_for_hash = std::to_string(cid) + name;
        uint64_t aid = CityHash64(archive_name_for_hash.c_str(), archive_name_for_hash.size());
        auto storyRecord = archiveMap_.find(aid);
        if (storyRecord != archiveMap_.end()) {
            Archive *pArchive = storyRecord->second;
            delete pArchive;
            LOGD("removing from archiveMap@%p with %lu entries in Chronicle@%p",
                 &archiveMap_, archiveMap_.size(), this);
            auto nErased = archiveMap_.erase(aid);
            if (nErased == 1) return CL_SUCCESS;
            else return CL_ERR_UNKNOWN;
        }
        return CL_ERR_NOT_EXIST;
    }

    uint64_t incrementAcquisitionCount() {
        stats_.count++;
        return stats_.count;
    }
    uint64_t decrementAcquisitionCount() {
        stats_.count--;
        return stats_.count;
    }
    uint64_t getAcquisitionCount() const { return stats_.count; }

    size_t getPropertyListSize() { return propertyList_.size(); }
    size_t getMetadataMapSize() { return metadataMap_.size(); }
    size_t getStoryMapSize() { return storyMap_.size(); }
    size_t getArchiveMapSize() { return archiveMap_.size(); }

private:
    std::string name_;
    uint64_t cid_{};
    ChronicleAttrs attrs_{};
    ChronicleStats stats_{};
    std::unordered_map<std::string, std::string> propertyList_;
    std::unordered_map<std::string, std::string> metadataMap_;
    std::unordered_map<uint64_t, Story *> storyMap_;
    std::unordered_map<uint64_t, Archive *> archiveMap_;
};

inline std::ostream& operator<<(std::ostream& os, const Chronicle& chronicle) {
    os << "name: " << chronicle.name_ << ", "
       << "cid: " << chronicle.cid_ << ", "
       << "access count: " << chronicle.stats_.count << ", "
       << "properties: ";
    os << "(";
    for (auto const& property : chronicle.propertyList_)
        os << property.first << ": " << property.second << ", ";
    os << ")";
    return os;
}

#endif //CHRONOLOG_CHRONICLE_H
