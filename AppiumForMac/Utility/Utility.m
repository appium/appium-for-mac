//
//  Utility.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "Utility.h"

@implementation Utility

+(NSString*) arch
{
    switch ([[NSRunningApplication currentApplication] executableArchitecture])
    {
        case NSBundleExecutableArchitectureI386:
            return @"i386";
        case NSBundleExecutableArchitectureX86_64:
            return @"x86_64";
        case NSBundleExecutableArchitecturePPC:
            return @"ppc";
        case NSBundleExecutableArchitecturePPC64:
            return @"ppc64";
        default:
            return @"unknown";
    }
}

+(NSString*) bundleRevision
{
    NSDictionary *info = [[NSBundle mainBundle] infoDictionary];
    return [info objectForKey:@"CFBundleVersion"];
}

+(NSString*) bundleVersion
{
    NSDictionary *info = [[NSBundle mainBundle] infoDictionary];
    return [info objectForKey:@"CFBundleShortVersionString"];
}

+(NSString*) getElementIDFromPath:(NSString*)path
{
    return [self getItemFromPath:path withSeparator:@"/element/"];
}

+(NSString*) getItemFromPath:(NSString*)path withSeparator:(NSString*)separator
{
    NSArray *components = [path componentsSeparatedByString:separator];
    if (components.count < 2)
    {
        return nil;
    }
    components = [(NSString*)[components objectAtIndex:1] componentsSeparatedByString:@"/"];
    return components.count > 1 ? (NSString*)[components objectAtIndex:0] : nil;
}

+(NSString*) getSessionIDFromPath:(NSString*)path
{
    return [self getItemFromPath:path withSeparator:@"/session/"];
}

+(NSString*) randomStringOfLength:(int)length
{
    NSString *letters = @"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    NSMutableString *randomString = [NSMutableString stringWithCapacity: length];

    for (int i=0; i<length; i++)
    {
        [randomString appendFormat: @"%C", [letters characterAtIndex: arc4random() % [letters length]]];
    }
    return randomString;
}

+(int) unixTimestamp
{
    return [[NSDate date] timeIntervalSince1970];
}

+(NSString*) version
{
    NSDictionary *systemVersionDictionary = [NSDictionary dictionaryWithContentsOfFile: @"/System/Library/CoreServices/SystemVersion.plist"];
    return [systemVersionDictionary objectForKey:@"ProductVersion"];
}

@end
