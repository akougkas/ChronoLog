//
// Created by kfeng on 7/11/22.
//

#ifndef CHRONOLOG_CLIENTREGISTRYMANAGER_H
#define CHRONOLOG_CLIENTREGISTRYMANAGER_H

#include <unordered_map>
#include <vector>
#include <memory>
#include <ClientRegistryInfo.h>

class ClientRegistryManager {
public:
    ClientRegistryManager();
    ~ClientRegistryManager();

    bool add_client_record(const std::string &client_id, const ClientRegistryInfo &record);
    bool remove_client_record(const std::string& client_id, int &flags);
    bool is_client(const std::string &client_id);
    int get_client_role(const std::string &client_id);
private:
    std::unordered_map<std::string, ClientRegistryInfo> *clientRegistry_;
};

#endif //CHRONOLOG_CLIENTREGISTRYMANAGER_H
