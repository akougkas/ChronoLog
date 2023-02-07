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
// Created by kfeng on 5/17/22.
//

#include "client.h"
#include "common.h"
#include <cassert>

#define NUM_CHRONICLE (1)
#define NUM_STORY (1)

#define CHRONICLE_NAME_LEN 32
#define STORY_NAME_LEN 32

int main() {
    ChronoLogRPCImplementation protocol = CHRONOLOG_THALLIUM_SOCKETS;
    std::string server_ip = "127.0.0.1";
    int base_port = 5555;
    ChronoLogClient client(protocol, server_ip, base_port);
    std::vector<std::string> chronicle_names;
    std::chrono::steady_clock::time_point t1, t2;
    std::chrono::duration<double, std::nano> duration_create_chronicle{},
            duration_acquire_chronicle{},
            duration_edit_chronicle_attr{},
            duration_create_story{},
            duration_acquire_story{},
            duration_release_story{},
            duration_destroy_story{},
            duration_get_chronicle_attr{},
            duration_release_chronicle{},
            duration_destroy_chronicle{};
    int flags;

    chronicle_names.reserve(NUM_CHRONICLE);
    for (int i = 0; i < NUM_CHRONICLE; i++) {
        std::string chronicle_name(gen_random(CHRONICLE_NAME_LEN));
        chronicle_names.emplace_back(chronicle_name);
        std::string attr = std::string("Priority=High");
        int ret;
        std::unordered_map<std::string, std::string> chronicle_attrs;
        chronicle_attrs.emplace("Priority", "High");
        chronicle_attrs.emplace("IndexGranularity", "Millisecond");
        chronicle_attrs.emplace("TieringPolicy", "Hot");
        t1 = std::chrono::steady_clock::now();
        ret = client.CreateChronicle(chronicle_name, chronicle_attrs, flags);
        t2 = std::chrono::steady_clock::now();
        assert(ret == CL_SUCCESS);
        duration_create_chronicle += (t2 - t1);

        flags = 1;
        t1 = std::chrono::steady_clock::now();
        ret = client.AcquireChronicle(chronicle_name, flags);
        t2 = std::chrono::steady_clock::now();
        assert(ret == CL_SUCCESS);
        duration_acquire_chronicle += (t2 - t1);

        std::string key("Date");
        t1 = std::chrono::steady_clock::now();
        ret = client.EditChronicleAttr(chronicle_name, key, "2023-01-15");
        t2 = std::chrono::steady_clock::now();
        //assert(ret == CL_SUCCESS);
        duration_edit_chronicle_attr += (t2 - t1);

        std::vector<std::string> story_names;
        story_names.reserve(NUM_STORY);
        for (int j = 0; j < NUM_STORY; j++) {
            std::string story_name(gen_random(STORY_NAME_LEN));
            story_names.emplace_back(story_name);
            std::unordered_map<std::string, std::string> story_attrs;
            chronicle_attrs.emplace("Priority", "High");
            chronicle_attrs.emplace("IndexGranularity", "Millisecond");
            chronicle_attrs.emplace("TieringPolicy", "Hot");
            flags = 1;
            t1 = std::chrono::steady_clock::now();
            ret = client.CreateStory(chronicle_name, story_name, story_attrs, flags);
            t2 = std::chrono::steady_clock::now();
            assert(ret == CL_SUCCESS);
            duration_create_story += (t2 - t1);

            flags = 2;
            t1 = std::chrono::steady_clock::now();
            ret = client.AcquireStory(chronicle_name, story_name, flags);
            t2 = std::chrono::steady_clock::now();
            assert(ret == CL_SUCCESS);
            duration_acquire_story += (t2 - t1);

            flags = 4;
            t1 = std::chrono::steady_clock::now();
            ret = client.ReleaseStory(chronicle_name, story_name, flags);
            t2 = std::chrono::steady_clock::now();
            assert(ret == CL_SUCCESS);
            duration_release_story += (t2 - t1);
        }

        flags = 8;
        for (int j = 0; j < NUM_STORY; j++) {
            t1 = std::chrono::steady_clock::now();
            ret = client.DestroyStory(chronicle_name, story_names[j], flags);
            t2 = std::chrono::steady_clock::now();
            assert(ret == CL_SUCCESS);
            duration_destroy_story += (t2 - t1);
        }

        std::string value = "pls_ignore";
        t1 = std::chrono::steady_clock::now();
        ret = client.GetChronicleAttr(chronicle_name, key, value);
        t2 = std::chrono::steady_clock::now();
        //ASSERT(ret, ==, CL_SUCCESS);
        //FIXME: returning data using parameter is not working, the following assert will fail
        //ASSERT(value, ==, "2023-01-15");
        duration_get_chronicle_attr += (t2 - t1);

        flags = 16;
        t1 = std::chrono::steady_clock::now();
        ret = client.ReleaseChronicle(chronicle_name, flags);
        t2 = std::chrono::steady_clock::now();
        assert(ret == CL_SUCCESS);
        duration_release_chronicle += (t2 - t1);
    }

    flags = 32;
    for (int i = 0; i < NUM_CHRONICLE; i++) {
        t1 = std::chrono::steady_clock::now();
        bool ret = client.DestroyChronicle(chronicle_names[i], flags);
        t2 = std::chrono::steady_clock::now();
        assert(ret == CL_SUCCESS);
        duration_destroy_chronicle += (t2 - t1);
    };

    LOGI("CreateChronicle takes %lf ns", duration_create_chronicle.count() / NUM_CHRONICLE);
    LOGI("AcquireChronicle takes %lf ns", duration_acquire_chronicle.count() / NUM_CHRONICLE);
    LOGI("EditChronicleAttr takes %lf ns", duration_edit_chronicle_attr.count() / NUM_CHRONICLE);
    LOGI("CreateStory takes %lf ns", duration_create_story.count() / (NUM_CHRONICLE * NUM_STORY));
    LOGI("AcquireStory takes %lf ns", duration_acquire_story.count() / (NUM_CHRONICLE * NUM_STORY));
    LOGI("ReleaseStory takes %lf ns", duration_release_story.count() / (NUM_CHRONICLE * NUM_STORY));
    LOGI("DestroyStory takes %lf ns", duration_destroy_story.count() / (NUM_CHRONICLE * NUM_STORY));
    LOGI("GetChronileAttr(Date) takes %lf ns", duration_get_chronicle_attr.count() / NUM_CHRONICLE);
    LOGI("ReleaseChronicle takes %lf ns", duration_release_chronicle.count() / NUM_CHRONICLE);
    LOGI("DestroyChronicle takes %lf ns", duration_destroy_chronicle.count() / NUM_CHRONICLE);

    duration_create_chronicle = std::chrono::duration<double, std::nano>();
    chronicle_names.clear();
    flags = 1;
    for (int i = 0; i < NUM_CHRONICLE; i++) {
        std::string chronicle_name(gen_random(CHRONICLE_NAME_LEN));
        chronicle_names.emplace_back(chronicle_name);
        std::string attr = std::string("Priority=High");
        int ret;
        std::unordered_map<std::string, std::string> chronicle_attrs;
        chronicle_attrs.emplace("Priority", "High");
        chronicle_attrs.emplace("IndexGranularity", "Millisecond");
        chronicle_attrs.emplace("TieringPolicy", "Hot");
        t1 = std::chrono::steady_clock::now();
        ret = client.CreateChronicle(chronicle_name, chronicle_attrs, flags);
        t2 = std::chrono::steady_clock::now();
        assert(ret == CL_SUCCESS);
        duration_create_chronicle += (t2 - t1);
    }

    flags = 32;
    duration_destroy_chronicle = std::chrono::duration<double, std::nano>();
    for (int i = 0; i < NUM_CHRONICLE; i++) {
        t1 = std::chrono::steady_clock::now();
        int ret = client.DestroyChronicle(chronicle_names[i], flags);
        t2 = std::chrono::steady_clock::now();
        assert(ret == CL_SUCCESS);
        duration_destroy_chronicle += (t2 - t1);
    }

    LOGI("CreateChronicle2 takes %lf ns", duration_create_chronicle.count() / NUM_CHRONICLE);
    LOGI("DestroyChronicle2 takes %lf ns", duration_destroy_chronicle.count() / NUM_CHRONICLE);

    return 0;
}
