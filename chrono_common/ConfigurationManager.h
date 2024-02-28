#ifndef CHRONOLOG_CONFIGURATIONMANAGER_H
#define CHRONOLOG_CONFIGURATIONMANAGER_H

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cassert>
#include <unordered_map>
#include <json-c/json.h>
#include <sstream>
#include <spdlog/common.h>
#include "enum.h"
#include "chronolog_errcode.h"

namespace ChronoLog
{
typedef struct ClockConf_
{
    ClocksourceType CLOCKSOURCE_TYPE;
    uint64_t DRIFT_CAL_SLEEP_SEC;
    uint64_t DRIFT_CAL_SLEEP_NSEC;

    [[nodiscard]] std::string to_String() const
    {
        return "CLOCKSOURCE_TYPE: " + std::string(getClocksourceTypeString(CLOCKSOURCE_TYPE)) +
               ", DRIFT_CAL_SLEEP_SEC: " + std::to_string(DRIFT_CAL_SLEEP_SEC) + ", DRIFT_CAL_SLEEP_NSEC: " +
               std::to_string(DRIFT_CAL_SLEEP_NSEC);
    }
} ClockConf;

typedef struct AuthConf_
{
    std::string AUTH_TYPE;
    std::string MODULE_PATH;

    [[nodiscard]] std::string to_String() const
    {
        return "AUTH_TYPE: " + AUTH_TYPE + ", MODULE_PATH: " + MODULE_PATH;
    }
} AuthConf;

typedef struct RPCProviderConf_
{
    ChronoLogRPCImplementation RPC_IMPLEMENTATION;
    std::string PROTO_CONF;
    std::string IP;
    uint16_t BASE_PORT;
    uint16_t SERVICE_PROVIDER_ID;

    [[nodiscard]] std::string to_String() const
    {
        return "[RPC_IMPLEMENTATION: " + std::string(getRPCImplString(RPC_IMPLEMENTATION)) + ", PROTO_CONF: " +
               PROTO_CONF + ", IP: " + IP + ", BASE_PORT: " + std::to_string(BASE_PORT) + ", SERVICE_PROVIDER_ID: " +
               std::to_string(SERVICE_PROVIDER_ID) + ", PORTS: " + "]";
    }
} RPCProviderConf;

typedef struct LogConf_
{
    std::string LOGTYPE;
    std::string LOGFILE;
    spdlog::level::level_enum LOGLEVEL;
    std::string LOGNAME;
    size_t LOGFILESIZE;
    size_t LOGFILENUM;

    // Helper function to convert spdlog::level::level_enum to string
    static std::string LogLevelToString(spdlog::level::level_enum level)
    {
        switch(level)
        {
            case spdlog::level::trace:
                return "TRACE";
            case spdlog::level::debug:
                return "DEBUG";
            case spdlog::level::info:
                return "INFO";
            case spdlog::level::warn:
                return "WARN";
            case spdlog::level::err:
                return "ERROR";
            case spdlog::level::critical:
                return "CRITICAL";
            case spdlog::level::off:
                return "OFF";
            default:
                return "UNKNOWN";
        }
    }

    [[nodiscard]] std::string to_String() const
    {
        return "[TYPE: " + LOGTYPE + ", FILE: " + LOGFILE + ", LEVEL: " + LogLevelToString(LOGLEVEL) + ", NAME: " +
               LOGNAME + ", LOGFILESIZE: " + std::to_string(LOGFILESIZE) + ", LOGFILENUM: " +
               std::to_string(LOGFILENUM) + "]";
    }
} LogConf;

typedef struct VisorClientPortalServiceConf_
{
    RPCProviderConf RPC_CONF;

    [[nodiscard]] std::string to_String() const
    {
        return "[RPC_CONF: " + RPC_CONF.to_String() + "]";
    }
} VisorClientPortalServiceConf;

typedef struct VisorKeeperRegistryServiceConf_
{
    RPCProviderConf RPC_CONF;

    [[nodiscard]] std::string to_String() const
    {
        return "[RPC_CONF: " + RPC_CONF.to_String() + "]";
    }
} VisorKeeperRegistryServiceConf;

typedef struct KeeperRecordingServiceConf_
{
    RPCProviderConf RPC_CONF;

    [[nodiscard]] std::string to_String() const
    {
        return "[RPC_CONF: " + RPC_CONF.to_String() + "]";
    }
} KeeperRecordingServiceConf;

typedef struct KeeperDataStoreAdminServiceConf_
{
    RPCProviderConf RPC_CONF;

    [[nodiscard]] std::string to_String() const
    {
        return "[RPC_CONF: " + RPC_CONF.to_String() + "]";
    }
} KeeperDataStoreAdminServiceConf;

typedef struct VisorConf_
{
    VisorClientPortalServiceConf VISOR_CLIENT_PORTAL_SERVICE_CONF;
    VisorKeeperRegistryServiceConf VISOR_KEEPER_REGISTRY_SERVICE_CONF;
    LogConf VISOR_LOG_CONF;
    size_t DELAYED_DATA_ADMIN_EXIT_IN_SECS;

    [[nodiscard]] std::string to_String() const
    {
        return "[VISOR_CLIENT_PORTAL_SERVICE_CONF: " + VISOR_CLIENT_PORTAL_SERVICE_CONF.to_String() +
               ", VISOR_KEEPER_REGISTRY_SERVICE_CONF: " + VISOR_KEEPER_REGISTRY_SERVICE_CONF.to_String() +
               ", VISOR_LOG: " + VISOR_LOG_CONF.to_String() + 
               ", DELAYED_DATA_ADMIN_EXIT_IN_SECS: "+ std::to_string(DELAYED_DATA_ADMIN_EXIT_IN_SECS) + "]";
    }
} VisorConf;

typedef struct KeeperConf_
{
    KeeperRecordingServiceConf KEEPER_RECORDING_SERVICE_CONF;
    KeeperDataStoreAdminServiceConf KEEPER_DATA_STORE_ADMIN_SERVICE_CONF;
    VisorKeeperRegistryServiceConf VISOR_KEEPER_REGISTRY_SERVICE_CONF;
    std::string STORY_FILES_DIR;
    LogConf KEEPER_LOG_CONF;

    [[nodiscard]] std::string to_String() const
    {
        return "[KEEPER_RECORDING_SERVICE_CONF: " + KEEPER_RECORDING_SERVICE_CONF.to_String() +
               ", KEEPER_DATA_STORE_ADMIN_SERVICE_CONF: " + KEEPER_DATA_STORE_ADMIN_SERVICE_CONF.to_String() +
               ", VISOR_KEEPER_REGISTRY_SERVICE_CONF: " + VISOR_KEEPER_REGISTRY_SERVICE_CONF.to_String() +
               ", STORY_FILES_DIR:" + STORY_FILES_DIR + ", KEEPER_LOG_CONF:" + KEEPER_LOG_CONF.to_String() + "]";
    }
} KeeperConf;

typedef struct ClientConf_
{
    VisorClientPortalServiceConf VISOR_CLIENT_PORTAL_SERVICE_CONF;
    LogConf CLIENT_LOG_CONF;

    [[nodiscard]] std::string to_String() const
    {
        return "[VISOR_CLIENT_PORTAL_SERVICE_CONF: " + VISOR_CLIENT_PORTAL_SERVICE_CONF.to_String() +
               ", CLIENT_LOG_CONF:" + CLIENT_LOG_CONF.to_String() + "]";
    }
} ClientConf;

class ConfigurationManager
{
public:
    ChronoLogServiceRole ROLE{};
    ClockConf CLOCK_CONF{};
    ClocksourceType CLOCKSOURCE_TYPE{};
    AuthConf AUTH_CONF{};
    VisorConf VISOR_CONF{};
    ClientConf CLIENT_CONF{};
    KeeperConf KEEPER_CONF{};

    ConfigurationManager()
    {
        std::cout << "[ConfigurationManager] Initializing configuration with default settings." << std::endl;
        ROLE = CHRONOLOG_UNKNOWN;

        /* Clock-related configurations */
        CLOCK_CONF.CLOCKSOURCE_TYPE = ClocksourceType::C_STYLE;
        CLOCK_CONF.DRIFT_CAL_SLEEP_SEC = 10;
        CLOCK_CONF.DRIFT_CAL_SLEEP_NSEC = 0;
        CLOCKSOURCE_TYPE = ClocksourceType::C_STYLE;

        /* Authentication-related configurations */
        AUTH_CONF.AUTH_TYPE = "RBAC";
        AUTH_CONF.MODULE_PATH = "";

        /* Visor-related configurations */
        VISOR_CONF.VISOR_CLIENT_PORTAL_SERVICE_CONF.RPC_CONF.RPC_IMPLEMENTATION = CHRONOLOG_THALLIUM_SOCKETS;
        VISOR_CONF.VISOR_CLIENT_PORTAL_SERVICE_CONF.RPC_CONF.PROTO_CONF = "ofi+sockets";
        VISOR_CONF.VISOR_CLIENT_PORTAL_SERVICE_CONF.RPC_CONF.IP = "127.0.0.1";
        VISOR_CONF.VISOR_CLIENT_PORTAL_SERVICE_CONF.RPC_CONF.BASE_PORT = 5555;
        VISOR_CONF.VISOR_CLIENT_PORTAL_SERVICE_CONF.RPC_CONF.SERVICE_PROVIDER_ID = 55;

        VISOR_CONF.VISOR_KEEPER_REGISTRY_SERVICE_CONF.RPC_CONF.RPC_IMPLEMENTATION = CHRONOLOG_THALLIUM_SOCKETS;
        VISOR_CONF.VISOR_KEEPER_REGISTRY_SERVICE_CONF.RPC_CONF.PROTO_CONF = "ofi+sockets";
        VISOR_CONF.VISOR_KEEPER_REGISTRY_SERVICE_CONF.RPC_CONF.IP = "127.0.0.1";
        VISOR_CONF.VISOR_KEEPER_REGISTRY_SERVICE_CONF.RPC_CONF.BASE_PORT = 8888;
        VISOR_CONF.VISOR_KEEPER_REGISTRY_SERVICE_CONF.RPC_CONF.SERVICE_PROVIDER_ID = 88;

        VISOR_CONF.DELAYED_DATA_ADMIN_EXIT_IN_SECS = 3;

        /* Keeper-related configurations */
        KEEPER_CONF.KEEPER_RECORDING_SERVICE_CONF.RPC_CONF.RPC_IMPLEMENTATION = CHRONOLOG_THALLIUM_SOCKETS;
        KEEPER_CONF.KEEPER_RECORDING_SERVICE_CONF.RPC_CONF.PROTO_CONF = "ofi+sockets";
        KEEPER_CONF.KEEPER_RECORDING_SERVICE_CONF.RPC_CONF.IP = "127.0.0.1";
        KEEPER_CONF.KEEPER_RECORDING_SERVICE_CONF.RPC_CONF.BASE_PORT = 6666;
        KEEPER_CONF.KEEPER_RECORDING_SERVICE_CONF.RPC_CONF.SERVICE_PROVIDER_ID = 66;

        KEEPER_CONF.KEEPER_DATA_STORE_ADMIN_SERVICE_CONF.RPC_CONF.RPC_IMPLEMENTATION = CHRONOLOG_THALLIUM_SOCKETS;
        KEEPER_CONF.KEEPER_DATA_STORE_ADMIN_SERVICE_CONF.RPC_CONF.PROTO_CONF = "ofi+sockets";
        KEEPER_CONF.KEEPER_DATA_STORE_ADMIN_SERVICE_CONF.RPC_CONF.IP = "127.0.0.1";
        KEEPER_CONF.KEEPER_DATA_STORE_ADMIN_SERVICE_CONF.RPC_CONF.BASE_PORT = 7777;
        KEEPER_CONF.KEEPER_DATA_STORE_ADMIN_SERVICE_CONF.RPC_CONF.SERVICE_PROVIDER_ID = 77;

        KEEPER_CONF.VISOR_KEEPER_REGISTRY_SERVICE_CONF.RPC_CONF.RPC_IMPLEMENTATION = CHRONOLOG_THALLIUM_SOCKETS;
        KEEPER_CONF.VISOR_KEEPER_REGISTRY_SERVICE_CONF.RPC_CONF.PROTO_CONF = "ofi+sockets";
        KEEPER_CONF.VISOR_KEEPER_REGISTRY_SERVICE_CONF.RPC_CONF.IP = "127.0.0.1";
        KEEPER_CONF.VISOR_KEEPER_REGISTRY_SERVICE_CONF.RPC_CONF.BASE_PORT = 8888;
        KEEPER_CONF.VISOR_KEEPER_REGISTRY_SERVICE_CONF.RPC_CONF.SERVICE_PROVIDER_ID = 88;

        KEEPER_CONF.STORY_FILES_DIR = "/tmp/";

        /* Client-related configurations */
        CLIENT_CONF.VISOR_CLIENT_PORTAL_SERVICE_CONF.RPC_CONF.RPC_IMPLEMENTATION = CHRONOLOG_THALLIUM_SOCKETS;
        CLIENT_CONF.VISOR_CLIENT_PORTAL_SERVICE_CONF.RPC_CONF.PROTO_CONF = "ofi+sockets";
        CLIENT_CONF.VISOR_CLIENT_PORTAL_SERVICE_CONF.RPC_CONF.BASE_PORT = 5555;
        CLIENT_CONF.VISOR_CLIENT_PORTAL_SERVICE_CONF.RPC_CONF.SERVICE_PROVIDER_ID = 55;

        PrintConf();
    }

    explicit ConfigurationManager(const std::string &conf_file_path)
    {
        std::cout << "[ConfigurationManager] Loading configuration from file: " << conf_file_path.c_str() << std::endl;
        LoadConfFromJSONFile(conf_file_path);
    }

    void PrintConf() const
    {
        std::cout << "******** Start of configuration output ********" << std::endl;
        std::cout << "CLOCK_CONF: " << CLOCK_CONF.to_String().c_str() << std::endl;
        std::cout << "AUTH_CONF: " << AUTH_CONF.to_String().c_str() << std::endl;
        std::cout << "VISOR_CONF: " << VISOR_CONF.to_String().c_str() << std::endl;
        std::cout << "KEEPER_CONF: " << KEEPER_CONF.to_String().c_str() << std::endl;
        std::cout << "CLIENT_CONF: " << CLIENT_CONF.to_String().c_str() << std::endl;
        std::cout << "******** End of configuration output ********" << std::endl;
    }

    void LoadConfFromJSONFile(const std::string &conf_file_path)
    {
        json_object*root = json_object_from_file(conf_file_path.c_str());
        if(root == nullptr)
        {
            std::cerr << "[ConfigurationManager] Failed to open configuration file at path: " << conf_file_path.c_str()
                      << ". Exiting..." << std::endl;
            exit(chronolog::CL_ERR_NOT_EXIST);
        }

        json_object_object_foreach(root, key, val)
        {
            if(strcmp(key, "clock") == 0)
            {
                json_object*clock_conf = json_object_object_get(root, "clock");
                if(clock_conf == nullptr || !json_object_is_type(clock_conf, json_type_object))
                {
                    std::cerr << "[ConfigurationManager] Error while parsing configuration file "
                              << conf_file_path.c_str() << ". Clock configuration is not found or is not an object."
                              << std::endl;
                    exit(chronolog::CL_ERR_INVALID_CONF);
                }
                parseClockConf(clock_conf);
            }
            else if(strcmp(key, "authentication") == 0)
            {
                json_object*auth_conf = json_object_object_get(root, "authentication");
                if(auth_conf == nullptr || !json_object_is_type(auth_conf, json_type_object))
                {
                    std::cerr << "[ConfigurationManager] Error while parsing configuration file "
                              << conf_file_path.c_str()
                              << ". Authentication configuration is not found or is not an object." << std::endl;
                    exit(chronolog::CL_ERR_INVALID_CONF);
                }
                parseAuthConf(auth_conf);
            }
            else if(strcmp(key, "chrono_visor") == 0)
            {
                json_object*chrono_visor_conf = json_object_object_get(root, "chrono_visor");
                if(chrono_visor_conf == nullptr || !json_object_is_type(chrono_visor_conf, json_type_object))
                {
                    std::cerr << "[ConfigurationManager] Error while parsing configuration file "
                              << conf_file_path.c_str()
                              << ". ChronoVisor configuration is not found or is not an object." << std::endl;
                    exit(chronolog::CL_ERR_INVALID_CONF);
                }
                parseVisorConf(chrono_visor_conf);
            }
            else if(strcmp(key, "chrono_keeper") == 0)
            {
                json_object*chrono_keeper_conf = json_object_object_get(root, "chrono_keeper");
                if(chrono_keeper_conf == nullptr || !json_object_is_type(chrono_keeper_conf, json_type_object))
                {
                    std::cerr << "[ConfigurationManager] Error while parsing configuration file "
                              << conf_file_path.c_str()
                              << ". ChronoKeeper configuration is not found or is not an object." << std::endl;
                    exit(chronolog::CL_ERR_INVALID_CONF);
                }
                parseKeeperConf(chrono_keeper_conf);
            }
            else if(strcmp(key, "chrono_client") == 0)
            {
                json_object*chrono_client_conf = json_object_object_get(root, "chrono_client");
                if(chrono_client_conf == nullptr || !json_object_is_type(chrono_client_conf, json_type_object))
                {
                    std::cerr << "[ConfigurationManager] Error while parsing configuration file "
                              << conf_file_path.c_str()
                              << ". ChronoClient configuration is not found or is not an object." << std::endl;
                    exit(chronolog::CL_ERR_INVALID_CONF);
                }
                parseClientConf(chrono_client_conf);
            }
            else
            {
                std::cerr << "[ConfigurationManager] Unknown configuration item: " << key << std::endl;
            }
        }
        json_object_put(root);
        PrintConf();
    }

private:
    void parseRPCImplConf(json_object*json_conf, ChronoLogRPCImplementation &rpc_impl)
    {
        if(json_object_is_type(json_conf, json_type_string))
        {
            const char*conf_str = json_object_get_string(json_conf);
            if(strcmp(conf_str, "Thallium_sockets") == 0)
            {
                rpc_impl = CHRONOLOG_THALLIUM_SOCKETS;
            }
            else if(strcmp(conf_str, "Thallium_tcp") == 0)
            {
                rpc_impl = CHRONOLOG_THALLIUM_TCP;
            }
            else if(strcmp(conf_str, "Thallium_roce") == 0)
            {
                rpc_impl = CHRONOLOG_THALLIUM_ROCE;
            }
            else
            {
                std::cout << "[ConfigurationManager] Unknown rpc implementation: " << conf_str << std::endl;
            }
        }
        else
        {
            std::cerr << "[ConfigurationManager] Invalid rpc implementation configuration" << std::endl;
        }
    }

    void parselogLevelConf(json_object*json_conf, spdlog::level::level_enum &log_level)
    {
        if(json_object_is_type(json_conf, json_type_string))
        {
            const char*conf_str = json_object_get_string(json_conf);
            if(strcmp(conf_str, "trace") == 0)
            {
                log_level = spdlog::level::trace;
            }
            else if(strcmp(conf_str, "info") == 0)
            {
                log_level = spdlog::level::info;
            }
            else if(strcmp(conf_str, "debug") == 0)
            {
                log_level = spdlog::level::debug;
            }
            else if(strcmp(conf_str, "warning") == 0)
            {
                log_level = spdlog::level::warn;
            }
            else if(strcmp(conf_str, "error") == 0)
            {
                log_level = spdlog::level::err;
            }
            else if(strcmp(conf_str, "critical") == 0)
            {
                log_level = spdlog::level::critical;
            }
            else if(strcmp(conf_str, "off") == 0)
            {
                log_level = spdlog::level::off;
            }
            else
            {
                std::cout << "[ConfigurationManager] Unknown log level: " << conf_str << std::endl;
            }
        }
        else
        {
            std::cerr << "[ConfigurationManager] Invalid rpc implementation configuration" << std::endl;
        }
    }

    void parseClockConf(json_object*clock_conf)
    {
        json_object_object_foreach(clock_conf, key, val)
        {
            if(strcmp(key, "clocksource_type") == 0)
            {
                if(json_object_is_type(val, json_type_string))
                {
                    const char*clocksource_type = json_object_get_string(val);
                    if(strcmp(clocksource_type, "C_STYLE") == 0)
                        CLOCK_CONF.CLOCKSOURCE_TYPE = ClocksourceType::C_STYLE;
                    else if(strcmp(clocksource_type, "CPP_STYLE") == 0)
                        CLOCK_CONF.CLOCKSOURCE_TYPE = ClocksourceType::CPP_STYLE;
                    else if(strcmp(clocksource_type, "TSC") == 0)
                        CLOCK_CONF.CLOCKSOURCE_TYPE = ClocksourceType::TSC;
                    else
                        std::cout << "[ConfigurationManager] Unknown clocksource type: " << clocksource_type
                                  << std::endl;
                }
                else
                {
                    std::cerr
                            << "[ConfigurationManager] Failed to parse configuration file: clocksource_type is not a string"
                            << std::endl;
                    exit(chronolog::CL_ERR_INVALID_CONF);
                }
            }
            else if(strcmp(key, "drift_cal_sleep_sec") == 0)
            {
                if(json_object_is_type(val, json_type_int))
                {
                    CLOCK_CONF.DRIFT_CAL_SLEEP_SEC = json_object_get_int(val);
                }
                else
                {
                    std::cerr
                            << "[ConfigurationManager] Failed to parse configuration file: drift_cal_sleep_sec is not an integer"
                            << std::endl;
                    exit(chronolog::CL_ERR_INVALID_CONF);
                }
            }
            else if(strcmp(key, "drift_cal_sleep_nsec") == 0)
            {
                if(json_object_is_type(val, json_type_int))
                {
                    CLOCK_CONF.DRIFT_CAL_SLEEP_NSEC = json_object_get_int(val);
                }
                else
                {
                    std::cerr
                            << "[ConfigurationManager] Failed to parse configuration file: drift_cal_sleep_nsec is not an integer"
                            << std::endl;
                    exit(chronolog::CL_ERR_INVALID_CONF);
                }
            }
        }
    }

    void parseAuthConf(json_object*auth_conf)
    {
        if(auth_conf == nullptr || !json_object_is_type(auth_conf, json_type_object))
        {
            std::cerr
                    << "[ConfigurationManager] Error while parsing configuration file. Authentication configuration is not found or is not an object."
                    << std::endl;
            exit(chronolog::CL_ERR_INVALID_CONF);
        }
        json_object_object_foreach(auth_conf, key, val)
        {
            if(strcmp(key, "auth_type") == 0)
            {
                if(json_object_is_type(val, json_type_string))
                {
                    AUTH_CONF.AUTH_TYPE = json_object_get_string(val);
                }
                else
                {
                    std::cerr << "[ConfigurationManager] Failed to parse configuration file: auth_type is not a string"
                              << std::endl;
                    exit(chronolog::CL_ERR_INVALID_CONF);
                }
            }
            else if(strcmp(key, "module_location") == 0)
            {
                if(json_object_is_type(val, json_type_string))
                {
                    AUTH_CONF.MODULE_PATH = json_object_get_string(val);
                }
                else
                {
                    std::cerr
                            << "[ConfigurationManager] Failed to parse configuration file: module_location is not a string"
                            << std::endl;
                    exit(chronolog::CL_ERR_INVALID_CONF);
                }
            }
        }
    }

    void parseRPCProviderConf(json_object*json_conf, RPCProviderConf &rpc_provider_conf)
    {
        json_object_object_foreach(json_conf, key, val)
        {
            if(strcmp(key, "rpc_implementation") == 0)
            {
                assert(json_object_is_type(val, json_type_string));
                parseRPCImplConf(val, rpc_provider_conf.RPC_IMPLEMENTATION);
            }
            else if(strcmp(key, "protocol_conf") == 0)
            {
                assert(json_object_is_type(val, json_type_string));
                rpc_provider_conf.PROTO_CONF = json_object_get_string(val);
            }
            else if(strcmp(key, "service_ip") == 0)
            {
                assert(json_object_is_type(val, json_type_string));
                rpc_provider_conf.IP = json_object_get_string(val);
            }
            else if(strcmp(key, "service_base_port") == 0)
            {
                assert(json_object_is_type(val, json_type_int));
                rpc_provider_conf.BASE_PORT = json_object_get_int(val);
            }
            else if(strcmp(key, "service_provider_id") == 0)
            {
                assert(json_object_is_type(val, json_type_int));
                rpc_provider_conf.SERVICE_PROVIDER_ID = json_object_get_int(val);
            }
            else
            {
                std::cerr << "[ConfigurationManager] Unknown client end configuration: " << key << std::endl;
            }
        }
    }

    void parseLogConf(json_object*json_conf, LogConf &log_conf)
    {
        json_object_object_foreach(json_conf, key, val)
        {
            if(strcmp(key, "type") == 0)
            {
                assert(json_object_is_type(val, json_type_string));
                log_conf.LOGTYPE = json_object_get_string(val);
            }
            else if(strcmp(key, "file") == 0)
            {
                assert(json_object_is_type(val, json_type_string));
                log_conf.LOGFILE = json_object_get_string(val);
            }
            else if(strcmp(key, "level") == 0)
            {
                assert(json_object_is_type(val, json_type_string));
                parselogLevelConf(val, log_conf.LOGLEVEL);
            }
            else if(strcmp(key, "name") == 0)
            {
                assert(json_object_is_type(val, json_type_string));
                log_conf.LOGNAME = json_object_get_string(val);
            }
            else if(strcmp(key, "filesize") == 0)
            {
                assert(json_object_is_type(val, json_type_int));
                log_conf.LOGFILESIZE = json_object_get_int(val);
            }
            else if(strcmp(key, "filenum") == 0)
            {
                assert(json_object_is_type(val, json_type_int));
                log_conf.LOGFILENUM = json_object_get_int(val);
            }
            else
            {
                std::cerr << "[ConfigurationManager] Unknown log configuration: " << key << std::endl;
            }
        }
    }

    void parseVisorConf(json_object*json_conf)
    {
        json_object_object_foreach(json_conf, key, val)
        {
            if(strcmp(key, "VisorClientPortalService") == 0)
            {
                assert(json_object_is_type(val, json_type_object));
                json_object*visor_client_portal_service_conf = json_object_object_get(json_conf
                                                                                      , "VisorClientPortalService");
                json_object_object_foreach(visor_client_portal_service_conf, key, val)
                {
                    if(strcmp(key, "rpc") == 0)
                    {
                        parseRPCProviderConf(val, VISOR_CONF.VISOR_CLIENT_PORTAL_SERVICE_CONF.RPC_CONF);
                    }
                    else
                    {
                        std::cerr << "[ConfigurationManager] Unknown VisorClientPortalService configuration: " << key
                                  << std::endl;
                    }
                }
            }
            else if(strcmp(key, "VisorKeeperRegistryService") == 0)
            {
                assert(json_object_is_type(val, json_type_object));
                json_object*visor_keeper_registry_service_conf = json_object_object_get(json_conf
                                                                                        , "VisorKeeperRegistryService");
                json_object_object_foreach(visor_keeper_registry_service_conf, key, val)
                {
                    if(strcmp(key, "rpc") == 0)
                    {
                        parseRPCProviderConf(val, VISOR_CONF.VISOR_KEEPER_REGISTRY_SERVICE_CONF.RPC_CONF);
                    }
                    else
                    {
                        std::cerr << "[ConfigurationManager] Unknown VisorKeeperRegistryService configuration: " << key
                                  << std::endl;
                    }
                }
            }
            else if(strcmp(key, "Logging") == 0)
            {
                assert(json_object_is_type(val, json_type_object));
                json_object*chronovisor_log = json_object_object_get(json_conf, "Logging");
                json_object_object_foreach(chronovisor_log, key, val)
                {
                    if(strcmp(key, "log") == 0)
                    {
                        parseLogConf(val, VISOR_CONF.VISOR_LOG_CONF);
                    }
                    else
                    {
                        std::cerr << "[ConfigurationManager] Unknown VisorLog configuration: " << key << std::endl;
                    }
                }
            }
            else if(strcmp(key, "delayed_data_admin_exit_in_secs") == 0)
            {
                assert(json_object_is_type(val, json_type_int));
                VISOR_CONF.DELAYED_DATA_ADMIN_EXIT_IN_SECS=json_object_get_int(val);
            }
            else
            {
                std::cerr << "[ConfigurationManager] Unknown VisorLog configuration: " << key << std::endl;
            }
        }
    }

    void parseKeeperConf(json_object*json_conf)
    {
        json_object_object_foreach(json_conf, key, val)
        {
            if(strcmp(key, "KeeperRecordingService") == 0)
            {
                assert(json_object_is_type(val, json_type_object));
                json_object*keeper_recording_service_conf = json_object_object_get(json_conf, "KeeperRecordingService");
                json_object_object_foreach(keeper_recording_service_conf, key, val)
                {
                    if(strcmp(key, "rpc") == 0)
                    {
                        parseRPCProviderConf(val, KEEPER_CONF.KEEPER_RECORDING_SERVICE_CONF.RPC_CONF);
                    }
                    else
                    {
                        std::cerr << "[ConfigurationManager] Unknown KeeperRecordingService configuration: " << key
                                  << std::endl;
                    }
                }
            }
            else if(strcmp(key, "KeeperDataStoreAdminService") == 0)
            {
                assert(json_object_is_type(val, json_type_object));
                json_object*keeper_data_store_admin_service_conf = json_object_object_get(json_conf
                                                                                          , "KeeperDataStoreAdminService");
                json_object_object_foreach(keeper_data_store_admin_service_conf, key, val)
                {
                    if(strcmp(key, "rpc") == 0)
                    {
                        parseRPCProviderConf(val, KEEPER_CONF.KEEPER_DATA_STORE_ADMIN_SERVICE_CONF.RPC_CONF);
                    }
                    else
                    {
                        std::cerr << "[ConfigurationManager] Unknown KeeperDataStoreAdminService configuration: " << key
                                  << std::endl;
                    }
                }
            }
            else if(strcmp(key, "VisorKeeperRegistryService") == 0)
            {
                assert(json_object_is_type(val, json_type_object));
                json_object*visor_keeper_registry_service_conf = json_object_object_get(json_conf
                                                                                        , "VisorKeeperRegistryService");
                json_object_object_foreach(visor_keeper_registry_service_conf, key, val)
                {
                    if(strcmp(key, "rpc") == 0)
                    {
                        parseRPCProviderConf(val, KEEPER_CONF.VISOR_KEEPER_REGISTRY_SERVICE_CONF.RPC_CONF);
                    }
                    else
                    {
                        std::cerr << "[ConfigurationManager] Unknown VisorKeeperRegistryService configuration: " << key
                                  << std::endl;
                    }
                }
            }
            else if(strcmp(key, "Logging") == 0)
            {
                assert(json_object_is_type(val, json_type_object));
                json_object*chronokeeper_log = json_object_object_get(json_conf, "Logging");
                json_object_object_foreach(chronokeeper_log, key, val)
                {
                    if(strcmp(key, "log") == 0)
                    {
                        parseLogConf(val, KEEPER_CONF.KEEPER_LOG_CONF);
                    }
                    else
                    {
                        std::cerr << "[ConfigurationManager] Unknown KeeperLog configuration: " << key << std::endl;
                    }
                }
            }
            else if(strcmp(key, "story_files_dir") == 0)
            {
                assert(json_object_is_type(val, json_type_string));
                KEEPER_CONF.STORY_FILES_DIR = json_object_get_string(val);
            }
            else
            {
                std::cerr << "[ConfigurationManager] Unknown Keeper configuration: " << key << std::endl;
            }
        }
    }

    void parseClientConf(json_object*json_conf)
    {
        const char*string_value = json_object_get_string(json_conf);
        json_object_object_foreach(json_conf, key, val)
        {
            if(strcmp(key, "VisorClientPortalService") == 0)
            {
                assert(json_object_is_type(val, json_type_object));
                json_object*visor_client_portal_service_conf = json_object_object_get(json_conf
                                                                                      , "VisorClientPortalService");
                json_object_object_foreach(visor_client_portal_service_conf, key, val)
                {
                    if(strcmp(key, "rpc") == 0)
                    {
                        parseRPCProviderConf(val, CLIENT_CONF.VISOR_CLIENT_PORTAL_SERVICE_CONF.RPC_CONF);
                    }
                    else
                    {
                        std::cerr << "[ConfigurationManager] Unknown VisorClientPortalService configuration: " << key
                                  << std::endl;
                    }
                }
            }
            else if(strcmp(key, "Logging") == 0)
            {
                assert(json_object_is_type(val, json_type_object));
                json_object*chronoclient_log = json_object_object_get(json_conf, "Logging");
                json_object_object_foreach(chronoclient_log, key, val)
                {
                    if(strcmp(key, "log") == 0)
                    {
                        parseLogConf(val, CLIENT_CONF.CLIENT_LOG_CONF);
                    }
                    else
                    {
                        std::cerr << "[ConfigurationManager] Unknown ClientLog configuration: " << key << std::endl;
                    }
                }
            }
            else
            {
                std::cerr << "[ConfigurationManager] Unknown ClientLog configuration: " << key << std::endl;
            }
        }
    }
};
}

#endif //CHRONOLOG_CONFIGURATIONMANAGER_H
