//
//  Utility.h
//  AppiumForMac
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

@interface Utility : NSObject

+(NSString*) arch;
+(NSString*) bundleRevision;
+(NSString*) bundleVersion;
+(NSString*) getElementIDFromPath:(NSString*)path;
+(NSString*) getItemFromPath:(NSString*)path withSeparator:(NSString*)separator;
+(NSString*) getSessionIDFromPath:(NSString*)path;
+(NSString*) randomStringOfLength:(int)length;
+(int) unixTimestamp;
+(NSString*) version;
+(NSDictionary*) dictionaryFromPostData:(NSData*)postData;
+ (BOOL)isRunningInSandbox;

// Returns a point guaranteed to be offscreen, to use as an invalid return value.
+ (HIPoint)invalidPoint;

@end
