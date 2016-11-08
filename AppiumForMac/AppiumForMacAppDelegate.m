//
//  AppiumAppDelegate.m
//  AppiumAppleScriptProxy
//
//  Created by Dan Cuellar on 7/27/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AppiumForMacAppDelegate.h"

#import "AfMHTTPServer.h"
#import "AfMHTTPConnection.h"
#import "HTTPServer.h"
#import "DDLog.h"
#import "DDTTYLogger.h"
#import "Utility.h"

static const int ddLogLevel = LOG_LEVEL_VERBOSE;

@implementation AppiumForMacAppDelegate

-(void) applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [DDLog addLogger:[DDTTYLogger sharedInstance]];
    self.afmHTTPServer = [[AfMHTTPServer alloc] init];
    [self.afmHTTPServer setType:@"_http._tcp."];
    [self.afmHTTPServer setPort:4622];
    [self.afmHTTPServer setName:[NSString stringWithFormat:@"Appium for Mac (%@)", [Utility bundleVersion]]];
    [self.afmHTTPServer setConnectionClass:[AfMHTTPConnection class]];
	NSError *error = nil;
	if(![self.afmHTTPServer start:&error])
	{
		DDLogError(@"Error starting HTTP Server: %@", error);
	}
}

@end
