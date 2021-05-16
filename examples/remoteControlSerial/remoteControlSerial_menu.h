/*
    The code in this file uses open source libraries provided by thecoderscorner

    DO NOT EDIT THIS FILE, IT WILL BE GENERATED EVERY TIME YOU USE THE UI DESIGNER
    INSTEAD EITHER PUT CODE IN YOUR SKETCH OR CREATE ANOTHER SOURCE FILE.

    All the variables you may need access to are marked extern in this file for easy
    use elsewhere.
 */

#ifndef MENU_GENERATED_CODE_H
#define MENU_GENERATED_CODE_H

#include <Arduino.h>
#include <tcMenu.h>
#include "tcMenuAdaFruitGfx.h"
#include <RemoteConnector.h>
#include "SerialTransport.h"
#include <ScrollChoiceMenuItem.h>
#include <RuntimeMenuItem.h>
#include <EditableLargeNumberMenuItem.h>

void setupMenu();  // forward reference of the menu setup function.
extern const PROGMEM ConnectorLocalInfo applicationInfo;  // contains app name and ID

// Global variables that need exporting

extern Adafruit_PCD8544 gfx;
extern AdafruitDrawable gfxDrawable;
extern GraphicsDeviceRenderer renderer;

// Global Menu Item exports

extern Rgb32MenuItem menuRGB;
extern EditableLargeNumberMenuItem menuLgeNm;
extern ListRuntimeMenuItem menuRtList;
extern BackMenuItem menuBackRuntimes;
extern SubMenuItem menuRuntimes;
extern ScrollChoiceMenuItem menuChoice;
extern ActionMenuItem menuPushMe;
extern EnumMenuItem menuFood;
extern TextMenuItem menuText;
extern AnalogMenuItem menuA2;
extern AnalogMenuItem menuA1;
extern AnalogMenuItem menuA0;
extern BackMenuItem menuBackAnalogIn;
extern SubMenuItem menuAnalogIn;

// Provide a wrapper to get hold of the root menu item
inline MenuItem& rootMenuItem() { return menuAnalogIn; }

// Callback functions must always include CALLBACK_FUNCTION after the return type
#define CALLBACK_FUNCTION

int fnRtListRtCall(RuntimeMenuItem* item, uint8_t row, RenderFnMode mode, char* buffer, int bufferSize);
void CALLBACK_FUNCTION onPushMe(int id);

#endif // MENU_GENERATED_CODE_H
