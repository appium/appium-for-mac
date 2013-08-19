//
//  AfMStatusCodes.h
//  AppiumForMac
//
//  Created by Dan Cuellar on 8/18/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import <Foundation/Foundation.h>

//
//  AfMStatusCodes.m
//  AppiumForMac
//
//  Created by Dan Cuellar on 8/18/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AfMStatusCodes.h"

FOUNDATION_EXPORT const int kAfMStatusCodeSuccess;
FOUNDATION_EXPORT const int kAfMStatusCodeNoSuchDriver;
FOUNDATION_EXPORT const int kAfMStatusCodeNoSuchElement;
FOUNDATION_EXPORT const int kAfMStatusCodeNoSuchFrame;
FOUNDATION_EXPORT const int kAfMStatusCodeUnknownCommand;
FOUNDATION_EXPORT const int kAfMStatusCodeStaleElementReference;
FOUNDATION_EXPORT const int kAfMStatusCodeElementNotVisible;
FOUNDATION_EXPORT const int kAfMStatusCodeInvalidElementState;
FOUNDATION_EXPORT const int kAfMStatusCodeUnknownError;
FOUNDATION_EXPORT const int kAfMStatusCodeElementIsNotSelectable;
FOUNDATION_EXPORT const int kAfMStatusCodeJavascriptError;
FOUNDATION_EXPORT const int kAfMStatusCodeXPathLookupError;
FOUNDATION_EXPORT const int kAfMStatusCodeTimeout;
FOUNDATION_EXPORT const int kAfMStatusCodeNoSuchWindow;
FOUNDATION_EXPORT const int kAfMStatusCodeInvalidCookieDomain;
FOUNDATION_EXPORT const int kAfMStatusCodeUnableToSetCookie;
FOUNDATION_EXPORT const int kAfMStatusCodeUnexpectedAlertOpen;
FOUNDATION_EXPORT const int kAfMStatusCodeNoAlertOpenError;
FOUNDATION_EXPORT const int kAfMStatusCodeScriptTimeout;
FOUNDATION_EXPORT const int kAfMStatusCodeInvalidElementCoordinates;
FOUNDATION_EXPORT const int kAfMStatusCodeIMENotAvailable;
FOUNDATION_EXPORT const int kAfMStatusCodeIMEEngineActivationFailed;
FOUNDATION_EXPORT const int kAfMStatusCodeInvalidSelector;
FOUNDATION_EXPORT const int kAfMStatusCodeSessionNotCreatedException;
FOUNDATION_EXPORT const int kAfMStatusCodeMoveTargetOutOfBounds;

FOUNDATION_EXPORT NSString * const kAfMStatusCodeMessages[];