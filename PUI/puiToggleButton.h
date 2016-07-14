
// puiToggleButton.h
// PUI
// Created by Gary Newby on 12/30/15.

#pragma once

#include "puiControl.h"


class PuiToggleButton : public PuiControl {
    
public:
    
    PuiToggleButton(PuiUI *puiInstance, int px, int py, int pw, int ph, std::string plabel) : PuiControl(puiInstance, px, py, pw, ph) {
        label = plabel;
    }
    
    bool draw() {
        
        bool needsRedraw = dirty;
        cairo_t *cr = pui->getContext();
        if (dirty && cr) {
            if (pressed) {
                cairo_set_source_rgba(cr, 0.4, 0.9, 0.1, 1);
            } else {
                cairo_set_source_rgba(cr, 0.3, 0.5, 0.1, 1);
            }
            
            // back
            roundRect(cr, x, y, width, height);
            
            cairo_fill_preserve(cr);
            
            // border
            cairo_set_source_rgba(cr, 0.4, 0.9, 0.1, 1);
            cairo_set_line_width(cr, 4.0);
            cairo_stroke(cr);
            
            // label
            cairo_text_extents_t extents;
            cairo_set_font_size(cr, height/2.5);
            cairo_text_extents(cr, label.c_str(), &extents);
            cairo_move_to(cr,(x + width / 2.0) - extents.width / 2.0, (y + height / 2.0) + extents.height / 2.0);
            cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
            cairo_show_text(cr, label.c_str());
            
            // Retina?
            //cairo_scale(cr, 2.0, 2.0);
        }
        
        setDirty(false);
        return needsRedraw;
    }
    
    void mouseDown(double px, double py) {
        if (inside(px, py)) {
            mouseIsDown = true;
            originalPressed = pressed;
            pressed = !pressed;
            newPressed = pressed;
            setDirty(true);
        }
    }
    
    void mouseUp(double px, double py) {
        if (mouseIsDown) {
            if (inside(px, py)) {
                pressed = newPressed;
            } else {
                pressed = originalPressed;
                setDirty(true);
            }
        }
        mouseIsDown = false;
    }
    
    void mouseMove(double px, double py) {
        if (mouseIsDown) {
            if (inside(px, py)) {
                setDirty(pressed != newPressed);
                pressed = newPressed;
                std::cout << "mouseMove:" << label << "\n";
            } else {
                setDirty(pressed != originalPressed);
                pressed = originalPressed;
            }
        }
    }
    
    void scroll(int x, int y, float dx, float dy) {
    }
    
private:
    
    std::string label;
};


