
// puiTest.cpp
// PUI
// Created by Gary Newby on 12/30/15.


#include "puiTest.h"
#include "IPlug_include_in_plug_src.h"
#include "resource.h"


puiTest::puiTest(IPlugInstanceInfo instanceInfo) : IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo) , Effect(0), UI(0)
{
	IPlugBase::SetLatency(PLUG_LATENCY);
    
	sampleRate = (int)GetSampleRate();
	Effect = new puiTest_Effect(sampleRate);

	// Parameters
	GetParam(kIn)->InitDouble  ("Input2",  0.5, 0.0, 1.0, 0.01, "");
    GetParam(kOut)->InitDouble ("Output", 0.5, 0.0, 1.0, 0.01, "");
    GetParam(kBypass)->InitBool("Bypass", 0, "");
    
	MakePresetFromNamedParams(
        "Init",     kNumParams,
        kIn,        0.5,
        kOut,       0.5,
        kBypass,    false
    );
}

puiTest::~puiTest() {
	delete Effect;
    if (UI) {
        delete UI;
    }
}

void puiTest::OnGUIOpen(void *ptr) {
    // Create window
    UI = new puiTestUI(ptr, GUI_WIDTH, GUI_HEIGHT, true, true);
}

void puiTest::OnGUIClose() {
    delete UI;
    UI = NULL;
}

void puiTest::Reset() {

	if (GetSampleRate() != mSampleRate) {
		sampleRate = (int)GetSampleRate();
		Effect->setSR(sampleRate);
	}
}

void puiTest::OnParamChange(int paramIdx) {

    Effect->set(paramIdx, GetParam(paramIdx)->GetNormalized(), sampleRate);
	
}

void puiTest::ProcessDoubleReplacing(double** inputs, double** outputs, int sampleFrames) {
    
    if (Effect->Bypassed()) {
        this->PassThroughBuffers(0.0, sampleFrames);
        return;
    }
    
    bool isMono = !IsInChannelConnected(1);

	if (isMono) {
		Effect->process_mono(inputs, outputs, sampleFrames);
	} else {
		Effect->process_stereo(inputs, outputs, sampleFrames);
	}
}




