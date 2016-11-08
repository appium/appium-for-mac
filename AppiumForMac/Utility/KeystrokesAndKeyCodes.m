//
//  KeystrokesAndKeyCodes.m
//  AppiumForMacFramework
//
//  Created by Stuart Russell at Intuit, Inc.
//  Copyright © 2016 Appium. All rights reserved.
//

#import <CoreFoundation/CoreFoundation.h>

#import "KeystrokesAndKeyCodes.h"

@implementation KeystrokesAndKeyCodes

+ (CGKeyCode)keyCodeWithWebDriverKeystroke:(UniChar)keystroke isShifted:(BOOL *)isShiftedPtr
{
    // First, look it up in the following switch statement. If not there, look it up the hard way.
    
    /* 
     WebDriver keystrokes not included here because I couldn't find a keycode to match:
     Cancel         0xE001
     Pause          0xE00B
     Separator      0xE026
     */
    // This could be a character with modifiers, e.g. shift. 
    // If so, the caller will need to press the modifier before typing the returned key code.
    // NOTE: currently only the shift modifier is supported.
    BOOL isShifted = NO;
    
    // Some cases are commented out because the WebDriver protocol does not define them.
    CGKeyCode returnedCode = UINT16_MAX;
    switch (keystroke) {
        /* keycodes for keys that are independent of keyboard layout*/
        case 0xE006:
            returnedCode = kVK_Return;         // 0x24
            break;
        case 0xE004:
            returnedCode = kVK_Tab;            // 0x30
            break;
        case 0xE00D:
            returnedCode = kVK_Space;          // 0x31
            break;
        case 0xE017:
            returnedCode = kVK_Delete;         // 0x33
            break;
        case 0xE00C:
            returnedCode = kVK_Escape;         // 0x35
            break;
        case 0xE03D:
            returnedCode = kVK_Command;        // 0x37
            break;
        case 0xE008:
            returnedCode = kVK_Shift;         // 0x38
            break;
//        case 0xFFFF:
//            returnedCode = kVK_CapsLock;       // 0x39
//            break;
        case 0xE00A:
            returnedCode = kVK_Option;         // 0x3A
            break;
        case 0xE009:
            returnedCode = kVK_Control;        // 0x3B
            break;
//        case 0xE008:
//            returnedCode = kVK_RightShift;     // 0x3C      // WD does not define 'right...' keystrokes, so map to non-right
//            break;
//        case 0xE00A:
//            returnedCode = kVK_RightOption;    // 0x3D      // WD does not define 'right...' keystrokes, so map to non-right
//            break;
//        case 0xE009:
//            returnedCode = kVK_RightControl;   // 0x3E      // WD does not define 'right...' keystrokes, so map to non-right
//            break;
//        case 0xFFFF:
//            returnedCode = kVK_Function;       // 0x3F
//            break;
//        case 0xFFFF:
//            returnedCode = kVK_F17;            // 0x40
//            break;
//        case 0xFFFF:
//            returnedCode = kVK_VolumeUp;       // 0x48
//            break;
//        case 0xFFFF:
//            returnedCode = kVK_VolumeDown;     // 0x49
//            break;
//        case 0xFFFF:
//            returnedCode = kVK_Mute;           // 0x4A
//            break;
//        case 0xFFFF:
//            returnedCode = kVK_F18;            // 0x4F
//            break;
//        case 0xFFFF:
//            returnedCode = kVK_F19;            // 0x50
//            break;
//        case 0xFFFF:
//            returnedCode = kVK_F20;            // 0x5A
//            break;
        case 0xE035:
            returnedCode = kVK_F5;             // 0x60
            break;
        case 0xE036:
            returnedCode = kVK_F6;             // 0x61
            break;
        case 0xE037:
            returnedCode = kVK_F7;             // 0x62
            break;
        case 0xE033:
            returnedCode = kVK_F3;             // 0x63
            break;
        case 0xE038:
            returnedCode = kVK_F8;             // 0x64
            break;
        case 0xE039:
            returnedCode = kVK_F9;             // 0x65
            break;
        case 0xE03B:
            returnedCode = kVK_F11;            // 0x67
            break;
//        case 0xFFFF:
//            returnedCode = kVK_F13;            // 0x69
//            break;
//        case 0xFFFF:
//            returnedCode = kVK_F16;            // 0x6A
//            break;
//        case 0xFFFF:
//            returnedCode = kVK_F14;            // 0x6B
//            break;
        case 0xE03A:
            returnedCode = kVK_F10;            // 0x6D
            break;
        case 0xE03C:
            returnedCode = kVK_F12;            // 0x6F
            break;
//        case 0xFFFF
//            returnedCode = kVK_F15;            // 0x71
//            break;
        case 0xE002:
            returnedCode = kVK_Help;           // 0x72
            break;
        case 0xE011:
            returnedCode = kVK_Home;           // 0x73
            break;
        case 0xE00E:
            returnedCode = kVK_PageUp;         // 0x74
            break;
//        case 0xFFFF
//            returnedCode = kVK_ForwardDelete;  // 0x75
//            break;
        case 0xE034:
            returnedCode = kVK_F4;             // 0x76
            break;
        case 0xE010:
            returnedCode = kVK_End;            // 0x77
            break;
        case 0xE032:
            returnedCode = kVK_F2;             // 0x78
            break;
        case 0xE00F:
            returnedCode = kVK_PageDown;       // 0x79
            break;
        case 0xE031:
            returnedCode = kVK_F1;             // 0x7A
            break;
        case 0xE012:
            returnedCode = kVK_LeftArrow;      // 0x7B
            break;
        case 0xE014:
            returnedCode = kVK_RightArrow;     // 0x7C
            break;
        case 0xE015:
            returnedCode = kVK_DownArrow;      // 0x7D
            break;
        case 0xE013:
            returnedCode = kVK_UpArrow;        // 0x7E
            break;
        /* keycodes for keys that are NOT independent of keyboard layout - may break with special keyboards */
        case 0xE005:
            returnedCode = kVK_ANSI_KeypadClear;       // 0x47
            break;
        case 0xE007:
            returnedCode = kVK_ANSI_KeypadEnter;       // 0x4C
            break;
        case 0xE018:
            returnedCode = kVK_ANSI_Semicolon;         // 0x29
            break;
        case 0xE019:
            returnedCode = kVK_ANSI_KeypadEquals;      // 0x51
            break;
        case 0xE01A:
            returnedCode = kVK_ANSI_Keypad0;           // 0x52
            break;
        case 0xE01B:
            returnedCode = kVK_ANSI_Keypad1;           // 0x53
            break;
        case 0xE01C:
            returnedCode = kVK_ANSI_Keypad2;           // 0x54
            break;
        case 0xE01D:
            returnedCode = kVK_ANSI_Keypad3;           // 0x55
            break;
        case 0xE01E:
            returnedCode = kVK_ANSI_Keypad4;           // 0x56
            break;
        case 0xE01F:
            returnedCode = kVK_ANSI_Keypad5;           // 0x57
            break;
        case 0xE020:
            returnedCode = kVK_ANSI_Keypad6;           // 0x58
            break;
        case 0xE021:
            returnedCode = kVK_ANSI_Keypad7;           // 0x59
            break;
        case 0xE022:
            returnedCode = kVK_ANSI_Keypad8;           // 0x5B
            break;
        case 0xE023:
            returnedCode = kVK_ANSI_Keypad9;           // 0x5C
            break;
        case 0xE024:
            returnedCode = kVK_ANSI_KeypadMultiply;    // 0x43
            break;
        case 0xE025:
            returnedCode = kVK_ANSI_KeypadPlus;        // 0x45
            break;
        case 0xE027:
            returnedCode = kVK_ANSI_KeypadMinus;       // 0x4E
            break;
        case 0xE028:
            returnedCode = kVK_ANSI_KeypadDecimal;     // 0x41
            break;
        case 0xE029:
            returnedCode = kVK_ANSI_KeypadDivide;      // 0x4B
            break;
        /* unshifted keycodes for non-alpha keys that require pressing the shift modifier */
        case 0x0021:
            // "!"
            isShifted = YES;
            returnedCode = kVK_ANSI_1;             // 0x12
            break;
        case 0x0040: 
            // "@"
            isShifted = YES;
            returnedCode = kVK_ANSI_2;             // 0x13
            break;
        case 0x0023: 
            // "#"
            isShifted = YES;
            returnedCode = kVK_ANSI_3;             // 0x14
            break;
        case 0x0024: 
            // "$"
            isShifted = YES;
            returnedCode = kVK_ANSI_4;             // 0x15
            break;
        case 0x0025: 
            // "%"
            isShifted = YES;
            returnedCode = kVK_ANSI_5;             // 0x17
            break;
        case 0x005E: 
            // "^"
            isShifted = YES;
            returnedCode = kVK_ANSI_6;             // 0x16
            break;
        case 0x0026: 
            // "&"
            isShifted = YES;
            returnedCode = kVK_ANSI_7;             // 0x1A
            break;
        case 0x002A: 
            // "*"
            isShifted = YES;
            returnedCode = kVK_ANSI_8;             // 0x1C
            break;
        case 0x0028: 
            // "("
            isShifted = YES;
            returnedCode = kVK_ANSI_9;             // 0x19
            break;
        case 0x0029: 
            // ")"
            isShifted = YES;
            returnedCode = kVK_ANSI_0;             // 0x1D
            break;
        case 0x005F: 
            // "_"
            isShifted = YES;
            returnedCode = kVK_ANSI_Minus;         // 0x1B
            break;
        case 0x002B: 
            // "+"
            isShifted = YES;
            returnedCode = kVK_ANSI_Equal;         // 0x18
            break;
        case 0x003F: 
            // "?"
            isShifted = YES;
            returnedCode = kVK_ANSI_Slash;         // 0x2C
            break;
        case 0x003C: 
            // "<"
            isShifted = YES;
            returnedCode = kVK_ANSI_Comma;         // 0x2B
            break;
        case 0x003E: 
            // ">"
            isShifted = YES;
            returnedCode = kVK_ANSI_Period;        // 0x2F
            break;
        case 0x007C: 
            // "|"
            isShifted = YES;
            returnedCode = kVK_ANSI_Backslash;     // 0x2A
            break;
        case 0x007B: 
            // "{"
            isShifted = YES;
            returnedCode = kVK_ANSI_LeftBracket;   // 0x21
            break;
        case 0x007D: 
            // "}"
            isShifted = YES;
            returnedCode = kVK_ANSI_RightBracket;  // 0x1E
            break;
        default:
        {
            CFBooleanRef cfIsShifted = kCFBooleanFalse;
            returnedCode = keyCodeForKeystroke(keystroke, &cfIsShifted);
            if (CFEqual(cfIsShifted, kCFBooleanTrue)) {
                isShifted = YES;
            }
            break;
        }
    }
    if (isShiftedPtr) {
        *isShiftedPtr = isShifted;
    }
    return returnedCode;
}

// Look up the key code the hard way using the actual keyboard layout.
// Thanks to http://stackoverflow.com/questions/1918841/how-to-convert-ascii-character-to-cgkeycode

/* Returns string representation of key, if it is printable.
 * Ownership follows the Create Rule; that is, it is the caller's
 * responsibility to release the returned object. */
CFStringRef createStringForKeyCode(CGKeyCode keyCode)
{
    TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardInputSource();
    CFDataRef layoutData =
    TISGetInputSourceProperty(currentKeyboard,
                              kTISPropertyUnicodeKeyLayoutData);
    const UCKeyboardLayout *keyboardLayout =
    (const UCKeyboardLayout *)CFDataGetBytePtr(layoutData);
    
    UInt32 keysDown = 0;
    UniChar chars[4];
    UniCharCount realLength;
    
    UCKeyTranslate(keyboardLayout,
                   keyCode,
                   kUCKeyActionDisplay,
                   0,
                   LMGetKbdType(),
                   kUCKeyTranslateNoDeadKeysBit,
                   &keysDown,
                   sizeof(chars) / sizeof(chars[0]),
                   &realLength,
                   chars);
    CFRelease(currentKeyboard);    
    
    return CFStringCreateWithCharacters(kCFAllocatorDefault, chars, 1);
}

/* Returns key code for given alphabetic web driver keystroke, or UINT16_MAX on error or not found. */
// Keystrokes can be shifted, e.g. "A" or "@". In these cases we need to get the unshifted character,
// If a keystroke is alphabetic, then it can be converted to lower case easily.
// But if it is a shifted symbol (e.g. "@" is a shifted "2") then we have to translate it manually.
CGKeyCode keyCodeForKeystroke(UniChar keystroke, CFBooleanRef* cfIsShifted)
{
    static CFMutableDictionaryRef charToCodeDict = NULL;
    CGKeyCode keyCode;
    *cfIsShifted = kCFBooleanFalse;
    
    /* Generate static table of keycodes and characters. Only needs to be done once. */
    if (charToCodeDict == NULL) {
        size_t i;
        charToCodeDict = CFDictionaryCreateMutable(kCFAllocatorDefault,
                                                   128,
                                                   &kCFCopyStringDictionaryKeyCallBacks,
                                                   NULL);
        if (charToCodeDict == NULL) return UINT16_MAX;
        
        /* Loop through every keycode (0 - 127) to find its current mapping. */
        for (i = 0; i < 128; ++i) {
            CFStringRef string = createStringForKeyCode((CGKeyCode)i);
            if (string != NULL) {
                CFDictionaryAddValue(charToCodeDict, string, (const void *)i);
                CFRelease(string);
            }
        }
    }
    
    CFStringRef charStr = CFStringCreateWithCharacters(kCFAllocatorDefault, &keystroke, 1);
    CFMutableStringRef lowerCharStr = CFStringCreateMutableCopy(CFAllocatorGetDefault(), 1, charStr);
    CFStringLowercase(lowerCharStr, CFLocaleCopyCurrent());
    if (cfIsShifted != nil) {
        *cfIsShifted = CFEqual(charStr, lowerCharStr) ? kCFBooleanFalse : kCFBooleanTrue;
    }
    
    /* Our values may be NULL (0), so we need to use this function. */
    if (!CFDictionaryGetValueIfPresent(charToCodeDict, lowerCharStr,
                                       (const void **)&keyCode)) {
        keyCode = UINT16_MAX;
    }
    
    CFRelease(charStr);
    CFRelease(lowerCharStr);
    return keyCode;
}

@end
