/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2020 RDK Management
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#include "SystemServices.h"

#include <fstream>
#include <iostream>
//#include <optional>
#include <cstdlib>
#include <utility>
#include <sstream>
#include <vector>

#define API_VERSION_NUMBER_MAJOR 1
#define API_VERSION_NUMBER_MINOR 1
#define API_VERSION_NUMBER_PATCH 5

#ifndef LGISYSTEMSERVICE_DEVICEINFO_FILENAME
#define LGISYSTEMSERVICE_DEVICEINFO_FILENAME "/tmp/device.info"
#endif

#ifndef LGISYSTEMSERVICE_DEVICEINFO_ENVVAR_PREFIX
#define LGISYSTEMSERVICE_DEVICEINFO_ENVVAR_PREFIX "LGISYSTEMSERVICE_DEVICEINFO_"
#endif



// TODO: remove this
#define registerMethod(...) for (uint8_t i = 1; GetHandler(i); i++) GetHandler(i)->Register<JsonObject, JsonObject>(__VA_ARGS__)

template <typename T>
class nano_optional
{
    public:
    nano_optional<T>() : value(std::make_pair(false, T())){}
    nano_optional<T>(T val) : value(std::make_pair(true, val)){}
    bool has_value() {return value.first;}
    T get() {return value.second;}

    private:
    std::pair<bool, T> value;
};


// std::optional<std::string> getDeviceInfoFromFile()
// {
//     std::ifstream ifs = std::ifstream(LGISYSTEMSERVICE_DEVICEINFO_FILENAME, std::ios_base::in);
//     if (!ifs.is_open())
//     {
//         return std::nullopt;
//     }
//     std::string ret;
//     ifs >> ret;
//     ifs.close();
//     return ret;
// }

// std::optional<std::string> getDeviceInfoFromEnv()
// {
//     if(const char* env = std::getenv(LGISYSTEMSERVICE_DEVICEINFO_ENVVAR))
//         return std::string(env);
//     else
//         return std::nullopt;
// }

std::vector<std::string> splitLines(const std::string& s)
{
    std::vector<std::string> ret;
    std::stringstream ss(s);
    std::string line;
    while (std::getline(ss, line, '\n')) 
    {
        std::cout <<"line: " << line << std::endl;
        ret.push_back(line);
    }
    return ret;
}

nano_optional<std::string> getDeviceInfoFromFile()
{
    std::ifstream ifs = std::ifstream(LGISYSTEMSERVICE_DEVICEINFO_FILENAME, std::ios_base::in);
    if (!ifs.is_open())
    {
        return nano_optional<std::string>();
    }
    std::stringstream ret;
    ret << ifs.rdbuf();

    ifs.close();
    return nano_optional<std::string>(ret.str());
}

nano_optional<std::string> getDeviceInfoFromEnv(char** environ_ = environ)
{
    std::vector<std::string> lines;
    char** env = environ_;
    while (*env)
    {
        //std::cout << *env << std::endl;
        std::string s(*env);
        env++;
        
        size_t n = s.find(LGISYSTEMSERVICE_DEVICEINFO_ENVVAR_PREFIX);
        if (n == std::string::npos || n > 0)
            continue;
        
        lines.push_back(s.substr(strlen(LGISYSTEMSERVICE_DEVICEINFO_ENVVAR_PREFIX)));
    }

    std::cout<<"$$$$\n";

    for (auto& s : lines)
        std::cout << s << std::endl;

    std::cout<<"****\n";


    if(const char* env = std::getenv(LGISYSTEMSERVICE_DEVICEINFO_ENVVAR_PREFIX))
    {
        splitLines(env);
        return nano_optional<std::string>(std::string(env));
    }
    else
        return nano_optional<std::string>();
}

std::string getDeviceInfo()
{
    nano_optional<std::string> str = getDeviceInfoFromEnv();
    if (str.has_value())
    {
        std::cout << str.get() << std::endl;
        return str.get();
    }

    str = getDeviceInfoFromFile();
    if (str.has_value())
    {
        std::cout << str.get() << std::endl;
        return str.get();
    }

    return "";
}


/**
 * @brief WPEFramework class for SystemServices
 */
namespace WPEFramework {

    namespace {

        static Plugin::Metadata<Plugin::SystemServices> metadata(
            // Version (Major, Minor, Patch)
            API_VERSION_NUMBER_MAJOR, API_VERSION_NUMBER_MINOR, API_VERSION_NUMBER_PATCH,
            // Preconditions
            {},
            // Terminations
            {},
            // Controls
            {}
        );
    }

    namespace Plugin {
        SERVICE_REGISTRATION(SystemServices, API_VERSION_NUMBER_MAJOR, API_VERSION_NUMBER_MINOR, API_VERSION_NUMBER_PATCH);

        //SystemServices* SystemServices::_instance = nullptr;

        /**
         * Register SystemService module as wpeframework plugin
         */
        SystemServices::SystemServices()
            : PluginHost::JSONRPC()
        {
            //SystemServices::_instance = this;

            CreateHandler({2});
            registerMethod("getDeviceInfo", &SystemServices::getDeviceInfo, this);

        }


        SystemServices::~SystemServices()
        {

        }

        const string SystemServices::Initialize(PluginHost::IShell* service)
        {

            //JsonObject s;
            //s.get();
            return std::string();
        }

        void SystemServices::Deinitialize(PluginHost::IShell*)
        {
            //SystemServices::_instance = nullptr;
        }


        uint32_t SystemServices::getDeviceInfo(const JsonObject& parameters,
                JsonObject& response)
        {
            JsonObject o(::getDeviceInfo());

            std::cout<<"-------------\n";

            std::cout<<o["dupa"].String() << std::endl;
            o["x"] = "ole";
            o["y"] = "";
            std::string s;
            o.ToString(s);
            std::cout<< s << std::endl;

            return 0;
        }
    }
}