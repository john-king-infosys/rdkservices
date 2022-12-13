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

#ifndef SYSTEMSERVICES_H
#define SYSTEMSERVICES_H

#include "Module.h"
#include "tracing/Logging.h"

#/* System Services Triggered Events. */
#define EVT_ONSYSTEMSAMPLEEVENT           "onSampleEvent"
#define EVT_ONSYSTEMPOWERSTATECHANGED     "onSystemPowerStateChanged"
#define EVT_ONSYSTEMMODECHANGED           "onSystemModeChanged"
#define EVT_ONNETWORKSTANDBYMODECHANGED   "onNetworkStandbyModeChanged"
#define EVT_ONFIRMWAREUPDATEINFORECEIVED  "onFirmwareUpdateInfoReceived"
#define EVT_ONFIRMWAREUPDATESTATECHANGED  "onFirmwareUpdateStateChange"
#define EVT_ONTEMPERATURETHRESHOLDCHANGED "onTemperatureThresholdChanged"
#define EVT_ONMACADDRESSRETRIEVED         "onMacAddressesRetreived"
#define EVT_ONREBOOTREQUEST               "onRebootRequest"
#define EVT_ON_SYSTEM_CLOCK_SET           "onSystemClockSet"
#define EVT_ONFWPENDINGREBOOT             "onFirmwarePendingReboot" /* Auto Reboot notifier */
#define EVT_ONREBOOTREQUEST               "onRebootRequest"
#define EVT_ONTERRITORYCHANGED            "onTerritoryChanged"
#define EVT_ONTIMEZONEDSTCHANGED          "onTimeZoneDSTChanged"
#define TERRITORYFILE                     "/opt/secure/persistent/System/Territory.txt"

namespace WPEFramework {
    namespace Plugin {

        // This is a server for a JSONRPC communication channel.
        // For a plugin to be capable to handle JSONRPC, inherit from PluginHost::JSONRPC.
        // By inheriting from this class, the plugin realizes the interface PluginHost::IDispatcher.
        // This realization of this interface implements, by default, the following methods on this plugin
        // - exists
        // - register
        // - unregister
        // Any other methood to be handled by this plugin  can be added can be added by using the
        // templated methods Register on the PluginHost::JSONRPC class.
        // As the registration/unregistration of notifications is realized by the class PluginHost::JSONRPC,
        // this class exposes a public method called, Notify(), using this methods, all subscribed clients
        // will receive a JSONRPC message as a notification, in case this method is called.

        /**
         * @struct modeInfo
         * @brief This structure contains information of modes.
         * @ingroup SERVMGR_SYSTEM
         */
        struct modeInfo {
            std::string mode;  //operating mode
            int duration;  // duration in seconds
        };

        class SystemServices : public PluginHost::IPlugin, public PluginHost::JSONRPC {

        public:
            SystemServices();
            virtual ~SystemServices();

            // We do not allow this plugin to be copied !!
            SystemServices(const SystemServices&) = delete;
            SystemServices& operator=(const SystemServices&) = delete;

            virtual const string Initialize(PluginHost::IShell* service) override;
            virtual void Deinitialize(PluginHost::IShell* service) override;
            virtual string Information() const override { return {}; }

            BEGIN_INTERFACE_MAP(SystemServices)
            INTERFACE_ENTRY(PluginHost::IPlugin)
            INTERFACE_ENTRY(PluginHost::IDispatcher)
            END_INTERFACE_MAP



            uint32_t getDeviceInfo(const JsonObject& parameters, JsonObject& response);

        private:
            //static SystemServices* _instance;
            
        }; /* end of system service class */
    } /* end of plugin */
} /* end of wpeframework */

#endif //SYSTEMSERVICES_H
