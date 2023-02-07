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
// Created by kfeng on 3/30/22.
//

#ifndef CHRONOLOG_CONFIGURATIONMANAGER_H
#define CHRONOLOG_CONFIGURATIONMANAGER_H

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
//#include <filesystem>
#include "data_structures.h"
#include "enum.h"
#include "log.h"

namespace ChronoLog {
    class ConfigurationManager {
    public:
        ChronoLogCharStruct RPC_SERVER_IP;
        uint16_t RPC_BASE_SERVER_PORT;
        uint16_t RPC_NUM_SERVER_PORTS;
        uint16_t RPC_NUM_SERVICE_THREADS;
        uint16_t RPC_CLIENT_PORT;
        ChronoLogRPCImplementation RPC_IMPLEMENTATION;
        ChronoLogCharStruct SOCKETS_CONF;
        ChronoLogCharStruct VERBS_CONF;
        ChronoLogCharStruct VERBS_DOMAIN;
        uint64_t MEMORY_ALLOCATED;

        bool IS_SERVER;
        int MY_SERVER_ID;
        ChronoLogCharStruct SERVER_LIST_FILE_PATH;
        std::vector<ChronoLogCharStruct> SERVER_LIST;
        ChronoLogCharStruct BACKED_FILE_DIR;

        ConfigurationManager() :
                RPC_SERVER_IP("127.0.0.1"),
                RPC_BASE_SERVER_PORT(5555),
                RPC_NUM_SERVER_PORTS(1),
                RPC_NUM_SERVICE_THREADS(1),
                RPC_CLIENT_PORT(6666),
                RPC_IMPLEMENTATION(CHRONOLOG_THALLIUM_SOCKETS),
                SOCKETS_CONF("ofi+sockets"), VERBS_CONF("verbs"), VERBS_DOMAIN("mlx5_0"),
                MEMORY_ALLOCATED(1024ULL * 1024ULL * 128ULL),
                IS_SERVER(true), MY_SERVER_ID(0), SERVER_LIST_FILE_PATH("./server_list"),
                SERVER_LIST(), BACKED_FILE_DIR("/dev/shm") {
            LOGI("initializing configuration with all default values: ");
            LOGI("RPC_SERVER_IP: %s", RPC_SERVER_IP.c_str());
            LOGI("RPC_BASE_PORT: %d", RPC_BASE_SERVER_PORT);
            LOGI("RPC_NUM_PORTS: %d", RPC_NUM_SERVER_PORTS);
            LOGI("RPC_NUM_SERVICE_THREADS: %d", RPC_NUM_SERVICE_THREADS);
            LOGI("RPC_CLIENT_PORT: %d", RPC_CLIENT_PORT);
            LOGI("RPC_IMPLEMENTATION (0 for sockets, 1 for TCP, 2 for verbs): %d", RPC_IMPLEMENTATION);
            LOGI("SOCKETS_CONF: %s", SOCKETS_CONF.c_str());
            LOGI("VERBS_CONF: %s", VERBS_CONF.c_str());
            LOGI("VERBS_DOMAIN: %s", VERBS_DOMAIN.c_str());
            LOGI("MEMORY_ALLOCATED: %lu", MEMORY_ALLOCATED);
            LOGI("IS_SERVER: %d", IS_SERVER);
            LOGI("MY_SERVER_ID: %d", MY_SERVER_ID);
            LOGI("SERVER_LIST_FILE_PATH: %s", realpath(SERVER_LIST_FILE_PATH.c_str(), NULL));
            LOGI("SERVER_LIST: ");
            LOGI("BACKED_FILE_DIR: %s", BACKED_FILE_DIR.c_str());
        }

        std::vector<ChronoLogCharStruct> LoadServers() {
            SERVER_LIST = std::vector<ChronoLogCharStruct>();
            std::fstream file;
            file.open(SERVER_LIST_FILE_PATH.c_str(), std::ios::in);
            if (file.is_open()) {
                std::string file_line;
                std::string server_node_name;
                while (getline(file, file_line)) {
                    // TODO: check if hostname is valid
                    auto colon_pos = file_line.find(':');
                    if (colon_pos == std::string::npos) {
                        // no port specified
                        SERVER_LIST.emplace_back(file_line);
                    } else {
                        // it has something after :, ignore things after :, custom port is not supported
                        // TODO: support different ports for different servers
                        SERVER_LIST.emplace_back(file_line.substr(0, colon_pos));
                    }

                }
                /*
                 * configuration file reading from HCL which uses MPI
                 * the serverList_ file follows the format of a hostfile for MPI
                 * each line contains a hostname and a number of processes for each node, separated by ':'
                 * e.g. ares-comp-05:4, where ares-comp-05 is the hostname of the server
                 * 4 is the number of processes on that node
                 */
                /*
                int count;
                while (getline(file, file_line)) {
                    unsigned long split_loc = file_line.find(':');  // split to node and count
                    if (split_loc != std::string::npos) {
                        server_node_name = file_line.substr(0, split_loc);
                        count = atoi(file_line.substr(split_loc + 1, std::string::npos).c_str());
                    } else {
                        // no special count
                        server_node_name = file_line;
                        count = 1;
                    }
                    // server list is list of network interfaces
                    for (int i = 0; i < count; ++i) {
                        SERVER_LIST.emplace_back(server_node_name);
                    }
                }
                */
            } else {
                LOGE("Error: Can't open server list file %s\n", SERVER_LIST_FILE_PATH.c_str());
                exit(EXIT_FAILURE);
            }
            file.close();
            return SERVER_LIST;
        }

        void ConfigureDefaultClient(const std::string &server_list_path = "") {
            if (!server_list_path.empty()) SERVER_LIST_FILE_PATH = server_list_path;
            LoadServers();
            IS_SERVER = false;
            LOGD("default configuration for client is loaded, changed configuration:");
            LOGD("IS_SERVER: %d", IS_SERVER);
            for (const auto& server : SERVER_LIST)
                LOGD("server: %s", server.c_str());
        }

        void ConfigureDefaultServer(const std::string &server_list_path = "") {
            if (!server_list_path.empty()) SERVER_LIST_FILE_PATH = server_list_path;
            LoadServers();
            IS_SERVER = true;
            LOGD("default configuration for server is loaded, changed configuration:");
            LOGD("IS_SERVER: %d", IS_SERVER);
            for (const auto& server : SERVER_LIST)
                LOGD("server: %s", server.c_str());
        }
    };

}

#endif //CHRONOLOG_CONFIGURATIONMANAGER_H
