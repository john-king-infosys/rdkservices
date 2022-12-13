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

#define API_VERSION_NUMBER_MAJOR 1
#define API_VERSION_NUMBER_MINOR 1
#define API_VERSION_NUMBER_PATCH 5


// TODO: remove this
#define registerMethod(...) for (uint8_t i = 1; GetHandler(i); i++) GetHandler(i)->Register<JsonObject, JsonObject>(__VA_ARGS__)

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
            return std::string();
        }

        void SystemServices::Deinitialize(PluginHost::IShell*)
        {
            //SystemServices::_instance = nullptr;
        }


        uint32_t SystemServices::getDeviceInfo(const JsonObject& parameters,
                JsonObject& response)
        {
            return 0;
        }
    }
}