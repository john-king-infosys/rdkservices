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

using namespace WPEFramework::Plugin;
using namespace std;

WifiManagerState::WifiManagerState()
{
    // TODO: why would we need that?
    // m_useWifiStateCache = false;
    // m_wifiStateCache = WifiState::FAILED;
}

WifiManagerState::~WifiManagerState()
{
}


void WifiManagerState::setWifiStateCache(bool value,WifiState Cstate)
{
    // TODO: why would we need that?
    // m_useWifiStateCache = value;
    // m_wifiStateCache = Cstate;
}

uint32_t WifiManagerState::getCurrentState(const JsonObject &parameters, JsonObject &response)
{
    // TODO: this is used by Amazon, but only 'state' is used by Amazon app and needs to be provided; the rest can be mocked
    LOGINFOMETHOD();
    response["state"] = static_cast<int>(0);
    returnResponse(false);
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
