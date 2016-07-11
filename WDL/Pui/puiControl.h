
// puiControl.h
// PUI
// Created by Gary Newby on 12/30/15.

#pragma once

#include "puiUI.h"
#include "puiCommon.h"

class PuiUI;
class PuiControl {
    
public:
    
    PuiControl(void *puiInstance, int px, int py, int pw, int ph) {
        pui = (PuiUI *)puiInstance;
        x = px;
        y = py;
        width = pw;
        height = ph;
        pressed = mouseIsDown = originalPressed = newPressed = false;
        dirty = true;
        
        a = 0.99;
        b = 1.0 - a;
        z = 0;
    }
    
    virtual bool draw() = 0;
    virtual void mouseUp(double x, double y) = 0;
    virtual void mouseDown(double x, double y) = 0;
    virtual void mouseMove(double x, double y) = 0;
    
    void setDirty(bool value) {
        dirty = value;
    }
    
    // Graphics & Helpers
    void roundRect(cairo_t* cr, double x, double y, double w, double h) {
        const double radius  = 10.0;
        const double degrees = M_PI / 180.0;
        cairo_new_sub_path(cr);
        cairo_arc(cr, x + w - radius, y + radius, radius, -90 * degrees, 0 * degrees);
        cairo_arc(cr, x + w - radius, y + h - radius, radius, 0 * degrees, 90 * degrees);
        cairo_arc(cr, x + radius, y + h - radius, radius, 90 * degrees, 180 * degrees);
        cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
        cairo_close_path(cr);
    }
    
    bool inside(double px, double py) {
        return (px >= x && px <= x + width && py >= y && py <= y + height);
    }
    
    double smooth(double in) {
        z = (in * b) + (z * a);
        return z;
    }
    
protected:
    
    PuiUI *pui;
    
    int x, y, width, height;
    bool dirty, mouseIsDown, pressed, originalPressed, newPressed;
    double a, b, z;
};
