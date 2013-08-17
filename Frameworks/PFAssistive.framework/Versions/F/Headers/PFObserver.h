/*
 PFObserver.h
 PFAssistive Framework 3.1.0
 Copyright 2003-2010 Bill Cheeseman. Used by permission.
 All rights reserved.
*/

/* IMPORTANT COPYRIGHT AND LICENSING INFORMATION:
 
 This is a summary of the PreForm Assistive Technology, LLC licenses. For the legally binding terms consult the licenses themselves. The licenses are embedded in the frameworks’ bundles, and they are also available for download at pfiddlesoft.com. 
 
 FREE FOR PERSONAL USE AND FOR DISTRIBUTION AND USE WITH FREE PRODUCTS
 
 The PFiddlesoft Frameworks may be licensed free of charge for personal use, including use during development of any client application or other software. They may also be licensed free of charge for distribution and use with any client application or other software that you distribute to the public free of charge (including freeware as well as free beta or trial versions of a product for which you intend to request or require payment in the future). You are required only to give notice to PreForm Assistive Technologies, LLC, to provide attribution to PreForm Assistive Technologies, LLC in your client application or other software, and to include the copyright notice and license in your client application or other software.
 
 ONE-TIME LICENSING FEE FOR DISTRIBUTION AND USE WITH PAID PRODUCTS
 
 If you distribute the PFiddlesoft Frameworks with or in a client application or other software product for which you request or require payment, or if you distribute a client application or other software product that makes use of the PFiddlesoft Frameworks for which you request or require payment, such as donationware, shareware and commercial applications, or for internal use within a for-profit organization, you must within thirty days of initial distribution of your product pay PreForm Assistive Technologies, LLC a flat one-time license fee of $250 U.S. for each framework that you distribution or use, regardless of the number of units of your product you distribute or use. This fee covers all present and future versions of your product, but any separate and distinct product requires you to pay PreForm Assistive Technologies, LLC an additional licensing fee of $250 U.S. for each framework that you distribute or use, as described above.
 
 An executed license is required both for free distribution or use, and for distribution or use subject to a flat one-time license fee with a product for which you request or require payment. Print the PFAssistive Framework distribution license or the PFEventTaps Framework distribution license or both of them, depending on which of the PFiddlesoft Frameworks you distribute or use, in duplicate. Then fill in the blanks, sign them, and mail them to:
 
 PreForm Assistive Technologies, LLC
 P.O. Box 326
 Quechee, VT 05059-0326
 
 DIFFERENT TERMS APPLY TO LARGE OR ESTABLISHED COMMERCIAL SOFTWARE DEVELOPERS. The source code is available for an additional fee. Contact PreForm Assistive Technologies, LLC for details.
 
 DISCLAIMERS
 
 This Framework is provided on an "AS IS" basis.
 
 PREFORM ASSISTIVE TECHNOLOGIES, LLC MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING THE FRAMEWORK OR ITS USE AND OPERATION ALONE OR IN COMBINATION WITH OTHER PRODUCTS. THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY DISCLAIMED.
 
 IN NO EVENT SHALL PREFORM ASSISTIVE TECHNOLOGIES, LLC BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION OF THE FRAMEWORK, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF PREFORM ASSISTIVE TECHNOLOGIES, LLC HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. YOUR REMEDY FOR ANY DEFECT OR FAULT IN THE FRAMEWORK IS LIMITED TO REFUND OF THE LICENSE FEE YOU PAID.
*/

/*!
 @header PFObserver.h
 @copyright 2003-2010 Bill Cheeseman. Used by permission. All rights reserved.
 @version 3.1.0
 @abstract PFObserver.h supports observing changes to UI elements in running applications in real time as changes occur.
 @discussion The accessibility API implements the concept of an "observer," an object based on Core Foundation's CFTypeRef class that registers to observe a user interface element on the screen in a running application and issues accessibility notifications when changes occur. PFObserver.h implements this same concept in its PFObserver class.
*/

#import <Cocoa/Cocoa.h>
#import <PFAssistive/PFUIElement.h>

//#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5

@protocol PFObserverDelegate;

#pragma mark -
/*!
 @class PFObserver
 @abstract A concrete class representing an observer that may be registered to observe changes to UI elements in a running application.
 @discussion A PFObserver object represents an observer that may be registered to observe changes to UI elements on the screen in a running application, such as a menu, window or button.
 
 The PFObserver class implements all of the functionality of the accessibility API's AXObserverRef object. It declares a delegate method, @link applicationWithIdentifier:atPath:didPostAccessibilityNotification:fromObservedUIElement:forAffectedUIElement: -applicationWithIdentifier:atPath:didPostAccessibilityNotification:fromObservedUIElement:forAffectedUIElement: @/link that a client application can implement to monitor and respond to accessibility notifications. As an alternative, it permits you to write a similar callback method using Objective-C.
 
 In this documentation "observer" refers to a programmatic PFObserver object. Depending on context, "UI element" or "element" may refer to a programmatic PFUIElement or PFApplicationUIElement object or to the user interface object in the running application that is represented by a PFUIElement or PFApplicationUIElement object. References to a "PFUIElement object" generally include a PFApplicationUIElement object, since the PFApplicationUIElement class is a subclass of PFUIElement.
 
 A PFObserver object can register to receive various notifications that are posted by UI elements in any running application when they change state; for example, when an application is activated or a window is closed. When an observer has registered for a notification, a client's delegate method or callback method is triggered to take any desired action in response to the change in the observed UI element. You can create multiple observers and register each of them for one or more notifications, but it is usually easier to register a single observer in an application and register it for all of the notifications of interest. Observers can be unregistered at will for individual notifications or for all of them. Observers maintain lists of all of their current registrations.
 
 When they change state, accessibility API objects post notifications identified by a string, usually beginning with the prefix "AX" as in "AXWindowCreated". In a client application, you normally use the NSString constants declared in the NSAccessibility informal protocol (in NSAccessibility.h, in the Cocoa AppKit framework). Alternatively, you can use the corresponding CFStringRef constants declared in the accessibility API headers and cast them to NSString*, or use the string itself. Some constants are omitted from NSAccessibility.h or from the accessibility API headers; if you can't find a desired constant in one of these headers, use a constant from the other header or the string itself. Note that some constants in these headers may be marked as obsolete; use only the non-obsolete constants or their string equivalents to maximize compatibility with future releases of Mac OS X.
 
 Some applications implement custom UI elements. Although Apple discourages the invention of new notifications, some applications may implement custom notifications that are not part of Apple's accessibility API. This framework is written so that you can read and manipulate them, as well as Apple's built-in accessibility API notifications. You may be able to identify the strings used to identify custom notifications by reading an application's documentation or by examining the .strings files in its application bundle. The accessibility API currently offers no means to obtain a list of available notifications programmatically.
 
 Public methods in the PFAssistive framework, unless otherwise noted, take Objective-C objects as parameters and, when they return Objective-C objects, return them autoreleased. Clients of the PFAssistive framework should not have to deal with Core Foundation objects. Unless otherwise noted, methods with return values return <code>nil</code>, <code>NULL</code>, <code>0</code>, or <code>NO</code> in the event of an error.
 
 Almost all standard UI element objects in Cocoa and Carbon applications come prewired to post notifications when they change state. Custom UI elements may have been written to post notifications, as well. However, there are many applications that do not use standard UI elements or that use custom elements that do not support the accessibility API. The only way to know which of an application's UI elements support the accessibility API is trial and error and, in some cases, an application's documentation.
 
 The following notifications were introduced in Mac OS X v10.6: NSAccessibilityRowExpandedNotification, NSAccessibilityRowCollapsedNotification, NSAccessibilitySelectedCellsChangedNotification, NSAccessibilityUnitsChangedNotification, and NSAccessibilitySelectedChildrenMovedNotification.

 Before a client application can make use of this class, the "Enable access for assistive devices" setting in the Universal Access pane of System Preferences must be turned on, or the client application must be made trusted process through use of the accessibility API's <code>AXMakeProcessTrusted()</code> function. Authentication may be required.
*/
@interface PFObserver : NSObject <PFUIElementDelegate> {
	@protected
	BOOL isValid;
	
	@private
	AXObserverRef observerRef;
	NSNumber *pidNumber;
	id notificationDelegate;
	SEL callbackSelector;
	NSMutableArray *registrations;
	NSMutableArray *refconPool; // retains registration refcons until self is deallocated
	NSString *lastNotification; // for double-notify bug
	id delegate;
	NSUInteger weakRetainCount;
	void *privateVars;
}

#pragma mark INITIALIZATION
/*!
 @methodgroup INITIALIZATION
*/

#pragma mark Initializers
// The designated initializer for a PFObserver object is -initWithPid:notificationDelegate:callbackSelector:.
// IMPORTANT: it is the client's responsibility to explicitly call -invalidate before releasing the PFObserver object or allowing it to be autoreleased. That removes the run loop source and releases the observerRef, as well as unregistering any observers that haven't already been unregistered, setting the observer's delegate to nil if it uses a delegate that hasn't already been set to nil, and performing other cleanup.

#pragma mark Initializers for Delegates
// Initializers to observe accessibility notifications using a delegate method.

// Initializes and returns a newly created PFObserver object using the path to a target application. Returns nil if the target application is not running or the observer could not be created.
/*!
 @method initWithPath:
 @abstract Initializes and returns a newly allocated PFObserver object with <code>path</code>, the full path to the target application's bundle or file.
 @param fullPath An NSString object giving the full path to the client application file.
 @result An initialized PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this convenience initializer to initialize a newly-allocated observer that uses a delegate method instead of a callback method to respond to accessibility notifications.
 
 A client application must call @link //apple_ref/occ/instm/PFObserver/setDelegate: -setDelegate: @/link and @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo: @/link in order to receive notifications.
 
 See the designated initializer @link initWithPid: -initWithPid:@/link for important information about creating new observer objects.
 
 See also @link initWithURL: -initWithURL: @/link, @link initWithBundleIdentifier: -initWithBundleIdentifier: @/link and @link initWithPid: -initWithPid:@/link.
*/
- (id)initWithPath:(NSString *)fullPath;

/*!
 @method initWithURL:
 @abstract Initializes and returns a newly allocated PFObserver object with <code>url</code>, the file URL for the target application's bundle or file.
 @param url An NSURL object referencing the application.
 @result An initialized PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this convenience initializer to initialize a newly-allocated observer that uses a delegate method instead of a callback method to respond to accessibility notifications.
 
 A client application must call @link //apple_ref/occ/instm/PFObserver/setDelegate: -setDelegate: @/link and @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo: @/link in order to receive notifications.
 
 See the designated initializer @link initWithPid: -initWithPid:@/link for important information about creating new observer objects.
 
 See also @link initWithPath: -initWithPath: @/link, @link initWithBundleIdentifier: -initWithBundleIdentifier: @/link and @link initWithPid: -initWithPid:@/link.
*/
- (id)initWithURL:(NSURL *)url;

/*!
 @method initWithBundleIdentifier:
 @abstract Initializes and returns a newly allocated PFObserver object with <code>bundleIdentifier</code>, the target application's bundle identifier.
 @param bundleIdentifier An NSString object, the application bundle's identifier.
 @result An initialized PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this convenience initializer to initialize a newly-allocated observer that uses a delegate method instead of a callback method to respond to accessibility notifications.
 
 A client application must call @link //apple_ref/occ/instm/PFObserver/setDelegate: -setDelegate: @/link and @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo: @/link in order to receive notifications.
 
 See the designated initializer @link initWithPid: -initWithPid:@/link for important information about creating new observer objects.
 
 See also @link initWithPath: -initWithPath: @/link, @link initWithURL: -initWithURL: @/link and @link initWithPid: -initWithPid:@/link.
*/
- (id)initWithBundleIdentifier:(NSString *)bundleIdentifier;

// Initializes and returns a newly created PFObserver object using a target application's UNIX process ID. Returns nil if the target application is not running or the observer could not be created.
/*!
 @method initWithPid:
 @abstract The designated initializer for the PFObserver class for an observer that uses a delegate method. Initializes and returns a newly allocated PFObserver object with <code>pid</code>, the target application's BSD Unix process identification number (PID).
 @param pid An integer, the running application's BSD Unix process identification number (PID).
 @result An initialized PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this designated initializer to initialize a newly-allocated application observer object that uses a delegate method instead of a callback method to respond to accessibility notifications.
 
 A client application must call @link //apple_ref/occ/instm/PFObserver/setDelegate: -setDelegate: @/link and @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo: @/link in order to receive notifications.
  
 Although this is a designated initializer for the PFObserver class, a client application normally creates new PFObserver objects without using this method directly, instead using the @link observerWithPath: +observerWithPath: @/link, @link observerWithURL: +observerWithURL: @/link, @link observerWithBundleIdentifier: +observerWithBundleIdentifier:@/link or @link observerWithPid: +observerWithPid:@/link class method. The class methods are strongly preferred because they set up private data structures that automatically manage observers.
 
 An observer does nothing unless it has been registered to receive at least one notification with respect to at least one observed UI element of the target application, using @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo:@/link. There can be multiple observers for any one target application, but it is normally sufficient to create one observer per target application and register it for multiple notifications. If you use one of the PFObserver factory methods, such as the @link observerWithURL: +observerWithURL:@/link class method, to create an observer, the observer is managed for you.
 
 When an observer receives a notification from an observed element, it sends the @link applicationWithIdentifier:atPath:didPostAccessibilityNotification:fromObservedUIElement:forAffectedUIElement: -applicationWithIdentifier:atPath:didPostAccessibilityNotification:fromObservedUIElement:forAffectedUIElement: @/link delegate message, passing the target application's identifier and path, the notification, the observed UI element, and the affected UI element (which may differ from the element being observed).
 
 The client's delegate method will always receive the affected element, even when observing <code>NSAccessibilityUIElementDestroyedNotification</code>. However, you must treat a destroyed element as invalid; do not pass it as a parameter to methods that call accessibility API functions. You can, however, use a destroyed element with PFUIElement's @link //apple_ref/occ/instm/PFUIElement/pid -pid@/link, and with PFUIElement's -isEqual: and @link //apple_ref/occ/instm/PFUIElement/isEqualToElement: -isEqualToElement:@/link to manage any element cache you maintain. Note that attempting to pass a destroyed UI element in an accessibility API function is not guaranteed to return an accessibility API error. PFUIElement's @link //apple_ref/occ/instm/PFUIElement/elementInfo -elementInfo@/link method will return some valid information about a destroyed element.
 
 If a delegate method initiates a time-consuming operation or an operation that might fail, consider executing the operation in a secondary thread using a method like NSThread's <code>+detachNewThreadSelector:toTarget:withObject:</code> method to avoid blocking the application. On the other hand, if a callback method runs an AppleScript script using a method like NSAppleScript's <code>-executeAndReturnError:</code> (or the preferred method, OSAScript's <code>-executeAndReturnError:</code>, in the OSAKit framework), always execute the operation using NSObject's <code>-performSelectorOnMainThread:withObject:waithUntilDone:</code> method with <code>wait</code> set to <code>NO</code>, because AppleScript scripts must run on the main application thread if they involve user interaction, as with the 'display dialog' command. AppleScript queues scripts on the main application thread in first-in, first-out (FIFO) order.
 
 All observers maintain a list of the notifications and elements for which they are currently registered. See @link registrations -registrations@/link for details.
 
 See also @link initWithPath: -initWithPath: @/link, @link initWithURL: -initWithURL: @/link and @link initWithBundleIdentifier: -initWithBundleIdentifier:@/link.
*/
- (id)initWithPid:(pid_t)pid;

#pragma mark Initializers for callbacks
// Initializers to observe accessibility notifications using a callback method.

// Initializes and returns a newly created PFObserver object using the path to a target application. Returns nil if the target application is not running or the observer could not be created.
/*!
 @method initWithPath:notificationDelegate:callbackSelector:
 @abstract Initializes and returns a newly allocated PFObserver object with <code>path</code>, the full path to the target application's bundle or file, temporary notification delegate <code>callbackDelegate</code>, and callback selector <code>callback</code>. Provides no observer management.
 @param fullPath An NSString object giving the full path to the client application file.
 @param callbackDelegate An object of any type. A client application typically passes <code>self</code> in this parameter.
 @param callback The notification delegate's Objective-C selector to be invoked when a notification is received.
 @result An initialized PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this convenience initializer or the designated initializer @link initWithPid:notificationDelegate:callbackSelector: -initWithPid:notificationDelegate:calbackSelector: @/link to initialize a newly-allocated application observer object that uses a callback method instead of a delegate method to respond to accessibility notifications.

 A client application must call @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo: @/link in order to receive notifications.

 See the designated initializer @link initWithPid:notificationDelegate:callbackSelector: -initWithPid:notificationDelegate:calbackSelector:@/link for important information about creating new observer objects.
 
 See also @link initWithURL:notificationDelegate:callbackSelector: -initWithURL:notificationDelegate:callbackSelector: @/link, @link initWithBundleIdentifier:notificationDelegate:callbackSelector: -initWithBundleIdentifier:notificationDelegate:callbackSelector:@/link and @link initWithPid:notificationDelegate:callbackSelector: -initWithPid:notificationDelegate:callbackSelector:@/link.
*/
- (id)initWithPath:(NSString *)fullPath notificationDelegate:(id)callbackDelegate callbackSelector:(SEL)callback;

/*!
 @method initWithURL:notificationDelegate:callbackSelector:
 @abstract Initializes and returns a newly allocated PFObserver object with <code>url</code>, the file URL for the target application's bundle or file, temporary notification delegate <code>callbackDelegate</code>, and callback selector <code>callback</code>. Provides no observer management.
 @param url An NSURL object referencing the application.
 @param callbackDelegate An object of any type. A client application typically passes <code>self</code> in this parameter.
 @param callback The notification delegate's Objective-C selector to be invoked when a notification is received.
 @result An initialized PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this convenience initializer to initialize a newly-allocated application observer object that uses a callback method instead of a delegate method to respond to accessibility notifications.
 
 A client application must call @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo: @/link in order to receive notifications.
 
 See the designated initializer @link initWithPid:notificationDelegate:callbackSelector: -initWithPid:notificationDelegate:calbackSelector:@/link for important information about creating new observer objects.
 
 See also @link initWithPath:notificationDelegate:callbackSelector: -initWithPath:notificationDelegate:callbackSelector: @/link, @link initWithBundleIdentifier:notificationDelegate:callbackSelector: -initWithBundleIdentifier:notificationDelegate:callbackSelector:@/link and @link initWithPid:notificationDelegate:callbackSelector: -initWithPid:notificationDelegate:callbackSelector:@/link.
*/
- (id)initWithURL:(NSURL *)url notificationDelegate:(id)callbackDelegate callbackSelector:(SEL)callback;

/*!
 @method initWithBundleIdentifier:notificationDelegate:callbackSelector:
 @abstract Initializes and returns a newly allocated PFObserver object with <code>bundleIdentifier</code>, the target application's bundle identifier, temporary notification delegate <code>callbackDelegate</code>, and callback selector <code>callback</code>. Provides no observer management.
 @param bundleIdentifier An NSString object, the application bundle's identifier.
 @param callbackDelegate An object of any type. A client application typically passes <code>self</code> in this parameter.
 @param callback The notification delegate's Objective-C selector to be invoked when a notification is received.
 @result An initialized PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this convenience initializer to initialize a newly-allocated application observer object that uses a callback method instead of a delegate method to respond to accessibility notifications.
 
 A client application must call @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo: @/link in order to receive notifications.
 
 See the designated initializer @link initWithPid:notificationDelegate:callbackSelector: -initWithPid:notificationDelegate:calbackSelector:@/link for important information about creating new observer objects.
 
 See also @link initWithPath:notificationDelegate:callbackSelector: -initWithPath:notificationDelegate:callbackSelector: @/link, @link initWithURL:notificationDelegate:callbackSelector: -initWithURL:notificationDelegate:callbackSelector:@/link and @link initWithPid:notificationDelegate:callbackSelector: -initWithPid:notificationDelegate:callbackSelector:@/link.
*/
- (id)initWithBundleIdentifier:(NSString *)bundleIdentifier notificationDelegate:(id)callbackDelegate callbackSelector:(SEL)callback;

// Initializes and returns a newly created PFObserver object using a target application's UNIX process ID. Returns nil if the target application is not running or the observer could not be created.
/*!
 @method initWithPid:notificationDelegate:callbackSelector:
 @abstract The designated initializer for the PFObserver class for an observer that uses a callback method. Initializes and returns a newly allocated PFObserver object with <code>pid</code>, the target application's BSD Unix process identification number (PID), temporary notification delegate <code>callbackDelegate</code>, and callback selector <code>callback</code>. Provides no observer management.
 @param pid An integer, the running application's BSD Unix process identification number (PID).
 @param callbackDelegate An object of any type. A client typically passes <code>self</code> in this parameter.
 @param callback The notification delegate's Objective-C selector to be invoked when a notification is received.
 @result An initialized PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this designated initializer to initialize a newly-allocated application observer object that uses a callback method instead of a delegate method to respond to accessibility notifications.
  
 Although this is a designated initializer for the PFObserver class, a client application normally creates new PFObserver objects without using this method directly, instead using the @link observerWithPath:notificationDelegate:callbackSelector: +observerWithPath:notificationDelegate:callbackSelector: @/link, @link observerWithURL:notificationDelegate:callbackSelector: +observerWithURL:notificationDelegate:callbackSelector: @/link, @link observerWithBundleIdentifier:notificationDelegate:callbackSelector: +observerWithBundleIdentifier:notificationDelegate:callbackSelector:@/link or the @link observerWithPid:notificationDelegate:callbackSelector: +observerWithPid:notificationDelegate:callbackSelector:@/link class method. The class methods are strongly preferred because they set up private data structures that automatically manage observers.
 
 An observer does nothing unless it has been registered to receive at least one notification with respect to at least one observed UI element of the target application, using @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo:@/link. There can be multiple observers for any one target application, but it is normally sufficient to create one observer per target application and register it for multiple notifications. If you use one of the PFObserver factory methods, such as the @link observerWithURL:notificationDelegate:callbackSelector: +observerWithURL:notificationDelegate:callbackSelector:@/link class method to create an observer, the observer is managed for you.
 
 When an observer receives a notification from an observed element, it invokes the specified callback selector on the notification delegate. It passes back to the client's callback method the observer, the notification, the affected UI element (which may differ from the element being observed), and any context information that was provided by the client when it registered the observer for the notification. A notification delegate is temporary; the relationship only lasts until the observer is deallocated. The client's callback selector should have the following signature:
 
 <code>- (void)observer:(PFObserver *)observer notification:(NSString *)notification element:(PFUIElement *)element contextInfo:(void *)contextInfo</code>
 
 The client's callback method will always receive the affected element, even when observing <code>NSAccessibilityUIElementDestroyedNotification</code>. However, you must treat a destroyed element as invalid; do not pass it as a parameter to methods that call accessibility API functions. You can, however, use a destroyed element with PFUIElement's @link //apple_ref/occ/instm/PFUIElement/pid -pid@/link, and with PFUIElement's -isEqual: and @link //apple_ref/occ/instm/PFUIElement/isEqualToElement: -isEqualToElement:@/link to manage any element cache you maintain. Note that attempting to pass a destroyed UI element in an accessibility API function is not guaranteed to return an accessibility API error. PFUIElement's @link //apple_ref/occ/instm/PFUIElement/elementInfo -elementInfo@/link method will return some valid information about a destroyed element.
 
 If a callback method initiates a time-consuming operation or an operation that might fail, consider executing the operation in a secondary thread using a method like NSThread's <code>+detachNewThreadSelector:toTarget:withObject:</code> method to avoid blocking the application. On the other hand, if a callback method runs an AppleScript script using a method like NSAppleScript's <code>-executeAndReturnError:</code> (or the preferred method, OSAScript's <code>-executeAndReturnError:</code>, in the OSAKit framework), always execute the operation using NSObject's <code>-performSelectorOnMainThread:withObject:waithUntilDone:</code> method with <code>wait</code> set to <code>NO</code>, because AppleScript scripts must run on the main application thread if they involve user interaction, as with the 'display dialog' command. AppleScript queues scripts on the main application thread in first-in, first-out (FIFO) order.
 
 All observers maintain a list of the notifications and elements for which they are currently registered. See @link registrations -registrations@/link for details.
 
 If you allocate your own observer and use this method to initialize it, you must manage the observer yourself. It is the client's responsibility to keep track of each observer created in this manner, retaining it as long as it is in use; to register and unregister it for notifications and elements as desired; to release it when done with it; and to maintain a list of its current registrations.
 
 See also @link initWithPath:notificationDelegate:callbackSelector: -initWithPath:notificationDelegate:callbackSelector:@/link, @link initWithURL:notificationDelegate:callbackSelector: -initWithURL:notificationDelegate:callbackSelector:@/link and @link initWithBundleIdentifier:notificationDelegate:callbackSelector: -initWithBundleIdentifier:notificationDelegate:callbackSelector:@/link.
*/
- (id)initWithPid:(pid_t)pid notificationDelegate:(id)callbackDelegate callbackSelector:(SEL)callback; // designated initializer

#pragma mark Deallocators

/*!
 @method invalidate
 @abstract Invalidates this PFObserver object when it is no longer needed.
 @discussion Optionally call this method when the client application is finished with this PFObserver object.
 
 It will be desirable to call this method in a garbage collected environment to avoid adding time-consuming activity at collection time (and it does no harm to call it in a reference counted environment). If the client application does not call this method, it will be called automatically when the object is deallocated or finalized. It sets any delegate to <code>nil</code> and takes care of memory management.
  
 NOTE: This version of PFAssistive Framework does not support garbage collected clients.

 See also @link //apple_ref/occ/instm/PFObserver/isValid -isValid@/link.
*/
- (void)invalidate;

/*!
 @method isValid
 @abstract Reports whether this PFObserver object has been invalidated.
 @discussion Returns <code>YES</code> if this PFUIElement object has been invalidated.
 
 See also @link //apple_ref/occ/instm/PFObserver/invalidate -invalidate@/link.
*/
- (BOOL)isValid;

#pragma mark Factory Convenience Methods

#pragma mark Factory Methods for Delegates
// Factory methods to observe accessibility notifications using a delegate method.

// Factory method creates, initializes and returns a newly created PFObserver object using the name of a target application. Returns nil if the target application is not running or the observer could not be created.

// This method is here for backwards compatibility. Use +observerWithPath: instead.
/*!
 @method observerWithName:
 @abstract DEPRECATED. Convenience class method creates and returns an initialized PFObserver object with <code>name</code>, the name of the target application. Provides full observer management.
 @param name An NSString object giving the name of the application file.
 @result An autoreleased PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this convenience method or @link observerWithPid: +observerWithPid: @/link to create an observer that uses a delegate method instead of a callback method to respond to accessibility notifications.
 
 A client application must call @link //apple_ref/occ/instm/PFObserver/setDelegate: -setDelegate: @/link and @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo: @/link in order to receive notifications.
 
 See the designated initializer, @link initWithPid: -initWithPid:@/link for important information about creating new observer objects.
 
 See also @link observerWithPath: +observerWithPath: @/link, @link observerWithURL: +observerWithURL: @/link, @link observerWithBundleIdentifier: +observerWithBundleIdentifier:@/link and @link observerWithPid: +observerWithPid:@/link.
*/
+ (PFObserver *)observerWithName:(NSString *)name; // DEPRECATED

/*!
 @method observerWithPath:
 @abstract Convenience class method creates and returns an initialized PFObserver object with <code>fullPath</code>, the full path to the target application's bundle or file. Provides full observer management.
 @param fullPath An NSString object giving the full path to the application.
 @result An autoreleased PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this convenience method to create an observer that uses a delegate method instead of a callback method to respond to accessibility notifications.
 
 A client application must call @link //apple_ref/occ/instm/PFObserver/setDelegate: -setDelegate: @/link and @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo: @/link in order to receive notifications.
 
 See the designated initializer, @link initWithPid: -initWithPid:@/link for important information about creating new observer objects.
 
 See also @link observerWithURL: +observerWithURL: @/link, @link observerWithBundleIdentifier: +observerWithBundleIdentifier:@/link and @link observerWithPid: +observerWithPid:@/link.
*/
+ (PFObserver *)observerWithPath:(NSString *)fullPath;

/*!
 @method observerWithURL:
 @abstract Convenience class method creates and returns an initialized PFObserver object with <code>url</code>, the file URL for the target application's bundle or file.
 @param url An NSURL object referencing the application.
 @result An autoreleased PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this convenience method to create an observer that uses a delegate method instead of a callback method to respond to accessibility notifications.
 
 A client application must call @link //apple_ref/occ/instm/PFObserver/setDelegate: -setDelegate: @/link and @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo: @/link in order to receive notifications.
 
 See the designated initializer, @link initWithPid: -initWithPid:@/link for important information about creating new observer objects.
 
 See also @link observerWithPath: +observerWithPath: @/link, @link observerWithBundleIdentifier: +observerWithBundleIdentifier:@/link and @link observerWithPid: +observerWithPid:@/link.
*/
+ (PFObserver *)observerWithURL:(NSURL *)url;

/*!
 @method observerWithBundleIdentifier:
 @abstract Convenience class method creates and returns an initialized PFObserver object with <code>bundleIdentifier</code>, the target application's bundle identifier.
 @param bundleIdentifier An NSString object, the application bundle's identifier.
 @result An autoreleased PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this convenience method to create an observer that uses a delegate method instead of a callback method to respond to accessibility notifications.
 
 A client application must call @link //apple_ref/occ/instm/PFObserver/setDelegate: -setDelegate: @/link and @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo: @/link in order to receive notifications.
 
 See the designated initializer, @link initWithPid: -initWithPid:@/link for important information about creating new observer objects.
 
 See also @link observerWithPath: +observerWithPath: @/link, @link observerWithURL: +observerWithURL:@/link and @link observerWithPid: +observerWithPid:@/link.
*/
+ (PFObserver *)observerWithBundleIdentifier:(NSString *)bundleIdentifier;

// Factory method creates, initializes and returns a PFObserver object using a target application's UNIX process identification number (PID). Returns nil if the target application is not running or the observer could not be created.
/*!
 @method observerWithPid:
 @abstract Convenience class method creates and returns an initialized PFObserver object with <code>pid</code>, the target application's BSD Unix process identification number (PID). Provides full observer management.
 @param pid An integer, the running application's BSD Unix process identification number (PID).
 @result An autoreleased PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this convenience method to create an observer that uses a delegate method instead of a callback method to respond to accessibility notifications.
 
 A client application must call @link //apple_ref/occ/instm/PFObserver/setDelegate: -setDelegate: @/link and @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo: @/link in order to receive notifications.
 
 See the designated initializer, @link initWithPid: -initWithPid:@/link for important information about creating new observer objects.
 
 See also @link observerWithPath: +observerWithPath: @/link, @link observerWithURL: +observerWithURL:@/link and @link observerWithBundleIdentifier: +observerWithBundleIdentifier:@/link.
*/
+ (PFObserver *)observerWithPid:(pid_t)pid;

#pragma mark Factory Methods for callbacks
// Factory methods to observe accessibility notifications using a callback method

// Factory method creates, initializes and returns a newly created PFObserver object using the name of a target application. Returns nil if the target application is not running or the observer could not be created.
/*!
 @method observerWithName:notificationDelegate:callbackSelector:
 @abstract DEPRECATED. Convenience class method creates and returns an initialized PFObserver object with <code>name</code>, the name of the target application, temporary notification delegate <code>callbackDelegate</code>, and callback selector <i>callback</i>. Provides full observer management.
 @param name An NSString object giving the name of the application file.
 @param callbackDelegate An object of any type. A client typically passes <code>self</code> in this parameter.
 @param callback The notification delegate's Objective-C selector to be invoked when a notification is received.
 @result An autoreleased PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this method to create an observer for a specific application given its name and to set up automatic management of the observer, for an observer that uses a callback method instead of a delegate method to respond to accessibility notifications.
 
 See the designated initializer, @link initWithPid:notificationDelegate:callbackSelector: -initWithPid:notificationDelegate:calbackSelector:@/link, for full information about use of this method.
 
 See also @link observerWithPath:notificationDelegate:callbackSelector: +observerWithPath:notificationDelegate:callbackSelector: @/link, @link observerWithURL:notificationDelegate:callbackSelector: +observerWithURL:notificationDelegate:callbackSelector: @/link, @link observerWithBundleIdentifier:notificationDelegate:callbackSelector: +observerWithBundleIdentifier:notificationDelegate:callbackSelector:@/link and @link observerWithPid:notificationDelegate:callbackSelector: +observerWithPid:notificationDelegate:callbackSelector:@/link.
*/
+ (PFObserver *)observerWithName:(NSString *)name notificationDelegate:(id)callbackDelegate callbackSelector:(SEL)callback;; // DEPRECATED

/*!
 @method observerWithPath:notificationDelegate:callbackSelector:
 @abstract Convenience class method creates and returns an initialized PFObserver object with <code>fullPath</code>, the full path to the target application's bundle or file, temporary notification delegate <code>callbackDelegate</code>, and callback selector <i>callback</i>. Provides full observer management.
 @param fullPath An NSString object giving the full path to the application.
 @param callbackDelegate An object of any type. A client typically passes <code>self</code> in this parameter.
 @param callback The notification delegate's Objective-C selector to be invoked when a notification is received.
 @result An autoreleased PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this method to create an observer for a specific application given its name and to set up automatic management of the observer, for an observer that uses a callback method instead of a delegate method to respond to accessibility notifications.
 
 See the designated initializer, @link initWithPid:notificationDelegate:callbackSelector: -initWithPid:notificationDelegate:calbackSelector:@/link, for full information about use of this method.
 
 See also @link observerWithURL:notificationDelegate:callbackSelector: +observerWithURL:notificationDelegate:callbackSelector: @/link, @link observerWithBundleIdentifier:notificationDelegate:callbackSelector: +observerinitWithBundleIdentifier:notificationDelegate:callbackSelector:@/link and @link observerWithPid:notificationDelegate:callbackSelector: +observerWithPid:notificationDelegate:callbackSelector:@/link.
*/
+ (PFObserver *)observerWithPath:(NSString *)fullPath notificationDelegate:(id)callbackDelegate callbackSelector:(SEL)callback;

/*!
 @method observerWithURL:notificationDelegate:callbackSelector:
 @abstract Convenience class method creates and returns an initialized PFObserver object with <code>url</code>, the file URL for the target application's bundle or file, temporary notification delegate <code>callbackDelegate</code>, and callback selector <i>callback</i>. Provides full observer management.
 @param url An NSURL object referencing the application.
 @param callbackDelegate An object of any type. A client typically passes <code>self</code> in this parameter.
 @param callback The notification delegate's Objective-C selector to be invoked when a notification is received.
 @result An autoreleased PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this method to create an observer for a specific application given its name and to set up automatic management of the observer, for an observer that uses a callback method instead of a delegate method to respond to accessibility notifications.
 
 See the designated initializer, @link initWithPid:notificationDelegate:callbackSelector: -initWithPid:notificationDelegate:calbackSelector:@/link, for full information about use of this method.
 
 See also @link observerWithPath:notificationDelegate:callbackSelector: +observerWithPath:notificationDelegate:callbackSelector: @/link, @link observerWithBundleIdentifier:notificationDelegate:callbackSelector: +observerWithBundleIdentifier:notificationDelegate:callbackSelector:@/link and @link observerWithPid:notificationDelegate:callbackSelector: +observerWithPid:notificationDelegate:callbackSelector:@/link.
*/
+ (PFObserver *)observerWithURL:(NSURL *)url notificationDelegate:(id)callbackDelegate callbackSelector:(SEL)callback;

/*!
 @method observerWithBundleIdentifier:notificationDelegate:callbackSelector:
 @abstract Convenience class method creates and returns an initialized PFObserver object with <code>bundleIdentifier</code>, the target application's bundle identifier, temporary notification delegate <code>callbackDelegate</code>, and callback selector <i>callback</i>. Provides full observer management.
 @param bundleIdentifier An NSString object, the application bundle's identifier.
 @param callbackDelegate An object of any type. A client typically passes <code>self</code> in this parameter.
 @param callback The notification delegate's Objective-C selector to be invoked when a notification is received.
 @result An autoreleased PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this method to create an observer for a specific application given its name and to set up automatic management of the observer, for an observer that uses a callback method instead of a delegate method to respond to accessibility notifications.
 
 See the designated initializer, @link initWithPid:notificationDelegate:callbackSelector: -initWithPid:notificationDelegate:calbackSelector:@/link, for full information about use of this method.
 
 See also @link observerWithPath:notificationDelegate:callbackSelector: +observerWithPath:notificationDelegate:callbackSelector: @/link, @link observerWithURL:notificationDelegate:callbackSelector: +observerWithURL:notificationDelegate:callbackSelector:@/link and @link observerWithPid:notificationDelegate:callbackSelector: +observerWithPid:notificationDelegate:callbackSelector:@/link.
*/
+ (PFObserver *)observerWithBundleIdentifier:(NSString *)bundleIdentifier notificationDelegate:(id)callbackDelegate callbackSelector:(SEL)callback;

// Factory method creates, initializes and returns a newly created PFObserver object using a target application's UNIX process ID. Returns nil if the target application is not running or the observer could not be created.
/*!
 @method observerWithPid:notificationDelegate:callbackSelector:
 @abstract Convenience class method creates and returns an initialized PFObserver object with <code>pid</code>, the target application's BSD Unix process identification number (PID), temporary notification delegate <code>callbackDelegate</code>, and callback selector <code>callback</code>. Provides full observer management.
 @param pid An integer, the running application's BSD Unix process identification number (PID).
 @param callbackDelegate An object of any type. A client typically passes <code>self</code> in this parameter.
 @param callback The notification delegate's Objective-C selector to be invoked when a notification is received.
 @result An autoreleased PFObserver object, or <code>nil</code> if the target application is not running.
 @discussion Use this method to create an observer for a specific application given its BSD Unix process identification number (PID) and to set up automatic management of the observer, for an observer that uses a callback method instead of a delegate method to respond to accessibility notifications.
 
 See the designated initializer, @link initWithPid:notificationDelegate:callbackSelector: -initWithPid:notificationDelegate:calbackSelector:@/link, for full information about use of this method.
 
 See also @link observerWithPath:notificationDelegate:callbackSelector: +observerWithPath:notificationDelegate:callbackSelector: @/link, @link observerWithURL:notificationDelegate:callbackSelector: +observerWithURL:notificationDelegate:callbackSelector:@/link and @link observerWithPid:notificationDelegate:callbackSelector: +observerWithPid:notificationDelegate:callbackSelector:@/link.
*/
+ (PFObserver *)observerWithPid:(pid_t)pid notificationDelegate:(id)callbackDelegate callbackSelector:(SEL)callback;

#pragma mark DELEGATE ACCESSORS
/*!
 @methodgroup DELEGATE ACCESSORS
*/

/*!
 @method delegate
 @abstract Returns the receiver's delegate.
 @result The receiver's delegate, an object of any type, or <code>nil</code> if it has no delegate.
 @discussion @link applicationWithIdentifier:atPath:didPostAccessibilityNotification:fromObservedUIElement:forAffectedUIElement: -applicationWithIdentifier:atPath:didPostAccessibilityNotification:fromObservedUIElement:forAffectedUIElement: @/link is the only available delegate method, which is optional. It is declared in the @link //apple_ref/occ/intf/PFObserverDelegate PFObserverDelegate @/link protocol.
 
 See also @link //apple_ref/occ/instm/PFObserver/setDelegate: -setDelegate: @/link.
*/
- (id <PFObserverDelegate>)delegate;

/*!
 @method setDelegate:
 @abstract Sets the receiver's delegate to <code>object</code>.
 @param object An object of any type, or <code>nil</code> to remove an existing delegate.
 @discussion The delegate must adopt the PFObserverDelegate formal protocol to receive delegate messages.
 
 The delegate method, which is optional, is declared in the @link //apple_ref/occ/intf/PFObserverDelegate PFObserverDelegate @/link protocol. If it is implemented by the delegate, it is automatically called when the receiving observer detects a change in a UI element which its has registered to observe. The delegate must adopt the PFObserverDelegate formal protocol to receive delegate messages.
 
 The only available delegate message is @link applicationWithIdentifier:atPath:didPostAccessibilityNotification:fromObservedUIElement:forAffectedUIElement: -applicationWithIdentifier:atPath:didPostAccessibilityNotification:fromObservedUIElement:forAffectedUIElement:@/link, which is optional.
 
 The delegate is not retained. To remove a delegate, pass <code>nil</code>.
 
 See also @link //apple_ref/occ/instm/PFObserver/delegate -delegate@/link.
*/
- (void)setDelegate:(id <PFObserverDelegate>)object;

#pragma mark ACCESSORS
/*!
 @methodgroup ACCESSORS
*/

/*!
 @method pidNumber
 @abstract Returns the BSD Unix process identification number (PID) of the receiver's target application.
 @result An NSNumber object encoding an integer value.
 @discussion It is sometimes useful to know the process identification number of an application. For example, it can be used with a variety of Carbon functions to access information not otherwise readily available in Cocoa.
 
 Use NSNumber's <code>-intValue</code> method to extract the integer value of the result.
*/
- (NSNumber *)pidNumber; // -intValue to extract integer

#pragma mark REGISTRATION
/*!
 @functiongroup REGISTRATION
*/

/*!
 @method registrations
 @abstract Returns a mutable array containing all registrations currently in effect in the receiver; that is, all registrations that the client has registered and has not unregistered using this observer.
 @result An NSMutableArray object containing all registrations currently in effect in the receiving observer.
 @discussion Each registration in the array is an NSDctionary object containing the registered notification string with key "registrationNotificationKey" and the registered PFUIElement object with key "registrationElementKey".
*/
- (NSMutableArray *)registrations;

/*!
 @method registerForNotification:fromElement:contextInfo:
 @abstract Registers the receiver to observe <code>element</code> for <code>notification</code>, saving <code>contextInfo</code> for return in the client's callback selector, returning <code>YES</code> if successful.
 @param notification An NSString representing a notification; you may use a string constant from the NSAccessibility informal protocol or from the accessibility API's AXConstants.h header file cast to NSString*, or the string itself.
 @param element The PFUIElement object to observe. If <code>element</code> is <code>nil</code>, the root application UI element is observed.
 @param contextInfo An object, typically of type NSDictionary, containing any information that the receiver's callback method may need at the time the notification is posted. This information is ignored if the client application uses a delegate method instead of a callback method.
 @result <code>YES</code> if the receiver was successfully registered; <code>NO</code>, if not.
 @discussion Use this method to register to observe a specific UI element in a target application in order to receive notifications that it posts. Optionally pass <code>nil</code> as the <code>element</code> argument to register to observe the root application element in order to receive notifications posted by all elements in the application. It is often more convenient to observe the root application element, because this ensures that you will receive all of the target application's notifications when any of its elements changes state.
 
 For clients that use a delegate method, the delegate method receives the bundle identifier and full path of the target application file, the notification, the observed element, and the affected element (which may differ from the observed element). For clients that use a callback method, the callback method receives the observer, the notification, the affected element (which may differ from the observed element), and contextInfo in its parameters.
 
 Any one observer is associated with a single client delegate method or callback method no matter how many elements it is registered to observe or how many accessibility notifications it is registered to receive. A delegate method or callback method distinguishes how to respond to different notifications affecting different elements by testing its parameters. See @link initWithPid:notificationDelegate:callbackSelector: -initWithPid:notificationDelegate:callbackSelector:@/link for the required callback method signature for clients that use a callback method instead of a delegate method.
 
 If the client needs to remember that <code>element</code> is the observed element, it may have to cache <code>element</code> at registration time. This is because the callback method will not necessarily return <code>element</code> as the affected element in every case. This is so when <code>element</code> is the root application element, because when the root application is observed, the affected element may be some other element.
 
 The client's delegate method or callback method will always receive the affected element, even when observing <code>NSAccessibilityUIElementDestroyedNotification</code>. However, you must treat a destroyed element as invalid; do not pass it as a parameter to methods that call accessibility API functions. You can, however, use a destroyed element with PFUIElement's @link //apple_ref/occ/instm/PFUIElement/pid -pid@/link, and with PFUIElement's <code>-isEqual:</code> and @link //apple_ref/occ/instm/PFUIElement/isEqualToElement: -isEqualToElement:@/link to manage any element cache you maintain. Note that attempting to pass a destroyed UI element in an accessibility API function is not guaranteed to return an accessibility API error. PFUIElement's @link //apple_ref/occ/instm/PFUIElement/elementInfo -elementInfo@/link method will return some valid information about a destroyed element.
 
 Many UI elements do not support every notification known to the accessibility API. If you attempt to register an element for a notification that it does not support, this method returns <code>NO</code>. Use the receiving observer's @link isRegisteredForNotification:fromElement: -isRegisteredForNotification:fromElement:@/link at any time to test whether a particular element is registered for a particular notification; it will always return <code>NO</code> for an element that does not support the specified notification.
 
 Under Mac OS X 10.5 Leopard, registering for either the AXApplicationActivated or AXApplicationDeactivated notification also registers for the other. The same is true of the AXApplicationHidden and AXApplicationShown notifications. The framework does this in order to work around a longstanding Accessibility API bug that Apple fixed in Mac OS X 10.6 Snow Leopard.
 
 It is the client's responsibility to unregister an observer for all notifications before releasing it. This can be done by sending @link unregisterForAllNotifications -unregisterForAllNotifications@/link. For convenience, @link removeObserversForPid: -removeObserversForPid:@/link unregisters all observers for all notifications and releases all observers at once.
 
 See also @link unregisterForNotification:fromElement: -unregisterForNotification:fromElement:@/link, @link unregisterForAllNotificationsFromElement: -unregisterForAllNotificationsFromElement:@/link, @link unregisterForAllNotifications -unregisterForAllNotifications@/link and @link isRegisteredForNotification:fromElement: -isRegisteredForNotification:fromElement:@/link.
*/
- (BOOL)registerForNotification:(NSString *)notification fromElement:(PFUIElement *)element contextInfo:(void *)contextInfo;

/*!
 @method unregisterForNotification:fromElement:
 @abstract Unregisters the receiver to observe <code>element</code> for <code>notification</code>.
 @param notification An NSString representing a notification; you may use a string constant from the NSAccessibility informal protocol or from the accessibility API's AXConstants.h header file cast to NSString*, or the string itself.
 @param element The PFUIElement object being observed. If <code>element</code> is <code>nil</code>, the notification is unregistered from the root application UI element.
 @discussion It is the client's responsibility to unregister the receiving observer for all notifications before releasing it. For convenience, @link removeObserversForPid: -removeObserversForPid:@/link unregisters all observers for all notifications and releases all observers at once.
 
 See also @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo:@/link, @link unregisterForAllNotificationsFromElement: -unregisterForAllNotificationsFromElement:@/link, @link unregisterForAllNotifications -unregisterForAllNotifications@/link and @link isRegisteredForNotification:fromElement: -isRegisteredForNotification:fromElement:@/link.
*/
- (void)unregisterForNotification:(NSString *)notification fromElement:(PFUIElement *)element;

/*!
 @method unregisterForAllNotificationsFromElement:
 @abstract Unregisters the receiver to observe <code>element</code> for all notifications.
 @param element The PFUIElement object being observed. If <code>element</code> is <code>nil</code>, the notifications are unregistered from the root application UI element.
 @discussion It is the client's responsibility to unregister the receiving observer for all notifications before releasing it. For convenience, @link removeObserversForPid: -removeObserversForPid:@/link unregisters all observers for all notifications and releases all observers at once.
 
 See also @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo:@/link, @link unregisterForNotification:fromElement: -unregisterForNotification:fromElement:@/link, @link unregisterForAllNotifications -unregisterForAllNotifications@/link and @link isRegisteredForNotification:fromElement: -isRegisteredForNotification:fromElement:@/link.
*/
- (void)unregisterForAllNotificationsFromElement:(PFUIElement *)element;

/*!
 @method unregisterForAllNotifications
 @abstract Unregisters the receiver to observe all elements for all notifications.
 @discussion It is the client's responsibility to unregister the receiving observer for all notifications before releasing it. For convenience, @link removeObserversForPid: -removeObserversForPid:@/link unregisters all observers for all notifications and releases all observers at once.
 
 See also @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo:@/link, @link unregisterForNotification:fromElement: -unregisterForNotification:fromElement:@/link, @link unregisterForAllNotificationsFromElement: -unregisterForAllNotificationsFromElement:@/link and @link isRegisteredForNotification:fromElement: -isRegisteredForNotification:fromElement:@/link.
*/
- (void)unregisterForAllNotifications;

/*!
 @method isRegisteredForNotification:fromElement:
 @abstract Returns <code>YES</code> if <code>element</code> is registered with the receiver for <code>notification</code>.
 @param notification An NSString representing a notification; you may use a string constant from the NSAccessibility informal protocol or from the accessibility API's AXConstants.h header file cast to NSString*, or the string itself.
 @param element A PFUIElement object.
 @discussion Returns <code>YES</code> if the receiver is registered for <code>notification</code> from <code>element</code>.
 
 Use this method at any time to test whether a particular element is registered with the receiver for a particular notification. It always returns <code>NO</code> for an element that does not support the specified notification.
 
 See also @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo:@/link, @link unregisterForNotification:fromElement: -unregisterForNotification:fromElement:@/link, @link unregisterForAllNotificationsFromElement: -unregisterForAllNotificationsFromElement:@/link and @link unregisterForAllNotifications -unregisterForAllNotifications@/link.
*/
- (BOOL)isRegisteredForNotification:(NSString *)notification fromElement:(PFUIElement *)element;

#pragma mark CLASS UTILITY METHODS
/*!
 @methodgroup CLASS UTILITY METHODS
*/

/*!
 @method observersForPath:
 @abstract Class method returns an array of observers for the running application at <code>fullPath</code>.
 @param fullPath An NSString object giving the full path to the application.
 @result  An NSArray object containing PFObserver objects. The array is empty if no observers exist for the application or the application is not running.
 @discussion This method only returns observers that were created using PFObserver factory methods.
*/
+ (NSArray *)observersForPath:(NSString *)fullPath;

/*!
 @method observersForURL:
 @abstract Class method returns an array of observers for the running application at <code>url</code>.
 @param url An NSURL object referencing the application.
 @result  An NSArray object containing PFObserver objects. The array is empty if no observers exist for the application or the application is not running.
 @discussion This method only returns observers that were created using PFObserver factory methods.
*/
+ (NSArray *)observersForURL:(NSURL *)url;

/*!
 @method observersForBundleIdentifier:
 @abstract Class method returns an array of observers for the running application at <code>url</code>.
 @param bundleIdentifier An NSString object, the application bundle's identifier.
 @result  An NSArray object containing PFObserver objects. The array is empty if no observers exist for the application or the application is not running.
 @discussion This method only returns observers that were created using PFObserver factory methods.
*/
+ (NSArray *)observersForBundleIdentifier:(NSString *)bundleIdentifier;

/*!
 @method observersForPid:
 @abstract Class method returns an array of observers for the running application whose BSD Unix application process identification number is <code>pid</code>.
 @param pid An integer, a running application's BSD Unix process identification number (PID).
 @result  An NSArray object containing PFObserver objects. The array is empty if no observers exist for the application or the application is not running.
 @discussion This method only returns observers that were created using PFObserver factory methods.
*/
+ (NSArray *)observersForPid:(pid_t)pid;

/*!
 @method removeObserversForPath:
 @abstract Class method unregisters and removes all observers for the running application at <code>fullPath</code>, deallocating them if they have not been retained by the client.
 @param fullPath An NSString object giving the full path to the application.
 @discussion This method only unregisters and removes observers that were created using PFObserver factory methods.
 
 See @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo:@/link for information regarding registration and unregistration.
*/
+ (void)removeObserversForPath:(NSString *)fullPath;

/*!
 @method removeObserversForURL:
 @abstract Class method unregisters and removes all observers for the running application at <code>url</code>, deallocating them if they have not been retained by the client.
 @param url An NSURL object referencing the application.
 @discussion This method only unregisters and removes observers that were created using PFObserver factory methods.
 
 See @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo:@/link for information regarding registration and unregistration.
*/
+ (void)removeObserversForURL:(NSURL *)url;

/*!
 @method removeObserversForBundleIdentifier:
 @abstract Class method unregisters and removes all observers for the running application at <code>url</code>, deallocating them if they have not been retained by the client.
 @param bundleIdentifier An NSString object, the application bundle's identifier.
 @discussion This method only unregisters and removes observers that were created using PFObserver factory methods.
 
 See @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo:@/link for information regarding registration and unregistration.
*/
+ (void)removeObserversForBundleIdentifier:(NSString *)bundleIdentifier;

/*!
 @method removeObserversForPid:
 @abstract Class method unregisters and removes all observers for the running application whose BSD Unix application process identification number is <code>pid</code>, deallocating them if they have not been retained by the client.
 @param pid An integer, a running application's BSD Unix process identification number (PID).
 @discussion This method only unregisters and removes observers that were created using PFObserver factory methods.
 
 See @link registerForNotification:fromElement:contextInfo: -registerForNotification:fromElement:contextInfo:@/link for information regarding registration and unregistration.
*/
+ (void)removeObserversForPid:(pid_t)pid;

#pragma mark UTILITIES
/*!
 @methodgroup UTILITIES
*/

/*!
 @method isEqualToObserver:
 @abstract Returns whether the receiving observer represents the same observer that <code>observer</code> represents.
 @param observer A PFObserver object to be tested for equality with the receiver.
 @discussion Returns <code>YES</code> if the receiving observer represents the same observer that <code>observer</code> represents.
 
 Use this method for efficiency if you know that observer is a PFObserver object. Otherwise, use <code>-isEqual:</code>.
*/
- (BOOL)isEqualToObserver:(PFObserver *)observer;

#pragma mark DEBUGGING UTILITIES
/*!
 @methodgroup DEBUGGING UTILITIES
*/

/*!
 @method description
 @abstract Returns a string that represents the receiver.
 @result An NSString object.
 @discussion The returned string is formatted as a property list showing all notifications for all UI elements for which the observer is registered. In client code, call <code>NSLog()</code> and pass in the observer to see this information in the Xcode run log and debug console log. 
*/
- (NSString *)description;

@end

#pragma mark -
/*!
 @protocol PFObserverDelegate
 @abstract A formal protocol declaring delegate messages sent by PFObserver.
 @discussion The PFObserverDelegate protocol implements one optional delegate method, @link applicationWithIdentifier:atPath:didPostAccessibilityNotification:fromObservedUIElement:forAffectedUIElement:@/link.
 
 Clients that implement this delegate method must declare that they adopt this formal protocol.
*/
@protocol PFObserverDelegate <NSObject>

// Clients that implement this delegate method must declare that they adopt this formal protocol.

@optional

// If implemented by the client, this delegate method is called when an observer receives a registered accessibility notification from an application.
/*!
 @method applicationWithIdentifier:atPath:didPostAccessibilityNotification:fromObservedUIElement:forAffectedUIElement:
 @abstract Sent when the receiving observer detects an accessibility notification.
 @param identifier The identifier of the observed target application.
 @param fullPath The full path to the observed target application.
 @param notification An NSString representing the notification.
 @param observedUIElement A PFUIElement object representing the observed UI element.
 @param affectedUIElement A PFUIElement object representing the affected UI element.
 @discussion Sent when the accessibility API detects a change in the observed UI element.
 
 To use this delegate method, a client application must implement it, create a PFObserver object using @link initWithPath: -initWithPath:@/link or one of the other initializers or factory methods that do not include parameters for a callback (i.e., temporary notification delegate and callback selector), set one of its classes as the delegate of the PFObserver object, and register to observe the notification. In the delegate method, use the <code>identifier</code> or <code>fullPath</code> parameter to identify the application that posted the notification, and use the <code>notification</code>, <code>observedUIElement</code> and <code>affectedUIElement</code> parameters to obtain information about the notification that was triggered by a change in the target application. It is the client's responsibility to manage the observer's delegate appropriately.

 As an alternative to this delegate method, a client application can implement an Objective-C callback method and register it with the observer using @link initWithPath:notificationDelegate:callbackSelector: -initWithPath:notificationDelegate:callbackSelector:@/link, or one of the other initializers or factory methods that include parameters for a callback (i.e., temporary notification delegate and callback selector). Use a callback method for more complex scenarios, where the client application must use information saved in the <code>contextInfo</code> parameter when the observer was registered. The client application's Objective-C callback selector must have the following signature:
 
- (void)observer:(PFObserver *)observer notification:(NSString *)notification element:(PFUIElement *)element contextInfo:(void *)contextInfo
*/
- (void)applicationWithIdentifier:(NSString *)identifier atPath:(NSString *)fullPath didPostAccessibilityNotification:(NSString *)notification fromObservedUIElement:(PFUIElement *)observedUIElement forAffectedUIElement:(PFUIElement *)affectedUIElement;

// To use this delegate method, a client application must implement it, create a PFObserver object using -initWithPath:, -initWithPid: or one of the factory methods that do not include parameters for a callback (i.e., temporary notification delegate and callback selector), and set one of its classes as the delegate of the PFObserver object. In the delegate method, use the identifier or fullPath parameter to identify the application that posted the notification, and use the notification, observedUIElement and affectedUIElement parameters to obtain information about the notification that was triggered by user action in the target application. It is the client's responsibility to manage the observer's delegate appropriately.
// As an alternative to this delegate method, a client application can implement an Objective-C callback method and register it with the observer using -initWithPath:notificationDelegate:callbackSelector:, -initWithPid:notificationDelegate:callbackSelector:, or one of the factory methods that include parameters for a callback (i.e., temporary notification delegate and callback selector). Use a callback method for more complex scenarios, where the client application must use information saved in the contextInfo parameter when the observer was registered. The client application's Objective-C callback selector must have the following signature:

// - (void)observer:(PFObserver *)observer notification:(NSString *)notification element:(PFUIElement *)element contextInfo:(void *)contextInfo

@end

//#endif