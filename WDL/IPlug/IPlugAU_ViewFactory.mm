#import <Cocoa/Cocoa.h>
//#import "../IPlug/IGraphicsCocoa.h"
#import <AudioUnit/AudioUnit.h>
#import <AudioUnit/AUCocoaUIView.h>
#import "IPlug_include_in_plug_hdr.h"
#import "resource.h"   // This is your plugin's resource.h.


@interface VIEW_CLASS : NSObject <AUCocoaUIBase>
{
    IPlugBase* mPlug;
}
- (id) init;
- (NSView*) uiViewForAudioUnit: (AudioUnit) audioUnit withSize: (NSSize) preferredSize;
- (unsigned) interfaceVersion;
- (NSString*) description;
@end


@implementation VIEW_CLASS

- (id) init
{
    TRACE;
    mPlug = 0;
    return [super init];
}

- (NSView*) uiViewForAudioUnit: (AudioUnit) audioUnit withSize: (NSSize) preferredSize
{
    TRACE;
    mPlug = (IPlugBase*) GetComponentInstanceStorage(audioUnit);
    if (mPlug) {
//        IGraphics* pGraphics = mPlug->GetGUI();
//        if (pGraphics) {
//            IGRAPHICS_COCOA* pView = (IGRAPHICS_COCOA*) pGraphics->OpenWindow(0);
//            return pView;
//        }
    }
    return 0;
}

- (unsigned) interfaceVersion
{
    return 0;
}

- (NSString *) description
{
    return @"description";//ToNSString(PLUG_NAME " View");
}

@end


