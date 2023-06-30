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

#ifndef DEVICEINFO_DEVICEINFO_H
#define DEVICEINFO_DEVICEINFO_H

#include "Module.h"
#ifdef USE_THUNDER_R4
#include <interfaces/IDeviceInfo.h>
#else
#include <interfaces/IDeviceInfo2.h>
#endif /* USE_THUNDER_R4 */
#include <interfaces/IFirmwareVersion.h>
#include <interfaces/json/JsonData_DeviceInfo.h>

namespace WPEFramework {
namespace Plugin {

    class DeviceInfo : public PluginHost::IPlugin,
                       public PluginHost::IWeb,
                       public PluginHost::JSONRPC {

    public:
        class Data : public Core::JSON::Container {
        public:
            Data()
                : Core::JSON::Container()
                , Addresses()
                , SystemInfo()
            {
                Add(_T("addresses"), &Addresses);
                Add(_T("systeminfo"), &SystemInfo);
                Add(_T("sockets"), &Sockets);
            }

            virtual ~Data()
            {
            }

        public:
            Core::JSON::ArrayType<JsonData::DeviceInfo::AddressesData> Addresses;
            JsonData::DeviceInfo::SysteminfoData SystemInfo;
            JsonData::DeviceInfo::SocketinfoData Sockets;
        };

    private:
        DeviceInfo(const DeviceInfo&) = delete;
        DeviceInfo& operator=(const DeviceInfo&) = delete;

    public:
        DeviceInfo()
            : _skipURL(0)
            , _service(nullptr)
            , _subSystem(nullptr)
            , _systemId()
            , _connectionId(0)
            , _deviceInfo(nullptr)
            , _deviceAudioCapabilityInterface(nullptr)
            , _deviceVideoCapabilityInterface(nullptr)
            , _firmwareVersion(nullptr)
        {
            RegisterAll();
        }

        virtual ~DeviceInfo()
        {
            UnregisterAll();
        }

        BEGIN_INTERFACE_MAP(DeviceInfo)
        INTERFACE_ENTRY(PluginHost::IPlugin)
        INTERFACE_ENTRY(PluginHost::IWeb)
        INTERFACE_ENTRY(PluginHost::IDispatcher)
        INTERFACE_AGGREGATE(Exchange::IDeviceInfo, _deviceInfo)
        INTERFACE_AGGREGATE(Exchange::IDeviceAudioCapabilities, _deviceAudioCapabilityInterface)
        INTERFACE_AGGREGATE(Exchange::IDeviceVideoCapabilities, _deviceVideoCapabilityInterface)
        INTERFACE_AGGREGATE(Exchange::IFirmwareVersion, _firmwareVersion)
        END_INTERFACE_MAP

    public:
        //   IPlugin methods
        // -------------------------------------------------------------------------------------------------------
        virtual const string Initialize(PluginHost::IShell* service) override;
        virtual void Deinitialize(PluginHost::IShell* service) override;
        virtual string Information() const override;

        //   IWeb methods
        // -------------------------------------------------------------------------------------------------------
        virtual void Inbound(Web::Request& request) override;
        virtual Core::ProxyType<Web::Response> Process(const Web::Request& request) override;

    private:
        // JsonRpc
        void RegisterAll();
        void UnregisterAll();

        uint32_t get_systeminfo(JsonData::DeviceInfo::SysteminfoData& response) const;
        uint32_t get_addresses(Core::JSON::ArrayType<JsonData::DeviceInfo::AddressesData>& response) const;
        uint32_t get_socketinfo(JsonData::DeviceInfo::SocketinfoData& response) const;
        uint32_t get_firmwareversion(JsonData::DeviceInfo::FirmwareversionData& response) const;
        uint32_t get_serialnumber(JsonData::DeviceInfo::SerialnumberData& response) const;
        uint32_t get_modelid(JsonData::DeviceInfo::ModelidData& response) const;
        uint32_t get_make(JsonData::DeviceInfo::MakeData& response) const;
        uint32_t get_modelname(JsonData::DeviceInfo::ModelnameData& response) const;
        uint32_t get_devicetype(JsonData::DeviceInfo::DevicetypeData& response) const;
        uint32_t get_distributorid(JsonData::DeviceInfo::DistributoridData& response) const;
        uint32_t get_supportedaudioports(JsonData::DeviceInfo::SupportedaudioportsData& response) const;
        uint32_t get_supportedvideodisplays(JsonData::DeviceInfo::SupportedvideodisplaysData& response) const;
        uint32_t get_hostedid(JsonData::DeviceInfo::HostedidData& response) const;
        uint32_t endpoint_defaultresolution(const JsonData::DeviceInfo::SupportedresolutionsParamsInfo& params, JsonData::DeviceInfo::DefaultresolutionResultData& response) const;
        uint32_t endpoint_supportedresolutions(const JsonData::DeviceInfo::SupportedresolutionsParamsInfo& params, JsonData::DeviceInfo::SupportedresolutionsResultData& response) const;
        uint32_t endpoint_supportedhdcp(const JsonData::DeviceInfo::SupportedresolutionsParamsInfo& params, JsonData::DeviceInfo::SupportedhdcpResultData& response) const;
        uint32_t endpoint_audiocapabilities(const JsonData::DeviceInfo::AudiocapabilitiesParamsInfo& params, JsonData::DeviceInfo::AudiocapabilitiesResultData& response) const;
        uint32_t endpoint_ms12capabilities(const JsonData::DeviceInfo::AudiocapabilitiesParamsInfo& params, JsonData::DeviceInfo::Ms12capabilitiesResultData& response) const;
        uint32_t endpoint_supportedms12audioprofiles(const JsonData::DeviceInfo::AudiocapabilitiesParamsInfo& params, JsonData::DeviceInfo::Supportedms12audioprofilesResultData& response) const;

        void SysInfo(JsonData::DeviceInfo::SysteminfoData& systemInfo) const;
        void AddressInfo(Core::JSON::ArrayType<JsonData::DeviceInfo::AddressesData>& addressInfo) const;
        void SocketPortInfo(JsonData::DeviceInfo::SocketinfoData& socketPortInfo) const;

        using VideoOutputTypes = Core::JSON::ArrayType<Core::JSON::EnumType<JsonData::DeviceInfo::VideodisplayType>>;
        using ScreenResolutionType = Core::JSON::EnumType<JsonData::DeviceInfo::Output_resolutionType>;
        using ScreenResolutionTypes = Core::JSON::ArrayType<ScreenResolutionType>;
        using CopyProtectionType = Core::JSON::EnumType<JsonData::DeviceInfo::CopyprotectionType>;
        uint32_t VideoOutputs(VideoOutputTypes& videoOutputs) const;
        uint32_t DefaultResolution(const Exchange::IDeviceVideoCapabilities::VideoOutput videoOutput, ScreenResolutionType& screenResolutionType) const;
        uint32_t Resolutions(const Exchange::IDeviceVideoCapabilities::VideoOutput videoOutput, ScreenResolutionTypes& screenResolutionTypes) const;
        uint32_t Hdcp(const Exchange::IDeviceVideoCapabilities::VideoOutput videoOutput, CopyProtectionType& copyProtectionType) const;

        using AudioOutputTypes = Core::JSON::ArrayType<Core::JSON::EnumType<JsonData::DeviceInfo::AudioportType>>;
        using AudioCapabilityTypes = Core::JSON::ArrayType<Core::JSON::EnumType<JsonData::DeviceInfo::AudiocapabilityType>>;
        using Ms12CapabilityTypes = Core::JSON::ArrayType<Core::JSON::EnumType<JsonData::DeviceInfo::Ms12capabilityType>>;
        using Ms12ProfileTypes = Core::JSON::ArrayType<Core::JSON::EnumType<JsonData::DeviceInfo::Ms12profileType>>;
        uint32_t AudioOutputs(AudioOutputTypes& audioOutputs) const;
        uint32_t AudioCapabilities(const Exchange::IDeviceAudioCapabilities::AudioOutput audioOutput, AudioCapabilityTypes& audioCapabilityTypes) const;
        uint32_t Ms12Capabilities(const Exchange::IDeviceAudioCapabilities::AudioOutput audioOutput, Ms12CapabilityTypes& ms12CapabilityTypes) const;
        uint32_t Ms12Profiles(const Exchange::IDeviceAudioCapabilities::AudioOutput audioOutput, Ms12ProfileTypes& ms12ProfileTypes) const;

        inline uint32_t HostEDID(Core::JSON::String& Hostedid) const
        {
            string edid;
            uint32_t status = _deviceVideoCapabilityInterface->HostEDID(edid);
            if (status == Core::ERROR_NONE) {
                Hostedid = edid;
            }
            return status;
        }



    private:
        uint8_t _skipURL;
        PluginHost::IShell* _service;
        PluginHost::ISubSystem* _subSystem;
        string _systemId;
        uint32_t _connectionId;
        Exchange::IDeviceInfo* _deviceInfo;
        Exchange::IDeviceAudioCapabilities* _deviceAudioCapabilityInterface;
        Exchange::IDeviceVideoCapabilities* _deviceVideoCapabilityInterface;
        Exchange::IFirmwareVersion* _firmwareVersion;
    };

} // namespace Plugin
} // namespace WPEFramework

#endif // DEVICEINFO_DEVICEINFO_H
