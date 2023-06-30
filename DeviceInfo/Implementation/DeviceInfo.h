#pragma once

#include "../Module.h"
#ifdef USE_THUNDER_R4
#include <interfaces/IDeviceInfo.h>
#else
#include <interfaces/IDeviceInfo2.h>
#endif /* USE_THUNDER_R4 */

namespace WPEFramework {
namespace Plugin {
    class DeviceInfoImplementation : public Exchange::IDeviceInfo {
    private:
        DeviceInfoImplementation(const DeviceInfoImplementation&) = delete;
        DeviceInfoImplementation& operator=(const DeviceInfoImplementation&) = delete;

    public:
        DeviceInfoImplementation();

        BEGIN_INTERFACE_MAP(DeviceInfoImplementation)
        INTERFACE_ENTRY(Exchange::IDeviceInfo)
        END_INTERFACE_MAP

    private:
        // IDeviceInfo interface
        uint32_t Configure(const PluginHost::IShell* service) override;

        uint32_t Make(string& value) const override;
        uint32_t ModelName(string& value) const override;
        uint32_t ModelYear(uint16_t& value) const override;
        uint32_t DeviceType(string& value) const override;
        uint32_t FriendlyName(string& value) const override;
        uint32_t DistributorId(string& value) const override;
        uint32_t SerialNumber(string& value) const override;
        uint32_t Sku(string& value) const override;
        uint32_t PlatformName(string& value) const override;
    };
}
}
