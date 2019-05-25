//
//  AXPathUtility.m
//  AppiumForMac
//
//  Created by Stuart Russell at Intuit, Inc. on 11/25/16.
//  Copyright © 2016 Appium. All rights reserved.
//

#import <AppKit/AppKit.h>
#import <PFAssistive/PFAssistive.h>

#import "AXPathUtility.h"
#import "AppiumForMacAppDelegate.h"
#import "DDLog.h"

NSUInteger const kMaximumTitleLength = 256;
NSString * const kNewTextPostedNotification = @"NewTextPosted";
static const int ddLogLevel = LOG_LEVEL_VERBOSE;

@implementation AXPathUtility

- (instancetype)init
{
    self = [super init];
    if (self) {
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(fnKeyTrigger:) name:AFMFnKeyTriggerNotification object:nil];
    }
    return self;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)fnKeyTrigger:(NSNotification *)notification
{
    HIPoint currentMousePosition = [self getMousePosition];
    NSSize offset;
    NSString *axPath = [self axPathAtMousePosition:currentMousePosition offsetPercent:&offset];
    DDLogCInfo(@"axPath:\n%@", axPath);
    if (axPath && axPath.length) {
        // Add surrounding double quotes to the path string.
        NSString *quotedAXPath = [NSString stringWithFormat:@"\"%@\"", axPath];
        
        // Copy the axPath to the clipboard with a couple of blank lines.
        NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
        [pasteboard clearContents];
        [pasteboard writeObjects:@[quotedAXPath]];
//        [pasteboard writeObjects:[quotedAXPath arrayByAddingObject:@"\n"]];
        
        [[NSNotificationCenter defaultCenter] postNotificationName:kNewTextPostedNotification object:quotedAXPath];
        
        // Bounce the dock icon once.
        NSInteger rq = [NSApp requestUserAttention:NSInformationalRequest];
        [NSApp cancelUserAttentionRequest:rq];
    }
}

// Given a screen-based point, returns an absolute axPath
- (NSString *)axPathAtMousePosition:(HIPoint)mousePosition offsetPercent:(NSSize *)offsetPercent
{
    NSError *error = nil;
    PFUIElement *hitElement = [PFUIElement elementAtPoint:mousePosition withDelegate:nil error:&error];
    if (!hitElement) {
        // No element under the mouse? How odd.
        DDLogCInfo(@"axPathAtMousePosition ERROR no hitElement at mousePosition: %@ NSError:%@", NSStringFromPoint(mousePosition), error);
        return nil;
    }
    
    // Get the offsetPercent as an HISize. This can be used to calculate the offset for the moveto command. 
    if (offsetPercent != nil) {
        *offsetPercent = NSMakeSize(0.0, 0.0);
        NSValue *value = [hitElement valueForAttribute:NSAccessibilityPositionAttribute];
        HIPoint elementUpperLeftCorner = [value pointValue];
        HIPoint absoluteOffset = NSMakePoint(mousePosition.x - elementUpperLeftCorner.x, mousePosition.y - elementUpperLeftCorner.y);
        value = [hitElement valueForAttribute:NSAccessibilitySizeAttribute];
        HISize elementSize = [value sizeValue];
        *offsetPercent = NSMakeSize(absoluteOffset.x / elementSize.width, absoluteOffset.y / elementSize.height);
        
        //        NSLog(@"axPathAtMousePosition elementUpperLeftCorner: %@", NSStringFromPoint(elementUpperLeftCorner));
        //        NSLog(@"axPathAtMousePosition elementSize           : %@", NSStringFromSize(elementSize));
        //        NSLog(@"axPathAtMousePosition offsetPercent         : %@", NSStringFromSize(*offsetPercent));
    }
    
    // Construct an axPath
    NSMutableString *axPath = [@"" mutableCopy];
    
    NSArray *hitPath = [hitElement path];
    for (PFUIElement *element in hitPath) {
        //        NSLog(@"hitTestAXElement: %@", [element debugDescription]);
        
        NSString *role = [element AXRole];
        NSString *title = [element AXTitle];
        id value = [element AXValue];
        
        // Dock icon contextual menus require special handling with a different method.
        if ([role isEqualToString:@"AXApplication"]) {
            if (title && [title isEqualToString:@"Dock"] && [[hitElement AXRole] isEqualToString:@"AXMenuItem"]) {
                NSString *dockAXPath = [self axPathForDockAXMenuItem:hitElement];
                if (dockAXPath && dockAXPath.length) {
                    return dockAXPath;
                }
            }
        }
        
        // Check if the item is child element that can be accessed with an index in the xPath.
        BOOL elementHasIndex = NO;
        NSUInteger index = NSNotFound;
        PFUIElement *parent = [element AXParent];
        if (parent) {
            NSArray *siblings = [parent AXChildren];
            if (siblings) {
                // This next method call shows how PFUIElement is a very nice API.
                index = [element indexAmongLikeElementsInArray:siblings];
                if (index != NSNotFound) {
                    elementHasIndex = YES;
                }
            }
        }
        
        [axPath appendString:@"/"];
        if (role) {
            [axPath appendString:role];
        } else {
            DDLogCInfo(@"axPathAtMousePosition ERROR role is nil for element:%@ partialAXPath:%@", hitElement, axPath);
        }
        
        // Add a predicate to the role.
        // Add as many interesting attributes as we can detect, that have non-nil values. The scripter can edit.
        // If no interesting attributes available, if the element is indexed, use the index integer; 
        // otherwise use the (sub)role string without a predicate.
        NSMutableArray *attributeComparisons = [@[] mutableCopy];
        
        if (title && title.length > 0 && title.length <= kMaximumTitleLength) {
            [attributeComparisons addObject:[NSString stringWithFormat:@"@AXTitle='%@'", title]];
        }
        if (value && [value isKindOfClass:[NSString class]] && [value length] > 0 && [value length] <= kMaximumTitleLength) {
            [attributeComparisons addObject:[NSString stringWithFormat:@"@AXValue='%@'", value]];
        }
        
        NSString *attrVal;
        NSArray *attrNames = @[@"AXIdentifier", @"AXDOMIdentifier", @"AXSubrole"];
        for (NSString *attrName in attrNames) {
            attrVal = [element valueForAttribute:attrName];
            if (attrVal && [attrVal isKindOfClass:[NSString class]] && [attrVal length] > 0 && [attrVal length] <= kMaximumTitleLength) {
                [attributeComparisons addObject:[NSString stringWithFormat:@"@%@='%@'", attrName, attrVal]];
            }
        }
        
        if ([attributeComparisons count]) {
            NSMutableString *predStr = [@"" mutableCopy];
            
            [predStr appendString:attributeComparisons[0]];
            for (int i = 1; i < [attributeComparisons count]; i++) {
                [predStr appendString:@" and "];
                [predStr appendFormat:@"%@", attributeComparisons[i]];
            }
            
            [axPath appendFormat:@"[%@]", predStr];
        } else if (elementHasIndex) {
            [axPath appendFormat:@"[%lu]", index];
        }
    }
    
    return axPath;
}

// Fix Dock item contextual menu Accessibility tree, which has different paths for going up and going down. 
// One path, constructed child to parent (see also Accessibility Inspector):
//      /AXApplication[@AXTitle='Dock']/AXMenu/AXMenuItem[@AXTitle='Hide']
//      In this case AXMenu is in the AXChildren array of the AXApplication.
// But the actual path does not go through the children array:
//      /AXApplication[@AXTitle='Dock']/AXFocusedUIElement/AXMenuItem[@AXTitle='Hide']
//      The AXFocusedUIElement is an AXMenu which is NOT in the AXChildren array.
// /AXApplication[@AXTitle='Dock']/AXFocusedUIElement/AXMenuItem[@Title='Show In Finder']
// /AXApplication[@AXTitle='Dock']/AXFocusedUIElement/AXMenuItem[@Title='Options']/AXMenu/AXMenuItem[@Title='Show In Finder']
// (indexes can vary per application and per application state e.g. running or not, active/non-active/hidden)
- (NSString *)axPathForDockAXMenuItem:(PFUIElement *)element
{
    DDLogCInfo(@"axPathForDockAXMenuItem element:%@", element);
    
    if (!element || ![element isRole:@"AXMenuItem"]) {
        return nil;
    }
    
    //  TODO: Pure indexes are problematic, so use strings. For non-US Engish locales, this may not work.
    NSMutableString *axPath = [@"" mutableCopy];
        
    // Check if the menu item is a submenu axPath variables for submenu.
    BOOL isSubmenu = NO;
    PFUIElement *parent = [element AXParent];       // AXMenu, which may or may not also be AXFocusedUIElement
    PFUIElement *grandParent = [parent AXParent];   // AXApplication or AXMenuItem
    PFApplicationUIElement *dockApp;
    if ([grandParent isRole:@"AXApplication"]) {
        isSubmenu = NO;
        dockApp = (PFApplicationUIElement *)grandParent;
    } else if ([grandParent isRole:@"AXMenuItem"]) {
        isSubmenu = YES;
        dockApp = (PFApplicationUIElement *)[[grandParent AXParent] AXParent];
    }
    
    DDLogCInfo(@"dockApp AXChildren:\n%@", [dockApp AXChildren]);
    DDLogCInfo(@"dockApp AXFocusedUIElement:\n%@", [dockApp AXFocusedUIElement]);
    // Big assumption: the Dock will always be named "Dock".
    [axPath appendString:@"/AXApplication[@AXTitle='Dock']"];
    
    // Build the axPath going through the Dock item for this contextual menu.
    // Dock items are accessed through an AXList in the Dock's AXChildren.
    // Assumption: never more than one Dock item with a non-nil AXShownMenuUIElement;
    NSArray *dockChildren = [dockApp AXChildren];
    NSUInteger indexOfAXList = [dockChildren indexOfObjectWithOptions:NSEnumerationConcurrent passingTest:^BOOL(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if ([obj isKindOfClass:[PFUIElement class]] && [obj isRole:@"AXList"]) {
            *stop = YES;
            return YES;
        }
        return NO;
    }];
    PFUIElement *dockAXList = [dockChildren objectAtIndex:indexOfAXList];
    NSArray *dockItemList = [dockAXList AXChildren];
    NSUInteger indexOfItemWithMenu = [dockItemList indexOfObjectWithOptions:NSEnumerationConcurrent passingTest:^BOOL(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
        if ([obj isKindOfClass:[PFUIElement class]] && [obj isRole:@"AXDockItem"] && [obj valueForAttribute:@"AXShownMenuUIElement"] != nil) {
            *stop = YES;
            return YES;
        }
        return NO;
    }];
    PFUIElement *dockItemWithMenu = [dockItemList objectAtIndex:indexOfItemWithMenu];
    // Bug in the Dock.app: AXShownMenuUIElement should be a UI Element but can be returned as an array.
    [axPath appendFormat:@"/AXList[0]/AXDockItem[@AXTitle='%@']/AXMenu[0]", [dockItemWithMenu AXTitle]];
    
    if (isSubmenu) {
        // Add the title/index of the top level menu item, which is no longer included in the child array,
        // and is no longer AXFocusedUIElement. It can only be found if you have the menu item,
        // then go upwards through the parent menus.
        
        NSString *grandTitle = [grandParent AXTitle];
        NSUInteger grandIndex = 0;
        PFUIElement *greatGrandParent = [grandParent AXParent];   // AXMenu which is also AXFocusedUIElement
        NSArray *grandSiblings = [greatGrandParent AXChildren];
        if (grandSiblings) {
            grandIndex = [grandParent indexAmongLikeElementsInArray:grandSiblings];
        }
        if (grandTitle && grandTitle.length > 0) {
            [axPath appendFormat:@"/AXMenuItem[@AXTitle='%@']/AXMenu[0]", grandTitle];
        } 
        else if (grandIndex != NSNotFound) {
            [axPath appendFormat:@"/AXMenuItem[%lu]/AXMenu[0]", grandIndex];
        }
    } else {
        // This is the title 
    }
    
    // Get an index. This is a menu item so we know it has an index in its menu.
    NSUInteger index = NSNotFound;
    NSArray *siblings = [parent AXChildren];
    if (siblings) {
        index = [element indexAmongLikeElementsInArray:siblings];
    }
    NSString *title = [element AXTitle];
    if (title && title.length > 0) {
        [axPath appendFormat:@"/AXMenuItem[@AXTitle='%@']", title];
    } else if (index != NSNotFound) {
        [axPath appendFormat:@"/AXMenuItem[%lu]", index];
    }
    if (index != NSNotFound) {
        [axPath appendFormat:@"/AXMenuItem[%lu]", index];
    }
    return axPath;
}

- (HIPoint)getMousePosition
{
    HIPoint position;
    HICoordinateSpace space = kHICoordSpaceScreenPixel;
    HIGetMousePosition(space, NULL, &position);
    return position;
}



@end
