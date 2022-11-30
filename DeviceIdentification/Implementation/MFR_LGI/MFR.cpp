/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
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
 */

#include "../../Module.h"

#include <fstream>

extern "C" {
#include "mfrTypes.h"
}

#define TO_PRINT(x) isprint(x) ? x : '!'

static void dumpMfrSerializedData(mfrSerializedData_t * mfrSerializedData, const char * prefix) {
    if (mfrSerializedData && prefix) {
        TRACE_L1("%s bufLen: %d", prefix, mfrSerializedData->bufLen);
        TRACE_L1("%s freeBuf: %p", prefix, mfrSerializedData->freeBuf);
        for (int i = 0; i < mfrSerializedData->bufLen; i++) {
            TRACE_L1("%s[%d/%d]\t %02x %c", prefix, i, mfrSerializedData->bufLen - 1, mfrSerializedData->buf[i], TO_PRINT(mfrSerializedData->buf[i]));
        }
    } else {
        TRACE_L1("invalid input data");
    }
}
namespace WPEFramework {
namespace Plugin {
    class DeviceImplementation : public PluginHost::ISubSystem::IIdentifier {

    public:
        DeviceImplementation()
        {
            if (mfr_init() == mfrERR_NONE) {
                UpdateChipset(_chipset);
                UpdateFirmwareVersion(_firmwareVersion);
                UpdateIdentifier();
#if 0
                dumpMfrValues();
#endif
                mfr_shutdown();
            } else {
                TRACE_L1("failed to initialize mfr");
            }
        }

        DeviceImplementation(const DeviceImplementation&) = delete;
        DeviceImplementation& operator=(const DeviceImplementation&) = delete;
        virtual ~DeviceImplementation()
        {
            /* Nothing to do here. */
        }

    public:
        // IIdentifier interface
        uint8_t Identifier(const uint8_t length, uint8_t buffer[]) const override
        {
            uint8_t result = 0;
            if ((_identity.length()) && (_status == mfrERR_NONE)) {
                result = (_identity.length() > length ? length : _identity.length());
                ::memcpy(buffer, _identity.c_str(), result);
            } else {
                SYSLOG(Logging::Notification, (_T("Cannot determine system identity; Error:[%d]!"), static_cast<uint8_t>(_status)));
            }
            return result;
        }
        string Architecture() const override
        {
            return Core::SystemInfo::Instance().Architecture();
        }
        string Chipset() const override
        {
            return _chipset;
        }
        string FirmwareVersion() const override
        {
            return _firmwareVersion;
        }

        BEGIN_INTERFACE_MAP(DeviceImplementation)
        INTERFACE_ENTRY(PluginHost::ISubSystem::IIdentifier)
        END_INTERFACE_MAP

    private:
        inline void UpdateFirmwareVersion(string& firmwareVersion) const
        {
            int retVal = -1;
            mfrSerializedData_t mfrSerializedData;
            retVal = mfrGetSerializedData(mfrSERIALIZED_TYPE_SOFTWAREVERSION, &mfrSerializedData);
            if ((mfrERR_NONE == retVal) && mfrSerializedData.bufLen) {
                int lastIdx = -1;
                // troubles around last 2 characters newline + sometimes new character
                for (int i = mfrSerializedData.bufLen -1; i >= 0; i--) {
                    if (isprint(mfrSerializedData.buf[i])) {
                        lastIdx = i;
                        break;
                    } else {
                        TRACE_L1("illegal char at: %d", i);
                    }
                }
                if (lastIdx > 0) {
                    firmwareVersion.assign(mfrSerializedData.buf, lastIdx + 1);
                    dumpMfrSerializedData(&mfrSerializedData, "mfrSERIALIZED_TYPE_SOFTWAREVERSION");
                    TRACE_L1("firmwareVersion is: %s", firmwareVersion.c_str());
                    if (mfrSerializedData.freeBuf) {
                        mfrSerializedData.freeBuf(mfrSerializedData.buf);
                    }
                }
            }
        }
        inline void UpdateChipset(string& chipset) const
        {
            chipset = "BCM72127";
            // lack of mfrSERIALIZED_TYPE_CHIPSETINFO
            //int retVal = -1;
            //mfrSerializedData_t mfrSerializedData;
            // lack of mfrSERIALIZED_TYPE_CHIPSETINFO in headers
            //retVal = mfrGetSerializedData(mfrSERIALIZED_TYPE_CHIPSETINFO, &mfrSerializedData);
            //if ((mfrERR_NONE == retVal) && mfrSerializedData.bufLen) {
            //    chipset = mfrSerializedData.buf;
            //    if (mfrSerializedData.freeBuf) {
            //        mfrSerializedData.freeBuf(mfrSerializedData.buf);
            //    }
            //}
            TRACE_L1("chipset is: %s", chipset.c_str());
        }

        inline void UpdateIdentifier()
        {
            mfrSerializedData_t mfrSerializedData;
            _status = mfrGetSerializedData(mfrSERIALIZED_TYPE_SERIALNUMBER, &mfrSerializedData);
            if ((mfrERR_NONE == _status) && mfrSerializedData.bufLen) {
                _identity.assign(mfrSerializedData.buf, mfrSerializedData.bufLen);
                dumpMfrSerializedData(&mfrSerializedData, "mfrSERIALIZED_TYPE_SERIALNUMBER");
                TRACE_L1("_identity: %s", _identity.c_str());
                if (mfrSerializedData.freeBuf) {
                    mfrSerializedData.freeBuf(mfrSerializedData.buf);
                }
            }
        }

#if 0
        void dumpMfrValue(mfrSerializedType_t type, const char * name) {
            mfrSerializedData_t mfrSerializedData;
            int retVal = -1;
            retVal = mfrGetSerializedData(type, &mfrSerializedData);
            if (retVal == mfrERR_NONE) {
                dumpMfrSerializedData(&mfrSerializedData, name);
                if (mfrSerializedData.freeBuf) {
                    mfrSerializedData.freeBuf(mfrSerializedData.buf);
                }
            } else {
                TRACE_L1("error getting %s %d", name, retVal);
            }
        }

        void dumpMfrValues() {
            dumpMfrValue(mfrSERIALIZED_TYPE_MANUFACTURER, "mfrSERIALIZED_TYPE_MANUFACTURER");
            dumpMfrValue(mfrSERIALIZED_TYPE_MANUFACTUREROUI, "mfrSERIALIZED_TYPE_MANUFACTUREROUI");
            dumpMfrValue(mfrSERIALIZED_TYPE_MODELNAME, "mfrSERIALIZED_TYPE_MODELNAME");
            dumpMfrValue(mfrSERIALIZED_TYPE_DESCRIPTION, "mfrSERIALIZED_TYPE_DESCRIPTION");
            dumpMfrValue(mfrSERIALIZED_TYPE_PRODUCTCLASS, "mfrSERIALIZED_TYPE_PRODUCTCLASS");
            dumpMfrValue(mfrSERIALIZED_TYPE_SERIALNUMBER, "mfrSERIALIZED_TYPE_SERIALNUMBER");
            dumpMfrValue(mfrSERIALIZED_TYPE_HARDWAREVERSION, "mfrSERIALIZED_TYPE_HARDWAREVERSION");
            dumpMfrValue(mfrSERIALIZED_TYPE_SOFTWAREVERSION, "mfrSERIALIZED_TYPE_SOFTWAREVERSION");
            dumpMfrValue(mfrSERIALIZED_TYPE_PROVISIONINGCODE, "mfrSERIALIZED_TYPE_PROVISIONINGCODE");
            dumpMfrValue(mfrSERIALIZED_TYPE_FIRSTUSEDATE, "mfrSERIALIZED_TYPE_FIRSTUSEDATE");
            dumpMfrValue(mfrSERIALIZED_TYPE_DEVICEMAC, "mfrSERIALIZED_TYPE_DEVICEMAC");
            dumpMfrValue(mfrSERIALIZED_TYPE_MOCAMAC, "mfrSERIALIZED_TYPE_MOCAMAC");
            dumpMfrValue(mfrSERIALIZED_TYPE_HDMIHDCP, "mfrSERIALIZED_TYPE_HDMIHDCP");
            dumpMfrValue(mfrSERIALIZED_TYPE_PDRIVERSION, "mfrSERIALIZED_TYPE_PDRIVERSION");
            dumpMfrValue(mfrSERIALIZED_TYPE_CMCHIPVERSION, "mfrSERIALIZED_TYPE_CMCHIPVERSION");
            dumpMfrValue(mfrSERIALIZED_TYPE_WIFIMAC, "mfrSERIALIZED_TYPE_WIFIMAC");
            dumpMfrValue(mfrSERIALIZED_TYPE_BLUETOOTHMAC, "mfrSERIALIZED_TYPE_BLUETOOTHMAC");
            dumpMfrValue(mfrSERIALIZED_TYPE_WPSPIN, "mfrSERIALIZED_TYPE_WPSPIN");
            dumpMfrValue(mfrSERIALIZED_TYPE_MANUFACTURING_SERIALNUMBER, "mfrSERIALIZED_TYPE_MANUFACTURING_SERIALNUMBER");
        }
#endif
    private:
        string _chipset;
        string _firmwareVersion;
        string _identity;
        mfrError_t _status;
    };

    SERVICE_REGISTRATION(DeviceImplementation, 1, 0);
}
}
