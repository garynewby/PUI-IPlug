#define PLUG_MFR "Puitest"
#define PLUG_NAME "puiTest"

#define PLUG_CLASS_NAME puiTest

#define BUNDLE_MFR "puitest"
#define BUNDLE_NAME "puiTest"

#define PLUG_ENTRY puiTest_Entry
#define PLUG_VIEW_ENTRY puiTest_ViewEntry

#define PLUG_ENTRY_STR "puiTest_Entry"
#define PLUG_VIEW_ENTRY_STR "puiTest_ViewEntry"

#define VIEW_CLASS puiTest_View
#define VIEW_CLASS_STR "puiTest_View"

#define PLUG_VER 0x00010300
#define VST3_VER_STR "1.0.0"

#define PLUG_UNIQUE_ID 'Eq3v'
#define PLUG_MFR_ID 'Mlmu'

// PROTOOLS
#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
  #define _PIDS_
  const int PLUG_TYPE_IDS[2]    = {'CCN1', 'CCN2'};
  const int PLUG_TYPE_IDS_AS[2] = {'CCA1', 'CCA2'};
#endif

#define PLUG_MFR_PT  "Puitest"
#define PLUG_NAME_PT "puiTest"
#define PLUG_TYPE_PT "EQ"

#define PLUG_DOES_AUDIOSUITE 1

#define PLUG_CHANNEL_IO "1-1 2-2"
#define PLUG_LATENCY 3
#define PLUG_IS_INST 0

#define PLUG_DOES_MIDI 0
#define PLUG_DOES_STATE_CHUNKS 0

// PRODUCT ID
#define PROD_ID 14

// GUI
#define GUI_WIDTH 713
#define GUI_HEIGHT 396

// IMAGE ID
#define BACK_ID 128

// IMAGE PATHS
#define BACK_IMG  "resources/img/bmp00128.png"

#if defined(SA_API) && !defined(OS_IOS)
#include "app_wrapper/app_resource.h"
#endif

// VST3
#define MFR_URL "www.puitest.com"
#define MFR_EMAIL "mmsupport@puitest.com"
#define EFFECT_TYPE_VST3 "Fx|EQ"





