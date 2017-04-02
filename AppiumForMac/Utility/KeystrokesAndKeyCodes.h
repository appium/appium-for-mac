//
//  KeystrokesAndKeyCodes.h
//  AppiumForMacFramework
//
//  Created by Stuart Russell at Intuit, Inc.
//  Copyright © 2016 Appium. All rights reserved.
//

#import <Carbon/Carbon.h>
#import <Foundation/Foundation.h>

// See Carbon/HIToolbox/Events.h for a list of virtual key codes

// These constants are used in multiple places, and given names for readability.
typedef NS_ENUM(UniChar, WebDriverKeyStroke) {
    wdShiftKey = 0xE008,
    wdControlKey = 0xE009,
    wdAltKey = 0xE00A,
    wdCommandKey = 0xE03D
};

@interface KeystrokesAndKeyCodes : NSObject

+ (CGKeyCode)keyCodeWithWebDriverKeystroke:(UniChar)keystroke isShifted:(BOOL *)isShiftedPtr;

@end
