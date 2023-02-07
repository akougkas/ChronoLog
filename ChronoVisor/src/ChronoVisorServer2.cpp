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
// Created by kfeng on 7/19/22.
//

#include "ChronoVisorServer2.h"
#include "macro.h"

namespace ChronoVisor {
    ChronoVisorServer2::ChronoVisorServer2() {
        CHRONOLOG_CONF->ConfigureDefaultServer("./server_list");
        switch (CHRONOLOG_CONF->RPC_IMPLEMENTATION) {
            CHRONOLOG_RPC_CALL_WRAPPER_THALLIUM_SOCKETS()
            [[fallthrough]];
            CHRONOLOG_RPC_CALL_WRAPPER_THALLIUM_TCP() {
                protocol_ = CHRONOLOG_CONF->SOCKETS_CONF.string();
                break;
            }
            CHRONOLOG_RPC_CALL_WRAPPER_THALLIUM_ROCE() {
                protocol_ = CHRONOLOG_CONF->VERBS_CONF.string();
                break;
            }
        }
        baseIP_ = CHRONOLOG_CONF->RPC_SERVER_IP.string();
        basePorts_ = CHRONOLOG_CONF->RPC_BASE_SERVER_PORT;
        numPorts_ = CHRONOLOG_CONF->RPC_NUM_SERVER_PORTS;
        numStreams_ = CHRONOLOG_CONF->RPC_NUM_SERVICE_THREADS;

        serverAddrVec_.reserve(CHRONOLOG_CONF->RPC_NUM_SERVER_PORTS);
        for (int i = 0; i < numPorts_; i++) {
            std::string server_addr = protocol_ + "://" +
                    baseIP_ + ":" +
                    std::to_string(basePorts_ + i);
            serverAddrVec_.emplace_back(std::move(server_addr));
        }
        engineVec_.reserve(numPorts_);
        midVec_.reserve(numPorts_);
        pTimeManager = new TimeManager();
        chronicleMetaDirectory_ = ChronoLog::Singleton<ChronicleMetaDirectory>::GetInstance();
        rpcProxy_ = ChronoLog::Singleton<RPCVisor>::GetInstance();
        clientRegistryManager_ = ChronoLog::Singleton<ClientRegistryManager>::GetInstance();
    }

    int ChronoVisorServer2::start() {
        LOGI("ChronoVisor server starting, listen on %d ports starting from %d ...", numPorts_, basePorts_);

        // bind functions first (defining RPC routines on engines)
        rpcProxy_->bind_functions();

        // start engines (listening for incoming requests)
        ChronoLog::Singleton<ChronoLogRPCFactory>::GetInstance()->
                    GetRPC(CHRONOLOG_CONF->RPC_BASE_SERVER_PORT)->start();

        return 0;
    }
}
