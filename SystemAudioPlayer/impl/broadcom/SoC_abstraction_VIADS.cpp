#include "../SoC_abstraction.h"


static std::shared_ptr<WPEFramework::JSONRPC::LinkType<WPEFramework::Core::JSON::IElement>> dsPlugin;

void Soc_Initialize()
{
    Core::SystemInfo::SetEnvironment(_T("THUNDER_ACCESS"), (_T("127.0.0.1:9998")));
    dsPlugin =  make_shared<WPEFramework::JSONRPC::LinkType<WPEFramework::Core::JSON::IElement>>("org.rdk.HdmiCecSink.1", "");
}

void Soc_Deinitialize()
{
}

void SoC_ChangePrimaryVol(MixGain gain, int volume)
{
    JsonObject hdmiCecSinkResult;
    JsonObject param;
    hdmiCecSinkPlugin->Invoke<JsonObject, JsonObject>(2000, "setupARCRouting", param, hdmiCecSinkResult);
}
