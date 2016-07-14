
// puiKnobMultiControl.h
// PUI
// Created by Gary Newby on 12/30/15.


#pragma once
#include "puiControl.h"


class PuiKnobMultiControl : public PuiControl {
    
public:
    
    PuiKnobMultiControl(PuiUI *puiInstance, int px, int py, int frameCount, const char* pimagePath) : PuiControl(puiInstance, px, py, 0, 0) {
        
        imagePath = pimagePath;
        
        // Load image from file
        imageSurface = cairo_image_surface_create_from_png(imagePath);
        
        width = cairo_image_surface_get_width(imageSurface);
        imageHeight = cairo_image_surface_get_height(imageSurface);
        
        value = 0.0;
        numberOfFrames = frameCount;
        increment = (1.0 / numberOfFrames) * 2.0;
        frameHeight = (imageHeight / numberOfFrames);
        height = frameHeight;
    }
    
    ~PuiKnobMultiControl() {
        cairo_surface_destroy(imageSurface);
    }
    
    bool draw() {
        bool needsRedraw = dirty;
        cairo_t *cr = pui->getContext();
        
        if (dirty && cr) {
            cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

            // Clip to rect
            cairo_rectangle (cr, x, y, width, height);
            cairo_clip (cr);
            
            double yOffset = (numberOfFrames * value * frameHeight);
            yOffset = BOUNDED(yOffset, 0, (imageHeight - frameHeight));
            cairo_surface_set_device_offset(imageSurface, 0, yOffset);
            
            // Draw image
            cairo_set_source_surface(cr, imageSurface, x, y);
            cairo_paint(cr);
            
            // Remove clip
            cairo_reset_clip (cr);
            cairo_set_source_surface(cr, pui->getSurface(), x, y);
        }
        
        setDirty(false);
        return needsRedraw;
    }
    
    void mouseDown(double px, double py) {
        if (inside(px, py)) {
            mouseIsDown = true;
            startY = py;
            setDirty(true);
        }
    }
    
    void mouseUp(double px, double py) {
        mouseIsDown = false;
    }
    
    void mouseMove(double px, double py) {
        if (mouseIsDown) {
            direction = (lastY - py) > 0;
            value += (direction ? increment : -increment);
            value = BOUNDED(value, 0.0, 1.0);
            std::cout << "value:" << value << "\n";
            lastY = py;
            setDirty(true);
        }
    }
    
    void scroll(int x, int y, float dx, float dy) {
        if (inside(x, y)) {
            direction = dy > 0;
            value += (direction ? increment : -increment);
            value = BOUNDED(value, 0.0, 1.0);
            setDirty(true);
        }
    }
    
private:
    
    bool direction;
    double startY;
    double lastY;
    double value;
    double increment;
    
    int imageHeight;
    int frameHeight;
    
    const char *imagePath;
    cairo_surface_t *imageSurface;
    
    int numberOfFrames;
    
};



