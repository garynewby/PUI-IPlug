
// puiCommon.h
// PUI
// Created by Gary Newby on 12/30/15.

#pragma once

#include <string>
#include <iostream>
#include <list>
#include "cairo/cairo.h"
#include "cairo/cairo-quartz.h"


typedef enum {
    PUI_SUCCESS = 0
} PuiStatus;

typedef enum {
    PUI_CHAR_BACKSPACE = 0x08,
    PUI_CHAR_ESCAPE    = 0x1B,
    PUI_CHAR_DELETE    = 0x7F
} PuiChar;

typedef enum {
    PUI_MOD_SHIFT = 1,
    PUI_MOD_CTRL  = 1 << 1,
    PUI_MOD_ALT   = 1 << 2,
    PUI_MOD_SUPER = 1 << 3
} PuiMod;

// Special (non-Unicode) keyboard keys
typedef enum {
    PUI_KEY_F1 = 1,
    PUI_KEY_F2,
    PUI_KEY_F3,
    PUI_KEY_F4,
    PUI_KEY_F5,
    PUI_KEY_F6,
    PUI_KEY_F7,
    PUI_KEY_F8,
    PUI_KEY_F9,
    PUI_KEY_F10,
    PUI_KEY_F11,
    PUI_KEY_F12,
    PUI_KEY_LEFT,
    PUI_KEY_UP,
    PUI_KEY_RIGHT,
    PUI_KEY_DOWN,
    PUI_KEY_PAGE_UP,
    PUI_KEY_PAGE_DOWN,
    PUI_KEY_HOME,
    PUI_KEY_END,
    PUI_KEY_INSERT,
    PUI_KEY_SHIFT,
    PUI_KEY_CTRL,
    PUI_KEY_ALT,
    PUI_KEY_SUPER
} PuiKey;

#pragma mark - Events

typedef enum {
    PUI_BUTTON_PRESS,
    PUI_BUTTON_RELEASE,
    PUI_CONFIGURE,
    PUI_EXPOSE,
    PUI_KEY_PRESS,
    PUI_KEY_RELEASE,
    PUI_ENTER_NOTIFY,
    PUI_LEAVE_NOTIFY,
    PUI_MOTION_NOTIFY,
    PUI_NOTHING,
    PUI_SCROLL,
    PUI_FOCUS_IN,
    PUI_FOCUS_OUT
} PuiEventType;

/**
 Reason for a PuiEventCrossing.
 */
typedef enum {
    PUI_CROSSING_NORMAL,  /**< Crossing due to pointer motion. */
    PUI_CROSSING_GRAB,    /**< Crossing due to a grab. */
    PUI_CROSSING_UNGRAB   /**< Crossing due to a grab release. */
} PuiCrossingMode;

/**
 Common header for all event structs.
 */
typedef struct {
    PuiEventType type;        /**< Event type. */
    bool          send_event;  /**< True iff event was sent explicitly. */
} PuiEventAny;

/**
 Button press or release event.
 
 For event types PUI_BUTTON_PRESS and PUI_BUTTON_RELEASE.
 */
typedef struct {
    PuiEventType type;        /**< PUI_BUTTON_PRESS or PUI_BUTTON_RELEASE. */
    bool          send_event;  /**< True iff event was sent explicitly. */
    uint32_t      time;        /**< Time in milliseconds. */
    double        x;           /**< View-relative X coordinate. */
    double        y;           /**< View-relative Y coordinate. */
    double        x_root;      /**< Root-relative X coordinate. */
    double        y_root;      /**< Root-relative Y coordinate. */
    unsigned      state;       /**< Bitwise OR of PuiMod flags. */
} PuiEventButton;

/**
 Configure event for when window size or position has changed.
 */
typedef struct {
    PuiEventType type;        /**< PUI_CONFIGURE. */
    bool          send_event;  /**< True iff event was sent explicitly. */
    double        x;           /**< New parent-relative X coordinate. */
    double        y;           /**< New parent-relative Y coordinate. */
    double        width;       /**< New width. */
    double        height;      /**< New height. */
} PuiEventConfigure;

/**
 Expose event for when a region must be redrawn.
 */
typedef struct {
    PuiEventType type;        /**< PUI_EXPOSE. */
    bool          send_event;  /**< True iff event was sent explicitly. */
    double        x;           /**< View-relative X coordinate. */
    double        y;           /**< View-relative Y coordinate. */
    double        width;       /**< Width of exposed region. */
    double        height;      /**< Height of exposed region. */
    int           count;       /**< Number of expose events to follow. */
} PuiEventExpose;

/**
 Key press/release event.
 
 Keys that correspond to a Unicode character have `character` and `utf8` set.
 Other keys will have `character` 0, but `special` may be set if this is a
 known special key.
 
 A key press may be part of a multi-key sequence to generate a wide
 character.  If `filter` is set, this event is part of a multi-key sequence
 and should be ignored if the application is reading textual input.
 Following the series of filtered press events, a press event with
 `character` and `utf8` (but `keycode` 0) will be sent.  This event will have
 no corresponding release event.
 
 Generally, an application should either work with raw keyboard press/release
 events based on `keycode` (ignoring events with `keycode` 0), or
 read textual input based on `character` or `utf8` (ignoring releases and
 events with `filter` 1).  Note that blindly appending `utf8` will yield
 incorrect text, since press events are sent for both individually composed
 keys and the resulting synthetic multi-byte press.
 */
typedef struct {
    PuiEventType type;        /**< PUI_KEY_PRESS or PUI_KEY_RELEASE. */
    bool          send_event;  /**< True iff event was sent explicitly. */
    uint32_t      time;        /**< Time in milliseconds. */
    double        x;           /**< View-relative X coordinate. */
    double        y;           /**< View-relative Y coordinate. */
    double        x_root;      /**< Root-relative X coordinate. */
    double        y_root;      /**< Root-relative Y coordinate. */
    unsigned      state;       /**< Bitwise OR of PuiMod flags. */
    unsigned      keycode;     /**< Raw key code. */
    uint32_t      character;   /**< Unicode character code, or 0. */
    PuiKey       special;     /**< Special key, or 0. */
    uint8_t       utf8[8];     /**< UTF-8 string. */
    bool          filter;      /**< True if part of a multi-key sequence. */
} PuiEventKey;

/**
 Pointer crossing event (enter and leave).
 */
typedef struct {
    PuiEventType    type;        /**< PUI_ENTER_NOTIFY or PUI_LEAVE_NOTIFY. */
    bool             send_event;  /**< True iff event was sent explicitly. */
    uint32_t         time;        /**< Time in milliseconds. */
    double           x;           /**< View-relative X coordinate. */
    double           y;           /**< View-relative Y coordinate. */
    double           x_root;      /**< Root-relative X coordinate. */
    double           y_root;      /**< Root-relative Y coordinate. */
    unsigned         state;       /**< Bitwise OR of PuiMod flags. */
    PuiCrossingMode mode;        /**< Reason for crossing. */
} PuiEventCrossing;

/**
 Pointer motion event.
 */
typedef struct {
    PuiEventType type;        /**< PUI_MOTION_NOTIFY. */
    bool          send_event;  /**< True iff event was sent explicitly. */
    uint32_t      time;        /**< Time in milliseconds. */
    double        x;           /**< View-relative X coordinate. */
    double        y;           /**< View-relative Y coordinate. */
    double        x_root;      /**< Root-relative X coordinate. */
    double        y_root;      /**< Root-relative Y coordinate. */
    unsigned      state;       /**< Bitwise OR of PuiMod flags. */
    bool          is_hint;     /**< True iff this event is a motion hint. */
    bool          focus;       /**< True iff this is the focused window. */
} PuiEventMotion;

/**
 Scroll event.
 
 The scroll distance is expressed in "lines", an arbitrary unit that
 corresponds to a single tick of a detented mouse wheel.  For example, `dy` =
 1.0 scrolls 1 line up.  Some systems and devices support finer resolution
 and/or higher values for fast scrolls, so programs should handle any value
 gracefully.
 */
typedef struct {
    PuiEventType type;        /**< PUI_SCROLL. */
    bool          send_event;  /**< True iff event was sent explicitly. */
    uint32_t      time;        /**< Time in milliseconds. */
    double        x;           /**< View-relative X coordinate. */
    double        y;           /**< View-relative Y coordinate. */
    double        x_root;      /**< Root-relative X coordinate. */
    double        y_root;      /**< Root-relative Y coordinate. */
    unsigned      state;       /**< Bitwise OR of PuiMod flags. */
    double        dx;          /**< Scroll X distance in lines. */
    double        dy;          /**< Scroll Y distance in lines. */
} PuiEventScroll;

/**
 Keyboard focus event.
 */
typedef struct {
    PuiEventType type;        /**< PUI_FOCUS_IN or PUI_FOCUS_OUT. */
    bool          send_event;  /**< True iff event was sent explicitly. */
    bool          grab;        /**< True iff this is a grab/ungrab event. */
} PuiEventFocus;

/**
 Interface event.
 
 This is a union of all event structs.  The `type` must be checked to
 determine which fields are safe to access.  A pointer to PuiEvent can
 either be cast to the appropriate type, or the union members used.
 */
typedef union {
    PuiEventType      type;       /**< Event type. */
    PuiEventAny       any;        /**< Valid for all event types. */
    PuiEventButton    button;     /**< PUI_BUTTON_PRESS, PUI_BUTTON_RELEASE. */
    PuiEventConfigure configure;  /**< PUI_CONFIGURE. */
    PuiEventCrossing  crossing;   /**< PUI_ENTER_NOTIFY, PUI_LEAVE_NOTIFY. */
    PuiEventExpose    expose;     /**< PUI_EXPOSE. */
    PuiEventKey       key;        /**< PUI_KEY_PRESS, PUI_KEY_RELEASE. */
    PuiEventMotion    motion;     /**< PUI_MOTION_NOTIFY. */
    PuiEventScroll    scroll;     /**< PUI_SCROLL. */
    PuiEventFocus     focus;      /**< PUI_FOCUS_IN, PUI_FOCUS_OUT. */
} PuiEvent;


// Trampolines
//void createWindow(puiUI *gui, void *window, const char *title);
void showWindow(void *Object_C_View);
void postRedisplay(void *Object_C_View);


// Structs

typedef struct {
    double left;
    double top;
    double right;
    double bottom;
} PRect;

typedef struct {
    double x;
    double y;
    
    bool insideRect(const PRect& r) const {
        return x >= r.left && x <= r.right && y >= r.top && y <= r.bottom;
    }
} PPoint;


