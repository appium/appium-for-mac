//
//  AfMStatusCodes.h
//  AppiumForMac
//
//  Created by Dan Cuellar on 8/18/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSUInteger, AFMStatusCode) {
    kAfMStatusCodeSuccess = 0,
    kAfMStatusCodeNoSuchDriver = 6,
    kAfMStatusCodeNoSuchElement = 7,
    kAfMStatusCodeNoSuchFrame = 8,
    kAfMStatusCodeUnknownCommand = 9,
    kAfMStatusCodeStaleElementReference = 10,
    kAfMStatusCodeElementNotVisible = 11,
    kAfMStatusCodeInvalidElementState = 12,
    kAfMStatusCodeUnknownError = 13,
    kAfMStatusCodeElementIsNotSelectable = 15,
    kAfMStatusCodeJavascriptError = 17,
    kAfMStatusCodeXPathLookupError = 19,
    kAfMStatusCodeTimeout = 21,
    kAfMStatusCodeNoSuchWindow = 23,
    kAfMStatusCodeInvalidCookieDomain = 24,
    kAfMStatusCodeUnableToSetCookie = 25,
    kAfMStatusCodeUnexpectedAlertOpen = 26,
    kAfMStatusCodeNoAlertOpenError = 27,
    kAfMStatusCodeScriptTimeout = 28,
    kAfMStatusCodeInvalidElementCoordinates = 29,
    kAfMStatusCodeIMENotAvailable = 30,
    kAfMStatusCodeIMEEngineActivationFailed = 31,
    kAfMStatusCodeInvalidSelector = 32,
    kAfMStatusCodeSessionNotCreatedException = 33,
    kAfMStatusCodeMoveTargetOutOfBounds = 34,
    kAfMStatusCodeInvalidArgument = 35,
};

FOUNDATION_EXPORT NSString * const kAfMStatusCodeMessages[];