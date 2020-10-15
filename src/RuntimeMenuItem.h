/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Nutricherry LTD).
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

/**
 * @file RuntimeMenuItem.h
 * Contains definitions of menu items that can be fully defined at runtime with no need for prog mem structures.
 */

#ifndef _RUNTIME_MENUITEM_H_
#define _RUNTIME_MENUITEM_H_

#include "MenuItems.h"
#include "tcUtil.h"

/** For items that dont need to have the same id each time (such as back menu items), we just randomly give them an ID */
#define RANDOM_ID_START 50000

/** For items that dont need to have the same id each time (such as back menu items), we just randomly give them an ID */
int nextRandomId();

/** This is the standard renderering function used for editable text items, for use with TextMenuItem */
int textItemRenderFn(RuntimeMenuItem* item, uint8_t row, RenderFnMode mode, char* buffer, int bufferSize);

/** This is the standard rendering function used for editable IP addresses, for use with IpAddressMenuItem */
int ipAddressRenderFn(RuntimeMenuItem* item, uint8_t row, RenderFnMode mode, char* buffer, int bufferSize);

/** The default rendering function for back menu items */
int backSubItemRenderFn(RuntimeMenuItem* item, uint8_t row, RenderFnMode mode, char* buffer, int bufferSize);

/** The default rendering function for time menu items */
int timeItemRenderFn(RuntimeMenuItem* item, uint8_t row, RenderFnMode mode, char* buffer, int bufferSize);

/** The default rendering function for time menu items */
int dateItemRenderFn(RuntimeMenuItem* item, uint8_t row, RenderFnMode mode, char* buffer, int bufferSize);

/** helper function for text items that finds the position of a char in the allowable set of editable chars */
int findPositionInEditorSet(char ch);

/**
 * Defines the filter that should be applied to values of multi edit menu items on the UI
 */
enum MultiEditWireType : uint8_t {
	EDITMODE_PLAIN_TEXT = 0, 
	EDITMODE_IP_ADDRESS = 1,
	EDITMODE_TIME_24H = 2,
	EDITMODE_TIME_12H = 3,
	EDITMODE_TIME_HUNDREDS_24H = 4,
    EDITMODE_GREGORIAN_DATE = 5
};

/**
 * A menu item that can be defined at runtime and needs no additional structures. This represents a single value in terms of
 * name and value. The itemPosition will be passed to the rendering function, so you can use the same function for many items.
 * This is the base that is the most configurable, but also most difficult to use, consider one of the more specific sub types 
 * of this for general use.
 */
class RuntimeMenuItem : public MenuItem {
protected:
	uint16_t id;
	uint8_t itemPosition;
	uint8_t noOfParts;
public:
	RuntimeMenuItem(MenuType menuType, uint16_t id, RuntimeRenderingFn renderFn, 
				    uint8_t itemPosition, uint8_t numberOfRows, MenuItem* next = NULL);
	
	void copyValue(char* buffer, int bufferSize) {
		renderFn(this, itemPosition, RENDERFN_VALUE, buffer, bufferSize);
	}

	void runCallback() { renderFn(this, itemPosition, RENDERFN_INVOKE, NULL, 0); }
	int getRuntimeId() { return id; }
	int getRuntimeEeprom() { return renderFn(this, itemPosition, RENDERFN_EEPROM_POS, NULL, 0); }
	uint8_t getNumberOfParts() { return noOfParts; }
	void copyRuntimeName(char* buffer, int bufferSize) { renderFn(this, itemPosition, RENDERFN_NAME, buffer, bufferSize); }

    uint8_t getNumberOfRows() { return noOfParts; }

    void setNumberOfRows(uint8_t rows) {
		noOfParts = rows; 
		setChanged(true); 
		setSendRemoteNeededAll(); 
	}
};

/**
 * Back menu item pairs with an associated SubMenuItem, it only exists in the embedded domain - not the API.
 * This type is always the first item in a series of items for a submenu. It provides the functionality required
 * to get back to root.
 *
 * For example
 *
 * 		SubMenu.getChild() -> Back Menu Item -> Sub Menu Item 1 ...
 */
class BackMenuItem : public RuntimeMenuItem {
private:
	MenuItem* child;
public:
	/**
	 * Create an instance of the class
	 *
	 * @param nextChild the next menu in the chain if there is one, or NULL.
	 * @param renderFn the callback that provides the runtime information about the menu.
	 */
	BackMenuItem(RuntimeRenderingFn renderFn, MenuItem* next) 
		: RuntimeMenuItem(MENUTYPE_BACK_VALUE, nextRandomId(), renderFn, 0, 1, next) { }
};

/**
 * The implementation of a Menuitem that can contain more menu items as children.
 */
class SubMenuItem : public RuntimeMenuItem {
private:
    MenuItem* child;
    const char* pgmNamePtr;
public:
    /**
     * Create an instance of SubMenuItem using the traditional SubMenuInfo block, this is no longer used, but we
     * still support it as a means of working with the name.
     * @deprecated use the other constructor, this constructor will be removed in a future version
     * @param info a SubMenuInfo structure
     * @param id the item ID
     * @param child the first child item - (normally a BackMenuItem)
     * @param next the next menu in the chain if there is one, or NULL.
     */
    SubMenuItem(const SubMenuInfo* info, MenuItem* child, MenuItem* next = nullptr)
                : RuntimeMenuItem(MENUTYPE_SUB_VALUE, get_info_uint(&info->id), backSubItemRenderFn, 0, 1, next) {
        this->child = child;
        this->pgmNamePtr = info->name;
    }

    /**
     * Create an instance of SubMenuItem using the runtime method, which can easily be created during system runtime.
     * @param id the ID of the item
     * @param renderFn the callback function for this item
     * @param child the first child item in the sub menu
     * @param next the next menu in the chain if there i one, or NULL.
     */
    SubMenuItem(uint16_t id, RuntimeRenderingFn renderFn, MenuItem* child, MenuItem* next = nullptr)
            : RuntimeMenuItem(MENUTYPE_SUB_VALUE, id, renderFn,
                              0, 1, next) {
        this->child = child;
        this->pgmNamePtr = nullptr;
    }

    /**
     * return the first child item
     */
    MenuItem* getChild() { return child; }
    void setChild(MenuItem* firstChildItem) { this->child = firstChildItem; }

    const char* getNamePGMUnsafe() { return pgmNamePtr; }
};

#define LIST_PARENT_ITEM_POS 0xff

/**
 * A menu item that represents a list of items and can be defined at runtime. This takes an ID that 
 * will act as a range, this is important, as this will use from ID through to ID + numberOfItems so 
 * always allocate enough ID range space. 
 * It can either represent a single item or a range of items, for a single item set the initial
 * rows to 0. The value and name of each item is obtained from the callback function.
 * Note that setting one item change sets all items in the list changed and similar with any other flag.
 * These are the only menu items that can presently be created dynamically at runtime.
 */
class ListRuntimeMenuItem : public RuntimeMenuItem {
private:
	uint8_t activeItem;
public:
	ListRuntimeMenuItem(uint16_t id, int numberOfRows, RuntimeRenderingFn renderFn, MenuItem* next = NULL);

	RuntimeMenuItem* getChildItem(int pos) {
		menuType = MENUTYPE_RUNTIME_LIST;
		itemPosition = pos;
		setActive((activeItem - 1) == pos);
		return this;
	}

	uint8_t getActiveIndex() { return activeItem; }

	void setActiveIndex(uint8_t idx) { 
		activeItem = idx; 
		setChanged(true);
	}

	RuntimeMenuItem* asParent() { 
		menuType = MENUTYPE_RUNTIME_LIST;
		itemPosition = LIST_PARENT_ITEM_POS;
		return this;
	}

	RuntimeMenuItem* asBackMenu() {
		setActive(activeItem == 0);
		menuType = MENUTYPE_BACK_VALUE;
		itemPosition = LIST_PARENT_ITEM_POS;
		return this;
	}

	bool isActingAsParent() {
		return itemPosition == LIST_PARENT_ITEM_POS;
	}
};

/**
 * This menu item allows local editing of anything that can be described as a range of values to be edited with the 
 * spinwheel or emulator. It extends from runtime menu item so has a small footprint in RAM.
 */
template<class V> class EditableMultiPartMenuItem : public RuntimeMenuItem {
protected:
	V data;
public:
	EditableMultiPartMenuItem(MenuType type, uint16_t id, int numberOfParts, RuntimeRenderingFn renderFn, MenuItem* next = NULL) 
			: RuntimeMenuItem(type, id, renderFn, 0, numberOfParts, next) {
	}

	uint8_t beginMultiEdit() {
		setEditing(true);
		itemPosition = 0;
		return noOfParts;
	}

	int changeEditBy(int amt) {
		itemPosition += amt;
		setChanged(true);
		setSendRemoteNeededAll();
		return renderFn(this, itemPosition, RENDERFN_GETRANGE, NULL, 0);
	}

	int previousPart() {
		if (itemPosition <= 1) {
			stopMultiEdit();
			return 0;
		}
		return changeEditBy(-1);
	}

	int nextPart() {
		if (itemPosition >= noOfParts) {
			stopMultiEdit();
			return 0;
		}

		return changeEditBy(1);
	}

	int getCurrentRange() {
		return renderFn(this, itemPosition, RENDERFN_GETRANGE, NULL, 0);
	}
	
	void stopMultiEdit() {
		itemPosition = 0xff;
		setEditing(false);
		setChanged(true);
		setSendRemoteNeededAll();
		runCallback();
	}

	int getPartValueAsInt() {
		return renderFn(this, itemPosition, RENDERFN_GETPART, NULL, 0);
	}

	bool valueChanged(int newVal) {
		setChanged(true);
		setSendRemoteNeededAll();

		uint8_t sz[2];
		sz[0] = lowByte(newVal);
		sz[1] = highByte(newVal);
		bool valueUpdated = renderFn(this, itemPosition, RENDERFN_SET_VALUE, reinterpret_cast<char*>(sz), sizeof(sz));
		if(valueUpdated) {
            runCallback();
        }
		return valueUpdated;
	}
};

// number of characters in the edit set.
#define ALLOWABLE_CHARS_ENCODER_SIZE 94

/**
 * An item that can represent a text value that is held in RAM, and therefore change at runtime. We now manually
 * configure the settings for this menu item in the constructor. This variant gets the name from program memory
 */
class TextMenuItem : public EditableMultiPartMenuItem<char*> {
private:
    bool passwordField;
public:
    TextMenuItem(RuntimeRenderingFn customRenderFn, uint16_t id, int size, MenuItem* next = NULL)
		: EditableMultiPartMenuItem(MENUTYPE_TEXT_VALUE, id, size, customRenderFn, next) {
		data = new char[size];
		memset(data, 0, size);
        passwordField = false;
	}

    void setPasswordField(bool pwd) {
        this->passwordField = pwd;
    }

    bool isPasswordField() const {
        return this->passwordField;
    }

	~TextMenuItem() { delete data; }

	/** get the max length of the text storage */
	uint8_t textLength() { return noOfParts; }

	/**
	 * Copies the text into the internal buffer.
  	 * @param text the text to be copied.
	 */
	void setTextValue(const char* text, bool silent = false);

	/** returns the text value in the internal buffer */
	const char* getTextValue() { return data; }

	/**
	 * Called after the array has been changed to ensure that it is in a good
	 * state for editing. IE zero's extended to the end.
	 */
	void cleanUpArray();
private:
	bool setCharValue(uint8_t location, char val);
	friend int textItemRenderFn(RuntimeMenuItem* item, uint8_t row, RenderFnMode mode, char* buffer, int bufferSize);
};

/**
 * finds the position of the character in the editor set
 * @param ch the character to find.
 * @return the location in the allowable chars
 */
int findPositionInEditorSet(char ch);

/**
 * This menu item represents an IP address that can be configured / or just displayed on the device,
 * if it is editable it is edited 
 */
class IpAddressMenuItem : public EditableMultiPartMenuItem<uint8_t[4]> {
public:
    IpAddressMenuItem(RuntimeRenderingFn renderFn, uint16_t id, MenuItem* next = NULL)
		: EditableMultiPartMenuItem(MENUTYPE_IPADDRESS, id, 4, renderFn, next) {
		setIpAddress(127, 0, 0, 1);
	}

	void setIpAddress(const char* source);

	/** Sets the whole IP address as four parts */
	void setIpAddress(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4);

	/** gets the IP address as four separate bytes */
	uint8_t* getIpAddress() { return data; }
	
	/** sets a single part in the address */
	void setIpPart(uint8_t part, uint8_t newVal) {
		if (part > 3) return;
		data[part] = newVal;
		setChanged(true);
		setSendRemoteNeededAll();
	}
};

/**
 * The storage for a time field can hold down to hundreds of a second, stored as a series of bytes for hours, minutes,
 * seconds and hundreds.
 */
struct TimeStorage {
    TimeStorage() {
        this->hours = this->minutes = this->seconds = this->hundreds = 0;
    }
    
    TimeStorage(uint8_t hours, uint8_t minutes, uint8_t seconds = 0, uint8_t hundreds = 0) {
        this->hours = hours;
        this->minutes = minutes;
        this->seconds = seconds;
        this->hundreds = hundreds;
    }

    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t hundreds;
};

/**
 * Storage of a date field where the month and day are represented as a single byte, and the year is a 16 bit value.
 */
struct DateStorage {
    uint8_t day;
    uint8_t month;
    uint16_t year;

    DateStorage() {
        year = day = month = 0;
    }

    DateStorage(int day, int month, int year) {
        this->day = day;
        this->month = month;
        this->year = year;
    }
};

/**
 * A runtime menu item that represents time value, either in the 24 hour clock or in the 12 hour clock. Further, the
 * hundreds can be configured to show as well. It is an extension of the multi part editor that supports editing
 * times in parts, hours, then minutes and so on.  Instances of this class should use the `dateItemRenderFn` for the
 * base rendering.
 */
class TimeFormattedMenuItem : public EditableMultiPartMenuItem<TimeStorage> {
private:
    MultiEditWireType format;
public:
    TimeFormattedMenuItem(RuntimeRenderingFn renderFn, uint16_t id, MultiEditWireType format, MenuItem* next = NULL)
		: EditableMultiPartMenuItem(MENUTYPE_TIME, id, format == EDITMODE_TIME_HUNDREDS_24H ? 4 : 3, renderFn, next) {
		setTime(TimeStorage(12, 0));
        this->format = format;
	}

	/** gets the IP address as four separate bytes */
	TimeStorage getTime() { return data; }
    
    /** sets the time */
	void setTime(TimeStorage newTime) { data = newTime; }

    /** sets a time from a string in the form HH:MM:SS[.ss]*/
    void setTimeFromString(const char* time);

    /** gets the formatting currently being used. */
    MultiEditWireType  getFormat() { return format; }	

    TimeStorage* getUnderlyingData() {return &data;}
};

/**
 * A runtime menu item that represents a date value in the gregorian calendar. It is an extension of the multipart editor
 * class that supports editing dates in parts, days, month and finally years. It has some very basic support for leap
 * years. Instances of this class should use the `dateItemRenderFn` for the base rendering.
 */
class DateFormattedMenuItem : public EditableMultiPartMenuItem<DateStorage>{
public:
    enum DateFormatOption { DD_MM_YYYY, MM_DD_YYYY, YYYY_MM_DD };
private:
    static char separator;
    static DateFormatOption dateFormatMode;
public:
    DateFormattedMenuItem(RuntimeRenderingFn renderFn, uint16_t id, MenuItem* next = nullptr)
    : EditableMultiPartMenuItem(MENUTYPE_DATE, id, 3, renderFn, next) {
        setDate(DateStorage(1, 1, 2020));
    }

    /**
     * sets the global separator for date rendering.
     * @param sep the new separator character
     */
    static void setDateSeparator(char sep) {
        separator = sep;
    }

    /**
     * @return the global separator for date rendering.
     */
    static char getDateSeparator() {
        return separator;
    }

    /**
     * Sets the global date formatting for dates, one of the enum DateFormatOption.
     * @param fmt the new format to use
     */
    static void setDateFormatStyle(DateFormatOption fmt) {
        dateFormatMode = fmt;
    }

    /**
     * @return the global date format option in use by all date formatters.
     */
    static DateFormatOption getDateFormatStyle() {
        return dateFormatMode;
    }

    DateStorage getDate() { return data; }

    void setDate(DateStorage newDate) { data = newDate; }

    void setDateFromString(const char *dateText);

    DateStorage* getUnderlyingData() { return &data; }
};

/**
 * Utility function to parse a string from a given offset to obtain an integer
 * @param ptr the pointer to the text
 * @param offset a ref to an integer that starts as the offset and is updated
 * @return the integer that was obtain before a non digit was found
 */
long parseIntUntilSeparator(const char* ptr, int& offset);

/**
 * This macro defines a rendering callback that will be often used with remote types, it takes as it's parameters
 * a parent function for the type in question, the variable containing the progmem name and the call back method
 * or NULL if there's no callback.
 */
#define RENDERING_CALLBACK_NAME_INVOKE(fnName, parent, namepgm, eepromPosition, invoke) \
const char fnName##Pgm[] PROGMEM = namepgm; \
int fnName(RuntimeMenuItem* item, uint8_t row, RenderFnMode mode, char* buffer, int buffSize) { \
	switch(mode) { \
	case RENDERFN_NAME: \
		safeProgCpy(buffer, fnName##Pgm, buffSize); \
		return true; \
	case RENDERFN_INVOKE: \
		if(invoke) (reinterpret_cast<MenuCallbackFn>(invoke))(int(item->getId())); \
		return true; \
	case RENDERFN_EEPROM_POS: \
		return eepromPosition; \
	default: \
		return parent(item, row, mode, buffer, buffSize); \
	} \
}

#endif //_RUNTIME_MENUITEM_H_

