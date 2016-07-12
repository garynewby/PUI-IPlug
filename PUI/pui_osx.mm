
// pui_osx.mm
// PUI
// Created by Gary Newby on 12/30/15.
// Based on by PuGL by David Robillard <http://drobilla.net>

#import <Cocoa/Cocoa.h>
#include <stdlib.h>
#include "puiUI.h"


#pragma mark - PuiWindow

@interface PuiWindow : NSWindow {
    PuiUI *pui;
}

- (id)initWithContentRect:(NSRect)contentRect styleMask:(unsigned int)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag;
- (void)setPuiview:(PuiUI *)puiInstance;
- (BOOL)windowShouldClose:(id)sender;
- (BOOL)canBecomeKeyWindow:(id)sender;
@end


@implementation PuiWindow

- (id)initWithContentRect:(NSRect)contentRect styleMask:(unsigned int)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag
{
    if (![super initWithContentRect:contentRect styleMask:(NSClosableWindowMask | NSTitledWindowMask | NSResizableWindowMask) backing:NSBackingStoreBuffered defer:NO]) {
        return nil;
    }
    
    [self setAcceptsMouseMovedEvents:YES];
    return self;
}

- (void)setPuiview:(PuiUI *)puiInstance
{
    pui = puiInstance;
    [self setContentSize:NSMakeSize(pui->getWidth(), pui->getHeight())];
}

- (BOOL)windowShouldClose:(id)sender
{
    pui->close();
    return YES;
}

- (BOOL)canBecomeKeyWindow
{
    return YES;
}

- (BOOL)canBecomeMainWindow
{
    return YES;
}

- (BOOL)canBecomeKeyWindow:(id)sender
{
    return NO;
}

@end


#pragma mark - PuiNSView

typedef NSView PuiNSViewType;


@interface PuiNSView : PuiNSViewType {
    
    PuiUI *pui;
    NSTrackingArea* trackingArea;
}

- (void)setPuiview:(PuiUI *)puiInstance;
- (void)reshape;
- (void)drawRect:(NSRect)rect;
- (void)mouseEntered:(NSEvent*)event;
- (void)mouseExited:(NSEvent*)event;
- (void)mouseMoved:(NSEvent*)event;
- (void)mouseDragged:(NSEvent*)event;
- (void)rightMouseDragged:(NSEvent*)event;
- (void)mouseDown:(NSEvent*)event;
- (void)mouseUp:(NSEvent*)event;
- (void)rightMouseDown:(NSEvent*)event;
- (void)rightMouseUp:(NSEvent*)event;
- (void)scrollWheel:(NSEvent*)event;
- (void)keyDown:(NSEvent*)event;
- (void)keyUp:(NSEvent*)event;
- (void)flagsChanged:(NSEvent*)event;
- (NSPoint)eventLocation:(NSEvent*)event;


@end

@implementation PuiNSView


- (void)setPuiview:(PuiUI *)puiInstance
{
    pui = puiInstance;
}

- (void)reshape
{
    if (!pui) {
        return;
    }
    const NSRect bounds = [self bounds];
    const PuiEventConfigure ev =  {PUI_CONFIGURE, false, bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height};
    pui->dispatchEvent((PuiEvent *)&ev);
}

- (void)drawRect:(NSRect)rect
{
    CGContextRef cairoContext = cairo_quartz_surface_get_cg_context(pui->getSurface());
    CGImageRef image = CGBitmapContextCreateImage(cairoContext);
    CGContextRef currentContext = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextDrawImage(currentContext, CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height), image);
    CGImageRelease(image);
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

-(void)updateTrackingAreas
{
    if (!trackingArea) {
        const int opts = (NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveInKeyWindow); //NSTrackingActiveInKeyWindow
        trackingArea = [ [NSTrackingArea alloc] initWithRect:[self bounds] options:opts owner:self userInfo:nil];
        [self addTrackingArea:trackingArea];
    }
}

- (void) viewWillMoveToWindow:(NSWindow *)newWindow {
    // Setup a new tracking area when the view is added to the window.
    [self updateTrackingAreas];
}

- (NSPoint) eventLocation:(NSEvent*)event
{
    return [self convertPoint:[event locationInWindow] fromView:nil];
}

- (void)mouseEntered:(NSEvent*)theEvent
{
    [self updateTrackingAreas];
}

- (void)mouseExited:(NSEvent*)theEvent
{
    [self updateTrackingAreas];
}

- (void)mouseMoved:(NSEvent*)event
{
    const NSPoint wloc = [self eventLocation:event];
    const NSPoint rloc = [NSEvent mouseLocation];
    
    const PuiEventMotion ev   =  {
        PUI_MOTION_NOTIFY,
        false,
        static_cast<uint32_t>([event timestamp]),
        wloc.x,
        pui->getHeight() - wloc.y,
        rloc.x,
        [[NSScreen mainScreen] frame].size.height - rloc.y,
        [self getModifiers:event],
        0,
        1
    };
    
    pui->dispatchEvent((PuiEvent*)&ev);
    [self updateTrackingAreas];
}

- (void)mouseDragged:(NSEvent*)event
{
    [self mouseMoved: event];
}

- (void)rightMouseDragged:(NSEvent*)event
{
    [self mouseMoved: event];
}

- (void)mouseDown:(NSEvent*)event
{
    const NSPoint wloc = [self eventLocation:event];
    const NSPoint rloc = [NSEvent mouseLocation];
    const PuiEventButton ev   =  {
        PUI_BUTTON_PRESS,
        false,
        static_cast<uint32_t>([event timestamp]),
        wloc.x, pui->getHeight() - wloc.y,
        rloc.x,
        [[NSScreen mainScreen] frame].size.height - rloc.y,
        [self getModifiers:event]
    };
    
    pui->dispatchEvent((PuiEvent*)&ev);
}

- (void)mouseUp:(NSEvent*)event
{
    const NSPoint wloc = [self eventLocation:event];
    const NSPoint rloc = [NSEvent mouseLocation];
    const PuiEventButton ev   =  {
        PUI_BUTTON_RELEASE,
        false,
        static_cast<uint32_t>([event timestamp]),
        wloc.x,
        pui->getHeight() - wloc.y,
        rloc.x,
        [[NSScreen mainScreen] frame].size.height - rloc.y,
        [self getModifiers:event]
    };
    
    pui->dispatchEvent((PuiEvent*)&ev);
    [self updateTrackingAreas];
}

- (void)rightMouseDown:(NSEvent*)event
{
    [self mouseDown: event];
}

- (void)rightMouseUp:(NSEvent*)event
{
    [self mouseUp: event];
}

- (void)scrollWheel:(NSEvent*)event
{
    const NSPoint wloc = [self eventLocation:event];
    const NSPoint rloc = [NSEvent mouseLocation];
    const PuiEventScroll ev =  {
        PUI_SCROLL,
        false,
        static_cast<uint32_t>([event timestamp]),
        wloc.x,
        pui->getHeight() - wloc.y,
        rloc.x,
        [[NSScreen mainScreen] frame].size.height - rloc.y,
        [self getModifiers:event],
        [event deltaX],
        [event deltaY]
    };
    
    pui->dispatchEvent((PuiEvent*)&ev);
    [self updateTrackingAreas];
}

- (unsigned int)getModifiers:(NSEvent *) ev
{
    const unsigned modifierFlags = (unsigned)[ev modifierFlags];
    pui->setEventTimeStampMs(fmod([ev timestamp] * 1000.0, UINT32_MAX));
    
    unsigned mods = 0;
    mods |= (modifierFlags & NSShiftKeyMask)     ? PUI_MOD_SHIFT : 0;
    mods |= (modifierFlags & NSControlKeyMask)   ? PUI_MOD_CTRL  : 0;
    mods |= (modifierFlags & NSAlternateKeyMask) ? PUI_MOD_ALT   : 0;
    mods |= (modifierFlags & NSCommandKeyMask)   ? PUI_MOD_SUPER : 0;
    
    return (unsigned int)mods;
}

- (void)keyDown:(NSEvent*)event
{
    if (pui->getIgnoreKeyRepeat() && [event isARepeat]) {
        return;
    }
    
    const NSPoint   wloc  = [self eventLocation:event];
    const NSPoint   rloc  = [NSEvent mouseLocation];
    const NSString *chars = [event characters];
    const char     *str   = [chars UTF8String];
    
    PuiEventKey ev =  {
        PUI_KEY_PRESS,
        false,
        static_cast<uint32_t>([event timestamp]),
        wloc.x,
        pui->getHeight() - wloc.y,
        rloc.x,
        [[NSScreen mainScreen] frame].size.height - rloc.y,
        [self getModifiers:event],
        [event keyCode],
        pui->decodeUTF8((const uint8_t*)str),
        PuiKey(0),  // TODO: Special keys?
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        false
    };
    
    strncpy((char*)ev.utf8, str, 8);
    pui->dispatchEvent((PuiEvent*)&ev);
}

- (void)keyUp:(NSEvent*)event
{
    const NSPoint   wloc  = [self eventLocation:event];
    const NSPoint   rloc  = [NSEvent mouseLocation];
    const NSString *chars = [event characters];
    const char     *str   = [chars UTF8String];
    
    const PuiEventKey ev =  {
        PUI_KEY_RELEASE,
        false,
        static_cast<uint32_t>([event timestamp]),
        wloc.x,
        pui->getHeight() - wloc.y,
        rloc.x,
        [[NSScreen mainScreen] frame].size.height - rloc.y,
        [self getModifiers:event],
        [event keyCode],
        pui->decodeUTF8((const uint8_t*)str),
        PuiKey(0),  // TODO: Special keys?
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        false,
    };
    
    strncpy((char*)ev.utf8, str, 8);
    pui->dispatchEvent((PuiEvent*)&ev);
}

- (void)flagsChanged:(NSEvent*)event
{
    const unsigned mods = [self getModifiers:event];
    pui->setMods(mods);
}

- (void)showWindow
{
    //NSWindow *window = (__bridge NSWindow *)pui->getWindow();
    //[window setIsVisible:YES];
}

void showWindow(void *Object_C_View)
{
    [(__bridge id)Object_C_View showWindow];
}

void postRedisplay(void *Object_C_View)
{
    [(__bridge NSView *)Object_C_View setNeedsDisplay:YES];
}

@end

#pragma mark - createWindow

void createWindow(PuiUI *gui, void *window, const char *title) {
    
    gui->setApp((__bridge void *)[NSApplication sharedApplication]);
    
    // Mac bundlepath
    NSString *bundleID = [NSString stringWithUTF8String:BUNDLE_ID];
    NSBundle *pluginBundle = [NSBundle bundleWithIdentifier:bundleID];
    std::string resourcePath = [[pluginBundle resourcePath] UTF8String];
    gui->setResourcePath(resourcePath);
    
    if (window) {
        
        // Embed in existing window
        PuiNSView *view = [[PuiNSView alloc] initWithFrame:NSMakeRect(0, 0, gui->getWidth(), gui->getHeight())];
        gui->setView((__bridge void *)view);
        gui->setWindow(window);
        
        [(__bridge NSView *)window addSubview:(__bridge PuiNSView *)gui->getView()];
        
        [view setPuiview:gui];
        
    } else {
        
        // Create a new window
        NSString* titleString = [[NSString alloc] initWithBytes:title length:strlen(title) encoding:NSUTF8StringEncoding];
        
        PuiWindow *newWindow = [PuiWindow new];
        [newWindow setPuiview:gui];
        [newWindow setTitle:titleString];
        [newWindow setContentMinSize:NSMakeSize(gui->getMinWidth(), gui->getMinHeight())];
        newWindow.contentView = (__bridge PuiNSView *)gui->getView();
        
        gui->setWindow((__bridge void *)newWindow);
        
        [(__bridge NSApplication *)gui->getApp() activateIgnoringOtherApps:YES];
        [newWindow makeFirstResponder:(__bridge PuiNSView *)gui->getView()];
        [newWindow makeKeyAndOrderFront:newWindow];
        
        if (gui->getResizable()) {
            [(__bridge PuiNSView *)gui->getView() setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
        }
        
    }
    
}



