
// puiUI.h
// PUI
// Created by Gary Newby on 12/30/15.

#pragma once

#include "puiCommon.h"
#include "puiControl.h"
#include "IPlug_include_in_plug_hdr.h"


class PuiUI;
void createWindow(PuiUI *gui, void *window, const char *title);


class PuiUI {
    
public:
    
    PuiUI(void *window, int gWidth, int gHeight, bool resizable, bool ignoreKeyRepeat) {
        
        setWindowSize(gWidth, gHeight);
        setWindowMinSize(320, 240);
        setResizable(resizable);
        setIgnoreKeyRepeat(ignoreKeyRepeat);
        setTransientFor(window);
        
        createWindow(this, window, "PluglWindow");
        showWindow(view);
    }
    
    
    // Setters
    
    void setApp(void *application) {
        app = application;
    }
    
    void setWindow(void *pwindow) {
        window = pwindow;
    }
    
    void setView(void *pview) {
        view = pview;
    }
    
    void setWindowSize(int pwidth, int pheight) {
        width  = pwidth;
        height = pheight;
        surface = cairo_quartz_surface_create(CAIRO_FORMAT_ARGB32, width, height);
        context = cairo_create(surface);
    }
    
    void setWindowMinSize(int pwidth, int pheight) {
        min_width  = pwidth;
        min_height = pheight;
    }
    
    void setResizable(bool presizable) {
        resizable = presizable;
    }
    
    void setTransientFor(void *parent) {
        transient_parent = parent;
    }
    
    void setContextType(PuiContextType ptype) {
        ctx_type = ptype;
    }
    
    void setIgnoreKeyRepeat(bool ignore) {
        ignoreKeyRepeat = ignore;
    }
    
    void setMods(const unsigned pmods) {
        if ((pmods & PUI_MOD_SHIFT) != (mods & PUI_MOD_SHIFT)) {
            special(pmods & PUI_MOD_SHIFT, PUI_KEY_SHIFT);
        } else if ((mods & PUI_MOD_CTRL) != (mods & PUI_MOD_CTRL)) {
            special(pmods & PUI_MOD_CTRL, PUI_KEY_CTRL);
        } else if ((mods & PUI_MOD_ALT) != (mods & PUI_MOD_ALT)) {
            special(pmods & PUI_MOD_ALT, PUI_KEY_ALT);
        } else if ((mods & PUI_MOD_SUPER) != (mods & PUI_MOD_SUPER)) {
            special(pmods & PUI_MOD_SUPER, PUI_KEY_SUPER);
        }
        mods = pmods;
    }
    
    
    // Getters
    
    void *getApp() {
        return app;
    }
    
    void *getWindow() {
        return window;
    }
    
    void *getView() {
        return view;
    }
    
    uint32_t getEventTimestamp() {
        return event_timestamp_ms;
    }
    
    int getWidth() {
        return width;
    }
    
    int getHeight() {
        return height;
    }
    
    int getMinWidth() {
        return min_width;
    }
    
    int getMinHeight() {
        return min_height;
    }
    
    int getResizable() {
        return resizable;
    }
    
    bool getIgnoreKeyRepeat() {
        return ignoreKeyRepeat;
    }
    
    cairo_t *getContext() {
        return context;
    }
    
    cairo_surface_t *getSurface() {
        return surface;
    }
    
    //
    
    void reshape(int width, int height) {
        setWindowSize(width, height);
        forceRedraw();
    }
    
    void forceRedraw() {
        
        for_each(controls.begin(), controls.end(),[=](PuiControl *control) {
            control->setDirty(true);
        });
        display();
    }
    
    void display() {
        
        bool needsDisplay;
        for_each(controls.begin(), controls.end(), [&](PuiControl *control) {
            bool controlDidDraw = control->draw();
            if (controlDidDraw) {
                needsDisplay = true;
            }
        });
        
        if (needsDisplay) {
            postRedisplay((void *)view);
        }
    }
    
    void mouseDown(int x, int y) {
        for_each(controls.begin(), controls.end(),[=](PuiControl *control) {
            control->mouseDown(x, y);
        });
        display();
    }
    
    void mouseUp(int x, int y) {
        for_each(controls.begin(), controls.end(), [=](PuiControl *control) {
            control->mouseUp(x, y);
        });
        display();
    }
    
    void mouseMove(int x, int y) {
        for_each(controls.begin(), controls.end(), [=](PuiControl *control) {
            control->mouseMove(x, y);
        });
        display();
    }
    
    void scroll(int x, int y, float dx, float dy) {
        std::cout <<  x << "\n";
        std::cout <<  y << "\n";
        std::cout <<  dx << "\n";
        std::cout <<  dy << "\n";
    }
    
    void special(bool press, PuiKey key) {}
    void keyboard(bool press, uint32_t key) {}
    void close() {}
    void enterContext() {}
    void leaveContext(bool flush) {}
    
    uint32_t decodeUTF8(const uint8_t* buf) {
        // Return the code point for buf, or the replacement character on error.
#define FAIL_IF(cond) { if (cond) return 0xFFFD; }
        // http://en.wikipedia.org/wiki/UTF-8
        
        if (buf[0] < 0x80) {
            return buf[0];
        } else if (buf[0] < 0xC2) {
            return 0xFFFD;
        } else if (buf[0] < 0xE0) {
            FAIL_IF((buf[1] & 0xC0) != 0x80);
            return (buf[0] << 6) + buf[1] - 0x3080;
        } else if (buf[0] < 0xF0) {
            FAIL_IF((buf[1] & 0xC0) != 0x80);
            FAIL_IF(buf[0] == 0xE0 && buf[1] < 0xA0);
            FAIL_IF((buf[2] & 0xC0) != 0x80);
            return (buf[0] << 12) + (buf[1] << 6) + buf[2] - 0xE2080;
        } else if (buf[0] < 0xF5) {
            FAIL_IF((buf[1] & 0xC0) != 0x80);
            FAIL_IF(buf[0] == 0xF0 && buf[1] < 0x90);
            FAIL_IF(buf[0] == 0xF4 && buf[1] >= 0x90);
            FAIL_IF((buf[2] & 0xC0) != 0x80);
            FAIL_IF((buf[3] & 0xC0) != 0x80);
            return ((buf[0] << 18) + (buf[1] << 12) + (buf[2] << 6) + buf[3] - 0x3C82080);
        }
        return 0xFFFD;
    }
    
    void dispatchEvent(const PuiEvent* event) {
        switch (event->type) {
            case PUI_CONFIGURE:
                enterContext();
                width  = event->configure.width;
                height = event->configure.height;
                reshape(width, height);
                leaveContext(false);
                display();
                break;
            case PUI_EXPOSE:
                if (event->expose.count == 0) {
                    enterContext();
                    display();
                    redisplay = false;
                    leaveContext(true);
                }
                break;
            case PUI_MOTION_NOTIFY:
                event_timestamp_ms = event->motion.time;
                mods               = event->motion.state;
                mouseMove(event->motion.x, event->motion.y);
                break;
            case PUI_SCROLL:
                scroll(event->scroll.x, event->scroll.y, event->scroll.dx, event->scroll.dy);
                break;
            case PUI_BUTTON_PRESS:
                event_timestamp_ms = event->button.time;
                mods               = event->button.state;
                mouseDown(event->button.x, event->button.y);
                break;
            case PUI_BUTTON_RELEASE:
                event_timestamp_ms = event->button.time;
                mods               = event->button.state;
                mouseUp(event->button.x, event->button.y);
                break;
            case PUI_KEY_PRESS:
            case PUI_KEY_RELEASE:
                event_timestamp_ms = event->key.time;
                mods               = event->key.state;
                if (event->key.special) {
                    special(event->type == PUI_KEY_PRESS, event->key.special);
                } else if (event->key.character) {
                    keyboard(event->type == PUI_KEY_PRESS, event->key.character);
                }
                break;
            default:
                break;
        }
    }
    
    void setEventTimeStampMs(uint32_t value) {
         event_timestamp_ms = value;
    }
    
    void setResourcePath(std::string path) {
        resourcePath = path;
    }
    
    std::string pathForImageWithName(const char *fileName) {
        std::string imagePath = resourcePath + "/" + fileName;
        return imagePath;
    }
    
    
protected:
    
    PuiContextType ctx_type;
    
    cairo_t *context;
    cairo_surface_t *surface;
    
    std::list<PuiControl *> controls;
    std::string resourcePath;
    
    void *app;
    void *window;
    void *view;
    void *transient_parent;
    
    int width;
    int height;
    int min_width;
    int min_height;
    
    bool ignoreKeyRepeat;
    bool redisplay;
    bool resizable;
    bool needsDisplay;
    
    unsigned mods;
    uint32_t event_timestamp_ms;
    
};



