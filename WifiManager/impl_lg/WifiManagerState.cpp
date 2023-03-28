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
        updateWifiStatus(dbus.networkconfig1_GetStatus(getWifiInterfaceName()));
    }
}

WifiManagerState::~WifiManagerState()
{
}

namespace {
    /*
    `0`: UNINSTALLED - The device was in an installed state and was uninstalled; or, the device does not have a Wifi radio installed   
    `1`: DISABLED - The device is installed but not yet enabled  
    `2`: DISCONNECTED - The device is installed and enabled, but not yet connected to a network
    `3`: PAIRING - The device is in the process of pairing, but not yet connected to a network  
    `4`: CONNECTING - The device is attempting to connect to a network  
    `5`: CONNECTED - The device is successfully connected to a network  
    */
    const std::map<InterfaceStatus, WifiState> statusToState {
        {Disabled, WifiState::DISABLED},
        {Disconnected, WifiState::DISCONNECTED},
        {Associating, WifiState::CONNECTING},
        {Dormant, WifiState::DISCONNECTED},
        {Binding, WifiState::CONNECTING},
        {Assigned, WifiState::CONNECTED},
        {Scanning, WifiState::CONNECTING}
    };
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
    // TODO: this is used by Amazon, but only 'ssid' is used by Amazon app and needs to be returned; the rest can be mocked
    LOGINFOMETHOD();
    // todo: call dbus
    response["ssid"] = string("todo");
    response["bssid"] = string("n/a");
    response["rate"] = string("n/a");
    response["noise"] = string("n/a");
    response["security"] = string("n/a");
    response["signalStrength"] = string("n/a");
    response["frequency"] = string("n/a");
    returnResponse(false);
}

void WifiManagerState::statusChanged(const std::string& interface, InterfaceStatus status)
{
    updateWifiStatus(status);
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

const std::string WifiManagerState::fetchWifiInterfaceName() {
    DBusClient &dbus = DBusClient::getInstance();
    std::vector<std::string> interfaces = dbus.networkconfig1_GetInterfaces();
    for (auto &intf : interfaces)
    {
        std::string type;
        if (dbus.networkconfig1_GetParam(intf, "type", type) && type == "wifi")
        {
            return intf;
        }
    }
    return "";
}

const std::string& WifiManagerState::getWifiInterfaceName() {
    static std::mutex interface_name_mutex;
    std::lock_guard<std::mutex> lock(interface_name_mutex);
    static std::string name = WifiManagerState::fetchWifiInterfaceName();
    return name;
}
/*

switching from eht2 to wlan2 and back:

root@E0B7B1-APPSTB-301000003204:~# dbus-monitor --system type=signal,interface=com.lgi.rdk.utils.networkconfig1,member='StatusChanged'

signal time=1679995849.373632 sender=:1.16 -> destination=(null destination) serial=732 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "eth2"
   string "Disconnected"
signal time=1679995854.349952 sender=:1.16 -> destination=(null destination) serial=785 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Disconnected"
signal time=1679995854.418572 sender=:1.16 -> destination=(null destination) serial=795 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Scanning"
signal time=1679995856.740837 sender=:1.16 -> destination=(null destination) serial=851 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Disconnected"
signal time=1679995857.095259 sender=:1.16 -> destination=(null destination) serial=938 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Associating"
signal time=1679995857.208994 sender=:1.16 -> destination=(null destination) serial=950 path=/co[ 1697.092260] set wnm_keepalives_max_idle failed : -2
m/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Dormant"
signal time=1679995858.525842 sender=:1.16 -> destination=(null destination) serial=987 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Binding"
signal time=1679995859.197049 sender=:1.16 -> destination=(null destination) serial=997 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Assigned"
signal time=1679995878.496053 sender=:1.16 -> destination=(null destination) serial=1075 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "eth2"
   string "Dormant"
signal time=1679995878.530651 sender=:1.16 -> destination=(null destination) serial=1083 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Dormant"
signal time=1679995878.780605 sender=:1.16 -> destination=(null destination) serial=1084 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "eth2"
   string "Binding"
signal time=1679995879.136574 sender=:1.16 -> destination=(null destination) serial=1102 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "eth2"
   string "Assigned"
signal time=1679995879.280652 sender=:1.16 -> destination=(null destination) serial=1121 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Disconnected"
signal time=1679995880.567282 sender=:1.16 -> destination=(null destination) serial=1175 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Disabled"
signal time=1679995880.919561 sender=:1.16 -> destination=(null destination) serial=1213 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Disconnected"
signal time=1679995880.996979 sender=:1.16 -> destination=(null destination) serial=1223 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Scanning"
signal time=1679995883.385561 sender=:1.16 -> destination=(null destination) serial=1305 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Disconnected"
signal time=1679995883.754459 sender=:1.16 -> destination=(null destination) serial=1413 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Associating"
signal time=1679995883.954857 sender=:1.16 -> destination=(null destination) serial=1421 path=/com/lgi/rdk/utils/networkconfig1; interface=com.lgi.rdk.utils.networkconfig1; member=StatusChanged
   string "wlan2"
   string "Dormant"

*/