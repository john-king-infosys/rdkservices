#include "../SoC_abstraction.h"

#include "dsMgr.h"
#include "libIBusDaemon.h"
#include "host.hpp"
#include "exception.hpp"
#include "audioOutputPort.hpp"
#include "audioOutputPortType.hpp"
#include "audioOutputPortConfig.hpp"
#include "manager.hpp"
#include "dsUtl.h"
#include "dsError.h"
#include "list.hpp"
#include "libIBus.h"

#include "utils.h"

#include "nexus_types.h"
#include "nexus_core_utils.h"
#include "nexus_platform.h"
#include "nxclient.h"
#include "nexus_simple_stc_channel.h"
#include "nexus_simple_audio_decoder.h"


void Soc_Initialize()
{
    printf("Soc_Initialize DISABLED FOR NOW");
    // Utils::IARM::init();
    // device::Manager::Initialize();
}

void Soc_Deinitialize()
{
    printf("Soc_Deinitialize DISABLED FOR NOW");
    // device::Manager::DeInitialize();
}

void SoC_ChangePrimaryVol(MixGain gain, int volume)
{
    printf("SoC_ChangePrimaryVol DISABLED FOR NOW: vol=%d\n", volume);

    // disabled chaning the 'main' volume - there are NEXUS_SimpleAudioDecoder_SetSolo/NEXUS_SimpleAudioDecoder_SetSoloEnd
    // calls directly in AudioPlayer instead

    // const string audioPort = "HDMI0";
    // device::AudioOutputPort &aPort = device::Host::getInstance().getAudioOutputPort(audioPort);
    // aPort.setLevel(1.f * volume / 100.f);

}
