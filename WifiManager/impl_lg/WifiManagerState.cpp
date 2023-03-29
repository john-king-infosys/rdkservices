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

#include "WifiManagerState.h"
#include "UtilsJsonRpc.h"
#include "../WifiManager.h" // Need access to WifiManager::getInstance so can't use 'WifiManagerInterface.h'

using namespace WPEFramework::Plugin;
using namespace WifiManagerImpl;
// TODO: why would we need that?
// m_useWifiStateCache = false;
// m_wifiStateCache = WifiState::FAILED;

// 1. we need active interface - com.lgi.rdk.utils.networkconfig1.GetActiveInterface
// 2. we need to check the interface type - com.lgi.rdk.utils.networkconfig1.GetParams(ParamType = "type")
// 3. if this is wifi, we need to listen for the interface status change - com.lgi.rdk.utils.networkconfig1.StatusChanged

// ..... or .....
// 1. com.lgi.rdk.utils.networkconfig1.GetInterfaces
// dbus-send --system --print-reply --dest=com.lgi.rdk.utils.networkconfig1 /com/lgi/rdk/utils/networkconfig1 com.lgi.rdk.utils.networkconfig1.GetInterfaces
// 2. for each interface, check the interface type; stop when WiFi is found
//   com.lgi.rdk.utils.networkconfig1.GetParams <arg name="id" direction="in" type="s"/>
// dbus-send --system --print-reply --dest=com.lgi.rdk.utils.networkconfig1 /com/lgi/rdk/utils/networkconfig1 com.lgi.rdk.utils.networkconfig1.GetParam string:wlan2 string:type
// or:
// dbus-send --system --print-reply --dest=com.lgi.rdk.utils.networkconfig1 /com/lgi/rdk/utils/networkconfig1 com.lgi.rdk.utils.networkconfig1.GetParams string:wlan2
// 3. get wifi status
// dbus-send --system --print-reply --dest=com.lgi.rdk.utils.networkconfig1 /com/lgi/rdk/utils/networkconfig1 com.lgi.rdk.utils.networkconfig1.GetStatus string:wlan2
// method return time=1679994562.375126 sender=:1.16 -> destination=:1.513 serial=133 reply_serial=2
// int32 0
// string "Disabled"
// 4. register for status changed & filter on type
// StatusChanged com.lgi.rdk.utils.networkconfig1.StatusChanged
// dbus-monitor "type='signal',interface='com.lgi.rdk.utils.networkconfig1',member='StatusChanged',path='/com/lgi/rdk/utils/networkconfig1'"

WifiManagerState::WifiManagerState()
{
}

void WifiManagerState::Initialize()
{
   DBusClient &dbus = DBusClient::getInstance();
   if (getWifiInterfaceName().empty())
   {
      LOGWARN("No 'wifi' interface found");
      // TODO: throw an exception?
   }
   else
   {
      // register for status updates
      dbus.registerStatusChanged(std::bind(&WifiManagerState::statusChanged, this, std::placeholders::_1, std::placeholders::_2));
      // get current wifi status
      InterfaceStatus status;
      const std::string iname = getWifiInterfaceName();
      if (dbus.networkconfig1_GetStatus(iname, status)) {
         updateWifiStatus(status);
      } else {
         LOGWARN("failed to get interface '%s' status", iname.c_str());
      }
   }
}

WifiManagerState::~WifiManagerState()
{
}

namespace
{
   /*
   `0`: UNINSTALLED - The device was in an installed state and was uninstalled; or, the device does not have a Wifi radio installed
   `1`: DISABLED - The device is installed but not yet enabled
   `2`: DISCONNECTED - The device is installed and enabled, but not yet connected to a network
   `3`: PAIRING - The device is in the process of pairing, but not yet connected to a network
   `4`: CONNECTING - The device is attempting to connect to a network
   `5`: CONNECTED - The device is successfully connected to a network
   */
   const std::map<InterfaceStatus, WifiState> statusToState{
       {Disabled, WifiState::DISABLED},
       {Disconnected, WifiState::DISCONNECTED},
       {Associating, WifiState::CONNECTING},
       {Dormant, WifiState::DISCONNECTED},
       {Binding, WifiState::CONNECTING},
       {Assigned, WifiState::CONNECTED},
       {Scanning, WifiState::CONNECTING}};
}

uint32_t WifiManagerState::getCurrentState(const JsonObject &parameters, JsonObject &response)
{
   // TODO: this is used by Amazon, but only 'state' is used by Amazon app and needs to be provided; the rest can be mocked
   LOGINFOMETHOD();
   response["state"] = static_cast<int>(statusToState.at(m_wifi_status));
   returnResponse(true);
}

uint32_t WifiManagerState::getConnectedSSID(const JsonObject &parameters, JsonObject &response) const
{
   // this is used by Amazon, but only 'ssid' is used by Amazon app and needs to be returned; the rest can be mocked
   LOGINFOMETHOD();

   const std::string &wifiInterface = getWifiInterfaceName();
   bool ret = false;
   if (!wifiInterface.empty())
   {
      std::string netid;
      if (DBusClient::getInstance().networkconfig1_GetParam(wifiInterface, "netid", netid))
      {
         size_t pos = netid.find(":");
         if (pos != std::string::npos)
         {
            response["ssid"] = netid.substr(pos + 1);
            ret = true;
         }
         else
         {
            LOGWARN("failed to parse ssid from netid");
         }
      }
      else
      {
         LOGWARN("failed to retrieve wifi netid param");
      }
   }

   // only 'ssid' is used by Amazon app and needs to be returned; the rest can be empty - at least for now
   response["bssid"] = string("");
   response["rate"] = string("");
   response["noise"] = string("");
   response["security"] = string("");
   response["signalStrength"] = string("");
   response["frequency"] = string("");
   returnResponse(ret);
}

void WifiManagerState::statusChanged(const std::string &interface, InterfaceStatus status)
{
   if (interface == getWifiInterfaceName())
   {
      updateWifiStatus(status);
   }
}

void WifiManagerState::updateWifiStatus(WifiManagerImpl::InterfaceStatus status)
{
   {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_wifi_status = status;
   }
   // Hardcode 'isLNF' for the moment (at the moment, the same is done in default rdk implementation)
   WifiManager::getInstance().onWIFIStateChanged(statusToState.at(m_wifi_status), false);
}

uint32_t WifiManagerState::setEnabled(const JsonObject &parameters, JsonObject &response)
{
   LOGINFOMETHOD();
   returnResponse(false);
}

uint32_t WifiManagerState::getSupportedSecurityModes(const JsonObject &parameters, JsonObject &response)
{
   LOGINFOMETHOD();
   returnResponse(false);
}

const std::string WifiManagerState::fetchWifiInterfaceName()
{
   printf("xaxa %s:%d\n", __FUNCTION__, __LINE__); fflush(stdout);
   DBusClient &dbus = DBusClient::getInstance();
   std::vector<std::string> interfaces;
   dbus.networkconfig1_GetInterfaces(interfaces);
   // TODO: check ret val for success !!! also, in other dbus using places
   for (auto &intf : interfaces)
   {      
      std::string type;
      if (dbus.networkconfig1_GetParam(intf, "type", type) && type == "wifi")
      {
         printf("xaxa %s:%d PICKED: %s\n", __FUNCTION__, __LINE__, intf.c_str()); fflush(stdout);
         return intf;
      }
      printf("xaxa %s:%d NOT PICKED interface: %s type: %s\n", __FUNCTION__, __LINE__, intf.c_str(), type.c_str()); fflush(stdout);
   }
   return "";
}

const std::string &WifiManagerState::getWifiInterfaceName()
{
   static std::mutex interface_name_mutex;
   std::lock_guard<std::mutex> lock(interface_name_mutex);
   static std::string name = WifiManagerState::fetchWifiInterfaceName();
   return name;
}