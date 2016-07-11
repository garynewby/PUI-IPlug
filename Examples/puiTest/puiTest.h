
// puiTest.h
// PUI
// Created by Gary Newby on 12/30/15.


#ifndef __puiTest__
#define __puiTest__

#include "IPlug_include_in_plug_hdr.h"
#include "puiTest_Effect.h"
#include "puiTest_UI.h"

class IRadioControl;
class IInfoControl;
class PuiUI;

class puiTest : public IPlug {
  
public:
  
  puiTest(IPlugInstanceInfo instanceInfo);
  ~puiTest();
  
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void OnParamChange(int paramIdx);
  void Reset();
  void OnGUIOpen(void *ptr);
  
  
private:
  
  puiTest_Effect *Effect;
  puiTestUI *UI;
  int sampleRate;
  
};



#endif
