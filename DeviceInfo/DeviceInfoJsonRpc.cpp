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

#include "DeviceInfo.h"

namespace WPEFramework {

namespace Plugin {

    using namespace JsonData::DeviceInfo;

    // Registration
    //

    void DeviceInfo::RegisterAll()
    {
        Property<SysteminfoData>(_T("systeminfo"), &DeviceInfo::get_systeminfo, nullptr, this);
        Property<Core::JSON::ArrayType<AddressesData>>(_T("addresses"), &DeviceInfo::get_addresses, nullptr, this);
        Property<SocketinfoData>(_T("socketinfo"), &DeviceInfo::get_socketinfo, nullptr, this);
        Property<FirmwareversionData>(_T("firmwareversion"), &DeviceInfo::get_firmwareversion, nullptr, this);
        Property<SerialnumberData>(_T("serialnumber"), &DeviceInfo::get_serialnumber, nullptr, this);
        Property<ModelidData>(_T("modelid"), &DeviceInfo::get_modelid, nullptr, this);
        Property<MakeData>(_T("make"), &DeviceInfo::get_make, nullptr, this);
        Property<ModelnameData>(_T("modelname"), &DeviceInfo::get_modelname, nullptr, this);
        Property<DevicetypeData>(_T("devicetype"), &DeviceInfo::get_devicetype, nullptr, this);
        Property<DistributoridData>(_T("distributorid"), &DeviceInfo::get_distributorid, nullptr, this);
        Property<SupportedaudioportsData>(_T("supportedaudioports"), &DeviceInfo::get_supportedaudioports, nullptr, this);
        Property<SupportedvideodisplaysData>(_T("supportedvideodisplays"), &DeviceInfo::get_supportedvideodisplays, nullptr, this);
        Property<HostedidData>(_T("hostedid"), &DeviceInfo::get_hostedid, nullptr, this);
        Register<SupportedresolutionsParamsInfo, DefaultresolutionResultData>(_T("defaultresolution"), &DeviceInfo::endpoint_defaultresolution, this);
        Register<SupportedresolutionsParamsInfo, SupportedresolutionsResultData>(_T("supportedresolutions"), &DeviceInfo::endpoint_supportedresolutions, this);
        Register<SupportedresolutionsParamsInfo, SupportedhdcpResultData>(_T("supportedhdcp"), &DeviceInfo::endpoint_supportedhdcp, this);
        Register<AudiocapabilitiesParamsInfo, AudiocapabilitiesResultData>(_T("audiocapabilities"), &DeviceInfo::endpoint_audiocapabilities, this);
        Register<AudiocapabilitiesParamsInfo, Ms12capabilitiesResultData>(_T("ms12capabilities"), &DeviceInfo::endpoint_ms12capabilities, this);
        Register<AudiocapabilitiesParamsInfo, Supportedms12audioprofilesResultData>(_T("supportedms12audioprofiles"), &DeviceInfo::endpoint_supportedms12audioprofiles, this);
    }

    void DeviceInfo::UnregisterAll()
    {
        Unregister(_T("socketinfo"));
        Unregister(_T("addresses"));
        Unregister(_T("systeminfo"));
        Unregister(_T("firmwareversion"));
        Unregister(_T("serialnumber"));
        Unregister(_T("modelid"));
        Unregister(_T("make"));
        Unregister(_T("modelname"));
        Unregister(_T("devicetype"));
        Unregister(_T("distributorid"));
        Unregister(_T("supportedaudioports"));
        Unregister(_T("supportedvideodisplays"));
        Unregister(_T("hostedid"));
        Unregister(_T("defaultresolution"));
        Unregister(_T("supportedresolutions"));
        Unregister(_T("supportedhdcp"));
        Unregister(_T("audiocapabilities"));
        Unregister(_T("ms12capabilities"));
        Unregister(_T("supportedms12audioprofiles"));
    }

    // API implementation
    //

    // Property: systeminfo - System general information
    // Return codes:
    //  - ERROR_NONE: Success
    uint32_t DeviceInfo::get_systeminfo(SysteminfoData& response) const
    {
        SysInfo(response);
        return Core::ERROR_NONE;
    }

    // Property: addresses - Network interface addresses
    // Return codes:
    //  - ERROR_NONE: Success
    uint32_t DeviceInfo::get_addresses(Core::JSON::ArrayType<AddressesData>& response) const
    {
        AddressInfo(response);
        return Core::ERROR_NONE;
    }

    // Property: socketinfo - Socket information
    // Return codes:
    //  - ERROR_NONE: Success
    uint32_t DeviceInfo::get_socketinfo(SocketinfoData& response) const
    {
        SocketPortInfo(response);
        return Core::ERROR_NONE;
    }

    // Property: firmwareversion - Versions maintained in version.txt
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::get_firmwareversion(FirmwareversionData& response) const
    {
        auto result = Core::ERROR_GENERAL;

        // imagename is required
        string value;
        if (_firmwareVersion->Imagename(value) == Core::ERROR_NONE) {
            response.Imagename = value;
            result = Core::ERROR_NONE;

            if (_firmwareVersion->Sdk(value) == Core::ERROR_NONE)
                response.Sdk = value;
            if (_firmwareVersion->Mediarite(value) == Core::ERROR_NONE)
                response.Mediarite = value;

            if (_firmwareVersion->Yocto(value) == Core::ERROR_NONE) {
                Core::EnumerateType<FirmwareversionData::YoctoType> yocto(value.c_str(), false);
                if (yocto.IsSet()) {
                    response.Yocto = yocto.Value();
                } else {
                    TRACE(Trace::Fatal, (_T("Unknown value %s"), value.c_str()));
                    result = Core::ERROR_GENERAL;
                }
            }
        }
        return result;
    }

    // Property: serialnumber - Serial number set by manufacturer
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::get_serialnumber(SerialnumberData& response) const
    {
        string serialNumber;

        auto result = _deviceInfo->SerialNumber(serialNumber);
        if (result == Core::ERROR_NONE) {
            response.Serialnumber = serialNumber;
        }

        return result;
    }

    // Property: modelid - Device model number or SKU
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::get_modelid(ModelidData& response) const
    {
        string sku;

        auto result = _deviceInfo->Sku(sku);
        if (result == Core::ERROR_NONE) {
            Core::EnumerateType<JsonData::DeviceInfo::ModelidData::SkuType> value(sku.c_str(), false);
            if (value.IsSet()) {
                response.Sku = value.Value();
            } else {
                TRACE(Trace::Fatal, (_T("Unknown value %s"), sku.c_str()));
                result = Core::ERROR_GENERAL;
            }
        }

        return result;
    }

    // Property: make - Device manufacturer
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::get_make(MakeData& response) const
    {
        string make;

        auto result = _deviceInfo->Make(make);
        if (result == Core::ERROR_NONE) {
            Core::EnumerateType<JsonData::DeviceInfo::MakeData::MakeType> value(make.c_str(), false);
            if (value.IsSet()) {
                response.Make = value.Value();
            } else {
                TRACE(Trace::Fatal, (_T("Unknown value %s"), make.c_str()));
                result = Core::ERROR_GENERAL;
            }
        }

        return result;
    }

    // Property: modelname - Friendly device model name
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::get_modelname(ModelnameData& response) const
    {
        string model;

        auto result = _deviceInfo->ModelName(model);
        if (result == Core::ERROR_NONE) {
            response.Model = model;
        }

        return result;
    }

    // Property: devicetype - Device type
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::get_devicetype(DevicetypeData& response) const
    {
        string deviceType;

        auto result = _deviceInfo->DeviceType(deviceType);
        if (result == Core::ERROR_NONE) {
            Core::EnumerateType<JsonData::DeviceInfo::DevicetypeData::DevicetypeType> value(deviceType.c_str(), false);
            if (value.IsSet()) {
                response.Devicetype = value.Value();
            } else {
                TRACE(Trace::Fatal, (_T("Unknown value %s"), deviceType.c_str()));
                result = Core::ERROR_GENERAL;
            }
        }

        return result;
    }

    // Property: distributorid - Partner ID or distributor ID for device
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::get_distributorid(DistributoridData& response) const
    {
        string distributorId;

        auto result = _deviceInfo->DistributorId(distributorId);
        if (result == Core::ERROR_NONE) {
            Core::EnumerateType<JsonData::DeviceInfo::DistributoridData::DistributoridType> value(distributorId.c_str(), false);
            if (value.IsSet()) {
                response.Distributorid = value.Value();
            } else {
                TRACE(Trace::Fatal, (_T("Unknown value %s"), distributorId.c_str()));
                result = Core::ERROR_GENERAL;
            }
        }

        return result;
    }

    // Property: supportedaudioports - Audio ports supported on the device (all ports that are physically present)
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::get_supportedaudioports(SupportedaudioportsData& response) const
    {
        return AudioOutputs(response.SupportedAudioPorts);
    }

    // Property: supportedvideodisplays - Video ports supported on the device (all ports that are physically present)
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::get_supportedvideodisplays(SupportedvideodisplaysData& response) const
    {
        return VideoOutputs(response.SupportedVideoDisplays);
    }

    // Property: hostedid - EDID of the host
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::get_hostedid(JsonData::DeviceInfo::HostedidData& response) const
    {
        return HostEDID(response.EDID);
    }

    // Method: defaultresolution - Default resolution on the selected video display port
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::endpoint_defaultresolution(const SupportedresolutionsParamsInfo& params, DefaultresolutionResultData& response) const
    {
        const VideodisplayType& videooutput = params.VideoDisplay.Value();
        return DefaultResolution(static_cast<Exchange::IDeviceVideoCapabilities::VideoOutput>(videooutput), response.DefaultResolution);

    }

    // Method: supportedresolutions - Supported resolutions on the selected video display port
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::endpoint_supportedresolutions(const SupportedresolutionsParamsInfo& params, SupportedresolutionsResultData& response) const
    {
        const VideodisplayType& videooutput = params.VideoDisplay.Value();
        return Resolutions(static_cast<Exchange::IDeviceVideoCapabilities::VideoOutput>(videooutput), response.SupportedResolutions);
    }

    // Method: supportedhdcp - Supported HDCP version on the selected video display port
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::endpoint_supportedhdcp(const SupportedresolutionsParamsInfo& params, SupportedhdcpResultData& response) const
    {
        const VideodisplayType& videooutput = params.VideoDisplay.Value();
        return Hdcp(static_cast<Exchange::IDeviceVideoCapabilities::VideoOutput>(videooutput), response.SupportedHDCPVersion);
    }

    // Method: audiocapabilities - Audio capabilities for the specified audio port
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::endpoint_audiocapabilities(const JsonData::DeviceInfo::AudiocapabilitiesParamsInfo& params, JsonData::DeviceInfo::AudiocapabilitiesResultData& response) const
    {

        const AudioportType& audiooutput = params.AudioPort.Value();
        return AudioCapabilities(static_cast<Exchange::IDeviceAudioCapabilities::AudioOutput>(audiooutput), response.AudioCapabilities);

    }

    // Method: ms12capabilities - MS12 audio capabilities for the specified audio port
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::endpoint_ms12capabilities(const JsonData::DeviceInfo::AudiocapabilitiesParamsInfo& params, JsonData::DeviceInfo::Ms12capabilitiesResultData& response) const
    {
        const AudioportType& audiooutput = params.AudioPort.Value();
        return Ms12Capabilities(static_cast<Exchange::IDeviceAudioCapabilities::AudioOutput>(audiooutput), response.MS12Capabilities);
    }

    // Method: supportedms12audioprofiles - Supported MS12 audio profiles for the specified audio port
    // Return codes:
    //  - ERROR_NONE: Success
    //  - ERROR_GENERAL: General error
    uint32_t DeviceInfo::endpoint_supportedms12audioprofiles(const JsonData::DeviceInfo::AudiocapabilitiesParamsInfo& params, JsonData::DeviceInfo::Supportedms12audioprofilesResultData& response) const
    {
        const AudioportType& audiooutput = params.AudioPort.Value();
        return Ms12Profiles(static_cast<Exchange::IDeviceAudioCapabilities::AudioOutput>(audiooutput), response.SupportedMS12AudioProfiles);
    }

} // namespace Plugin

}
