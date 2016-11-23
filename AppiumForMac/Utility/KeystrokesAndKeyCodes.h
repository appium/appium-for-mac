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

@interface KeystrokesAndKeyCodes : NSObject

+ (CGKeyCode)keyCodeWithWebDriverKeystroke:(UniChar)keystroke isShifted:(BOOL *)isShiftedPtr;

@end
