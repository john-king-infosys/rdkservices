#include "../SoC_abstraction.h"
#include "audioOutputPort.hpp"
#include "audioOutputPortType.hpp"
#include "audioOutputPortConfig.hpp"
#include "manager.hpp"

void Soc_Initialize()
{
    Utils::IARM::init();
    device::Manager::Initialize();
}

void Soc_Deinitialize()
{
    device::Manager::DeInitialize();
}

void SoC_ChangePrimaryVol(MixGain gain, int volume)
{
    device::AudioOutputPort aPort = device::Host::getInstance().getAudioOutputPort(audioPort);
    aPort.setLevel(level);
}
