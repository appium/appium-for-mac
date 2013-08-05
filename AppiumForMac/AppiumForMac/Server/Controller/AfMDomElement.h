//
//  AfMDomElement.h
//  AppiumForMac
//
//  Created by Dan Cuellar on 8/4/13.
//  Copyright (c) 2013 Appium. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface AfMDomElement : NSObject

@property NSString *type;
@property NSString *identifier;
@property AfMDomElement *parent;
@property NSMutableArray *children;

+(NSDictionary*) pageSource:(NSString*)content;

-(BOOL) isEqualTo:(AfMDomElement*)otherElement;
-(NSString*) stringValue;
-(NSString*) jsonify;

@end
