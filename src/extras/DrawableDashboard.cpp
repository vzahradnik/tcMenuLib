#include <BaseRenderers.h>
#include "DrawableDashboard.h"
#include "ScrollChoiceMenuItem.h"

void DrawableDashboard::addDrawingItem(MenuItem *theItem, Coord topLeft, DashDrawParameters *params, int numCharsInValue,
                                  const char *titleOverrideText, int updateTicks) {
    drawingItems.add(DashMenuItem(theItem, topLeft, params, numCharsInValue, titleOverrideText, updateTicks));
    serdebugF2("added item to list #", drawingItems.count());
}

void DrawableDashboard::stop() {
    running = false;
    renderer->giveBackDisplay();
}

void DrawableDashboard::reset() {
    if (drawingMode == DASH_ON_RESET_CLICK_EXIT || drawingMode == DASH_ON_RESET_MANUAL_EXIT) {
        if (!running) renderer->takeOverDisplay();
    }
}

void DrawableDashboard::started(BaseMenuRenderer *currentRenderer) {
    renderer = currentRenderer;
    drawable->setDrawColor(screenBg);
    drawable->drawBox(Coord(0, 0), drawable->getDisplayDimensions(), true);
    serdebugF2("drawing titles #", drawingItems.count());

    for (int i = 0; i < drawingItems.count(); i++) {
        auto drawing = drawingItems.itemAtIndex(i);
        drawing->paintTitle(drawable);
    }

    drawWidgets(true);
}

void DrawableDashboard::renderLoop(unsigned int currentValue, RenderPressMode userClick) {
    if (userClick != RPRESS_NONE && (drawingMode == DASH_ON_RESET_CLICK_EXIT || drawingMode == DASH_MANUAL_START_CLICK_EXIT)) {
        serlogF(SER_TCMENU_INFO, "Dashboard exit, clicked");
        stop();
        return;
    }

    for (int i = 0; i < drawingItems.count(); i++) {
        auto drawing = drawingItems.itemAtIndex(i);
        if (drawing->needsPainting()) {
            drawing->paintItem(drawable);
        }
    }

    drawWidgets(false);
}

void DrawableDashboard::drawWidgets(bool force) {
    auto widget = firstWidget;
    int xPos = drawable->getDisplayDimensions().x;
    while(widget) {
        if(force || widget->isChanged()) {
            xPos -= (widget->getWidth() + 3);
            drawable->setColors(coreItemFg, screenBg);
            drawable->drawXBitmap(Coord(xPos, 3), Coord(widget->getWidth(), widget->getHeight()), widget->getCurrentIcon());
            widget->setChanged(false);
        }
        widget = widget->getNext();
    }
}

DashDrawParameters::DashDrawParameters(color_t fgColor_, color_t bgColor_, const GFXfont *font_, DashAlign align) {
    alignment = align;
    adaFont = font_;
    isAdaFont = true;
    fgColor = fgColor_;
    bgColor = bgColor_;
}

DashDrawParameters::DashDrawParameters(color_t fgColor_, color_t bgColor_, const UnicodeFont *font_, DashAlign align) {
    alignment = align;
    uniFont = font_;
    isAdaFont = false;
    fgColor = fgColor_;
    bgColor = bgColor_;
}

int DashDrawParametersIntUpdateRange::findIndexForChoice(MenuItem* item) {
    int val;
    if(isMenuBasedOnValueItem(item)) {
        val = reinterpret_cast<ValueMenuItem *>(item)->getCurrentValue();
    } else if(item->getMenuType() == MENUTYPE_SCROLLER_VALUE) {
        val = reinterpret_cast<ScrollChoiceMenuItem*>(item)->getCurrentValue();
    } else {
        return -1;
    }

    for(int i=0;i<numOfRanges;i++) {
        if(val >= colorRanges[i].minValue && val <= colorRanges[i].maxValue) {
            return i;
        }
    }

    return -1;
}

color_t DashDrawParametersIntUpdateRange::getBgColor(MenuItem *item, bool updated) {
    if (useUpdateColor && updated) return DashDrawParametersUpdate::getBgColor(item, updated);
    auto idx = findIndexForChoice(item);
    return (idx != -1) ? colorRanges[idx].bgColor : bgColor;
}

color_t DashDrawParametersIntUpdateRange::getFgColor(MenuItem *item, bool updated) {
    if (useUpdateColor && updated) return DashDrawParametersUpdate::getFgColor(item, updated);
    auto idx = findIndexForChoice(item);
    return (idx != -1) ? colorRanges[idx].fgColor : fgColor;
}

int DashDrawParametersTextUpdateRange::findIndexForChoice(MenuItem* item) {
    if(isMenuRuntime(item)) {
        auto rtMenu = reinterpret_cast<RuntimeMenuItem*>(item);
        char sz[32];
        rtMenu->copyValue(sz, sizeof sz);
        for(int i=0;i<numOfRanges;i++) {
            if(strcmp(colorOverrides[i].text, sz) == 0) {
                return i;
            }
        }
    }
    return -1;
}

color_t DashDrawParametersTextUpdateRange::getBgColor(MenuItem *item, bool updated) {
    if (useUpdateColor && updated) return DashDrawParametersUpdate::getBgColor(item, updated);
    auto idx = findIndexForChoice(item);
    return (idx != -1) ? colorOverrides[idx].bgColor : bgColor;
}

color_t DashDrawParametersTextUpdateRange::getFgColor(MenuItem *item, bool updated) {
    if (useUpdateColor && updated) return DashDrawParametersUpdate::getFgColor(item, updated);
    auto idx = findIndexForChoice(item);
    return (idx != -1) ? colorOverrides[idx].fgColor : fgColor;
}

color_t DashMenuItem::staticPalette[4] = {};

DashMenuItem::DashMenuItem(MenuItem *theItem, Coord topLeft, DashDrawParameters* params, int numCharsInValue,
                           const char* titleOverride, int countDownTicks) : item(theItem), screenLoc(topLeft),
                               parameters(params), updateCountDown(countDownTicks), numChars(numCharsInValue),
                               valueWidth(0), titleExtents(0, 0), countDownTicks(countDownTicks) {

    if(titleOverride != nullptr) {
        strncpy(titleText, titleOverride, sizeof(titleText));
    }
    else {
        theItem->copyNameToBuffer(titleText, sizeof(titleText));
    }
    titleText[sizeof(titleText)-1] = 0; // make sure it's null terminated.
}

bool DashMenuItem::needsPainting() {
    if (item == nullptr) return false;

    if(item->isChanged()) updateCountDown = countDownTicks;

    if(updateCountDown > 0) --updateCountDown;
    return item->isChanged() || updateCountDown != 0;
}

void DashMenuItem::setFont(DashDrawParameters* params, UnicodeFontHandler* unicodeHandler) {
    if(parameters->isAdafruitFont()) {
        unicodeHandler->setFont(parameters->getAsAdaFont());
    } else {
        unicodeHandler->setFont(parameters->getAsUnicodeFont());
    }
}

/**
 * Wraps a drawable regardless of if we are on a sub device or root device, this class handles all the differences
 * between the two with helper functions for dealing with palette colors and offset differences.
 */
class DrawableWrapper {
private:
    color_t palette[2] = {};
    DeviceDrawable* drawable = nullptr;
    Coord startPos = {};
    bool isSubDevice = false;
public:
    DrawableWrapper(DeviceDrawable* root, DashDrawParameters* parameters, MenuItem* item, const Coord& startPosition, const Coord& size) {
        palette[0] = parameters->getBgColor(item, item->isChanged());
        palette[1] = parameters->getFgColor(item, item->isChanged());

        isSubDevice = false;
        drawable = root;
        if (root->getSubDeviceType() != tcgfx::DeviceDrawable::NO_SUB_DEVICE) {
            auto subDrawable = root->getSubDeviceFor(startPosition, size, palette, 4);
            if (subDrawable) {
                isSubDevice = true;
                drawable = subDrawable;
                drawable->startDraw();
                startPos = startPosition;
            }
        }
    }

    DeviceDrawable* getDrawable() { return drawable; }
    color_t getPaletteColor(size_t idx) { return isSubDevice ? idx : palette[idx]; }
    Coord offsetLocation(const Coord& source) const { return isSubDevice ? Coord(source.x - startPos.x, source.y - startPos.y) : source; }
    Coord offsetLocation(const Coord& source, int xOffs, int yOffs) const {
        return isSubDevice ? Coord((source.x + xOffs) - startPos.x, (source.y + yOffs) - startPos.y) : Coord(source.x + xOffs, source.y + yOffs);
    }

    void endDraw() { if(isSubDevice) drawable->endDraw(true); }
};

void DashMenuItem::paintTitle(DeviceDrawable* drawableRoot) {
    int baseline;
    UnicodeFontHandler* unicodeHandler = drawableRoot->getUnicodeHandler(true);
    setFont(parameters, unicodeHandler);
    titleExtents = unicodeHandler->textExtents(titleText, &baseline);
    valueWidth = unicodeHandler->textExtents("0", &baseline).x * numChars;
    valueWidth = int(valueWidth * 1.20);

    DrawableWrapper wrapper(drawableRoot, parameters, item, screenLoc, Coord(titleExtents.x + valueWidth, titleExtents.y));
    unicodeHandler = wrapper.getDrawable()->getUnicodeHandler(true);

    if(!parameters->isTitleDrawn()) return;

    auto startX = (parameters->isTitleLeftAlign()) ? 0 : valueWidth + 1;
    wrapper.getDrawable()->setDrawColor(wrapper.getPaletteColor(0));
    wrapper.getDrawable()->drawBox(wrapper.offsetLocation(screenLoc), Coord(titleExtents.x + valueWidth, titleExtents.y), true);

    unicodeHandler->setDrawColor(wrapper.getPaletteColor(1));
    unicodeHandler->setCursor(wrapper.offsetLocation(screenLoc, startX, titleExtents.y));
    unicodeHandler->print(titleText);

    wrapper.endDraw();
}

void DashMenuItem::paintItem(DeviceDrawable* drawableRoot) {
    item->setChanged(false);
    char sz[20];
    DrawableWrapper wrapper(drawableRoot, parameters, item, screenLoc, Coord(valueWidth, titleExtents.y));

    copyMenuItemValue(item, sz, sizeof(sz));
    wrapper.getDrawable()->setDrawColor(wrapper.getPaletteColor(0));
    wrapper.getDrawable()->drawBox(wrapper.offsetLocation(screenLoc), Coord(valueWidth, titleExtents.y), true);

    UnicodeFontHandler* unicodeHandler = wrapper.getDrawable()->getUnicodeHandler(true);
    unicodeHandler->setDrawColor(wrapper.getPaletteColor(1));
    setFont(parameters, unicodeHandler);
    auto padding = 0;
    if(!parameters->isValueLeftAlign()) {
        int baseline;
        Coord valueLen = unicodeHandler->textExtents(sz, &baseline);
        padding = valueWidth - (valueLen.x + 4);
    }
    unicodeHandler->setCursor(wrapper.offsetLocation(screenLoc, padding, unicodeHandler->getYAdvance()));
    unicodeHandler->print(sz);

    wrapper.endDraw();
}