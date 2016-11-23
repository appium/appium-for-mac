//
//  AfMHTTPServer.h
//  AppiumForMac
//
//  Created by Dan Cuellar on 7/28/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import "AfMHandlers.h"
#import "HTTPServer.h"

@interface AfMHTTPServer : HTTPServer

@property AfMHandlers *handlers;

@end
