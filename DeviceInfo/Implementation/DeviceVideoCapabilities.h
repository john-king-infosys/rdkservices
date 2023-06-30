#pragma once

#include "../Module.h"
#ifdef USE_THUNDER_R4
#include <interfaces/IDeviceInfo.h>
#else
#include <interfaces/IDeviceInfo2.h>
#endif /* USE_THUNDER_R4 */

namespace WPEFramework {
namespace Plugin {
    class DeviceVideoCapabilities : public Exchange::IDeviceVideoCapabilities {
    private:
        DeviceVideoCapabilities(const DeviceVideoCapabilities&) = delete;
        DeviceVideoCapabilities& operator=(const DeviceVideoCapabilities&) = delete;

    public:
        DeviceVideoCapabilities();

        BEGIN_INTERFACE_MAP(DeviceVideoCapabilities)
        INTERFACE_ENTRY(Exchange::IDeviceVideoCapabilities)
        END_INTERFACE_MAP

    private:
        // IDeviceVideoCapabilities interface
        uint32_t VideoOutputs(IVideoOutputIterator*& videoOutputs /* @out */) const override;
        uint32_t DefaultResolution(const VideoOutput videoOutput /* @in */, ScreenResolution& defaultResolution /* @out */) const override;
        uint32_t Resolutions(const VideoOutput videoOutput /* @in */, IScreenResolutionIterator*& resolutions /* @out */) const override;
        uint32_t Hdcp(const VideoOutput videoOutput /* @in */, CopyProtection& hdcpVersion /* @out */) const override;
        uint32_t HostEDID(string& edid /* @out */) const override;
        uint32_t HDR(bool& supportsHDR /*@out*/) const override;
        uint32_t Atmos(bool& supportsAtmos /*@out*/) const override;
        uint32_t CEC(bool& supportsCEC /*@out*/) const override;
    };
}
}
