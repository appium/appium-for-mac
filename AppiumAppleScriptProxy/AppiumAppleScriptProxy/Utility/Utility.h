//
//  Utility.h
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import <Foundation/Foundation.h>

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

@end
