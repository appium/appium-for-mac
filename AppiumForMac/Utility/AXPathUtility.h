//
//  AXPathUtility.h
//  AppiumForMac
//
//  Created by Russell, Stuart on 11/25/16.
//  Copyright © 2016 Appium. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Carbon/Carbon.h>

extern NSString * const kNewTextPostedNotification;

@interface AXPathUtility : NSObject

- (NSString *)axPathAtMousePosition:(HIPoint)mousePosition offsetPercent:(NSSize *)offsetPercent;

@end
