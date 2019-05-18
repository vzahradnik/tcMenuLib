/*
    The code in this file uses open source libraries provided by thecoderscorner

    DO NOT EDIT THIS FILE, IT WILL BE GENERATED EVERY TIME YOU USE THE UI DESIGNER
    INSTEAD EITHER PUT CODE IN YOUR SKETCH OR CREATE ANOTHER SOURCE FILE.

    All the variables you may need access to are marked extern in this file for easy
    use elsewhere.
 */

#include <tcMenu.h>
#include "nokia5110_menu.h"

// Global variable declarations

AdaColorGfxMenuConfig gfxConfig;
AdaFruitGfxMenuRenderer renderer;
const char PROGMEM applicationName[] = "Lighting";
EthernetServer server(3333);

// Global Menu Item declarations

const PROGMEM RemoteMenuInfo minfoR0 = { "R0", 13, 0xffff, 0, NO_CALLBACK };
RemoteMenuItem menuR0(&minfoR0, remoteServer.getRemoteConnector(0), NULL);
const PROGMEM SubMenuInfo minfoRemote = { "Remote", 12, 0xffff, 0, NO_CALLBACK };
BackMenuItem menuBackRemote(&menuR0, (const AnyMenuInfo*)&minfoRemote);
SubMenuItem menuRemote(&minfoRemote, &menuBackRemote, NULL);
const PROGMEM TextMenuInfo minfoTxt = { "Txt", 14, 0xffff, 6, NO_CALLBACK };
TextMenuItem menuTxt(&minfoTxt, NULL);
const PROGMEM FloatMenuInfo minfoCurrent = { "Current", 10, 0xffff, 2, NO_CALLBACK };
FloatMenuItem menuCurrent(&minfoCurrent, &menuTxt);
const PROGMEM FloatMenuInfo minfoVoltsIn = { "Volts in", 9, 0xffff, 2, NO_CALLBACK };
FloatMenuItem menuVoltsIn(&minfoVoltsIn, &menuCurrent);
const PROGMEM SubMenuInfo minfoStatus = { "Status", 8, 0xffff, 0, NO_CALLBACK };
BackMenuItem menuBackStatus(&menuVoltsIn, (const AnyMenuInfo*)&minfoStatus);
SubMenuItem menuStatus(&minfoStatus, &menuBackStatus, &menuRemote);
const PROGMEM AnyMenuInfo minfoShutdownNow = { "Shutdown now", 11, 0xffff, 0, NO_CALLBACK };
ActionMenuItem menuShutdownNow(&minfoShutdownNow, NULL);
const PROGMEM AnalogMenuInfo minfoDelay = { "Delay", 7, 11, 10, NO_CALLBACK, 0, 1, "S" };
AnalogMenuItem menuDelay(&minfoDelay, 0, &menuShutdownNow);
const PROGMEM BooleanMenuInfo minfoPwrDelay = { "Pwr Delay", 6, 10, 1, NO_CALLBACK, NAMING_YES_NO };
BooleanMenuItem menuPwrDelay(&minfoPwrDelay, false, &menuDelay);
const PROGMEM SubMenuInfo minfoSettings = { "Settings", 5, 0xffff, 0, NO_CALLBACK };
BackMenuItem menuBackSettings(&menuPwrDelay, (const AnyMenuInfo*)&minfoSettings);
SubMenuItem menuSettings(&minfoSettings, &menuBackSettings, &menuStatus);
const char enumStrOnAlm_0[] PROGMEM = "All On";
const char enumStrOnAlm_1[] PROGMEM = "Silient";
const char* const enumStrOnAlm[] PROGMEM  = { enumStrOnAlm_0, enumStrOnAlm_1 };
const PROGMEM EnumMenuInfo minfoOnAlm = { "On Alm", 4, 8, 1, NO_CALLBACK, enumStrOnAlm };
EnumMenuItem menuOnAlm(&minfoOnAlm, 0, &menuSettings);
const PROGMEM AnalogMenuInfo minfoKitchen = { "Kitchen", 3, 6, 100, onKitchenLight, 0, 1, "%" };
AnalogMenuItem menuKitchen(&minfoKitchen, 0, &menuOnAlm);
const PROGMEM AnalogMenuInfo minfoLiving = { "Living", 2, 4, 100, onLivingRoomLight, 0, 1, "%" };
AnalogMenuItem menuLiving(&minfoLiving, 0, &menuKitchen);
const PROGMEM AnalogMenuInfo minfoHall = { "Hall", 1, 2, 100, onHallLight, 0, 1, "%" };
AnalogMenuItem menuHall(&minfoHall, 0, &menuLiving);

// Set up code

void setupMenu() {
    prepareAdaMonoGfxConfigLoRes(&gfxConfig);
    renderer.setGraphicsDevice(&gfx, &gfxConfig);
    switches.initialise(ioUsingArduino(), true);
    menuMgr.initForEncoder(&renderer, &menuHall, ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_PIN_OK);
    remoteServer.begin(&server, applicationName);
}
