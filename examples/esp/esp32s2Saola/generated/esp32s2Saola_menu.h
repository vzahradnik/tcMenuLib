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
#include "tcMenuU8g2.h"
#include "EthernetTransport.h"
#include <RemoteConnector.h>
#include <RemoteMenuItem.h>
#include <RuntimeMenuItem.h>
#include <ScrollChoiceMenuItem.h>
#include <IoAbstraction.h>
#include <EepromItemStorage.h>
#include <ArduinoEEPROMAbstraction.h>
#include <RemoteAuthentication.h>

// variables we declare that you may need to access
extern const PROGMEM ConnectorLocalInfo applicationInfo;
extern TcMenuRemoteServer remoteServer;
extern U8G2_SH1106_128X64_NONAME_F_HW_I2C gfx;
extern U8g2Drawable gfxDrawable;
extern GraphicsDeviceRenderer renderer;
extern WiFiServer server;
extern EthernetInitialisation ethernetInitialisation;

// Any externals needed by IO expanders, EEPROMs etc


// Global Menu Item exports
extern EepromAuthenticationInfoMenuItem menuConnectivityAuthenticator;
extern RemoteMenuItem menuConnectivityIoTMonitor;
extern IpAddressMenuItem menuConnectivityIPAddress;
extern EnumMenuItem menuConnectivityWiFiMode;
extern TextMenuItem menuConnectivityPasscode;
extern TextMenuItem menuConnectivitySSID;
extern BackMenuItem menuBackConnectivity;
extern SubMenuItem menuConnectivity;
extern ListRuntimeMenuItem menuExtrasMyList;
extern Rgb32MenuItem menuExtrasColor;
extern TextMenuItem menuExtrasText;
extern BackMenuItem menuBackExtras;
extern SubMenuItem menuExtras;
extern BooleanMenuItem menuSelectMeNewBoolItem;
extern ActionMenuItem menuSelectMePressMe;
extern FloatMenuItem menuSelectMeFloat2;
extern FloatMenuItem menuSelectMeFloat1;
extern BackMenuItem menuBackSelectMe;
extern SubMenuItem menuSelectMe;
extern BooleanMenuItem menuDoorOpen;
extern EnumMenuItem menuFoods;
extern AnalogMenuItem menuHalves;
extern AnalogMenuItem menuDecEdit;
extern AnalogMenuItem menuIntEdit;

// Provide a wrapper to get hold of the root menu item and export setupMenu
inline MenuItem& rootMenuItem() { return menuIntEdit; }
void setupMenu();

// Callback functions must always include CALLBACK_FUNCTION after the return type
#define CALLBACK_FUNCTION

void CALLBACK_FUNCTION onListSelected(int id);
int fnExtrasMyListRtCall(RuntimeMenuItem* item, uint8_t row, RenderFnMode mode, char* buffer, int bufferSize);
void CALLBACK_FUNCTION pressMeActionRun(int id);

#endif // MENU_GENERATED_CODE_H
