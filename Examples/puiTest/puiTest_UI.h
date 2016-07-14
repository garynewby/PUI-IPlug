
// puiTest_UI.h
// PUI
// Created by Gary Newby on 12/30/15.


#pragma once

#include "puiUI.h"
#include "puiControl.h"
#include "puiToggleButton.h"
#include "puiKnobMultiControl.h"

class puiTestUI : public PuiUI  {
    
public:
    
    puiTestUI(void *window, int gWidth, int gHeight, bool resizable, bool ignoreKeyRepeat) : PuiUI(window, gWidth, gHeight, resizable, ignoreKeyRepeat) {
                
        controls.push_back(new PuiKnobMultiControl(this, 50, 200, 60, pathForImageWithName("knob.png").c_str()));
        controls.push_back(new PuiKnobMultiControl(this, 150, 200, 60, pathForImageWithName("knob.png").c_str()));
        controls.push_back(new PuiKnobMultiControl(this, 250, 200, 60, pathForImageWithName("knob.png").c_str()));

        controls.push_back(new PuiToggleButton(this, 50,  50, 100, 44, "Attack"));
        controls.push_back(new PuiToggleButton(this, 200, 50, 100, 44, "Release"));
        controls.push_back(new PuiToggleButton(this, 350, 50, 100, 44, "Bypass"));
        
        forceRedraw();
    }
    
private:
    
    
};




