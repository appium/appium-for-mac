/*
 PFUIElement.h
 PFAssistive Framework 3.4.0
 Copyright 2003-2014 Bill Cheeseman. Used by permission.
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
 @header PFUIElement.h
 @copyright 2003-2014 Bill Cheeseman. Used by permission. All rights reserved.
 @version 3.4.0
 @abstract The PFUIElement class supports monitoring and controlling a UI element in a running application.
 @discussion The accessibility API implements the concept of a "UI element," an object based on Core Foundation's CFTypeRef type that represents a user interface element on the screen in a running application. The PFAssistive Framework implements this same concept in its PFUIElement and PFApplicationUIElement classes.
 
 The following preprocessor macros are used in this header:
 <ul><li><b>PFA_DEBUG</b> Defining this macro causes the framework to log information about accessibility API errors to stderr.
 <li><b>PFA_BLOCK_EXPECTED_ERROR_LOGS</b> Defining this macro when PFA_DEBUG is also defined suppresses logging of accessibility API errors in certain situations where normal usage is expected to generate errors, in order to prevent flooding stderr with uninteresting error messages.</ul>
 It is recommended that you set both of these macros in the framework Target's Preprocessor Macros build setting for the Debug configuration only.
*/

#import <Cocoa/Cocoa.h>

@class PFApplicationUIElement;
@class PFObserver;
@protocol PFUIElementDelegate;

#pragma mark -
/*!
 @class PFUIElement
 @abstract A concrete class representing a UI element in a running application.
 @discussion A PFUIElement object represents a UI element on the screen in a running application, such as a menu, window or button. The PFApplicationUIElement subclass represents a root-level UI element&mdash;either an application or the system-wide element representing all running applications.
 
 The PFUIElement class implements all of the functionality of the accessibility API's AXUIElementRef object. A PFApplicationUIElement subclass implements additional AXUIElementRef functionality relating to the root application element and the system-wide element. PFUIElement objects fully support the accessibility API's actions, attributes, keystrokes and notifications. A client application can get the value of a PFUIElement object's attributes, set the value of its settable attributes, and perform any actions it supports. It can send keystrokes, including keyboard shortcuts, to the target application to control its focused UI element. And, by creating a PFObserver object, it can observe any application or UI element and receive notifications when changes occur.
 
 In this documentation, depending on context, "UI element" or "element" may refer either to a programmatic PFUIElement or PFApplicationUIElement object or to the user interface object in the running application that is represented by a PFUIElement or PFApplicationUIElement object. References to a "PFUIElement object" generally include a PFApplicationUIElement object, since the PFApplicationUIElement class is a subclass of PFUIElement.
 
 An application's UI elements are organized in a tree-like containment hierarchy of PFUIElement objects, with the root application element on one end and leaf elements on the screen such as buttons and checkboxes on the other. A target application's hierarchy can be navigated in either direction by traversing any element's parent and children. An application's individual elements can be read by getting their attributes, and in many cases they can be controlled by setting their attributes and performing actions on them and by sending keystrokes to the application. Methods are provided to obtain the application's root element given its path or its process identification number (PID) as an entry point into the tree, or, alternatively, to read the screen to obtain the root UI element of whatever application currently has a user interface object at a specified point on the screen.
 
 UI elements have attributes and, in many cases, actions identified by a string, usually beginning with the prefix "AX" as in "AXRole" or "AXPress". In a client application, you normally use the NSString constants declared in the NSAccessibility informal protocol (in NSAccessibility.h, in the Cocoa AppKit framework). Alternatively, you can use the corresponding CFStringRef constants declared in the accessibility API headers and cast them to NSString*, or use the string itself. Some constants are omitted from NSAccessibility.h or from the accessibility API headers; if you can't find a desired constant in one of these headers, use a constant from the other header or the string itself. Note that some constants in these headers may be marked as obsolete; use only the non-obsolete constants or their string equivalents to maximize compatibility with future releases of Mac OS X.
 
 Some applications, such as the Finder and Safari, implement custom UI elements with custom actions and attributes that are not part of Apple's accessibility API. This framework is written so that you can read and manipulate them as well as Apple's built-in accessibility API actions and attributes.  You may be able to identify the strings used to identify custom actions and attributes by reading an application's documentation or by examining the .strings files in its application bundle. You will find it much easier, however, to use PreForm UI Browser, available from PreForm Assistive Technologies, LLC at <a href="http://pfiddlesoft.com/uibrowser/">pfiddlesoft.com/uibrowser</a>.
 
 Many methods return PFUIElement or PFApplicationUIElement objects. Think of these methods as factories that can be called upon to churn out these objects as often as needed, whenever needed, even if you use them only for a moment. Separate PFUIElement or PFApplicationUIElement objects representing the same UI element in the running application are interchangeable. Whether they represent the same element can be tested using <code>-isEqual:</code> or @link isEqualToElement: -isEqualToElement:@/link. Whether the element still exists in the running application can be tested using @link exists -exists@/link or @link isDestroyed -isDestroyed @/link. It is prudent to test whether an element still exists before using it, because objects representing destroyed elements may be <code>nil</code> or may have been recycled to refer to other elements, leading to unexpected behavior.
 
 Clients should not subclass PFUIElement, because they would lose access to PFApplicationUIElement. Instead, they should use categories or similar techniques to add functionality to PFUIElement.

 The following new subroles were introduced in OS X v10.9 Mavericks: <code>kAXDescriptionListSubrole</code> (preferred over the old <code>kAXDefinitionListSubrole</code> to represent HTML5 objects indicated by the DL tag), <code>kAXSwitchSubrole</code> and <code>kAXToggleSubrole</code>. New actions, attributes and notifications that were introduced in Mavericks in OS X v10.10 Yosemite are noted in their individual topics.
 
 Before a client application can make use of this class in OS X v10.9 Mavericks or newer, access must be granted to it using the new per-application trust mechanism in the Accessibility list in the Security & Privacy preference's Privacy pane in System Preferences. Global access and the old per-application trust mechanism no longer work in Mavericks. In OS X v10.8 Mountain Lion and older, the global "Enable access for assistive devices" setting in the Accessibility (or Universal Access) pane of System Preferences must be turned on, or the client application must be made a trusted process through use of the accessibility API's <code>AXMakeProcessTrusted()</code> function. Authentication may be required. See @link isProcessTrustedWithOptions: +isProcessTrustedWithOptions:@/link for details.
*/
@interface PFUIElement : NSObject <NSCopying> {
	@protected
	AXUIElementRef elementRef;
	BOOL isValid;
	
	@private
	NSDictionary *elementInfo;
	AXObserverRef destructionObserverRef;
	BOOL isDestroyed;
	id delegate;
	NSUInteger weakRetainCount;
    
//	void *privateVars; // removed in 3.1.3 in anticipation of an all-64 bit world.
}

#pragma mark INITIALIZATION
/*!
 @methodgroup INITIALIZATION
*/

#pragma mark Initializers
// The designated initializer for a PFUIElement object is -initWithElementRef:delegate:.

/*!
 @method initWithElementRef:delegate:
 @abstract Initializes and returns a newly allocated PFUIElement object.
 @param uiElementRef An opaque accessibility API object representing a UI element in the target application.
 @param aDelegate An object that may implement the optional @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link or @link PFUIElementReportError: -PFUIElementReportError: @/link delegate method. May be <code>nil</code>.
 @result An initialized PFUIElement or PFApplicationUIElement object, or <code>nil</code> if the target application is not running or the UI element does not exist in the target application's user interface.
 @discussion This is the designated initializer for the PFUIElement class. Use it to initialize a newly-allocated PFUIElement object with <code>uiElementRef</code>, an opaque accessibility API AXUIElementRef object.
 
 The <code>uiElementRef</code> argument may be obtained by sending @link elementRef -elementRef @/link to any valid PFUIElement or PFApplicationUIElement object, or by calling any of several accessibility API functions. You may pass <code>nil</code>, but then you are responsible for calling @link setElementRef: -setElementRef: @/link with a <i>uiElementRef</i> object and @link setElementInfo: -setElementInfo: @/link before you use the new PFUIElement object.
 
 The object passed in the <code>aDelegate</code> argument, if it implements the optional @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method, is automatically registered to receive <code>PFUIElementWasDestroyedNotification</code> when the UI element represented by the new PFUIElement object is destroyed (for example, when a window closes). Pass <code>nil</code> if you don't need to know when the element is destroyed. 
  
 Although this is the designated initializer for the PFUIElement class, a client application normally creates and initializes a new PFUIElement object in one of the four ways listed here, instead of allocating a new PFUIElement object and sending it the <code>-initWithElementRef:delegate:</code> message:
 
 1. with PFUIElement's @link elementAtPoint:withDelegate:error: +elementAtPoint:withDelegate:error: @/link class method or PFApplicationUIElement's @link elementAtPoint: -elementAtPoint: @/link instance method, when you want to browse an application's UI elements from a leaf element by reading the screen;
 
 2. with PFApplicationUIElement's @link initWithPath:delegate: -initWithPath:delegate: @/link, @link initWithURL:delegate: -initWithURL:delegate: @/link, @link initWithBundleIdentifier:delegate: -initWithBundleIdentifier:delegate: @/link or @link initWithPid:delegate: -initWithPid:delegate: @/link method, or their corresponding convenience factory methods, when you want to browse an application's UI elements from its root application element without reading the screen;
 
 3. with PFApplicationUIElement's @link initSystemWideWithDelegate: -initSystemWideWithDelegate: @/link method, or its corresponding convenience factory method, when you want to use the system-wide UI element to access the active application's elements; or
 
 4. with any of several methods, such as @link applicationElement -applicationElement@/link, @link //apple_ref/occ/instm/PFUIElement(Attributes)/AXParent -AXParent @/link and @link //apple_ref/occ/instm/PFUIElement(Attributes)/AXChildren -AXChildren@/link, that return a PFUIElement object or an array of PFUIElement objects related to a given element, when you already have a PFUIElement object and can use it as the receiver.
 
 This method is nevertheless handy when you need to create a temporary UI element object; for example, an object representing a known UI element in order to test it for equality with a cached element using @link isEqualToElement: -isEqualToElement:@/link. You can allocate as many UI elements as you like in this way, as often as you like, and initialize each of them with <code>-initWithElementRef:delegate:</code>. Typically, in this situation, you get the <code>uiElementRef</code> argument from an existing or newly-created element by sending it @link elementRef -elementRef@/link, but you can also get it directly by calling any of several accessibility API functions.
 
 This method may return a PFApplicationUIElement object, substituting it for the PFUIElement object that was initially allocated. You must therefore always follow the convention of allocating and initializing a UI element object in a single statement in this form: <code>[[PFUIElement alloc] initWithElementRef:myElementRef delegate:MyObject]</code>.
 
 All PFUIElement objects except a PFApplicationUIElement object use internal mechanisms to learn when they are destroyed in the target application's user interface (for example, when a window closes), even if the client application does not register to observe <code>PFUIElementWasDestroyedNotification</code> or implement the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method. Use @link exists -exists @/link to determine whether a PFUIElement object currently exists in the user interface. When an element is initialized, it caches information about itself that can still be accessed after the element is destroyed. See the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method and @link elementInfo -elementInfo @/link for details.
 
 <b>IMPORTANT:</b> if you use <code>-initWithElementRef:delegate:</code> to create a new PFUIElement object to represent a UI element that has already been destroyed (for example, in response to a notification that it has been destroyed), the cached information in the newly-created object may be invalid. You should therefore substitute known good elementInfo, which you can obtain from the elementInfo that was cached in the destroyed PFUIElement object when it was initially created.
*/
- (id)initWithElementRef:(AXUIElementRef)uiElementRef delegate:(id)aDelegate; // designated initializer

#pragma mark Deallocators

/*!
 @method invalidate
 @abstract Invalidates this PFUIElement object when it is no longer needed.
 @discussion Optionally call this method when the client application is finished with this PFUIElement object.
 
 It is desirable to call this method to avoid adding time-consuming activity at dealloc time. If the client application does not call this method, it is called automatically when the object is deallocated. It sets any delegate to <code>nil</code>, unregisters an internal destruction observer and takes care of memory management.
 
 Invalidation is not related to @link exists -exists@/link or @link isDestroyed -isDestroyed @/link, which report the existence or nonexistence of the underlying UI element in the running application. Invalidation applies only to a specific instance of PFUIElement. PFUIElement objects representing destroyed and nonexistent elements can still be used for equality tests and the like, but invalidated PFUIElement objects cannot.
 
 NOTE: PFAssistive Framework does not support garbage collected clients.
 
 See also @link //apple_ref/occ/instm/PFUIElement/isValid -isValid@/link.
*/
- (void)invalidate;

/*!
 @method isValid
 @abstract Reports whether this PFUIElement object has been initialized and has not been invalidated.
 @discussion Returns <code>YES</code> if this PFUIElement object has been initialized and has not been invalidated.
 
 See also @link //apple_ref/occ/instm/PFUIElement/invalidate -invalidate@/link.
*/
- (BOOL)isValid;

#pragma mark Factory Convenience Methods

/*!
 @method UIElementWithElementRef:delegate:
 @abstract Convenience class method creates and returns an initialized PFUIElement object.
 @param uiElementRef An opaque accessibility API object representing a UI element in the target application.
 @param aDelegate An object that may implement the optional @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link or @link PFUIElementReportError: -PFUIElementReportError: @/link delegate method. May be <code>nil</code>.
 @result An initialized PFUIElement object, or <code>nil</code> if the target application is not running or the UI element does not exist in the target application's user interface.
 @discussion Use this convenience method to create and return an initialized PFUIElement object with <code>uiElementRef</code>, an opaque accessibility API AXUIElementRef object.
 
 See the designated initializer, @link initWithElementRef:delegate: -initWithElementRef:delegate:@/link for important information about creating new UI element objects.
*/
+ (PFUIElement *)UIElementWithElementRef:(AXUIElementRef)uiElementRef delegate:(id)aDelegate;

#pragma mark DELEGATE ACCESSORS
/*!
 @methodgroup DELEGATE ACCESSORS
*/

/*!
 @method delegate
 @abstract Returns the receiver's delegate.
 @result The receiver's delegate, an object of any type, or <code>nil</code> if it has no delegate.
 @discussion @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link and @link PFUIElementReportError: -PFUIElementReportError: @/link are the only available delegate methods, which are optional. They are declared in the @link //apple_ref/occ/intf/PFUIElementDelegate PFUIElementDelegate @/link protocol.
 
 See also @link //apple_ref/occ/instm/PFUIElement/setDelegate: -setDelegate: @/link.
*/
- (id <PFUIElementDelegate>)delegate;

/*!
 @method setDelegate:
 @abstract Sets the receiver's delegate to <code>object</code>.
 @param object An object of any type, or <code>nil</code> to remove an existing delegate.
 @discussion The delegate must adopt the PFUIElementDelegate formal protocol to receive delegate messages.
 
 The delegate methods, both of which are optional, are declared in the @link //apple_ref/occ/intf/PFUIElementDelegate PFUIElementDelegate @/link protocol. If it is implemented by the delegate, a delegate method is automatically called when the UI element represented by the receiver is destroyed in the user interface or an error is reported. The delegate must adopt the PFUIElementDelegate formal protocol to receive delegate messages.

 The only available delegate messages are @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link and @link PFUIElementReportError: -PFUIElementReportError:@/link, which are optional. If the delegate implements the <code>PFUIElementWasDestroyed:</code> delegate method, it is automatically registered to receive <code>PFUIElementWasDestroyedNotification</code>. Other objects can register to observe that notification, as well.
  
 See the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method for important information about detecting when a UI element is destroyed in the running application's user interface.
 
 See also @link //apple_ref/occ/instm/PFUIElement/delegate -delegate@/link.
*/
- (void)setDelegate:(id <PFUIElementDelegate>)object;

#pragma mark CLASS UTILITIES
/*!
 @methodgroup  CLASS UTILITIES
*/

/*!
 @method isAccessImplemented
 @abstract Class method reports whether the accessibility API is implemented on this computer.
 @discussion Returns <code>YES</code> if the accessibility API is implemented on this computer. This method should always return <code>YES</code> because the accessibility API was introduced in Mac OS X 10.2 Jaguar, so there is no longer any reason to call it.
 
 See also @link isAccessEnabled +isAccessEnabled@/link, @link isProcessTrusted +isProcessTrusted @/link and @link isProcessTrustedWithOptions:  +isProcessTrustedWithOptions:@/link.
*/
+ (BOOL)isAccessImplemented;

/*!
 @method isAccessEnabled
 @abstract Class method reports whether global access for assistive devices is enabled. DEPRECATED in OS X 10.9 Mavericks.
 @discussion This method is deprecated in OS X 10.9 Mavericks and newer because Apple has deprecated the function on which it relies, <code>AXAPIEnabled()</code> in AXUIElement.h, because global accessibility is no longer available. See @link isProcessTrustedWithOptions: +isProcessTrustedWithOptions: @/link for information on the new trust features that take its place in Mavericks.
 
 In OS X 10.8 Mountain Lion and older, returns <code>YES</code> if global access for assistive devices is enabled in the Accessibility (or Universal Access) pane of System Preferences. (Testing indicates that, though deprecated in Mavericks, this method returns the same value in Mavericks as @link isProcessTrusted +isProcessTrusted @/link and @link isProcessTrustedWithOptions: +isProcessTrustedWithOptions: @/link.)
 
 In OS X 10.8 Mountain Lion and older, global access for assistive devices is enabled manually in the Accessibility (or Universal Access) pane of System Preferences. It can also be enabled programmatically using AppleScript by setting the <code>UI elements enabled</code> property in System Events or the <code>GUI Scripting enabled</code> property in AppleScript Utility. System Events and AppleScript Utility are scriptable faceless background applications. Enabling this setting requires user authentication whether done manually or by AppleScript. Apple removed the global access setting from System Preferences in OS X 10.9 Mavericks and replaced it with application-specific settings in the Accessibility list in the Security & Privacy preference's Privacy pane. The <code>UI elements enabled</code> and <code>GUI Scripting enabled</code> properties are now read-only.
 
 Global access for assistive devices is turned off by default in OS X 10.8 Mountain Lion and older, and client applications therefore cannot use the methods of the PFAssistive framework unless they arrange to turn it on. Assistive applications commonly instruct the user to turn access on manually in System Preferences. One technique is to send the <code>isAccessEnabled</code> message in a client application's <code>-awakeFromNib</code> or similar method and, if the result is <code>NO</code>, present an alert explaining how the user can turn the setting on. Applications optionally open System Preferences to the appropriate pane for the user. A more user-friendly technique is to enable access for assistive devices for the user with an embedded AppleScript command using Scripting Bridge. For this to work, the developer of an assistive application has to use the sdef and sdp tools to generate a System Events.h header file, which is added to the project before compiling. The SBApplication.framework has to be linked. Here is an action method that toggles the state of the setting in OS X 10.8 Mountain Lion or older:
 
<pre>
 @textblock - (IBAction)enableAccessAllAction:(id)sender {
     SystemEventsApplication *systemEventsApp
    	  = [SBApplication applicationWithBundleIdentifier:@"com.apple.systemevents"];
     if ([systemEventsApp isRunning]) {
         [systemEventsApp activate]; // so authentication dialog opens frontmost
         [systemEventsApp setUIElementsEnabled:[PFUIElement isAccessEnabled] ? NO : YES];
     }
     [sender setState:[PFUIElement isAccessEnabled] ? NSOnState : NSOffState];
 } @/textblock
 </pre>
 
 The most sophisticated technique is to ask the user to make the assistive application alone "trusted." This is complicated for the developer in OS X 10.8 Mountain Lion and older because it requires an embedded helper application with root privileges and an embedded helper application to relaunch the application. It is much easier in OS X 10.9 Mavericks and newer, where it requires no helper applications. See @link isProcessTrustedWithOptions: +isProcessTrustedWithOptions: @/link for information about making a apecific client application trusted in Mavericks.
 
 Now that global access for assistive devices has been eliminated in OS X 10.9 Mavericks, the meaning of the System Events <code>UI element enabled</code> property and the AppleScript Utilities <code>GUI Scripting enabled</code> property have changed. They used to mean that global access was enabled for all applications, including the System Events application which supports GUI Scripting. These AppleScript properties and this <code>isAccessEnabled</code> method always returned the same result. Now, in OS X 10.9 Mavericks and newer, <code>isAccessEnabled</code> is deprecated because global access no longer exists, but the <code>UI elements enabled</code> property of System Events and the <code>GUI Scripting enabled</code> property of AppleScript Utility return <code>true</code> for an assistive application that is trusted in the Accessibility list in the Security & Privacy preference's Privacy pane.
 
 See also @link isAccessImplemented +isAccessImplemented@/link, @link isProcessTrusted +isProcessTrusted @/link and @link isProcessTrustedWithOptions: +isProcessTrustedWithOptions:@/link.
*/
+ (BOOL)isAccessEnabled;

/*!
 @method isProcessTrusted
 @abstract Class method reports whether the client application's process is trusted.
 @discussion A "trusted" process is an application that has been granted access to the computer to monitor or control other applications using the accessibility features of OS X. It is known as an "asssistive" application, especially if it is designed to facilitate the use of the computer by persons with disabilities. An application is untrusted by default, and a client application cannot use the accessibility methods of the PFAssistive framework until the user grants access to make it a trusted process. Access is typically granted to an assistive application when it is first run or when its accessibility features are first needed. User authentication is required. See @link isProcessTrustedWithOptions: +isProcessTrustedWithOptions: @/link for details about making a process trusted and testing its status.
 
 This method returns <code>YES</code> if the client application's process is trusted in Mac OS X 10.4 Tiger and newer. In OS X 10.9 Mavericks and newer, it can be used interchangeably with @link isProcessTrusted +isProcessTrusted @/link or @link isProcessTrustedWithOptions: +isProcessTrustedWithOptions: @/link with a parameter value of <code>nil</code> or an NSDictionary object with a value of <code>YES</code> for the <code>AXTrustedCheckOptionPrompt</code> key to test the application's accessibility status without presenting the built-in system alert giving the user an option to grant or deny access.
 
 See also @link isAccessImplemented +isAccessImplemented@/link, @link isAccessEnabled +isAccessEnabled @/link and  @link isProcessTrustedWithOptions: +isProcessTrustedWithOptions:@/link.
*/
+ (BOOL)isProcessTrusted;

/*!
 @method isProcessTrustedWithOptions:
 @abstract Class method reports whether the client application's process is trusted, optionally presenting an alert allowing the user to grant or deny access if it is not already trusted. Introduced in OS X 10.9 Mavericks.
 @param options An NSDictionary object. Set the value associated with the <code>kAXTrustedCheckOptionPrompt</code> key to <code>YES</code> to present an alert offering to grant or deny access if access is not already allowed.
 @discussion A "trusted" process is an application that has been granted access to the computer to monitor or control other applications using the accessibility features of OS X. It is known as an "asssistive" application, especially if it is designed to facilitate the use of the computer by persons with disabilities. An application is untrusted by default, and a client application cannot use the accessibility methods of the PFAssistive framework until the user grants access to make it a trusted process. Access is typically granted to an assistive application when it is first run or when its accessibility features are first needed. User authentication is required.
 
 Until OS X 10.9 Mavericks, a user could easily grant access to all assistive applications at once on a global basis instead of relying on a per-application trust mechanism. Global access was granted manually by selecting the "Enable access for assistive devices" setting in the Accessibility (or, formerly, Universal Access) pane of System Preferences. Global access could also be granted programmatically using AppleScript by setting to <code>true</code> the <code>UI elements enabled</code> property of System Events or the <code>GUI Scripting enabled</code> property of AppleScript Utility, both of which were read-write properties.
 
 Per-application access was introduced by Apple in Mac OS X 10.4 Tiger to move away from the global on/off model, which was judged to be too broad and insecure. Initially, assistive applications could be made trusted on a per-application basis only programmatically by calling the <code>AXMakeProcessTrusted()</code> function. However, per-application trust was difficult to implement, and very few applications did so because global access was readily available. (One application that did implement per-application trust was PFiddlesoft's own UI Browser.) As a result, Apple removed the global access mechanism in OS X 10.9 Mavericks, and the previous programmatic per-application trust mechanism was replaced at the same time by a new, simpler trust mechanism. An existing assistive application that relies on global access or on the old trust mechanism must now be granted access using the new Mavericks trust mechanism, and developers may need to revise their user interfaces. The "Enable access for assistive devices" setting no longer exists in System Preferences, and the <code>AXIsAccessEnabled()</code> function is deprecated. The <code>AXMakeProcessTrusted()</code> function is also deprecated. In addition, Apple made the <code>UI elements enabled</code> property of the System Events application's AppleScript terminology dictionary and the <code>GUI Scripting enabled</code> property of AppleScript Utility's dictionary read-only. As a result, assistive applications and GUI Scripting scripts may no longer set these properties to <code>true</code> to turn on global access.
 
 In Mavericks, making an assistive application trusted requires a user to add the application to the new Accessibility list in the Security & Privacy preference's Privacy pane in System Preferences and select the application's checkbox. The application can be added to the list manually by dropping its application icon into the list after authenticating to unlock the preference pane, and the application's checkbox is selected to make it trusted. The application can be added to the list programmatically by calling this <code)isProcessTrustedWithOptions:</code> method, which covers the new <code>AXIsProcessTrustedWithOptions()</code> function introduced in OS X 10.9 Mavericks. Passing in a properly configured options dictionary presents an alert giving the user the option to open System Preferences or to deny access, and if the user clicks the Open System Preferences button the application is added to the list automatically. For compiled GUI Scripting scripts, the application running the script must now be added to the Accessibility list; use the AppleScript <code>current application</code> command to learn its identity). For GUI Scripting applets, the applet itself must now be added to the list.
 
 Either of the following statements can be used to create the options dictionary to be passed in as the parameter to this method. Passing in the first dictionary adds the application to the Accessibility list and presents the built-in system alert. Passing in <code>nil</code> or the second dictionary adds the application to the Accessibility list without presenting an alert.
 
<pre>
NSDictionary *withAlertOption = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:(NSString *)kAXTrustedCheckOptionPrompt];
 
NSDictionary *withoutAlertOption = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:NO] forKey:(NSString *)kAXTrustedCheckOptionPrompt];
</pre>

 This method enables an application to determine whether it is trusted using the new Security & Privacy preference mechanism. It returns <code>YES</code> if the client application's process is trusted. It returns <code>NO</code> if the application is not trusted, and it optionally presents an alert with buttons to deny access or to open the Accessibility list in the Security & Privacy preference's Privacy pane in System Preferences. If the user clicks the Open System Preferences button, the system opens the Security & Privacy preference's Privacy pane and selects the Accessibility list, and it automatically adds the application to the list. The application's checkbox must be selected to grant access to the application. Thereafter, access can be granted or denied at any time by selecting or deselecting its checkbox. Unlike the old trust mechanism, the change takes effect without requiring the application to be relaunched. Authentication is required to unlock the Security & Privacy preference pane before any changes can be made. The method is asynchronous; it returns its result based on the application's current accessibility status, and then it presents the optional alert allowing the user to change its status. The alert is presented if the options dictionary is passed to this method with a value of <code>YES</code> for the <code>kAXTrustedCheckOptionPrompt</code> key and the application is not already trusted using the new mechanism.
 
An assistive application can register to observe the "com.apple.accessibility.api" distributed notification posted by System Preferences to learn when the user grants or denies access. This notification is especially helpful when using the optional alert presented by this method, because the alert provides no way to learn which button the user clicked or what happened after the user clicked the Open System Preferences button. A suggested pattern for responding to the notification is to do it in two stages. First capture the application's old or "before" accessibility status in -applicationDidFinishLaunching:, and then in the next iteration of the run loop after the notification is received compare it to the application's new accessibility status. If the status has changed, then it is the assistive application's accessibility status that the user altered. This technique is necessary because the "com.apple.accessibility.api" notification is posted for every change in the Accessibility list, and its <code>object</code> and <code>userInfo</code> accessors do not report the identity of the affected application or its "before" status. The changed status apparently does not take effect until the next iteration of the run loop after the notification is received. Note that the "before" accessibility status cannot be reliably obtained when the notification is received because distributed notifications are asynchronous and can be delayed. Call this method or @link isProcessTrusted +isProcessTrusted @/link in the first stage to get the "before" status, and then call the other method using <code>performSelector:withObject:afterDelay:</code> passing <code>0</code> in the parameter labeled <code>withDelay:</code>.
 
 If this method is called under OS X 10.8 Mountain Lion or older, it ignores the options parameter and returns YES if the process is trusted or if global access is enabled, because before Mavericks either setting allows the client application to monitor or control other applications using accessibility features. If called in Mountain Lion or older, it should not be used to distinguish between global access and per-application trust; instead, call @link isAccessEnabled +isAccessEnabled @/link or @link isProcessTrusted +isProcessTrusted @/link to test those settings separately.
 
 See also @link isAccessImplemented +isAccessImplemented @/link, @link isAccessEnabled +isAccessEnabled @/link and @link isProcessTrusted +isProcessTrusted@/link.
*/
+ (BOOL)isProcessTrustedWithOptions:(NSDictionary *)options;

/*!
 @method activateApplication
 @abstract Brings the target application to the front, returning <code>YES</code> if successful.
 @discussion The target application must be frontmost before some PFAssistive framework methods can be used. For example, it is necessary to make a target application's menu visible before its menu items can be sent AXPress actions, and menus are not visible when the application is not frontmost. The accessibility API is designed to emulate the experience of a typical user, and UI elements are generally not usable when they are not visible on the screen.
 
 If you call this method, it is your responsibility to return the client application to the front, if appropriate. You can call NSApplication's <code>-activateIgnoringOtherApps:</code> to do this.
 
 If the target application is background-only, this method returns <code>NO</code> and the current frontmost application is not changed.
*/
- (BOOL)activateApplication;

#pragma mark SYSTEM VERSION UTILITIES

+ (unsigned long)QSWStandardizedOperatingSystemVersion; // private

#pragma mark UI ELEMENTS
/*!
 @methodgroup UI ELEMENTS
*/

/*!
 @method isDestroyed
 @abstract Returns whether the element has been destroyed.
 @discussion Returns <code>YES</code> if this PFUIElement object has been destroyed (for example, a window has closed).
 
 See also @link exists -exists @/link, which is the antonym of <code>-isDestroyed</code>, and @link setExists: -setExists: @/link, which can be used to mark children of a destroyed element as destroyed by passing NO.
*/
- (BOOL)isDestroyed;

/*!
 @method elementRef
 @abstract Returns the accessibility API AXUIElementRef opaque object that was associated with the receiver when the receiver was created.
 @result An opaque accessibility API object representing a UI element in the target application. The object is returned CFRetained and will be CFReleased when the PFUIElement object with which it is associated is deallocated. Do not CFRelease it yourself unless you CFRetain it yourself.
 @discussion The AXUIElementRef object returned by this method can be used with accessibility API functions, passed to the @link initWithElementRef:delegate: -initWithElementRef:delegate: @/link method to create another PFUIElement object representing the same UI element, or passed to the @link setElementRef: -setElementRef: @/link method for a PFUIElement object that was initialized with a <code>nil</code> <code>uiElementRef</code> parameter.
 
 The returned AXUIElementRef object may represent a UI element that no longer exists in the running application. In that case, you should not call any accessibility API function on it except to get its process identification number (PID) or to test it for equality with another AXUIElementRef ofject. It can always be used to test cached AXUIElementRef objects using <code>CFEqual()</code>; for example, to decide whether to remove them from the cache. For this reason, <code>-isEqual:</code> and @link isEqualToElement: -isEqualToElement:@/link can also be used with destroyed UI elements.
 
 You should CFRetain the returned object if you want to ensure that it persists while you are using it even after the UI element represented by the receiver is destroyed. If you do CFRetain it, CFRelease it when you are done with it. An AXUIElementRef object returned by this method is already CFRetained once, but it will be CFReleased by the framework when the PFUIElement object with which it is associated is deallocated.
 
 See also @link setElementRef: -setElementRef: @/link.
*/
- (AXUIElementRef)elementRef; // return value is CFRetained

/*!
 @method setElementRef:
 @abstract Sets the receiver's accessibility API AXUIElementRef opaque object to <code>ref</code>.
 @param ref An opaque accessibility API object representing a UI element in the target application.
 @discussion Use this method to finish initializing the receiver if <code>nil</code> was passed to the @link initWithElementRef:delegate: -initWithElementRef:delegate: @/link method when the receiver was created. You should call @link setElementInfo: -setElementInfo: @/link at the same time.
 
 See @link initWithElementRef:delegate: -initWithElementRef:delegate: @/link for information about how to obtain AXUIElementRef objects to use with this method.
 
 See also @link elementRef -elementRef @/link.
*/
- (void)setElementRef:(AXUIElementRef)ref;

/*!
 @method elementInfo
 @abstract Returns information about the receiver that was cached when the receiver was created, and that may have been modified later.
 @result An NSDictionary object using NSString constants from the NSAccessibility informal protocol as keys.
 @discussion When the UI element represented by the receiver is destroyed, the receiver can no longer be queried by the accessibility API for valid data about it (except for its process identification number, or PID). Any information about it that must survive its destruction therefore has to be cached when it is created. See @link elementRef -elementRef @/link for information regarding destroyed UI elements.
 
 The role and, if available, the subrole, role description and title of the receiver are automatically cached when a new PFUIElement or PFAppplicationUIElement object is created. Note that the title may have changed by the time the UI element is destroyed. To make the cached title useful, a client application can update the cache at any time by sending @link setElementInfo: -setElementInfo: @/link with an appropriate dictionary entry. To retrieve these values from the dictionary returned by this method, or to set them, use as keys the NSAccessibility informal protocol string constants <code>NSAccessibilityRoleAttribute</code>, <code>NSAccessibilityRoleDescriptionAttribute</code>, <code>NSAccessibilitySubroleAttribute</code>, and <code>NSAccessibilityTitleAttribute</code>, or their counterparts in AXAttributeConstants.h in the accessibility API cast to NSString*, or the strings themselves.
 
 If the client has registered to observe the root application element or the receiver for accessibility notifications, the client's callback method will always receive the affected element, even when observing <code>NSAccessibilityUIElementDestroyedNotification</code>. However, you must treat a destroyed element as invalid; do not pass it as a parameter to methods that call accessibility API functions. You can nevertheless use a destroyed element with @link pid -pid@/link, and with <code>-isEqual:</code and @link isEqualToElement: -isEqualToElement: @/link to manage any element cache you maintain. Note that attempting to pass a destroyed UI element in an accessibility API function is not guaranteed to return an accessibility API error.
 
 This method will return valid information about a destroyed element that was cached when the receiver was created, as described above, with one exception: If the observed element differs from the affected element (typically, when observing the root application element), this method will return <code>nil</code> if the element was destroyed. It is the client's responsibility to cache information about the receiver if it will be needed in this situation.
 
 If you added key-value pairs of your own to the dictionary, you can retrieve the information from the dictionary returned by this method using the keys you added.
 
 See also @link setElementInfo: -setElementInfo:@/link.
*/
- (NSDictionary *)elementInfo;

/*!
 @method setElementInfo:
 @abstract Caches information about the receiver.
 @param info An NSDictionary object using NSString constants from the NSAccessibility informal protocol as keys.
 @discussion See @link elementInfo -elementInfo @/link for details regarding the information that is cached. This message is sent automatically when a new PFUIElement or PFApplicationUIElement object is created. You can send it again if, for example, you want to update the receiver's cached title when the UI element's title changes.
 
 You can add key-value pairs of your own to the dictionary before sending this method, if you want to cache additional information about the receiver. In that case, you can retrieve the information from the dictionary returned by @link elementInfo -elementInfo @/link using the keys you added.
 
 See also @link elementInfo -elementInfo@/link.
*/
- (void)setElementInfo:(NSDictionary *)info;

#pragma mark UI ELEMENT UTILITIES
/*!
 @methodgroup UI ELEMENT UTILITIES
*/

/*!
 @method elementAtPoint:withDelegate:error:
 @abstract Class method creates and returns a PFUIElement object representing the visible UI element located at <code>point</code> on the screen, no matter what application owns it.
 @param point An NSPoint structure using top-left relative screen coordinates.
 @param delegate An object that may implement the optional @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link or @link PFUIElementReportError: -PFUIElementReportError: @/link delegate method. May be <code>nil</code>.
 @param  error An indirect reference to an NSError object describing the error if the result is <code>nil</code>. Pass NULL if you don't care about the error.
 @result The PFUIElement object at <code>point</code>, or <code>nil</code> if an error occurred.
 @discussion Use this method to read the screen based on the current location of the mouse in top-left relative screen coordinates, or any other point of interest. This method can be the foundation of any screen reader application.
 
 In Cocoa, the mouse location can be obtained at any time in bottom-left relative screen coordinates, regardless of the current event or pending events, using NSEvent's <code>+mouseLocation</code> class method. You must convert it to the accessibility API's top-left relative screen coordinates before passing it to this method. To avoid having to convert the coordinates, you can use Carbon's <code>HIGetMousePosition()</code> function, which returns top-left relative screen coordinates. It is available in 64-bit applications as well as 32-bit applications.
 
 Get the root application element that owns the returned UI element by sending @link applicationElement -applicationElement@/link to it. Navigate the accessibility containment hierarchy in either direction using @link //apple_ref/occ/instm/PFUIElement(Attributes)/AXParent -AXParent @/link and @link //apple_ref/occ/instm/PFUIElement(Attributes)/AXChildren -AXChildren@/link.
 
 Use the PFApplicationUIElement instance method @link elementAtPoint: -elementAtPoint: @/link instead of this class method to get a PFUIElement object representing the UI element visible at <code>point</code> on the screen belonging to a specific application.
 
 See @link initWithElementRef:delegate: -initWithElementRef:delegate:@/link for important information regarding the creation of UI element objects.
*/
+ (PFUIElement *)elementAtPoint:(NSPoint)point withDelegate:(id)delegate error:(NSError **)error;

/*!
 @method firstElementWithRole:inArray:
 @abstract Class method returns the first PFUIElement object with attribute <code>role</code> in <code>array</code>.
 @param role an NSString describing an accessibility API role; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @param array An array of PFUIElement objects.
 @result The first PFUIElement object with attribute <code>role</code>, or <code>nil</code> if none exists in <code>array</code>.
 @discussion Use this method to test whether, for example, an application's children include any windows or a window's children include any scroll areas.
 
 This method does a flat search in <code>array</code>; it does not search nested elements such as grandchildren.
 
 See also @link elementsWithRole:inArray: +elementsWithRole:inArray:@/link.
*/
+ (PFUIElement *)firstElementWithRole:(NSString *)role inArray:(NSArray *)array;

/*!
 @method elementsWithRole:inArray:
 @abstract Class method returns all PFUIElement objects having attribute <code>role</code> in <code>array</code>.
 @param role an NSString representing an accessibility API role; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @param array An array of PFUIElement objects.
 @result An array of PFUIElement objects with attribute <code>role</code>, or an empty array if none exists in <code>array</code>.
 @discussion Use this method to return, for example, all of an element's children that are buttons.
 
 This method does a flat search in <code>array</code>; it does not search nested elements such as grandchildren.
 
 See also @link firstElementWithRole:inArray: +firstElementWithRole:inArray:@/link.
*/
+ (NSArray *)elementsWithRole:(NSString *)role inArray:(NSArray *)array;

/*!
 @method exists
 @abstract Returns whether the UI element represented by the receiver still exists in the running application.
 @discussion Returns <code>YES</code> if the UI element represented by the receiver still exists in the running application. Use this method to test, for example, whether an application's window is still open. This method is very fast, so it can and should be called liberally. 
 
 This method is important because the AXUIElementRef object associated with the receiver may be invalid if the UI element it represents has been destroyed; that is, if it no longer exists in the running application. A UI element can be destroyed under many circumstances, not all of them obvious. The accessibility API defines existence in terms of an element's availability in the user interface, rather than its programmatic availability. For example, a window UI element is destroyed when its application is hidden, even if the window object still exists in memory. To be prudent, you should not call any accessibility API function on a UI element that might have been destroyed without first sending it this message.
 
 You can, however, always safely get a destroyed UI element's @link pid -pid@/link, and you can always test destroyed PFUIElement objects for equality using <code>-isEqual:</code> or @link isEqualToElement: -isEqualToElement:@/link. You can also get information that was cached when a destroyed PFUIElement object was created, by sending @link elementInfo -elementInfo @/link to it.
 
 Due to the design of the accessibility API, some UI elements do not issue <code>NSAccessibilityUIElementDestroyedNotification</code> notificatiions when the UI element containing them is destroyed, such as a Cocoa window's title and grow area elements. This method depends on this notification internally, so it will report that such elements still exist in the running application, although Apple advises that all UI elements descended from a destroyed element should nevertheless be considered destroyed. One way to deal with this issue is to explicitly mark all contained UI elements as destroyed when their container is destroyed, by sending @link setExists: -setExists: @/link to each of them with <code>flag</code> passed as <code>NO</code>. If this technique is not adopted, a prudent client should always anticipate that a PFUIElement object it is accessing might not exist, in case this method returns an incorrect result. There is no better way to deal with this issue; using an accessibility API function to access a destroyed UI element is not guaranteed to return an error because AXUIElementRef objects are recycled and therefore might refer to another valid UI element.
 
 A client can observe when a UI element is destroyed by calling @link //apple_ref/occ/instm/PFUIElement/setDelegate: -setDelegate: @/link to set itself as PFUIElement's delegate and implementing the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed:@/link delegate method, or by registering with NSNotificationCenter to observe <code>PFUIElementWasDestroyedNotification</code>. As noted above, some elements do not issue a notification when their container is destroyed.
 
 For other issues relating to UI elements that have been destroyed, see the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed:@/link delegate method.
 
 See also @link isDestroyed -isDestroyed @/link, which is the antonym of <code>-exists</code>, and @link setExists: -setExists: @/link, which can be used to mark children of a destroyed element as destroyed by passing <code>NO</code>.
*/
- (BOOL)exists;

/*!
 @method setExists:
 @param flag <code>YES</code> if the receiver exists in the running application; <code>NO</code>, if not.
 @abstract Sets whether the receiver exists in the running application to <code>flag</code>.
 @discussion If flag is passed as <code>NO</code>, the @link exists -exists @/link method thereafter reports that the receiver has been destroyed. This method does not actually destory the receiver, and it does not send <code>PFUIElementWasDestroyedNotification</code>. Use it only to mark elements as destroyed when their container has been destroyed.
 
 Due to the design of the accessibility API, some UI elements do not issue <code>NSAccessibilityUIElementDestroyedNotification</code> when the UI element containing them is destroyed, such as a Cocoa window's title and grow area elements. Apple advises that all UI elements descended from a destroyed element should nevertheless be considered destroyed.
 
 This method enables a client to mark any UI element as destroyed in response to notification that an ancestor has been destroyed. If this technique is not adopted, a prudent client should always anticipate that a PFUIElement object it is accessing might not exist, in case @link exists -exists@/link returns an incorrect result. There is no better way to deal with this issue; using an accessibility API function to access a destroyed UI element is not guaranteed to return an error because AXUIElementRef objects are recycled and therefore might refer to another valid UI element.
 
 <b>IMPORTANT:</b> If flag is passed as <code>NO</code>, it is the client's responsibility to unregister this element for all notifications by sending @link //apple_ref/occ/instm/PFObserver/unregisterForAllNotificationsFromElement: -unregisterForAllNotificationsFromElement: @/link to all relevant observers. Since by definition this element did not post a notification when it was destroyed, it is also the client's responsibility to ensure that all other PFUIElement objects representing the same UI Element are marked as destroyed.
 
 There should be no reason to pass flag as <code>YES</code>.
 
 See also @link exists -exists@/link.
*/
- (void)setExists:(BOOL)flag;

/*!
 @method isVisibleAtPoint:
 @abstract Returns whether the UI element represented by the receiver is visible at <code>point</code> on the screen.
 @param point An NSPoint structure representing a point on the screen in top-left relative screen coordinates.
 @discussion Returns <code>YES</code> if the UI element represented by the receiver is visible at <code>point</code> on the screen; that is, not obscured by another application's UI element.
 
 If the UI element represented by the receiver is not visible because another application's UI element is in front of it, then PFUIElement's @link elementAtPoint:withDelegate:error: +elementAtPoint:withDelegate:error: @/link class method cannot read it by its location on the screen. The PFApplicationUIElement's @link elementAtPoint: -elementAtPoint: @/link instance method can read it even if it is not visible.
 
 See @link elementAtPoint:withDelegate:error: +elementAtPoint:withDelegate:error: @/link for information about how to obtain the mouse pointer's current position on the screen.
*/
- (BOOL)isVisibleAtPoint:(NSPoint)point;

/*!
 @method isEqualToElement:
 @abstract Returns whether <code>element</code> and the receiver represent the same UI element in the target application.
 @param element A PFUIElement or PFApplicationUIElement object to be tested for equality with the receiver.
 @discussion Returns <code>YES</code> if <code>element</code> represents the same UI element in the target application that the receiver represents.
 
 This method tests whether two PFUIElement objects represent the same UI element in the running application. This works even if the UI elements represented by either or both elements no longer exist in the running application.
 
 Use this method for efficiency if you know that element is a PFUIElement or PFApplicationUIElement object. Otherwise, use <code>-isEqual:</code>.
*/
- (BOOL)isEqualToElement:(PFUIElement *)element;

/*!
 @method isSiblingOfElement:
 @abstract Returns whether <code>element</code> and the receiver have the same parent.
 @param element A PFUIElement or PFApplicationUIElement object to be compared with the receiver.
 @discussion Returns <code>YES</code> if <code>element</code> is a sibling of the receiver; that is, if they have the same parent.
 
 This method works even if the UI elements represented by either or both elements no longer exist in the running application.
 
 Two root application objects are never considered siblings.
*/
- (BOOL)isSiblingOfElement:(PFUIElement *)element;

/*!
 @method indexAmongLikeElementsInArray:
 @abstract Returns the index of the receiver among all UI elements in <code>array</code> having the same role as the receiver, or <code>NSNotFound</code> if the receiver is not in <code>array</code>.
 @param array An array of PFUIElement objects.
 @result The receiver's index among like elements in <code>array</code>, or <code>NSNotFound</code> if the receiver is not in <code>array</code>.
 @discussion This method does a flat search in <code>array</code>; it does not search nested elements such as grandchildren.
 
 The returned index is zero-based. For AppleScript GUI Scripting compatibility, add 1 to the index returned by this method because AppleScript indexes are one-based. UI elements whose role is "AXUnknown" should not be indexed with this method, but instead with NSArray's <code>-indexOfObject:</code> for compatibility with GUI Scripting.
 
 The indexes of windows represent their front-to-back ordering.
*/
- (NSUInteger)indexAmongLikeElementsInArray:(NSArray *)array;

/*!
 @method applicationElement
 @abstract Creates and returns a root application PFUIElement object representing the application owning the receiver.
 @result The receiver's PFApplicationUIElement object.
 @discussion It is sometimes useful to have a reference to the root application UI element as a PFApplicationUIElement object, in order to access features that are unique to that subclass of PFUIElement.
*/
- (id)applicationElement;

/*!
 @method pid
 @abstract Returns the BSD Unix process identification number (PID) of the application owning the receiver.
 @result An integer, or 0 if an error occurred.
 @discussion It is sometimes useful to know the process identification number of the application owning the receiver. For example, it can be used with a variety of Carbon functions to access information not otherwise readily available in Cocoa. 0 is never a valid PID.
 
 The PID returned by this method can be used even if the receiver has been destroyed in the running application (for example, a window has closed).
 
 See also PFApplicationUIElement's @link //apple_ref/occ/instm/PFApplicationUIElement/pidNumber -pidNumber@/link, which returns the PID as a Cocoa NSNumber object.
*/
- (pid_t)pid;

/*!
 @method parentPath
 @abstract Returns an array of PFUIElement objects representing the containment hierarchy of the receiver, excluding the receiver itself.
 @result An array of PFUIElement objects starting with the root PFApplicationUIElement object, or an empty array if the UI element represented by the receiver has been destroyed in the target application.
 @discussion The returned array represents the path from the root application element to and including the receiver's parent in the target application's UI element containment hierarchy. Use @link path -path @/link to obtain an array that includes the receiver itself.
 
 If the array is not empty, its first item is a PFApplicationUIElement object.
*/
- (NSArray *)parentPath; // array of PFUIElement*

/*!
 @method path
 @abstract Returns and array of PFUIElement objects representing the containment hierarchy of the receiver, including the receiver itself.
 @result An array of PFUIElement objects starting with the root PFApplicationUIElement object. The array contains only the receiver if it has been destroyed in the running application.
 @discussion The returned array represents the path from the root application element to and including the receiver in the target application's UI element containment hierarchy. Use @link parentPath -parentPath @/link to obtain an array that excludes the receiver.
 
 If the receiver has been destroyed in the running application, the returned array contains only the receiver, which is now an orphan. Otherwise, the first item in the array is a PFApplicationUIElement object.
*/
- (NSArray *)path; // array of PFUIElement*

/*!
 @method clippedRect
 @abstract Returns an NSRect structure representing the origin and size of the portion of the receiver that is visible in its container.
 @result An NSRect structure representing the intersection of the element's visible rectangle and the rectangle of its smallest ancestor.
 @discussion Returns an NSRect representing the intersection of the element's rectangle visible on the screen and the rectangle of its smallest ancestor. If the two rectangles don't overlap, the returned rectangle has its origin at {0.0, 0.0} and a size of {0.0, 0.0}. This method is useful because some elements are sized larger than their containing elements, such as a text view in a scroll view. Use the result to determine what part of the receiver, if any, is currently scrolled into view.
 
 A drawer is treated as an independent element because its parent window occupies a different screen position.
*/
- (NSRect)clippedRect;

/*!
 @method setMessagingTimeout:
 @abstract Sets the receiver's timeout to <code>timeoutInSeconds</code>.
 @param timeoutInSeconds A float representing a number of seconds.
 @discussion  Pass 0.0 to set the timeout to the default global timeout for the target application.
 
 If the receiver is the root application UI element, this method sets the global timeout for all UI elements in the target application.
 
 When any accessibility call fails to work within the timeout period, a kAXErrorCannotComplete error is generated.
*/
- (void)setMessagingTimeout:(float)timeoutInSeconds;

#pragma mark ATTRIBUTES
/*!
 @methodgroup ATTRIBUTES
*/

/*!
 @method attributes
 @abstract Returns an array of the technical, non-localized names of all attributes recognized by the receiver.
 @result An array of NSString objects, or an empty array if the receiver does not recognize any attributes.
 @discussion Cocoa and Carbon target applications respond differently to this method. In general, Carbon target applications return only those attributes that are implemented by the receiving UI element, and all such attributes therefore have a value. Cocoa target applications return all attributes implemented in the receiver's class, whether or not a particular attribute for a particular UI element has a value. In cases where an attribute is not implemented, or is implemented but does not contain a value, the PFUIElement accessor method for that attribute returns <code>nil</code>. In all cases, an empty string is considered a valid attribute value.
 
 The preferred and most efficient way to get or set the value of an attribute of the receiver is to send one of the specific attribute accessor messages, such as @link //apple_ref/occ/instm/PFUIElement(Attributes)/AXRole -AXRole @/link declared in the Attributes category. The names of these accessor methods are identical to the technical, non-localized attribute strings declared in the accessibility API and returned by this method, including the "AX" prefix. The accessor methods are convenient to use and descriptive when you know the name of the attribute you want to access. The PFAssistive framework implements accessor methods for all attributes declared in the Cocoa and Carbon accessibility APIs (except those marked obsolete).
 
 While many specific attribute accessor methods apply to almost any kind of UI element, others apply only to particular kinds of UI elements. In addition, applications are free to implement new kinds of UI elements and new attributes. Apple encourages developers to minimize the number of new attributes by reusing existing attributes for analogous new purposes wherever appropriate. As time goes by, therefore, existing attributes may apply to more and more kinds of UI elements. When in doubt, test the return value of an attribute for <code>nil</code> or an empty value, or use one of the <code>-exists...</code> methods. For specific usage information regarding individual attributes, consult the "Accessibility (ApplicationServices/HIServices) Reference" and the "Carbon Accessibility Reference" in the Xcode Documentation window, and the AccessibilityDefinitions.plist file at /System/Library/Accessibility/AccessibilityDefinitions.plist.
 
 Another way to get or set the value of a non-parameterized attribute of the receiver is to send the generic messages @link valueForAttribute: -valueForAttribute: @/link and @link setValue:forAttribute: -setValue:forAttribute:@/link. They use key-value coding with keys generated by @link nonParameterizedAttributes -nonParameterizedAttributes@/link. They can be used instead of the specific accessor methods described above to get any non-parameterized attribute value using its NSAccessibility informal protocol constant, its accessibility API string constant cast to NSString*, or the attribute string itself. You must always use these two generic methods when you don't know which attribute you need, for example, when iterating over a list of attributes, and when a specific attribute accessor method is not implemented here. The @link valueForAttribute: -valueForAttribute: @/link method automatically sends the appropriate specific attribute accessor method, if one exists for the attribute, and returns an object of the appropriate type. It even returns the value of a non-parameterized attribute that is not covered by a specific accessor method, including attributes added to the accessibility API in the future or implemented in another application like the Finder or Safari, because the framework implements the Cocoa key-value coding fallback methods using whatever attribute strings you provide.
 
 This method also returns the parameterized attribute strings if the receiver is a text area or a text field. However, you cannot obtain or set the value of parameterized attributes using key-value coding in this version of the framework. For these, you must use the specific parameterized attribute accessor methods.
 
 A few applications may not expose the AXChildren attribute in the list of attributes for some custom elements, even though these applications may in fact support the AXChildren attribute. The accessibility API may not expose the AXChildren attribute on behalf of these applications, as it used to do. Pursuant to Apple's recommendation, this method therefore always includes the AXChildren attribute in the list for all UI elements. The PFAssistive framework will always return a value for this attribute for elements that support them.
 
 See also @link nonParameterizedAttributes -nonParameterizedAttributes @/link and @link parameterizedAttributes -parameterizedAttributes@/link.
*/
- (NSArray *)attributes; // treat as array of NSString*

/*!
 @method nonParameterizedAttributes
 @abstract Returns the technical, non-localized names of all non-parameterized attributes recognized by the receiver.
 @result An array of NSString objects, or an empty array if the receiver does not recognize any non-parameterized attributes.
 @discussion All elements should return at least one non-parameterized attribute, AXRole. However, if the receiver does not support the accessibility API, this method returns an empty array.
 
 See also @link attributes -attributes @/link and @link parameterizedAttributes -parameterizedAttributes@/link.
*/
- (NSArray *)nonParameterizedAttributes; // treat as array of NSString*

/*!
 @method parameterizedAttributes
 @abstract Returns the technical, non-localized names of all parameterized attributes recognized by the receiver.
 @result An array of NSString objects, or an empty array if the receiver does not recognize any parameterized attributes.
 @discussion Parameterized attributes exist for text areas, text fields, cell-based tables, and layout areas. If the receiver does not recognize any parameterized attributes, this method returns an empty array.
 
 See also @link attributes -attributes @/link and @link nonParameterizedAttributes -nonParameterizedAttributes@/link.
*/
- (NSArray *)parameterizedAttributes; // treat as array of NSString*

#pragma mark ATTRIBUTE UTILITIES
/*!
 @methodgroup ATTRIBUTE UTILITIES
*/

/*!
 @method existsAttribute:
 @abstract Returns whether the UI element represented by the receiver still exists in the running application and recognizes <code>attribute</code>.
 @param attribute An NSString representing an attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @discussion Returns <code>YES</code> if the UI element represented by the receiver still exists in the running application and recognizes <code>attribute</code>.
 
 Use this method to test, for example, whether a button UI element has a subrole attribute and might therefore be a close, minimize, or zoom button. Note that some UIElements have attributes with NULL values, such as 0, or an empty string.

 See also @link existsNonParameterizedAttribute: -existsNonParamterizedAttribute:@/link, @link existsParameterizedAttribute: -existsParameterizedAttribute:@/link, and @link existsValueForAttribute: -existsValueForAttribute:@/link.
*/
- (BOOL)existsAttribute:(NSString *)attribute;

/*!
 @method existsNonParameterizedAttribute:
 @abstract Returns whether the UI element represented by the receiver still exists in the running application and recognizes <code>attribute</code> as a non-parameterized attribute.
 @param attribute An NSString representing a non-parameterized attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @discussion Returns <code>YES</code> if the UI element represented by the receiver still exists in the running application and recognizes <code>attribute</code> as a non-parameterized attribute.
 
 See also @link existsAttribute: -existsAttribute:@/link, @link existsParameterizedAttribute: -existsParameterizedAttribute:@/link, and @link existsValueForAttribute: -existsValueForAttribute:@/link.
*/
- (BOOL)existsNonParameterizedAttribute:(NSString *)attribute;

/*!
 @method existsParameterizedAttribute:
 @abstract Returns whether the UI element represented by the receiver still exists in the running application and recognizes <code>attribute</code> as a parameterized attribute.
 @param attribute An NSString representing a parameterized attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @discussion Returns <code>YES</code> if the UI element represented by the receiver still exists in the running application and recognizes <code>attribute</code> as a parameterized attribute.
 
 See also @link existsAttribute: -existsAttribute:@/link, @link existsNonParameterizedAttribute: -existsNonParameterizedAttribute:@/link, and @link existsValueForAttribute: -existsValueForAttribute:@/link.
*/
- (BOOL)existsParameterizedAttribute:(NSString *)attribute;

/*!
 @method existsValueForAttribute:
 @abstract Returns whether the UI element represented by the receiver still exists in the running application, recognizes <code>attribute</code>, and <code>attribute</code> has a value.
 @param attribute An NSString representing a non-parameterized attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @discussion Returns <code>YES</code> if the UI element represented by the receiver still exists in the running application, if it recognizes <code>attribute</code>, and if <code>attribute</code> has a value. The <i>attribute</i> argument must be a non-parameterized attribute.
 
 A value such as <code>0</code> or an empty string is considered a valid value for a UI element's attribute, but <code>nil</code> is not.
*/
- (BOOL)existsValueForAttribute:(NSString *)attribute;

/*!
 @method isRole:
 @abstract Returns whether the receiver represents a UI element whose AXRole attribute is <code>role</code>.
 @param role An NSString representing an accessibility API role attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @discussion Returns <code>YES</code> if the receiver represents a UI element whose AXRole attribute has a value of <code>role</code>.
 
 This method tests whether the receiver's role string is equal to the string provided in the <code>role</code> argument. It tests the receiver's cached <code>elementInfo</code>, so it returns the correct result even if the receiver has been destroyed.
*/
- (BOOL)isRole:(NSString *)role;

/*!
 @method childrenCount
 @abstract Returns the number of children the receiver contains.
 @result An integer counting the receiver's children.
 @discussion This method counts the number of PFUIElement objects in the receiver's children array attribute value.
*/
- (NSUInteger)childrenCount;

/*!
 @method isParameterizedAttribute:
 @abstract Returns whether <code>attribute</code> is a parameterized attribute.
 @param attribute An NSString representing a parameterized attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @discussion Returns <code>YES</code> if <code>attribute</code> is a parameterized attribute.
 
 If <code>attribute</code> is not a parameterized attribute, it is a non-parameterized attribute. See @link existsAttribute: -existsAttribute:@/link for details.
*/
- (BOOL)isParameterizedAttribute:(NSString *)attribute;

/*!
 @method isSettableAttribute:
 @abstract Returns whether <code>attribute</code> can be modified.
 @param attribute An NSString representing an attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @discussion Returns <code>YES</code> if <code>attribute</code> can be modified.
 
 This method tests whether <code>attribute</code> is marked as settable in the accessibility API. Some UI elements report that an attribute is settable when in fact it is not.
*/
- (BOOL)isSettableAttribute:(NSString *)attribute;

/*!
 @method setValue:forAttribute:
 @abstract Sets the value of <code>attribute</code> to <code>object</code>.
 @param object An object of any appropriate type.
 @param attribute An NSString representing a non-parameterized attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result <code>YES</code> if the value was successfully set; <code>NO</code>, if not.
 @discussion This method sets the value of <code>attribute</code> to <code>object</code> if attribute is a non-parameterized attribute, returning <code>YES</code> if successful. It always returns <code>NO</code> if attribute is a parameterized attribute.
 
 Use this method to set the value of a non-parameterized attribute in the general case, when you don't know which attribute you will receive. Also use this method when this framework does not include, or you prefer not to use, a specific attribute accessor method, when this framework does not implement a specific attribute accessor method, or when <code>attribute</code> is a custom application attribute.
 
 It is your responsibility to ensure that <code>object</code> is an object of an appropriate type for <code>attribute</code> and, in the case of objects of type point, size, rect, and range, that the values that <code>object</code> encodes make sense for <code>attribute</code> (for example, that windows are on screen, have positive width and height values, and are within the limits of the Mac OS X window server).
 
 This method uses key-value coding to send the appropriate specific attribute accessor method, if it is implemented, falling back to a private generic method if a specific attribute accessor method is not implemented for <code>attribute</code>.
 
 Some applications mark attributes as settable when in fact they cannot be modified, and it is possible that this method will return <code>YES</code> even if the value of the attribute was not successfully set. It is the client's responsibility to deal with such false positives, if necessary, perhaps by testing the value of the attribute after setting it to see if the value was successfully changed. We don't perform such a test here because it is relatively expensive.
 
 See also @link valueForAttribute: -valueForAttribute:@/link.
*/
- (BOOL)setValue:(id)object forAttribute:(NSString *)attribute;

/*!
 @method valueForAttribute:
 @abstract Returns the value of <code>attribute</code>.
 @param attribute An NSString representing a non-parameterized attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result An object of an appropriate type, or <code>nil</code> if attribute is a parameterized attribute.
 @discussion Returns the value of <code>attribute</code> as an object of an appropriate type.
 
 Use this method to get the value of a non-parameterized attribute in the general case, when you don't know which attribute you will receive. Also use this method when this framework does not include, or you prefer not to use, a specific attribute accessor method, when this framework does not implement a specific attribute accessor method, or when <code>attribute</code> is a custom application attribute.
 
 This method uses key-value coding to send the appropriate specific attribute accessor method, if it is implemented, falling back to a private generic method if a specific attribute accessor method is not implemented for <code>attribute</code>.
 
 The <code>attribute</code> parameter should be a non-parameterized attribute. See @link typeForAttribute: -typeForAttribute:@/link for a list of recognized attribute value types.
 
 See also @link setValue:forAttribute: -setValue:forAttribute:@/link.
*/
- (id)valueForAttribute:(NSString *)attribute;

/*!
 @method valuesForAttributes:
 @abstract Returns the values of <code>attributes</code>.
 @param attributes An NSArray of NSStrings representing non-parameterized attributes; may be constants from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result An array of objects of any type.
 @discussion Returns the values of <code>attributes</code>.
 
 Use this method to get multiple objects at once for the non-parameterized attributes in the <code>attributes</code> array. This method has performance advantages over getting attributes individually.
 
 The <code>attributes</code> parameter should be an array of non-parameterized attributes.
*/
- (NSArray *)valuesForAttributes:(NSArray *)attributes;

/*!
 @method valuesForAttribute:range:
 @abstract Returns the values of <code>attributes</code> in <code>range</code>.
 @param attribute An NSString representing an array attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result An array of objects of any type.
 @discussion Returns the values of <code>attribute</code> in <code>range</code>.
 
 Use this method to get multiple objects at once for the non-parameterized attributes in the <code>attribute</code> array, restricted to <code>range</code>. It is not an error if the <code>length</code> of <code>range</code> exceeds the number of array elements remaining from the <code>location</code> of <code>range</code>; instead, the returned array will contain the remaining array elements.
 
 Use this method instead of AXChildren when the <code>attribute</code> array is very large, in order to process the array in small pieces. For example, this method can bring in a small number of array elements in order to make them visible in increments when a table view is scrolled.
 
 The <code>attribute</code> parameter should be an array of non-parameterized attributes.
*/
- (NSArray *)valuesForAttribute:(NSString *)attribute range:(NSRange)range;

/*!
 @method valueForAttribute:parameter:
 @abstract Returns the value of <code>attribute</code> for <code>parameter</code>.
 @param attribute An NSString representing a parameterized attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @param parameter An object of the type required by the parameterized attribute.
 @result An object of an appropriate type, or <code>nil</code> if attribute is a non-parameterized attribute.
 @discussion Returns the value of <code>attribute</code> for <code>parameter</code>.
 
 Use this method to get the value of a parameterized attribute in the general case, when you don't know which attribute you will receive.
 
 The <code>attribute</code> parameter should be a parameterized attribute.
 
 See @link typeForAttribute: -typeForAttribute:@/link for a list of recognized attribute value types.
*/
- (id)valueForAttribute:(NSString *)attribute parameter:(id)parameter;

/*!
 @method valueForAttribute:atIndex:
 @abstract Returns the value of the item at <code>index</code> in the array returned by <code>attribute</code>.
 @param attribute An NSString representing an array attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @param index The index of the desired value in the array.
 @result An object of an appropriate type, or <code>nil</code> if index is not in the range of the  array returned by <code>attribute</code>.
 @discussion Returns the value of the item at <code>index</code> in the array returned by <code>attribute</code>.
 
 Use this method to get an object at any index in the value of an array attribute.
 
 The <code>attribute</code> parameter should be an array attribute.
 
 See @link typeForAttribute: -typeForAttribute:@/link for a list of recognized attribute value types.
*/
- (id)valueForAttribute:(NSString *)attribute atIndex:(NSUInteger)index;

/*!
 @method typeForAttribute:
 @abstract Returns a non-localized string for the data type of the value of <code>attribute</code>.
 @param attribute An NSString representing an attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result An NSString object describing the data type of <code>attribute</code>.
 @discussion Returns a non-localized string for the data type of the value of <code>attribute</code>.
 
 These type identifiers are unique to the PFAssistive framework; Apple's accessibility API has no equivalent. They are used for program control in the PFAssistive framework, and a client application can use them in a similar fashion, for example, as table column identifiers. According to AccessibilityDefinitions.plist, he possible strings are "array", "attributed string", "Boolean", "data", "number", "point", "range", "rect", "size", "string", "UIElement" and "URL". It appears that "date" is also a possible value. This method returns "unknown" if it cannot determine the type. This method does not return "text marker" or "text marker range" because they are marked as private in AccessibilityDefinitions.plist.
 
 This method returns a string for the structured types point, size, rect, and range when appropriate. It may return a string for an unknown type or <code>nil</code> if <code>attribute</code> does not exist or has no value.
 
 In the case of non-parameterized attributes, this method gets the attribute's value first and then determines its type.
 
 For a localized string describing the data type of <code>attribute</code>, use @link descriptionOfTypeForAttribute: -descriptionOfTypeForAttribute:@/link.
*/
- (NSString *)typeForAttribute:(NSString *)attribute;

/*!
 @method descriptionForAttribute:
 @abstract Returns a localized string describing <code>attribute</code>, suitable for display or speech.
 @param attribute An NSString representing an attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result An NSString object, an empty string if the receiver does not implement <code>attribute</code>.
 @discussion These attribute descriptions are unique to the PFAssistive framework; Apple's accessibility API has no equivalent. The key-value pairs are defined in the PFAttributeDescriptions.strings file in the PFAssistive framework bundle.
 
 It may be possible for a target application to return an empty description string. It is up to the client to deal with this, perhaps by supplying a localizable string in a strings file in its bundle or by substituting the technical attribute name.
*/
- (NSString *)descriptionForAttribute:(NSString *)attribute;

/*!
 @method descriptionOfTypeForAttribute:
 @abstract Returns a localized string describing the data type of <code>attribute</code>, suitable for display or speech.
 @param attribute An NSString representing an attribute; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result An NSString object, an empty string if the receiver does not implement <code>attribute</code>.
 @discussion These type descriptions are unique to the PFAssistive framework; Apple's accessibility API has no equivalent. The key-value pairs are defined in the PFTypeDescriptions.strings file in the PFAssistive framework bundle.
 
 This method returns a string for the structured types point, size, rect, and range when appropriate. It may return a string for an unknown type or <code>nil</code> if <code>attribute</code> does not exist or has no value.
 
 In the case of non-parameterized attributes, this method gets the attribute's value first and then determines the type of the attribute.
 
 For a non-localized string identifying the data type of <code>attribute</code>, use @link typeForAttribute: -typeForAttribute:@/link.
*/
- (NSString *)descriptionOfTypeForAttribute:(NSString *)attribute;

/*!
 @method descriptionForSubrole:
 @abstract Returns a localized string describing <code>subrole</code>, suitable for display or speech.
 @param subrole An NSString representing a subrole; may be a constant from the NSAccessibility informal protocol or from AXAttributeConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result An NSString object, an empty string if the receiver does not implement <code>attribute</code>.
 @discussion These attribute descriptions are unique to the PFAssistive framework; Apple's accessibility API has no equivalent. The key-value pairs are defined in the PFSubroleDescriptions.strings file in the PFAssistive framework bundle.
 
 It may be possible for a target application to return an empty description string. It is up to the client to deal with this, perhaps by supplying a localizable string in a strings file in its bundle or by substituting the technical subrole name. 
*/
- (NSString *)descriptionForSubrole:(NSString *)subrole;

/*!
 @method attributedStringFromAXAttributedString:
 @abstract Returns an attributed string suitable for display in a text view, converted from an accessibility attributed string.
 @param AXAttributedString An NSAttributedString with accessibility text attributes, returned by the @link //apple_ref/occ/instm/PFUIElement(Attributes)/AXAttributedStringForRange: -AXAttributedStringForRange:@/link parameterized attribute method.
 @result An NSAttributedString object with standard Cocoa text system attributes.
 @discussion The attributed string returned by this method is a standard text system attributed string suitable for display in a text view based on NSTextStorage. It is converted from an accessibility text attributed string passed to it from the @link //apple_ref/occ/instm/PFUIElement(Attributes)/AXAttributedStringForRange: -AXAttributedStringForRange: @/link parameterized attribute method. The conversion is lossy, as described below.
 
 Use this method only when you wish to display a reasonable facsimile of the accessibility attributed string. For an assistive application, use the accessibility attributed string and its attributes, instead.
 
 An accessibility text attributed string is an NSAttributedString in the sense that it has a dictionary of attributes and responds to NSAttributedString methods. However, its attributes, declared in AXTextAttributedString.h, differ from the standard text system attributes, and it therefore cannot be used for display without first converting it using this method. An accessibility text attributed string omits many appearance-related attributes of a standard text system attributed string that do not convey meaningful information in the accessibility context, so an attributed string returned by this method may not look identical to the original attributed string in the target application. For example, baseline offset, kerning and ligature information is omitted, and double strikethroughs are not distinguished from single strikethroughs. Other standard text system attributes are included, such as foreground and background color, superscript and subscript, underline, strikethrough, shadow, link, and text attachment. An accessibility text attributed string also includes some attributes that are not recognized by the standard text system, namely, natural language, replacement string, and misspelled string. 
*/
- (NSAttributedString *)attributedStringFromAXAttributedString:(NSAttributedString *)AXAttributedString;

/*!
 @method attributesForRole:andSubrole:
 @abstract Class method returns an NSArray of NSDictionary objects containing information about all accessibility attributes that may or should be supported by a UI element with the specified role and subrole.
 @param role An NSString representing a UI element role; may be a constant from the NSAccessibility informal protocol or from AXRoleConstants.h in the accessibility API cast to NSString*, or the string itself.
 @param subrole An NSString representing a UI element subrole, or <code>nil</code> or an empty string if it has no subrole; may be a constant from the NSAccessibility informal protocol or from AXRoleConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result An array of NSDictionary objects, or an empty array if a UI element with the specified role and subrole should not support any attributes.
 @discussion This class method returns an array of dictionaries containing the technical names and other features of all of the accessibility attributes that a UI element with the specified role and subrole may or should support. In addition to the technical name of the attribute and the associated role, a dictionary item discloses whether the attribute is required, whether it may or should be settable, and the data types of the values in the returned array. This is general information obtained from the AccessibilityDefinitions.plist file installed at /System/Library/Accessibility/AccessibilityDefinitions.plist. It reads information from the role_attributes array and the attributes dictionary in the plist file. It does not guarantee that any particular UI element does in fact support the listed attributes.
 
 Each dictionary returned in the array has keys "attribute" (NSString, technical name of the attribute), "required" (NSNumber with BOOL value, NO if attribute is optional or YES if attribute is required), "role" (NSString), "settable" (NSString, "NO" if attribute must not be settable, "YES" if attribute may optionally be settable, or "REQUIRED" if attribute must be settable), and "types" (NSArray of NSStrings). The "required" field may be omitted if the attribute is not required.
 
 As of OS X 10.9 Mavericks, the attributes and role_attributes arrays are believed to have been fully validated by Apple and are complete.
*/
+ (NSArray *)attributesForRole:(NSString *)role andSubrole:(NSString *)subrole;

/*!
 @method attributeDefinitions
 @abstract Class method returns an NSDictionary object containing information about all accessibility attributes.
 @result A dictionary containing information about all accessibility attributes.
 @discussion This class method returns a dictionary containing the technical names and other features of all accessibility attributes as listed in the AccessibilityDefinitions.plist file installed at /System/Library/Accessibility/AccessibilityDefinitions.plist starting with Mac OS X 10.7 Lion. It reads information from the attributes dictionary in the plist file.
*/
+ (NSDictionary *)attributeDefinitions;

/*!
 @method roleDefinitions
 @abstract Class method returns an NSDictionary object containing information about all accessibility attribute roles and subroles.
 @result A dictionary containing information about all accessibility attribute roles and subroles.
 @discussion This class method returns a dictionary containing the technical names and other features of all accessibility attribute roles and subroles as listed in the AccessibilityDefinitions.plist file installed at /System/Library/Accessibility/AccessibilityDefinitions.plist starting with Mac OS X 10.7 Lion. It reads information from the roles dictionary in the plist file. Subrole strings are in the form role:subrole.
 */
+ (NSDictionary *)roleDefinitions;

#pragma mark ACTIONS
/*!
 @methodgroup ACTIONS
*/

/*!
 @method actions
 @abstract Returns the technical, non-localized names of all actions implemented by the receiver.
 @result An array of NSString objects, or an empty array if the receiver does not recognize any actions.
 @discussion The accessibility API implements relatively few actions, and many UI elements do not support any actions at all. Specific applications may implement custom actions, such as the AXOpen action in the Finder, and they are included in the array returned by this method.
 
 Sometimes performing an action and setting an attribute would achieve the same goal. In such cases, Apple generally prefers the action and makes the attribute unsettable. The prime example is checkboxes, whose value you set by sending them an AXPress action.
 
 A client application can test whether a UI element exists and supports a specific action with @link existsAction: -existsAction:@/link; it can get a localized description of an action with @link descriptionForAction: -descriptionForAction:@/link; and it can perform an action with @link performAction: -performAction:@/link.
 
 The following actions are supported by the accessibility API (obsolete actions are omitted):
 <ul>
 <li>AXCancel - a cancel action, such as pressing the Cancel button.
 <li>AXConfirm - a confirm action, such as pressing Return in a text field.
 <li>AXDecrement - a decrement action, such as pressing a stepper's down arrow.
 <li>AXIncrement - an increment action, such as pressing a stepper's up arrow.
 <li>AXPress - a press action, such as clicking a button or a menu.
 <li>AXRaise - a raise action, such as bringing a window to the front within the application.
 <li>AXShowMenu	- a show menu user action, such as opening a pop-up button's menu or a contextual menu in a text view or text field.
 <li>AXShowAlternateUI - a show alternate UI action, such as causing a button to appear that is normally hidden, as when the mouse hovers over it.
 <li>AXShowDefaultUI - a show default UI action, such as causing a button that is normally hidden to hide, as when the mouse stops hovering over it.
 </ul>
*/
- (NSArray *)actions; // treat as array of NSString*

#pragma mark ACTION UTILITIES
/*!
 @methodgroup ACTION UTILITIES
*/

/*!
 @method existsAction:
 @abstract Returns whether the receiver still exists in the running application and can perform <code>action</code>.
 @param action An NSString representing an action; may be a constant from the NSAccessibility informal protocol or from AXActionConstants.h in the accessibility API cast to NSString*, or the string itself.
 @discussion Returns <code>YES</code> if the receiver still exists in the running application and can perform <code>action</code>.
 
 Use this method to test, for example, whether a button element exists and supports the AXPress action.
*/
- (BOOL)existsAction:(NSString *)action;

/*!
 @method descriptionForAction:
 @abstract Returns a localized string describing <code>action</code>, suitable for display or speech.
 @param action An NSString representing an action; may be a constant from the NSAccessibility informal protocol or from AXActionConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result An NSString object, or <code>nil</code> if <code>action</code> can't be performed by the receiver.
 @discussion These action descriptions are provided by Apple's accessibility API.
 
 It is possible for a target application to return an empty description string (as does the AXOpen action in Finder 10.3 and newer). It is up to the client to deal with this, perhaps by supplying a localizable string in a strings file in its bundle or by substituting the technical action name.
*/
- (NSString *)descriptionForAction:(NSString *)action;

/*!
 @method performAction:
 @abstract Causes the receiver to perform <code>action</code>, returning <code>YES</code> if successful.
 @param action An NSString representing an action; may be a constant from the NSAccessibility informal protocol or from AXActionConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result <code>YES</code> if the action was successfully performed; <code>NO</code>, if not.
 @discussion This method causes the receiver to perform <code>action</code> as if the user had performed it directly, such as clicking a button, returning <code>YES</code> if successful. The target application should support undo and redo and behave in every other respect as if the user had used the keyboard or mouse to perform the action.
*/
- (BOOL)performAction:(NSString *)action;

/*!
 @method actionsForRole:andSubrole:
 @abstract Class method returns an NSArray containing the technical names of all accessibility actions that should be supported by a UI element with the specified role and subrole.
 @param role An NSString representing a UI element role; may be a constant from the NSAccessibility informal protocol or from AXRoleConstants.h in the accessibility API cast to NSString*, or the string itself.
 @param subrole An NSString representing a UI element subrole, or <code>nil</code> or an empty string if it has no subrole; may be a constant from the NSAccessibility informal protocol or from AXRoleConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result An array of NSString objects, or an empty array if a UI element with the specified role and subrole should not support any actions.
 @discussion This class method returns an array listing all of the accessibility actions that a UI element with the specified role and subrole should support. This is general information obtained from the AccessibilityDefinitions.plist file installed at /System/Library/Accessibility/AccessibilityDefinitions.plist starting with Mac OS X 10.7 Lion. It does not guarantee that any particular UI element does in fact support the actions.
 
 As of OS X 10.9 Mavericks, it is not known whether the role_actions array has been fully validated by Apple and is complete.
*/
+ (NSArray *)actionsForRole:(NSString *)role andSubrole:(NSString *)subrole;

/*!
 @method actionDefinitions
 @abstract Class method returns an NSDictionary object containing information about all accessibility actions.
 @result A dictionary containing information about all accessibility actions.
 @discussion This class method returns a dictionary containing the technical names and other features of all accessibility actions as listed in the AccessibilityDefinitions.plist file installed at /System/Library/Accessibility/AccessibilityDefinitions.plist starting with Mac OS X 10.7 Lion. It reads information from the actions dictionary in the plist file.
*/
+ (NSDictionary *)actionDefinitions;

#pragma mark NOTIFICATIONS
/*!
 @methodgroup NOTIFICATIONS
*/

/*!
 @method notifications
 @abstract Returns the technical, non-localized names of all notifications issued by the receiver.
 @result An NSArray of NSString objects. All UI elements issue some notifications.
 @discussion The accessibility API does not include a function to return the notifications issued by a UI element, so the list returned by this method is arbitrary and is only as current as the version of Mac OS X existing when this framework was last updated.
 
 The notifications returned by this method are based on the class of the receiver. The root application element reports all notifications that may be issued by any element in the application. Other elements report all notifications that might be appropriate to the nature and function of that element. Notifications returned by this method should be considered candidates until a particular application is tested. The only notifications that this method excludes are those that are plainly inappropriate for a particular element, such as menu notifications in the case of a window element, and window notififications in the case of a menu or menu item element. Note that, as of OS X 10.9 Mavericks, custom UI elements that do not inherit from NSView, NSWindow or NSCell can issue notifications. The role_nofitications array in the AccessibilityDefinitions.plist file installed at /System/Library/Accessibility/AccessibilityDefinitions.plist still has not been fully validated by Apple and is incomplete.
 
 Apple discourages application developers from inventing new notifications, but there is no guarantee that they will not do so. The list returned by this method does not include any custom notifications implemented by third-party applications. If a client application must know of newer notifications or custom notifications implemented by other applications, it is the client's responsibility to provide them. An example of a user interface for letting users add new notifications is the Notifications pane in the Preferences window for UI Browser, an application from PFiddlesoft.
 
 The PFAssistive framework handles registration to observe notifications in the PFObserver class in PFObserver.h. In general, it is best to register to receive notifications on the root application UI element of the target application, because this guarantees that all notifications will be received from all UI elements affected by a user's activity in the user interface, even as to elements that don't yet exist at registration time. It might be sensible to register for notifications on a particular element if registering on the application element floods the client application with irrelevant notifications.
 
 Use the class method @link //apple_ref/occ/clm/PFUIElement/notifications +notifications@/link to get an array of all notifications recognized in the accessibility API.
 
 The following notifications are supported by the accessibility API (obsolete notifications are omitted):
 <ul>
 <li>AXAnnouncementRequested    - Returns the announcement requested by the application and its priority. (Mavericks)
 <li>AXApplicationActivated		- Returns the application UI element for the activated application.
 <li>AXApplicationDeactivated	- Returns the application UI element for the deactivated application.
 <li>AXApplicationHidden		- Returns the application UI element for the hidden application.
 <li>AXApplicationShown			- Returns the application UI element for the shown application.
 <li>AXCreated					- Sent when a window is shown.
 <li>AXDrawerCreated			- Returns the drawer that was created.
 <li>AXElementBusyChanged       - Returns the UI element whose busy state has changed. (Yosemite)
 <li>AXFocusedUIElementChanged	- Returns the UI element that acquired keyboard focus, or the application UI element if no UI element has focus.
 <li>AXFocusedWindowChanged		- Returns the window that acquired keyboard focus, or the application UI element if there is no focused window.
 <li>AXHelpTagCreated			- Returns the Help tag that was created.
 <li>AXLayoutChanged            - Optionally returns an array of UI elements that have been shown, hidden or changed by user action such as hovering a mouse somewhere. (Mavericks)
 <li>AXMainWindowChanged		- Returns the window that became main, or the application UI element if there is no main window.
 <li>AXMenuClosed				- Returns the menu that closed.
 <li>AXMenuItemSelected			- Returns the menu item that was selected.
 <li>AXMenuOpened				- Returns the  menu that opened.
 <li>AXMoved					- Sent when an element has moved.
 <li>AXResized					- Sent when an element has resized.
 <li>AXRowCollapsed				- Sent when an outline view's row collapses. (Snow Leopard)
 <li>AXRowCountChanged			- Sent when a table's row count changes.
 <li>AXRowExpanded				- Sent when an outline view's row expands. (Snow Leopard)
 <li>ASSelectedCellsChanged		- Send when a cell-based table's cell selection changes. (Snow Leopard)
 <li>AXSelectedChildrenChanged	- Sent when a different subset of an element's children is selected.
 <li>AXSelectedChildrenMoved	- Sent when an element's children move. (Snow Leopard)
 <li>AXSelectedColumnsChanged	- Sent when a different set of a table's columns is selected.
 <li>AXSelectedRowsChanged		- Sent when a different set of a table's rows is selected.
 <li>AXSelectedTextChanged		- Sent when different text is selected.
 <li>AXSheetCreated				- Returns the sheet that was created.
 <li>AXTitleChanged				- Sent when an element's title has changed.
 <li>AXUnitsChanged				- Sent when units of measurement change. (Snow Leopard)
 <li>AXUIElementDestroyed		- Returns the UI element that was destroyed, such as a window that closed. The returned PFUIElement object is not valid except to compare it to another PFUIElement object for equality.
 <li>AXValueChanged				- Returns the UI element whose value was modified.
 <li>AXWindowCreated			- Returns the window that was created.
 <li>AXWindowDeminiaturized		- Returns the window that was maximized from the Dock.
 <li>AXWindowMiniaturized		- Returns the window that was minimized to the Dock.
 <li>AXWindowMoved				- Returns the window that was moved (after it was moved).
 <li>AXWindowResized			- Returns the window that was resized (after it was resized).
 </ul>
*/
- (NSArray *)notifications;

#pragma mark NOTIFICATION UTILITIES
/*!
 @methodgroup NOTIFICATION UTILITIES
*/

/*!
 @method notifications
 @abstract Class method returns the technical, non-localized names of all notifications recognized by the accessibility API in the currently-running version of Mac OS X.
 @result An array of NSString objects.
 @discussion The accessibility API does not include a function to return all available notifications, so the list returned by this method is only as current as the version of Mac OS X existing when this framework was last updated.
 
 Use the instance method @link //apple_ref/occ/instm/PFUIElement/notifications -notifications@/link to get an array of notifications that might be issued by a specific UI element.
*/
+ (NSArray *)notifications;

/*!
 @method descriptionForNotification:
 @abstract Class method returns a localized string describing <code>notification</code>, suitable for display or speech. 
 @param notification An NSString representing a notification; may be a constant from the NSAccessibility informal protocol or from AXNotificationConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result An NSString object.
 @discussion These notification descriptions are unique to the PFAssistive framework; Apple's accessibility API has no equivalent. The key-value pairs are defined in the PFNotificationDescriptions.strings file in the PFAssistive framework bundle.
 
 It may be possible for a target application to return an empty description string. It is up to the client to deal with this, perhaps by supplying a localizable string in a strings file in its bundle or by substituting the technical notification name.
*/
+ (NSString *)descriptionForNotification:(NSString *)notification;

/*!
 @method notificationsForRole:andSubrole:
 @abstract Class method returns an NSArray containing the technical names of all accessibility notifications that should be supported by a UI element with the specified role and subrole.
 @param role An NSString representing a UI element role; may be a constant from the NSAccessibility informal protocol or from AXRoleConstants.h in the accessibility API cast to NSString*, or the string itself.
 @param subrole An NSString representing a UI element subrole, or <code>nil</code> or an empty string if it has no subrole; may be a constant from the NSAccessibility informal protocol or from AXRoleConstants.h in the accessibility API cast to NSString*, or the string itself.
 @result An array of NSString objects, or an empty array if a UI element with the specified role and subrole should not support any notifications.
 @discussion This class method returns an array listing all of the accessibility notifications that a UI element with the specified role and subrole should support. This is general information obtained from the AccessibilityDefinitions.plist file installed at /System/Library/Accessibility/AccessibilityDefinitions.plist starting with Mac OS X 10.7 Lion. It does not guarantee that any particular UI element does in fact support the notifications.

 As of OS X 10.9 Mavericks, the role_nofitications array still has not been fully validated by Apple and is incomplete.
*/
+ (NSArray *)notificationsForRole:(NSString *)role andSubrole:(NSString *)subrole;

/*!
 @method notificationDefinitions
 @abstract Class method returns an NSDictionary object containing information about all accessibility notifications.
 @result A dictionary containing information about all accessibility notifications.
 @discussion This class method returns a dictionary containing the technical names and other features of all accessibility notifications as listed in the AccessibilityDefinitions.plist file installed at /System/Library/Accessibility/AccessibilityDefinitions.plist starting with Mac OS X 10.7 Lion. It reads information from the notifications dictionary in the plist file.
 */
+ (NSDictionary *)notificationDefinitions;

#pragma mark DEBUGGING UTILITIES
/*!
 @methodgroup DEBUGGING UTILITIES
*/

/*!
 @method description
 @abstract Returns a string describing the receiver.
 @result An NSString object.
 @discussion The returned string is formatted as a property list showing the values for the receiver's AXDescription, AXHelp, AXRole, AXRoleDescription and AXSubrole attributes. In client code, call NSLog and pass in the element to see this information in the Xcode run log and debug console log. 
*/
- (NSString *)description;

@end

#pragma mark -
/*!
 @category PFUIElement (Attributes)
 @abstract A category on PFUIElement for attribute properties and methods.
 @discussion The Attributes category on PFUIElement includes two kinds of attributes: nonparameterized attributes and parameterized attributes.
 
 The nonparameterized attributes are declared as properties. To use them, you send a <code>key</code> message to their receivers (where <code>key</code> is the property name, such as <code>AXRole</code> to read the "AXRole" attribute), just as you would use a getter accessor method. They take no parameters, and they return an Objective-C object. To set those that are settable, you send a <code>setKey:</code> message to their receivers passing an Objective-C object (where <code>setKey:</code> incorporates the property name, such as <code>setAXValue:</code> to set the "AXValue" attribute), just as you would use a setter accessor method.
 
 The parameterized attributes are implemented as methods. You send them to a receiver with a parameter, and the value returned depends on the value of the parameter.
 
 The accessor methods specified by the properties in the Attributes category on PFUIElement return UI element attribute values for attributes having the same name as the property. All attributes declared in the AXAttributeConstants.h and NSAccessibility.h headers are included (except where they are marked obsolete). Note that some attributes appear in only one of those headers.
 
 Individual properties are named according to the string value of the attribute, not its constant name in the AXAttributeConstants.h or NSAccessibility.h header. They are classified approximately the same as in AXAttributeConstants.h, which differs in minor ways from the classification in NSAccessibility.h. Classification of attributes is for ease of understanding only and has no functional significance. It can be misleading, because Apple encourages developers to use existing attribute names for custom objects where possible, and they may therefore be used for new purposes.
 
 The properties in this category are mostly for convenience. Instead of using them, you can send @link valueForAttribute: -valueForAttribute:@/link and @link setValue:forAttribute: -setValue:forAttribute: @/link with the appropriate accessibility API constant from AXAttributeConstants.h (cast to NSString*) or NSAccessibility.h. These two methods use key-value coding to find and send one of these properties by name or, if it is not found, to fall back on a private method that calls the accessibility API directly. The net effect is that the framework will always find all attributes, even new attributes that are added to the accessibility API in future versions of Mac OS X and custom attributes that are implemented by other applications such as the Finder and Safari. You must send @link valueForAttribute: -valueForAttribute:@/link and @link setValue:forAttribute: -setValue:forAttribute: @/link instead of a specific property when you don't know in advance which attribute to use, for example, when using a list of attributes obtained from the @link nonParameterizedAttributes -nonParameterizedAttributes @/link method.

 There is a slight efficiency advantage in sending an accessor message instead of using the -valueForAttribute: and -setValue:forAttribute: methods. Either technique will execute any special code, such as bug fixes, that is included in a particular accessor method.
 
 All attribute values are Objective-C objects returned autoreleased.
 
 See @link attributes -attributes @/link for information regarding the use of the attribute properties.
*/

@interface PFUIElement (Attributes)

// The accessor methods specified by these properties return UI element attribute values for attributes having the same name. All attributes in AXAttributeConstants.h and NSAccessibility.h are included (except where we have been advised they are obsolete). Note that some attributes appear in only one of those headers.
// Individual accessor methods are named according to the string value of the attribute, not its constant name in AXAttributeConstants.h or NSAccessibility.h. They are classified and ordered approximately the same as in AXAttributeConstants.h, which differs in minor ways from the classification and ordering in NSAccessibility.h. Classification and ordering of attribute accessors is for ease of understanding only and has no functional significance. It can be misleading, both because the classifications and ordering of attributes in Apple's headers are not always accurate and because Apple encourages developers to use existing constants for custom objects where possible and they may therefore be used for new purposes.
// The accessor methods in this category are mostly for convenience. Instead of using them, you can send -valueForAttribute: and -setValue:forAttribute: with the appropriate accessibility API constant from AXAttributeConstants.h (cast to NSString*) or NSAccessibility.h. The -valueForAttribute: and -setValue:forAttribute: methods use key-value coding to find and send one of these accessor methods or, if it is not found, to fall back on a private method that calls the accessibility API directly. The net effect is that the framework will always find all attributes, even new attributes that are added to the accessibility API in future versions of Mac OS X and custom attributes that are implemented by other applications such as the Finder and Safari. You must send -valueForAttribute: and -setValue:forAttribute: instead of a specific accessor when you don't know in advance which attribute to use, for example, when using a list of attributes obtained from the -nonParameterizedAttributes method.
// There is a slight efficiency advantage in sending an accessor message instead of using the -valueForAttribute: and -setValue:forAttribute: methods. Either technique will execute any special code, such as a bug fix, that is included in a particular accessor method.
// All attribute values are Objective-C objects returned autoreleased.

// NOTE: HeaderDoc documentation comments for the Attributes category have been disabled (by removing the leading exclamation point) as of version 3.3.0 (OS X v10.9 Mavericks) because they are now well documented by Apple.

#pragma mark Non-Parameterized Attributes
// Arranged and categorized to correspond to the organization in AXAttributeConstants.h, modified with information from NSAccessibility.h and Apple documentation.

#pragma mark informational attributes
/*
 @group ATTRIBUTES - informational
*/

/*
 @property AXRole
 @abstract A technical, non-localized string representing the receiver's role.
 @result An NSString object.
 @discussion The <code>AXRole</code> attribute represents the element's role or kind using strings typically beginning with "AX" defined by the accessibility API, such as "AXButton" and "AXWindow", or custom strings defined by an application, such as the Finder's "AXFinderItem" and Safari's "AXWebArea". The string might not be human readable. 
 
 Custom UI elements should be given an existing role if possible, possibly with a new subrole.
 
 The accessibility API also provides localizable, human-readable descriptions of the element, suitable for display or speech, in the @link AXRoleDescription AXRoleDescription @/link attribute and in the more verbose @link AXDescription AXDescription @/link attribute indicating the exact function or purpose of a specific element.
 
 An <code>AXRole</code> attribute is required for all elements, even if the role is "AXUnknown". Some elements supplement the generic role attribute with an AXSubrole attribute indicating one of several subcategories of the basic role. See @link AXSubrole AXSubrole @/link for more information.
 
 See also @link AXHelp AXHelp@/link.
*/
@property(readonly) NSString *AXRole;

/*
 @property AXSubrole
 @abstract A technical, non-localized string representing the receiver's subrole.
 @result An NSString object, or <code>nil</code> if the receiver does not have a subrole attribute.
 @discussion Some elements have an <code>AXSubrole</code> attribute more specifically defining their role. The subrole is a string beginning with "AX" defined by the accessibility API, such as "AXCloseButton" and "AXZoomButton" for the AXButton role and "AXStandardWindow" and "AXDialog" for the AXWindow role, or a custom subrole defined by an application.
 
 When an element has a subrole, its @link AXRoleDescription AXRoleDescription @/link attribute typically reflects the subrole rather than the role.
 
 See also @link AXRole -AXRole@/link, @link AXRoleDescription AXRoleDescription@/link, @link AXDescription AXDescription@/link, and @link AXHelp AXHelp@/link.
*/
@property(readonly) NSString *AXSubrole;

/*
 @property AXRoleDescription
 @abstract A localized string describing the receiver's role or subrole, suitable for display or speech.
 @result An NSString object.
 @discussion An element's <code>AXRoleDescription</code> attribute is used to explain to a user what the element is. It typically echoes the technical @link AXRole -AXRole @/link or @link AXSubrole AXSubrole @/link attribute, but in human-readable form without the "AX" prefix and broken into discrete words. For example, an element whose role is "AXWindow" and whose subrole is "AXDialog" has a role description of "dialog". It is sometimes represented in accessibility documentation in the form AXRole:AXSubrole, such as AXWindow:AXDialog.
 
 See also @link AXRole -AXRole@/link, @link AXSubrole AXSubrole@/link, @link AXDescription AXDescription@/link, and @link AXHelp AXHelp@/link.
*/
@property(readonly) NSString *AXRoleDescription;

/*
 @property AXTitle
 @abstract A localized string that is the receiver's title.
 @result An NSString object, or <code>nil</code> if the receiver does not have a title attribute.
 @discussion Many elements have <code>AXTitle</code> attributes, such as a button. The title is the text that an application displays as part of its normal visual interface, such as "OK" for an OK button or the text of a menu item.
 
 Elements are often associated with a separate UI element in the visual interface that serves as their title, such as a static text field. See @link AXTitleUIElement AXTitleUIElement @/link for more information.
*/
@property(readonly) NSString *AXTitle;

/*
 @property AXDescription
 @abstract A localized string specifically describing the receiver.
 @result An NSString object, or <code>nil</code> if the receiver does not have a description attribute.
 @discussion An element's <code>AXDescription</code> attribute explains its function or purpose. It is slightly more specific than the role description attribute but less verbose than the help attribute. It is typically formed in English by combining a property of the specific application feature that the element represents or controls with its role description. For example, a slider's role description is "slider" while the description of a particular instance of a slider might be "line width slider".
 
 See also @link AXRole -AXRole@/link, @link AXSubrole AXSubrole@/link, @link AXRoleDescription AXRoleDescription@/link, and @link AXHelp AXHelp@/link.
*/
@property(readonly) NSString *AXDescription;

/*
 @property AXHelp
 @abstract A localized string providing helpful information regarding the receiver.
 @result An NSString object, or <code>nil</code> if the receiver does not have a help attribute.
 @discussion An element's <code>AXHelp</code> attribute may be identical to the text of its help tag (tooltip), but it can be different. When different, it should be worded specifically to assist users with disabilities.

 See also @link AXRole -AXRole@/link, @link AXSubrole AXSubrole@/link, @link AXRoleDescription AXRoleDescription@/link, and @link AXDescription AXDescription@/link.
*/
@property(readonly) NSString *AXHelp; // e.g., a tooltip

/*
 @property AXIdentifier
 @abstract A string identifying the receiver across application launches.
 @result An NSString object, or <code>nil</code> if the receiver does not have an identifier attribute.
 @discussion An element's <code>AXIdentifier</code> attribute is a persistent identifier that remains the same for the same UI element over multiple launches of the application. It might not be unique.
 
 Availability: Introduced in Mac OS X v10.7.
*/
@property(readonly) NSString *AXIdentifier;

#pragma mark hierarchy attributes
/*
 @group ATTRIBUTES - hierarchy
*/

/*
 @property AXParent
 @abstract The receiver's parent UI element.
 @result A PFUIElement or PFApplicationUIElement object, or <code>nil</code> if the receiver is the root application element.
 @discussion An element's <code>AXParent</code> attribute represents the element containing it in the visual, on-screen accessibility containment hierarchy. This may not be its immediate container in the application's programmatic view hierarchy, because the view hierarchy often includes unseen objects that are irrelevant to accessibility and therefore ignored by it. For example, a drawer's parent is the window from which the drawer opened, not the unseen programmatic window object that contains the drawer's view. More conventionally, a push button's parent may be a group or a window, a window's parent is the root application element, a menu item's parent is a menu, a menu bar item's parent is the menu bar, and a menu's parent may be a menu bar item or a menu item.
 
 The <code>AXParent</code> and @link AXChildren AXChildren @/link properties are key players in an assistive application's navigation up and down a target application's accessibility containment hierarchy.
 
 This property returns <code>nil</code> if the receiver is the target application's root UI element. Every element in the visual element hierarchy has a parent, except for the root application element.
 
 See the @link AXChildren AXChildren @/link property for requirements regarding the relationship between a parent and its children.
*/
@property(readonly) PFUIElement *AXParent; // nil if receiver's role is "AXApplication"

/*
 @property AXChildren
 @abstract The receiver's child UI elements in the accessibility hierarchy.
 @result An array of PFUIElement objects, or <code>nil</code> if the receiver does not have a children attribute.
 @discussion An element's <code>AXChildren</code> attribute represents all of the elements that it contains in the visual, on-screen element containment hierarchy. For example, a window's children are the elements, such as buttons and text fields, that appear at the top level of the window, and a menu's children are its menu items.
 
 There is a one-to-many relationship between a parent and its children. A child can have only one parent, while a parent can have many children.
 
 The @link AXParent AXParent @/link and <code>AXChildren</code> attributes are key players in an assistive application's navigation up and down a target application's accessibility containment hierarchy.
 
 An <code>AXChildren</code> attribute is required for all elements.
 
 A parent and its children must match in both directions. That is, a child's parent must have the child as one of its children, and a parent's child must have the parent as its only parent. Any failure of the accessibility containment hierarch to match in both directions is the result of an application bug. The bug may cause an element browser (which looks "up" the hierarchy from the "root" application element through its children) or a screen reader (which looks "down" the hierarchy from a "leaf" element on the screen through its parents) to fail. A surprising number of applications exhibit this bug with respect to some of their elements.
 
 See also @link AXParent AXParent@/link.
*/
@property(readonly) NSArray *AXChildren; // array of PFUIElement*

/*
 @property AXSelectedChildren
 @abstract The receiver's selected children UI elements.
 @result An array of PFUIElement objects, which may be empty, or <code>nil</code> if the receiver does not have a selected children attribute.
 @discussion An element's <code>AXSelectedChildren</code> attribute represents the subset of its children that are currently selected in the user interface. For example, it may identify the items that are selected in a table, outline or browser column list, or the Finder items that are selected on the desktop.
 
 To change the selection in the user interface, call <code>-setAXSelectedChildren:</code> passing an NSArray object containing PFUIElement objects that are children of the receiver, or an empty array to clear the current selection.
*/
@property(readwrite, retain) NSArray *AXSelectedChildren; // array of PFUIElement*

/*
 @property AXVisibleChildren
 @abstract The receiver's visible children UI elements.
 @result An array of PFUIElement objects, or <code>nil</code> if the receiver does not have a visible children attribute.
 @discussion An element's <code>AXVisibleChildren</code> attribute represents a subset of its children that are currently visible in their container; for example, the rows of a table or outline view that are scrolled into view.
*/
@property(readonly) NSArray *AXVisibleChildren; // array of PFUIElement*

/*
 @property AXWindow
 @abstract The window UI element containing the receiver.
 @result A PFUIElement object.
 @discussion The <code>AXWindow</code> attribute is a convenient shortcut, providing immediate access to an element's containing window without requiring explicit traversal of the intervening hierarchy.
 
 The <code>AXWindow</code> attribute is similar to @link AXTopLevelUIElement AXTopLevelUIElement @/link except that the element returned can only be a window element, not a sheet or a drawer element.
 
 Returns the window from which a sheet or drawer opens if the receiver is a sheet or a drawer, not the window containing the views in the sheet or drawer.
*/
@property(readonly) PFUIElement *AXWindow;

/*
 @property AXTopLevelUIElement
 @abstract The top-level UI element containing the receiver.
 @result A PFUIElement object.
 @discussion The <code>AXTopLevelUIElement</code> attribute is a convenient shortcut, providing immediate access to an element's top-level container without requiring explicit traversal of the intervening hierarchy.
 
 The <code>AXTopLevelUIElement</code> attribute is similar to @link AXWindow AXWindow@/link, but the element returned can be something other than a window element, such as a sheet or a drawer. The top level UI element of an application dock item is the Dock application's list of dock items.
*/
@property(readonly) PFUIElement *AXTopLevelUIElement;

#pragma mark relationship attribute accessors
/*
 @group ATTRIBUTES - relationship
*/

/*
 @property AXTitleUIElement
 @abstract A separate UI element that serves as the receiver's title.
 @result A PFUIElement object.
 @discussion An element's <code>AXTitleUIElement</code> attribute represents a separate element that is associated with the receiver and serves as its title, such as the static text field in a window's title bar or the static text field to the left of a text field or pop-up button.
 
 The <code>AXTitleUIElement</code> attribute is the inverse of @link AXServesAsTitleForUIElements AXServesAsTitleForUIElements@/link.
*/
@property(readonly) PFUIElement *AXTitleUIElement;

/*
 @property AXServesAsTitleForUIElements
 @abstract The UI elements for which the receiver serves as title.
 @result An array of PFUIelement objects.
 @discussion The <code>AXServesAsTitleForUIElements</code> attribute represents all of the elements for which the receiver serves as title in the user interface.
 
 This attribute is rarely available because to implement it application developers must provide it in code; it cannot be set in Interface Builder. Apple documentation indicates that it is used in AXMenuItem and AXMenuBarItem elements only.
 
 The <code>AXServesAsTitleForUIElements</code> attribute is the inverse of @link AXTitleUIElement -AXTitleUIElement@/link.
*/
@property(readonly) id AXServesAsTitleForUIElements; // array of PFUIElement*

/*
 @property AXLinkedUIElements
 @abstract The UI elements that are linked to the receiver.
 @result An array of PFUIelement objects.
 @discussion An element's <code>AXLinkedUIElements</code> attribute represents elements that are associated with the receiver in the sense that their values depend upon the receiver's current value. For example, the contents of the To, From and Subject text fields and the Message text view of an e-mail message area in the user interface may depend upon which item is currently selected in an associated table listing available e-mail messages.
*/
@property(readonly) NSArray *AXLinkedUIElements; // array of PFUIElement*

/*
 @property AXSharedFocusElements
 @abstract The UI elements that have keyboard focus along with the current element. Introduced in OS X v10.10 Yosemite.
 @result An array of PFUIelement objects.
 @discussion The <code>AXSharedFocusElements</code> attribute returns an array of elements that also have keyboard focus when the current element has keyboard focus. A common usage of this attribute is to report that both a search text field and a list of resulting suggestions share keyboard focus because keyboard events can be handled by either UI element. In this example, the text field would be the first responder and it would report the list of suggestions as an element in the array returned for kAXSharedFocusElementsAttribute.
*/
@property(readonly) NSArray *AXSharedFocusElements; // array of PFUIElement*

#pragma mark visual state attributes
/*
 @group ATTRIBUTES - visual state
*/

/*
 @property AXEnabled
 @abstract Whether the receiver is enabled.
 @result An NSNumber object encoding a BOOL value.
 @discussion An element is enabled if a user can interact with it. Views, the menu bar, menu bar items, menus, and menu items always have an AXEnabled attribute; windows typically do not.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
*/
@property(readonly) NSNumber *AXEnabled; // -boolValue to extract BOOL

/*
 @property AXFocused
 @abstract Whether the receiver has keyboard focus.
 @result An NSNumber object encoding a BOOL value.
 @discussion Multiple elements in a window's accessibility containment hierarchy can have keyboard focus at the same time. To determine whether other elements share focus with the current focused element, use @link AXSharedFocusElements AXSharedFocusElements@/link.
 
 To access the primary element having keyboard focus in a specific application no matter what window it is in, use @link AXFocusedUIElement AXFocusedUIElement@/link. To access the window having keyboard focus in a specific application, use @link AXFocusedWindow AXFocusedWindow@/link. To access the active application system-wide, use @link AXFocusedApplication AXFocusedApplication@/link.
 
 In the case of a window element, the window having keyboard focus need not be the application's main window. To determine if a window is the main window, use @link AXMain AXMain@/link.
 
 Keyboard events sent to an application are "typed" into the element currently having keyboard focus, if it is an editable text view or text field (except for command-down keyboard shortcuts, which are always handled by the application itself).
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
 
 To assign keyboard focus to the receiver, call <code>-setAXFocused:</code> passing an NSNumber object encoding <code>YES</code>. To remove keyboard focus from the receiver, you must set focus to another element; passing an NSNumber object encoding <code>NO</code> to this property does nothing. Many elements cannot have keyboard focus.
*/
@property(readwrite, retain) NSNumber *AXFocused; // -boolValue to extract BOOL

/*
 @property AXPosition
 @abstract The receiver's position on the screen.
 @result An NSValue object encoding an NSPoint structure.
 @discussion An element's screen position in the accessibility API is its top-left corner in global top-left relative screen coordinates, where {0.0, 0.0} is the top-left corner of the screen displaying the menu bar and values increase in magnitude to the right and downwards.
 
 Use NSValue's <code>-pointValue</code> method to extract the NSPoint structure of the result.
 
 To set the receiver's position, call <code>-setAXPosition:</code> passing an NSValue object encoding the desired point in top-left relative screen coordinates. When setting a window's position, the accessibility API does not allow its title bar to be moved into the menu bar or the Dock. Instead, the window is moved to the nearest valid position. Many elements cannot be moved.
*/
@property(readwrite, retain) NSValue *AXPosition; // -pointValue to extract NSPoint; in screen coordinates

/*
 @property AXSize
 @abstract The receiver's size on the screen.
 @result An NSValue object encoding an NSSize structure.
 @discussion An element's size on the screen is its horizontal and vertical dimensions.
 
 Use NSValue's <code>-sizeValue</code> method to extract the NSSize structure of the result.
 
 To set the receiver's position, call <code>-setAXSize:</code> passing an NSValue object encoding the desired size. Many elements cannot be resized.
*/
@property(readwrite, retain) NSValue *AXSize; // -sizeValue to extract NSSize

#pragma mark value attributes
/*
 @group ATTRIBUTES - value
*/

/*
 @property AXValue
 @abstract The receiver's value.
 @result An object of any type, or <code>nil</code> if the receiver does not have a value attribute.
 @discussion An element typically has a value attribute if a user can modify it by interacting with it on the screen. In addition, many elements have values even though they are not modifiable by a user, such as a static text field.
 
 The value can be almost any type, but it is always the same type for any given element. For example, a text field's value is the text it contains, a scroll bar's value is a real number between 0.0 and 1.0 representing the position of its thumb, and a checkbox's value is an integer, either 1 or 0 depending on whether it is checked.
 
 To set the receiver's value, call <code>-setAXValue:</code> passing an object of the appropriate type. Some elements, such as a checkbox, can only be modified by clicking. In those cases, the element supports an AXPress action instead of a settable value attribute.
 
 See also @link AXValueDescription AXValueDescription@/link.
*/
@property(readwrite, retain) id AXValue; // some UI elements are not settable; e.g., checkboxes

/*
 @property AXValueDescription
 @abstract A localized string describing the receiver's value, suitable for display or speech.
 @result An NSString object, or <code>nil</code> if the receiver does not have a value attribute.
 @discussion An element's <code>AXValueDescription</code> attribute is used to explain to a user what the element's value is.
 
 See also @link AXValue AXValue@/link.
*/
@property(readonly) id AXValueDescription;

/*
 @property AXMinValue
 @abstract The smallest legal value of the receiver.
 @result An object of any type (typically NSNumber), or <code>nil</code> if the receiver does not have a minimum value attribute.
 @discussion An element's <code>AXMinValue</code> attribute represents the smallest value it can display. The returned value is an object of the same type as the receiving element's <code>AXValue</code> attribute.
 
 In conjunction with its <code>AXMaxValue</code> attribute, it enables a client to set the element's value within a valid range. The <code>AXMinValue</code> attribute is typically recognized by elements that allow a user to set their values within a range of permissible values, such as sliders. A scroll bar may not recognize this attribute; its actual minimum and maximum values are always 0.0 and 1.0, respectively.
 
 See also @link AXValue AXValue@/link and @link AXMaxValue AXMaxValue@/link.
*/
@property(readonly) id AXMinValue;

/*
 @property AXMaxValue
 @abstract The largest legal value of the receiver.
 @result An object of any type (typically NSNumber), or <code>nil</code> if the receiver does not recognize a maximum value attribute.
 @discussion An element's <code>AXMaxValue</code> attribute is the largest value it can display. The returned value is an object of the same type as the receiving element's <code>AXValue</code> attribute.
 
 In conjunction with its <code>AXMinValue</code> attribute, it enables a client to set the element's value within a valid range. The <code>AXMaxValue</code> attribute is typically recognized by elements that allow a user to set their values within a range of permissible values, such as sliders. A scroll bar may not recognize this attribute; its actual minimum and maximum values are always 0.0 and 1.0, respectively.
 
 See also @link AXValue AXValue@/link and @link AXMinValue AXMinValue@/link.
*/
@property(readonly) id AXMaxValue;

/*
 @property AXValueIncrement
 @abstract The magnitude of each increment of the receiver's value.
 @result An object of any type (typically NSNumber).
 @discussion The <code>AXValueIncrement</code> attribute applies to a stepper and similar elements, returning the amount by which the value manipulated by the stepper or similar element changes with a single click. The returned value is an object of the same type as the receiving element's <code>AXValue</code> attribute.
*/
@property(readonly) id AXValueIncrement;

/*
 @property AXValueWraps
 @abstract Whether the receiver wraps; that is, if stepping in one direction eventually returns the receiver to its initial value.
 @result An NSNumber object encoding a BOOL value.
 @discussion The AXValueWraps attribute applies to a stepper and similar elements, returning whether its value wraps around to its minimum value when the maximum value is exceeded, and vice versa.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
*/
@property(readonly) NSNumber *AXValueWraps; // -boolValue to extract BOOL

/*
 @property AXAllowedValues
 @abstract An array of discrete values that the receiver can assume.
 @result An NSArray object containing objects of any type (typically NSNumber).
 @discussion The <code>AXAllowedValues</code> attribute applies to a slider, level indicator or similar noncontinuous elements, returning an array of discrete values that the receiving element can assume. The objects in the array are of the same type as the element's <code>AXValue</code> attribute.
*/
@property(readonly) NSArray *AXAllowedValues; // array of id

/*
 @property AXPlaceholderValue
 @abstract The string that appears in a text field or similar element as its placeholder.
 @result An NSString object.
 @discussion The <code>AXPlaceholder</code> attribute typically applies to text fields, returning the string that appears in the element as its placeholder.
*/
@property(readonly) NSString *AXPlaceholderValue; // for text fields and other elements that have a placeholder value

#pragma mark text-specific attributes
/*
 @group ATTRIBUTES - text-specific
*/

/*
 @property AXSelectedText
 @abstract The receiver's selected text.
 @result An NSString object, or <code>nil</code> if the receiver does not have a selected text attribute.
 @discussion The <code>AXSelectedText</code> attribute applies to text elements, such as a text view or a text field, returning the portion of the element's text that is currently selected.
 
 To set the receiver's selected text, call <code>-setAXSelectedText:</code> passing an NSString object representing the new text for the selection. Setting the selected text removes the text that is currently selected and inserts text in its place at the insertion point. The insertion point remains unchanged, but the length of the selection may change. To select different text instead of replacing the text that is currently selected, call <code>-setAXSelectedTextRange:</code> or <code>-setAXSelectedTextRanges:</code>.
 
 See also @link AXSelectedTextRange AXSelectedTextRange @/link and @link AXSelectedTextRanges AXSelectedTextRanges@/link.
*/
@property(readwrite, retain) NSString *AXSelectedText;

/*
 @property AXSelectedTextRange
 @abstract The range of the receiver's text selection.
 @result An NSValue object encoding an NSRange structure, or <code>nil</code> if the receiver does not have a selected text range attribute.
 @discussion The <code>AXSelectedTextRange</code> attribute applies to editable text elements, such as a text view or a text field, returning the range of characters of the portion of the receiver's text that is currently selected. It is a range of Unicode characters, not bytes. The insertion point is a range having a length of 0.
 
 Use NSValue's <code>-rangeValue</code> method to extract the NSRange structure of the result.
 
 To set the receiver's selected text range, call <code>-setAXSelectedTextRange:</code> passing an NSValue object encoding the range for a new selection. To replace the currently selected text with different text, instead of selecting different text, call <code>-setAXSelectedText:</code>.
 
 See also @link AXSelectedText AXSelectedText @/link and @link AXSelectedTextRanges AXSelectedTextRanges@/link.
*/
@property(readwrite, retain) NSValue *AXSelectedTextRange; // -rangeValue to extract NSRange

/*
 @property AXSelectedTextRanges
 @abstract The ranges of the receiver's noncontiguous multiple text selections.
 @result An NSArray object containing NSValue objects encoding noncontiguous NSRange structures, or <code>nil</code> if the receiver does not have a selected text range attribute.
 @discussion The <code>AXSelectedTextRange</code> attribute applies to editable text elements, such as a text view or a text field, returning an array of the ranges of characters of the portions of the receiver's text that are currently part of a noncontiguous multiple selection. They are ranges of Unicode characters, not bytes. The insertion point is an array containing a single empty selection range, which has a length of 0.
 
 Use NSValue's <code>-rangeValue</code> method to extract the NSRange structure of any NSValue object in the array.
 
 To set the receiver's selected text ranges, call <code>-setAXSelectedTextRanges:</code> passing an NSArray object containing NSValue objects encoding the ranges for a new multiple selection.
 
 See also @link AXSelectedText AXSelectedText @/link and @link AXSelectedTextRange AXSelectedTextRange@/link.
*/
@property(readwrite, retain) NSArray *AXSelectedTextRanges; // array of NSValue; -rangeValue to extract NSRange

/*
 @property AXVisibleCharacterRange
 @abstract The range of the receiver's visible text.
 @result An NSValue object encoding an NSRange structure, or <code>nil</code> if the receiver does not have a visible text range attribute.
 @discussion The <code>AXVisibleCharacterRange</code> attribute typically applies to text views, not text fields, returning the range of characters of the portion of the receiver's text that is currently scrolled into view. It includes whole lines even if they are clipped horizontally. It is a range of Unicode characters, not a range of bytes.
 
 Use NSValue's <code>-rangeValue</code> method to extract the NSRange structure of the result.
 
 To set the receiver's visible character range, call <code>-setAXVisibleCharacterRange:</code> passing an NSValue object encoding the range for a new range of characters. This has the effect of scrolling the characters into view.
*/
@property(readwrite, retain) NSValue *AXVisibleCharacterRange; // -rangeValue to extract NSRange

/*
 @property AXNumberOfCharacters
 @abstract The number of characters in the receiver's text.
 @result An NSNumber object encoding an integer value, or <code>nil</code> if the receiver does not have a number of characters attribute.
 @discussion The <code>AXNumberOfCharacters</code> attribute applies to text elements, such as a text view or a text field. It returns the number of Unicode characters in the receiver's text, not the number of bytes.
 
 Use NSNumber's <code>-intValue</code> method or similar to extract the integer value of the result.
*/
@property(readonly) NSNumber *AXNumberOfCharacters; // -intValue or similar to extract integer

/*
 @property AXSharedTextUIElements
 @abstract An array containing all of the text view elements that share a larger text object, a portion of which is contained in the receiving text view.
 @result An array of PFUIelement objects.
 @discussion The <code>AXSharedTextUIElements</code> attribute typically applies to text views, not text fields. It returns all of the text views that share the larger text object, part of which is contained in the receiving text view, such as multiple text columns across which the text object flows.
*/
@property(readonly) NSArray *AXSharedTextUIElements; // array of PFUIElement*

/*
 @property AXSharedCharacterRange
 @abstract The range of characters in a larger text object that are contained in the receiving text view, which shares the text object with other text view elements.
 @result An NSValue object encoding an NSRange structure.
 @discussion The <code>AXSharedCharacterRange</code> attribute typically applies to text views, not text fields. It returns the range of characters in a larger text object that are contained in the receiving text view, which shares the text object with other text view elements, such as the range of characters in one of multiple text columns that share a single text object. It returns the range of Unicode characters in the receiver's text, not the range of bytes.
 
 Use NSValue's <code>-rangeValue</code> method to extract the NSRange structure of the result.
*/
@property(readonly) NSValue *AXSharedCharacterRange; // -rangeValue to extract NSRange

/*
 @property AXInsertionPointLineNumber
 @abstract The line index of a naked insertion point in the receiving text object.
 @result An NSNumber object encoding an integer value, or <code>nil</code> if text is currently selected in the receiver.
 @discussion Use the <code>AXInsertionPointLineNumber</code> attribute to determine whether a naked insertion point is at the end of one wrapped line or at the beginning of the next wrapped line of a word-wrapped line of text.
 
 This attribute returns a line number only if no text is currently selected. It returns <code>nil</code> if text is currently selected in the receiver.
 
 To determine the location of the insertion point as a character index, get the starting character index of the current selection range using @link AXSelectedTextRange AXSelectedTextRange@/link. The <code>location</code> field of the structure it encodes holds a character index representing the insertion point whether or not text is currently selected. If the length of the current selection is greater than 0, you can determine the line index of the insertion point (that is, the line on which the first character of the selection is located) without ambiguity by calling @link AXLineForIndex: -AXLineForIndex:@/link. The line index can be important, for example, when you want to scroll to the insertion point.
 
 To determine the line index of the insertion point when the length of the current selection is 0, two steps are required. This is because the character index is the same when there is no text selection and the insertion point is at the end of one wrapped line or the beginning of the next wrapped line (there is no return character separating wrapped lines). First, pass the character index of the current selection range to @link AXLineForIndex: -AXLineForIndex:@/link, as above, and note the result. Second, to distinguish between a naked insertion point that is located at the end of one wrapped line and one that is located at the beginning of the next wrapped line even though both have the same character index, use this property, <code>AXInsertionPointLineNumber</code>. If the result is <code>nil</code>, you know that the length of the current selection is greater than 0 and the line index returned by @link AXLineForIndex: -AXLineForIndex:@/link was therefore correct.
 
 Use NSNumber's <code>-intValue</code> method or similar to extract the integer value of the result.
*/
@property(readonly) NSNumber *AXInsertionPointLineNumber; // -intValue or similar to extract integer

/*
 @property AXTextInputMarkedRange
 @abstract The marked range of a text input session from an input manager.
 @result An NSValue object encoding an NSRange structure.
 @discussion The <code>AXTextInputMarkedRange</code> attribute applies to editable text elements, such as a text view or a text field.
 
 Use NSValue's <code>-rangeValue</code> method to extract the NSRange structure of the result.
 
 The NSAccessibilityTextInputMarkedRangeAttribute constant is private Apple API. Use at your own risk.
*/
@property(readonly) NSValue *AXTextInputMarkedRange; // -rangeValue to extract NSRange

#pragma mark window, sheet, or drawer-specific attributes
/*
 @group ATTRIBUTES - window, sheet, or drawer-specific
*/

/*
 @property AXMain
 @abstract Whether the receiver is the target application's main window.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXMain</code> attribute applies to a window object, returning whether it is the main window of the application.
 
 An application may have a single main window whether it is the active application or not. If the application is inactive, its main window is the window that would be its main window if it were active.
 
 The main window need not be the window having keyboard focus. To determine if it is the key window, use @link AXFocused AXFocused@/link.
 
 To get the application's main window element, use @link AXMainWindow AXMainWindow@/link. To get its key window, the window that has keyboard focus, use @link AXFocusedWindow AXFocusedWindow@/link.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
 
 To set the receiver to be the target application's main window, call <code>-setAXMain:</code> passing an NSNumber object encoding <code>YES</code>. Whether making a window the target application's main window brings it in front of other windows belonging to the application is application-dependent. To make another window the main window, you must send the <code>-setAXMain:</code> message to it; passing an NSNumber object encoding <code>NO</code> to this property does nothing.
*/
@property(readwrite, retain) NSNumber *AXMain; // -boolValue to extract BOOL

/*
 @property AXMinimized
 @abstract Whether the receiving window is minimized to the Dock.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXMinimized</code> attribute applies to a window object, returning whether it is currently minimized to the Dock.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
 
 To minimize a window, call <code>-setAXMinimized:</code> passing an NSNumber object encoding <code>YES</code>; to maximize it, an NSNumber object encoding <code>NO</code>.
*/
@property(readwrite, retain) NSNumber *AXMinimized; // -boolValue to extract BOOL

/*
 @property AXFullScreen
 @abstract Whether the receiving window is full screen.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXFullScreen</code> attribute applies to a window object, returning whether it is currently full screen.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
 
 To make a window full screen, call <code>-setFullScreen:</code> passing an NSNumber object encoding <code>YES</code>; to return it to its normal mode, an NSNumber object encoding <code>NO</code>.
 
 The NSAccessibilityFullScreenAttribute constant is private Apple API. Use at your own risk.
 
 Availability: Introduced in Mac OS X v10.7.
*/
@property(readwrite, retain) NSNumber *AXFullScreen; // -boolValue to extract BOOL

/*
 @property AXCloseButton
 @abstract The close button in the receiving window's title bar.
 @result A PFUIElement object with a role of <code>AXButton</code> and a subrole of <code>AXCloseButton</code>, or <code>nil</code> if the receiving window does not have a close button.
 @discussion The <code>AXCloseButton</code> attribute applies to a window object. It is a used to access a window's close button.
*/
@property(readonly) PFUIElement *AXCloseButton;

/*
 @property AXZoomButton
 @abstract The zoom button in the receiving window's title bar.
 @result A PFUIElement object, or <code>nil</code> if the receiving window does not have a zoom button.
 @discussion The <code>AXZoomButton</code> attribute applies to a window object. It is used access a window's zoom button.
*/
@property(readonly) PFUIElement *AXZoomButton;

/*
 @property AXMinimizeButton
 @abstract The minimize button in the receiving window's title bar.
 @result A PFUIElement object, or <code>nil</code> if the receiving window does not have a minimize button.
 @discussion The <code>AXMinimizeButton</code> attribute applies to a window object. It is used to access a window's minimize button.
*/
@property(readonly) PFUIElement *AXMinimizeButton;

/*
 @property AXFullScreenButton
 @abstract The full screen button in the receiving window's title bar.
 @result A PFUIElement object, or <code>nil</code> if the receiving window does not have a full screen button.
 @discussion The <code>AXFullScreenButton</code> attribute applies to a window object. It is a convenience method to make it easy to access a window's full screen button.
 
 Availability: Introduced in Mac OS X v10.7.
*/
@property(readonly) PFUIElement *AXFullScreenButton;

/*
 @property AXToolbarButton
 @abstract The toolbar button in the receiving window's title bar.
 @result A PFUIElement object, or <code>nil</code> if the receiving window does not have a toolbar button.
 @discussion The <code>AXToolbarButton</code> attribute applies to a window object. It is a convenience method to make it easy to access a window's toolbar button.
*/
@property(readonly) PFUIElement *AXToolbarButton;

/*
 @property AXProxy
 @abstract The proxy icon in the receiving window's title bar representing the window's document.
 @result A PFUIElement object, or <code>nil</code> if the receiving window does not have a proxy icon (that is, if the receiver's document has never been saved or it does not have a document).
 @discussion The <code>AXProxy</code> attribute applies to a window object. It is a convenience method to make it easy to access a window's proxy icon.
*/
@property(readonly) PFUIElement *AXProxy; // document proxy

/*
 @property AXGrowArea
 @abstract The grow area in the bottom-right corner of the receiving window.
 @result A PFUIElement object, or <code>nil</code> if the receiving window does not have a grow area.
 @discussion The <code>AXGrowArea</code> attribute applies to a window object. It is a convenience method to make it easy to access a window's grow area element.
*/
@property(readonly) PFUIElement *AXGrowArea;

/*
 @property AXModal
 @abstract Whether the receiver is a modal window.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXModal</code> attribute applies to a window object, returning <code>YES</code> if the window is modal.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
*/
@property(readonly) NSNumber *AXModal; // -boolValue to extract BOOL

/*
 @property AXDefaultButton
 @abstract The receiving window's default button.
 @result A PFUIElement object, or <code>nil</code> if the receiving window does not have a default button.
 @discussion The <code>AXDefaultButton</code> attribute applies to a window object. It is a convenience method to make it easy to access a window's default button.
*/
@property(readonly) PFUIElement *AXDefaultButton;

/*
 @property AXCancelButton
 @abstract The receiving window's or sheet's cancel button.
 @result A PFUIElement object with a role of AXButton, or <code>nil</code> if the receiving window or sheet does not have a cancel button.
 @discussion The <code>AXCancelButton</code> attribute applies to a window or sheet object, returning its cancel button.
*/
@property(readonly) PFUIElement *AXCancelButton;

#pragma mark menu or menu item-specific attributes
/*
 @group ATTRIBUTES - menu or menu item-specific
*/

/*
 @property AXMenuItemCmdChar
 @abstract The receiving menu item's keyboard shortcut as a single-character string.
 @result An NSString object, an empty string if the receiving menu item does not have a keyboard shortcut that is represented by a character.
 @discussion The <code>AXMenuItemCmdChar</code> attribute applies to a menu item, returning its keyboard shortcut as a character if it is implemented as a key represented by a character such as "X". 
*/
@property(readonly) NSString *AXMenuItemCmdChar;

/*
 @property AXMenuItemCmdVirtualKey
 @abstract The receiving menu item's keyboard shortcut as a virtual key code.
 @result An NSNumber object encoding an integer value, or <code>nil</code> if the receiving menu item does not have a keyboard shortcut that is represented by a virtual key code.
 @discussion The <code>AXMenuItemCmdVirtualKey</code> attribute applies to a menu item, returning its keyboard shortcut as a virtual key code if it is implemented as a key represented by a virtual key code.
 
 Virtual key codes returned by this method are the keycodes described in Inside Macintosh Volume V. They are the same keycodes returned by NSEvent's -keyCode method. They are hardware-independent numbers representing specific keys on the keyboard. They are generally used to represent keyboard shortcuts only when there is more than one key that returns the same character or glyph code.
 
 Use NSNumber's <code>-intValue</code> method to extract the integer value of the result.
*/
@property(readonly) NSNumber *AXMenuItemCmdVirtualKey; // -intValue or similar to extract integer

/*
 @property AXMenuItemCmdGlyph
 @abstract The receiving menu item's keyboard shortcut as a glyph code.
 @result An NSNumber object encoding an integer value, or <code>nil</code> if the receiving menu item does not have a keyboard shortcut that is represented by a glyph code.
 @discussion The <code>AXMenuItemCmdGlyph</code> attribute applies to a menu item, returning its keyboard shortcut as a glyph code, if it is implemented as a key represented by a glyph code.
 
 Glyph codes returned by this method are described in the Carbon Menu Manager documentation and declared in Menus.h in the Carbon HIToolbox framework. They are numbers representing all of the control and function keys on the keyboard. For example, the glyph code for the left arrow key is <code>kMenuLeftArrowGlyph</code>, 0x64 or 100, and for the right arrow key, <code>kMenuRightArrowGlyph</code>, 0x65 or 101.
 
 Use NSNumber's <code>-intValue</code> method to extract the integer value of the result.
*/
@property(readonly) NSNumber *AXMenuItemCmdGlyph; // -intValue or similar to extract integer

/*
 @property AXMenuItemCmdModifiers
 @abstract The modifier key combination required by the receiving menu item's keyboard shortcut.
 @result An NSNumber object encoding an integer value, or <code>nil</code> if the receiving menu item does not have a keyboard shortcut.
 @discussion The <code>AXMenuItemCmdModifiers</code> attribute applies to a menu item, returning the modifier key combination required by its keyboard shortcut.
 
 The modifier key combination is the Modifier Key Mask described in the Carbon Menu Manager documentation and declared in Menus.h in the Carbon HIToolbox framework. It is a number representing a particular combination of modifier keys. The number is based on the combination of bits in a 4-bit bit field where bit 0 is the Shift key, <code>kMenuShiftModifie</code>; bit 1 is the Option key, <code>kMenuOptionModifier</code>; bit 2 is the Control key, <code>kMenuControlModifier</code>; and bit 3 means no modifier, not even the Command key, is not required, as in many keyboard shortcuts in Aperture, <code>kMenuNoCommandModifier</code>. The Command key is assumed unless bit 3 is set, and it is not included in the mask. Thus, for example, the number 0 means that only the Command key is required; 1, Command-Shift; 2, Command-Option; 3, Command-Shift-Option; 4, Command-Control; and so on.
 
 In OS X 10.9 Mavericks, the following mask constants were added to simplify interpretation of the return value: <code>kAXMenuItemModifierNone</code>, <code>kAXMenuItemModifierShift</code>, <code>kAXMenuItemModifierOption</code>, <code>kAXMenuItemModifierControl</code>, <code>kAXMenuItemModifierNoCommand</code>. The <code>kAXMenuItemModifierNone</code> mask means no modifiers other than the command key (which is used by default). The <code>kAXMenuItemModifierNoCommand</code> mask means no modifiers at all, not even the command key
 
 Use NSNumber's <code>-intValue</code> method to extract the integer value of the result.
*/
@property(readonly) NSNumber *AXMenuItemCmdModifiers; // -intValue or similar to extract integer

/*
 @property AXMenuItemMarkChar
 @abstract The receiving menu item's mark character.
 @result An NSString object, or an empty string if the mark character is not currently set because the menu is closed or the receiving menu item does not have a mark character.
 @discussion The <code>AXMenuItemMarkChar</code> attribute applies to a menu item, returning its mark character, such as a check mark.
 
 Many applications set a menu item's mark character only as the menu is being opened. For this reason, it may not be possible to obtain the mark character except by first opening the menu. The PFAssistive framework does not do this automatically, but a client can do it by sending @link activateApplication -activateApplication@/link and then sending @link performAction: -performAction:@/link to perform an AXPress action on the menu to open it.
*/
@property(readonly) NSString *AXMenuItemMarkChar;

/*
 @property AXMenuItemPrimaryUIElement
 @abstract The receiving menu item's primary menu item element.
 @result A PFUIElement object, or <code>nil</code> if the menu item does not have a secondary menu item element.
 @discussion The <code>AXMenuItemPrimaryUIElement</code> attribute applies to a menu item, returning its primary menu item element. Some menu items have both a primary menu item element and a secondary menu item element that appears when the Option key is held down.
*/
@property(readonly) PFUIElement *AXMenuItemPrimaryUIElement;

/*
 @property AXShownMenuUIElement
 @abstract The menu element currently being displayed.
 @result A PFUIElement object.
 @discussion The <code>AXShownMenuUIElement</code> attribute returns the open menu associated with the receiver, or <code>nil</code> if no menu is open for on the receiver. For example, the Finder's Finder item element and the Dock's Dock item element recognize the <code>AXShownMenuUIElement</code> attribute.
*/
@property(readonly) PFUIElement *AXShownMenuUIElement;

#pragma mark application element-specific attributes
/*
 @group	ATTRIBUTES - application element-specific
*/

/*
 @property AXMenuBar
 @abstract The receiving application's menu bar element.
 @result A PFUIElement object.
 @discussion The <code>AXMenuBar</code> attribute applies to the application object, returning its one and only menu bar.
 
 There are typically two menu bars visible on the screen at a time, the active application's menu bar and the systemUIServer application's menu bar. The latter is located in the top-right corner of the same screen that displays the active application's menu bar. Its children are all of the system's menu extras, such as the clock menu extra. (This does not include a variety of third-party buttons that may appear to the left of the SystemUIServer's menu bar.)
*/
@property(readonly) PFUIElement *AXMenuBar;

/*
 @property AXWindows
 @abstract The receiving application's open window elements.
 @result An array of PFUIElement objects.
 @discussion The <code>AXWindows</code> attribute applies to the application object, listing all of its windows.
*/
@property(readonly) NSArray *AXWindows; // array of PFUIElement*

/*
 @property AXFrontmost
 @abstract Whether the receiving application is frontmost.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXFrontmost</code> attribute applies to the application object, indicating whether it is currently the active (frontmost) application.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
 
 To bring an application to the front, call <code>-setAXFrontmost:</code> passing an NSNumber object encoding <code>YES</code>. To bring another application to the front, you must send the <code>-setAXMain:</code> message to it; passing an NSNumber object encoding <code>NO</code> to this property does nothing.
*/
@property(readwrite, retain) NSNumber *AXFrontmost; // -boolValue to extract BOOL

/*
 @property AXHidden
 @abstract Whether the receiving application is hidden.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXHidden</code> attribute applies to the application object, indicating whether the application is currently hidden.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
 
 To hide an application or other hideable UI element, call <code>-setAXHidden:</code> passing an NSNumber object encoding <code>YES</code>; to unhide it, an NSNumber object encoding <code>NO</code>. Some UI elements respond specially to this attribute; for example, in Mac OS X 10.7 Lion, a scroller can be unhidden temporarily, but it cannot be forced to hide by passing YES.
*/
@property(readwrite, retain) NSNumber *AXHidden; // -boolValue to extract BOOL

/*
 @property AXMainWindow
 @abstract The receiving application's main window.
 @result A PFUIElement object.
 @discussion The <code>AXMainWindow</code> attribute applies to the application object, returning its main window.
 
 See @link AXMain AXMain@/link for more information.
*/
@property(readonly) PFUIElement *AXMainWindow;

/*
 @property AXFocusedWindow
 @abstract The receiving application's key window, the window that has keyboard focus.
 @result A PFUIElement object.
 @discussion The <code>AXFocusedWindow</code> attribute applies to the application object, returning its key window.
 
 See @link AXFocused AXFocused@/link for more information.
*/
@property(readonly) PFUIElement *AXFocusedWindow;

/*
 @property AXFocusedUIElement
 @abstract The receiving application's focused UI element.
 @result A PFUIElement object.
 @discussion The <code>AXFocusedUIElement</code> attribute applies to the application object, returning the element that currently has keyboard focus.
 
 See @link AXFocused AXFocused@/link for more information.
*/
@property(readonly) PFUIElement *AXFocusedUIElement;

/*
 @property AXExtrasMenuBar
 @abstract The receiving application's extras menu bar element.
 @result A PFUIElement object.
 @discussion The <code>AXExtrasMenuBar</code> attribute applies to the application object, returning its extras menu bar.
 
 Availability: Introduced in OS X v10.8.
*/
@property(readonly) PFUIElement *AXExtrasMenuBar;

/*
 @property AXEnhancedUserInterface
 @abstract Whether the receiving application is using enhanced user interface behaviors.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXEnhancedUserInterface</code> attribute applies to the application object, indicating whether it is using advanced user interface behaviors.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
 
 To cause an application to use enhanced user interface behaviors, call <code>-setAXEnhancedUserInterface:</code> passing an NSNumber object encoding <code>YES</code>; to return to normal behaviors, an NSNumber object encoding <code>NO</code>.
 
 The NSAccessibilityEnhancedUserInterfaceAttribute constant is private Apple API. Use at your own risk. VoiceOver sets this attribute to indicate that it wants subtle changes made to the behavior of an application's user interface that would otherwise interfere with VoiceOver. The effect of setting this attribute is subject to change, and the attribute may disappear in future versions of Mac OS X. Currently, it has to do with keyboard navigation in text fields and tab views. The value of this attribute does not necessarily correlate with VoiceOver being active or inactive.
*/
@property(readwrite, retain) NSNumber *AXEnhancedUserInterface; // -boolValue to extract BOOL

#pragma mark date/time-specific attributes
/*
 @group ATTRIBUTES - date/time-specific
*/

/*
 @property AXHourField
 @abstract The hour field in the receiving clock control.
 @result A PFUIElement object.
 @discussion The <code>AXHourField</code> attribute applies to a clock control element, returning its hour field.
*/
@property(readonly) PFUIElement *AXHourField;

/*
 @property AXMinuteField
 @abstract The minute field in the receiving clock control.
 @result A PFUIElement object.
 @discussion The <code>AXMinuteField</code> attribute applies to a clock control element, returning its minute field.
*/
@property(readonly) PFUIElement *AXMinuteField;

/*
 @property AXSecondField
 @abstract The second field in the receiving clock control.
 @result A PFUIElement object.
 @discussion The <code>AXSecondField</code> attribute applies to a clock control element, returning its second field.
*/
@property(readonly) PFUIElement *AXSecondField;

/*
 @property AXAMPMField
 @abstract The AM/PM field in the receiving <code>AXTimeField</code> element.
 @result A PFUIElement object with a role of <code>AXTextField</code>.
 @discussion The <code>AXAMPMField</code> attribute applies to an <code>AXTimeField</code> element, such as a clock control, returning the <code>AXTextField</code> element used to edit the AM/PM setting.
 
 An <code>AXAMPMField</code> attribute is required for all <code>AXTimeField</code>elements.

*/
@property(readonly) PFUIElement *AXAMPMField;

/*
 @property AXDayField
 @abstract The day field in the receiving <code>AXTimeField</code> element.
 @result A PFUIElement object with a role of <code>AXTextField</code>.
 @discussion The <code>AXDayField</code> attribute applies to an <code>AXTimeField</code> element, such as a clock control, returning the <code>AXTextField</code> element used to edit the day-of-week setting.
 
 An <code>AXDayField</code> attribute is required for all <code>AXTimeField</code>elements.
*/
@property(readonly) PFUIElement *AXDayField;

/*
 @property AXMonthField
 @abstract The month field in the receiving clock control.
 @result A PFUIElement object.
 @discussion The <code>AXMonthField</code> attribute applies to a clock control element, returning its month field.
*/
@property(readonly) PFUIElement *AXMonthField;

/*
 @property AXYearField
 @abstract The year field in the receiving clock control.
 @result A PFUIElement object.
 @discussion The <code>AXYearField</code> attribute applies to a clock control element, returning its year field.
*/
@property(readonly) PFUIElement *AXYearField;

#pragma mark table, outline, or browser-specific attribute accessors
/*
 @group ATTRIBUTES - table, outline, or browser-specific
*/

/*
 @property AXRows
 @abstract The rows of the receiving table or outline.
 @result An array of PFUIElement objects.
 @discussion The <code>AXRows</code> attribute applies to a  table or outline, returning its row elements.
*/
@property(readonly) NSArray *AXRows; // array of PFUIElement*

/*
 @property AXVisibleRows
 @abstract The visible rows of the receiving table or outline.
 @result An array of PFUIElement objects.
 @discussion The <code>AXVisibleRows</code> attribute applies to a table or outline, returning the subset of its row elements that are scrolled into view.
*/
@property(readonly) NSArray *AXVisibleRows; // array of PFUIElement*

/*
 @property AXSelectedRows
 @abstract The selected rows of the receiving table or outline.
 @result An array of PFUIElement objects.
 @discussion The <code>AXSelectedRows</code> attribute applies to a table or outline, returning the subset of its row elements that are currently selected.

 To change the selection in the user interface, call <code>-setAXSelectedRows:</code> passing an NSArray object containing PFUIElement objects that are rows of the receiver, or an empty array to clear the current selection.
*/
@property(readwrite, retain) NSArray *AXSelectedRows; // array of PFUIElement*

/*
 @property AXColumns
 @abstract The columns of the receiving table, outline or browser.
 @result An array of PFUIElement objects, with a role of AXColumn for tables and outlines and <code>AXScrollArea</code> for browsers.
 @discussion The <code>AXColumns</code> attribute applies to a table, outline or browser, returning an array of <code>AXColumn</code> elements in the case of a table or outline and <code>AXScrollArea</code> in the case of a browser.

 The <code>AXColumns</code> attribute is required for all outline, table and browser elements.
*/
@property(readonly) NSArray *AXColumns; // array of PFUIElement*

/*
 @property AXVisibleColumns
 @abstract The visible columns of the receiving table or outline.
 @result An array of PFUIElement objects.
 @discussion The <code>AXVisibleColumns</code> attribute applies to a table or outline, returning the subset of its column elements that are scrolled into view.
*/
@property(readonly) NSArray *AXVisibleColumns; // array of PFUIElement*

/*
 @property AXSelectedColumns
 @abstract The selected columns of the receiving table or outline.
 @result An array of PFUIElement objects.
 @discussion The <code>AXSelectedColumns</code> attribute applies to a table or outline, returning the subset of its column objects that are currently selected.
 
 Prior to Mac OS X 10.6 Snow Leopard, this attribute incorrectly returned the selected column indexes.

 To change the selection in the user interface, call <code>-setAXSelectedColumns:</code> passing an NSArray object containing PFUIElement objects that are columns of the receiver, or an empty array to clear the current selection.
*/
@property(readwrite, retain) NSArray *AXSelectedColumns; // array of PFUIElement*

/*
 @property AXSortDirection
 @abstract A technical, non-localized string designating the receiving column's sort direction.
 @result An NSString object.
 @discussion The <code>AXSortDirection</code> attribute applies to elements having a sort direction, such as column headers in tables and outlines.
 
 The result can be compared to the string constants <code>NSAccessibilityAscendingSortDirectionValue</code> ("AXAscendingSortDirection"), <code>NSAccessibilityDescendingSortDirectionValue</code> ("AXDescendingSortDirection"), and <code>NSAccessibilityUnknownSortDirectionValue</code> ("AXUnknownSortDirection").
 
 To set the sort direction, call <code>-setAXSortDirection:</code> passing the NSString object <code>NSAccessibilityAscendingSortDirectionValue</code> ("AXAscendingSortDirection"), <code>NSAccessibilityDescendingSortDirectionValue</code> ("AXDescendingSortDirection"), or <code>NSAccessibilityUnknownSortDirectionValue</code> ("AXUnknownSortDirection").
*/
@property(readwrite, retain) NSString *AXSortDirection;

/*
 @property AXIndex
 @abstract The index of the receiver.
 @result An NSNumber object encoding an integer value.
 @discussion The <code>AXIndex</code> attribute returns an element's index, such as the index of a table or outline column. The index is zero-based.
 
 Use NSNumber's <code>-intValue</code> method to extract the integer value of the result.
*/
@property(readonly) NSNumber *AXIndex; // -intValue or similar to extract integer

/*
 @property AXColumnTitles
 @abstract The receiving browser's column title elements.
 @result An array of PFUIElement objects.
 @discussion The <code>AXColumnTitles</code> attribute applies to a browser, returning its column titles. A column title is a static text item. Each of a browser's column titles is also a child of the browser.
 
 This attribute has two accessibility constants, kAXColumnTitlesAttribute, which is the correct constant, and kAXColumnTitleAttribute, a misspelling introduced in Mac OS X 10.4 Tiger and never deprecated.
*/
@property(readonly) NSArray *AXColumnTitles; // array of PFUIElement*

/*
 @property AXHeader
 @abstract The receiving table or outline column's header element.
 @result A PFUIElement object.
 @discussion The <code>AXHeader</code> attribute applies to a column in a table or outline, returning its header. A header may be a static text item, a sort button element representing the column's title, or another object. All of a table or outline column's headers are also collected in a group element that is a child of the table or outline.
*/
@property(readonly) PFUIElement *AXHeader;

#pragma mark level indicator attributes
/*
 @group ATTRIBUTES - level indicator
*/

/*
 @property AXWarningValue
 @abstract The receiving level indicator's warning value.
 @result An object of any value, typically an NSNumber.
 @discussion The <code>AXWarningValue</code> attribute applies to a level indicator, returning its warning value.
*/
@property(readonly) id AXWarningValue; // typically NSNumber

/*
 @property AXCriticalValue
 @abstract The receiving level indicator's critical value.
 @result An object of any value, typically an NSNumber.
 @discussion The <code>AXCriticalValue</code> attribute applies to a level indicator displaying capacity, returning its critical value, such as a disk becoming too full or a battery charge too low. This is similar to its warning value, but more serious.
 
 The <code>AXCriticalValue</code> attribute is required for level indicator elements displaying capacity.
*/
@property(readonly) id AXCriticalValue; // typically NSNumber

#pragma mark cell-based table attributes
/*
 @group ATTRIBUTES - cell-based tables
*/

/*
 @property AXSelectedCells
 @abstract The receiving cell-based table's selected cell UI elements.
 @result An array of PFUIElement objects, which may be empty, or <code>nil</code> if the receiver does not have a selected cells attribute..
 @discussion An element's <code>AXSelectedCells</code> attribute represents the subset of its cells that are currently selected in the user interface. For example, it may identify the cells that are selected in a table in Apple's Numbers application.
 
 To change the selection in the user interface, call <code>-setAXSelectedCells:</code> passing an NSArray object containing PFUIElement objects that are cells of the receiver, or an empty array to clear the current selection.
*/
@property(readwrite, retain) NSArray *AXSelectedCells; // array of PFUIElement*

/*
 @property AXVisibleCells
 @abstract The receiving cell-based table's visible cell UI elements.
 @result An array of PFUIElement objects, which may be empty, or <code>nil</code> if the receiver does not have a selected cells attribute..
 @discussion An element's <code>AXVisibleCells</code> attribute represents a subset of its cells that are currently visible in their container; for example, the cells of a table in Apple's Numbers application that are scrolled into view. This attribute is required for cell-based tables, but optional for view-based tables in Mac OS X 10.7 Lion and newer.
*/
@property(readwrite, retain) NSArray *AXVisibleCells; // array of PFUIElement*

/*
 @property AXRowHeaderUIElements
 @abstract The receiver's row header UI elements.
 @result An array of PFUIElement objects, which may be <code>nil</code> if the receiver does not have a row header UI elements attribute.
 @discussion An element's <code>AXRowHeaderUIElements</code> attribute represents its row header UI elements.
*/
@property(readonly) NSArray *AXRowHeaderUIElements; // array of PFUIElement*

/*
 @property AXColumnHeaderUIElements
 @abstract The receiving table or outline object's column header UI elements.
 @result An array of PFUIElement objects, which may be <code>nil</code> if the receiver does not have a column header UI elements attribute.
 @discussion An element's <code>AXColumnHeaderUIElements</code> attribute represents its column header UI elements ordered from left to right.
*/
@property(readonly) NSArray *AXColumnHeaderUIElements; // array of PFUIElement*

#pragma mark cell attributes
/*
 @group ATTRIBUTES - cells
*/

/*
 @property AXRowIndexRange
 @abstract The range of the receiver's row indexes.
 @result An NSValue object encoding an NSRange structure, or <code>nil</code> if the receiver does not have a selected row index range attribute.
 @discussion The <code>AXRowIndexRange</code> attribute applies to tables, returning the range of row indexes in the table.
 
 Use NSValue's <code>-rangeValue</code> method to extract the NSRange structure of the result.

 See also @link AXColumnIndexRange AXColumnIndexRange@/link.
*/
@property(readonly) NSValue *AXRowIndexRange; // -rangeValue to extract NSRange

/*
 @property AXColumnIndexRange
 @abstract The range of the receiver's column indexes.
 @result An NSValue object encoding an NSRange structure, or <code>nil</code> if the receiver does not have a selected column index range attribute.
 @discussion The <code>AXColumnIndexRange</code> attribute applies to table and outline cells. The <code>location</code> of the range is the cell's index in the columns of the table or outline, and the <code>length</code> of the range is the number of columns the cell spans.
 
 The <code>AXColumnIndexRange</code> attribute is required for all outline and table cell elements.
 
 Use NSValue's <code>-rangeValue</code> method to extract the NSRange structure of the result.

 See also @link AXRowIndexRange AXRowIndexRange@/link.
*/
@property(readonly) NSValue *AXColumnIndexRange; // -rangeValue to extract NSRange

#pragma mark layout area attributes
/*
 @group ATTRIBUTES - layout areas
*/

/*
 @property AXHorizontalUnits
 @abstract A technical, non-localized string representing the receiver's horizontal units of measurement.
 @result An NSString object.
 @discussion The <code>AXHorizontalUnits</code> attribute represents the element's horizontal units of measurement using strings beginning with "AX" defined by the accessibility API, such as "AXInchesUnit" and "AXPointsUnit".
 
 The accessibility API also provides localizable, human-readable descriptions of the element, suitable for display or speech, in the @link AXHorizontalUnitDescription AXHorizontalUnitDescription @/link attribute.

 See also @link AXUnits AXUnits@/link.
*/
@property(readonly) NSString *AXHorizontalUnits; // for ruler unit values (below)

/*
 @property AXVerticalUnits
 @abstract A technical, non-localized string representing the receiver's vertical units of measurement.
 @result An NSString object.
 @discussion The <code>AXVerticalUnits</code> attribute represents the element's vertical units of measurement using strings beginning with "AX" defined by the accessibility API, such as "AXInchesUnit" and "AXPointsUnit".
 
 The accessibility API also provides localizable, human-readable descriptions of the element, suitable for display or speech, in the @link AXVerticalUnitDescription AXVerticalUnitDescription @/link attribute.

 See also @link AXUnits AXUnits@/link.
*/
@property(readonly) NSString *AXVerticalUnits; // for ruler unit values (below)

/*
 @property AXHorizontalUnitDescription
 @abstract A localized string describing the receiver's horizontal units of measurement, suitable for display or speech.
 @result An NSString object.
 @discussion An element's <code>AXHorizontalUnitDescription</code> attribute is used to explain to a user what the receiver's horizontal units of measurement are. It typically echoes the technical @link AXHorizontalUnits AXHorizontalUnits@/link, but in human-readable form without the "AX" prefix and broken into discrete words. For example, an element whose horizontal units are "AXInches" has a role description of "inches".

 See also @link AXUnitDescription AXUnitDescription@/link.
*/
@property(readonly) NSString *AXHorizontalUnitDescription; // for ruler unit values (below)

/*
 @property AXVerticalUnitDescription
 @abstract A localized string describing the receiver's vertical units of measurement, suitable for display or speech.
 @result An NSString object.
 @discussion An element's <code>AXVerticalUnitDescription</code> attribute is used to explain to a user what the receiver's vertical units of measurement are. It typically echoes the technical @link AXVerticalUnits AXVerticalUnits@/link, but in human-readable form without the "AX" prefix and broken into discrete words. For example, an element whose vertical units are "AXInches" has a role description of "inches".

 See also @link AXUnitDescription AXUnitDescription@/link.
*/
@property(readonly) NSString *AXVerticalUnitDescription; // for ruler unit values (below)

#pragma mark layout item attributes
/*
 @group ATTRIBUTES - item
*/

/*
 @property AXHandles
 @abstract The receiver's handle UI elements.
 @result An array of PFUIElement objects, or <code>nil</code> if the receiver does not have a handles attribute.
 @discussion An element's <code>AXHandles</code> attribute represents all of the resize handles adorning a resizable UI element, such as a layout area item in Apple's Numbers application.
*/
@property(readonly) NSArray *AXHandles; // array of PFUIElement*

#pragma mark outline-specific attributes
/*
 @group ATTRIBUTES - outline-specific
*/

/*
 @property AXDisclosing
 @abstract Whether the receiving outline view row is expanded.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXDisclosing</code> attribute applies to an outline row, indicating whether it is a parent row that is expanded to disclose its child rows.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
 
 To expand the receiver, call <code>-setAXDisclosing:</code> passing an NSNumber object encoding <code>YES</code>; to collapse it, an NSNumber object encoding <code>NO</code>.
*/
@property(readwrite, retain) NSNumber *AXDisclosing; // -boolValue to extract BOOL

/*
 @property AXDisclosedRows
 @abstract The expanded child rows of the receiving parent outline view row.
 @result An array of PFUIElement objects.
 @discussion The <code>AXDisclosedRows</code> attribute applies to an outline row, returning its disclosed child rows.
*/
@property(readonly) NSArray *AXDisclosedRows; // array of PFUIElement*

/*
 @property AXDisclosedByRow
 @abstract The receiving outline child row's parent row.
 @result A PFUIElement object.
 @discussion The <code>AXDisclosedByRow</code> attribute applies to an outline row, returning the parent row disclosing it.
*/
@property(readonly) PFUIElement *AXDisclosedByRow;

/*
 @property AXDisclosureLevel
 @abstract The indentation level of the receiving outline row.
 @result An NSNumber object encoding an integer value.
 @discussion The <code>AXDisclosureLevel</code> attribute applies to an outline row, returning the indentation level of its row object. The disclosure level is zero-based.
 
 Use NSNumber's <code>-intValue</code> method to extract the integer value of the result.
*/
@property(readonly) NSNumber *AXDisclosureLevel; // -intValue or similar to extract integer

#pragma mark slider-specific attributes
/*
 @group ATTRIBUTES - slider-specific
*/

/*
 @property AXLabelUIElements
 @abstract The labels for a slider or similar control.
 @result An NSArray of PFUIElement objects.
 @discussion The <code>AXLabelUIElements</code> attribute applies to a slider or similar control, returning its label elements.
*/
@property(readonly) NSArray *AXLabelUIElements; // array of PFUIElement*

/*
 @property AXLabelValue
 @abstract The value of the receiving label.
 @result An NSNumber object encoding an integer or other numeric value.
 @discussion The <code>AXLabelValue</code> attribute applies to a slider or similar control, returning the value of the receiving label.
 
 Use NSNumber's <code>-intValue</code> method or similar to extract the integer value of the result.
*/
@property(readonly) NSNumber *AXLabelValue; // -intValue or similar to extract integer

#pragma mark screen matte-specific attributes
/*
 @group ATTRIBUTES - screen matte-specific
*/

/*
 @property AXMatteHole
 @abstract The bounds of the receiving screen matte's hole in screen coordinates. DEPRECATED in OS X 10.10 Yosemite.
 @result An NSValue object encoding an NSRect structure.
 @discussion This method is deprecated in OS X 10.10 Yosemite and newer because Apple has deprecated the constant on which it relies, <code>kAXMatteHoleAttribute</code> in AXAttributeConstants.h. The <code>AXMatteHole</code> attribute applies to a screen matte, returning the rectangle bounding the screen matte's hole.
 
 Use NSValue's <code>-rectValue</code> method to extract the NSRect structure of the result.
*/
@property(readonly) NSValue *AXMatteHole; // -rectValue to extract NSRect

/*
 @property AXMatteContentUIElement
 @abstract The object clipped by the receiving screen matte. DEPRECATED in OS X 10.10 Yosemite.
 @result A PFUIElement object.
 @discussion This method is deprecated in OS X 10.10 Yosemite and newer because Apple has deprecated the constant on which it relies, <code>kAXMatteContentUIElementAttribute</code> in AXAttributeConstants.h. The <code>AXMatteContentUIElement</code> attribute applies to a screen matte, returning the element clipped by the screen matte.
*/
@property(readonly) PFUIElement *AXMatteContentUIElement;

#pragma mark ruler-specific attributes
/*
 @group ATTRIBUTES - ruler-specific
*/

/*
 @property AXMarkerUIElements
 @abstract The receiving text ruler's marker elements.
 @result An NSArray of PFUIElement objects.
 @discussion The <code>AXMarkerUIElements</code> attribute applies to a text ruler element, returning its marker elements, such as a left tab stop marker or the head indent marker.
*/
@property(readonly) NSArray *AXMarkerUIElements; // array of PFUIElement*

/*
 @property AXMarkerValues
 @abstract The values of the receiving text ruler's marker elements.
 @result An NSArray of NSString.
 @discussion The <code>AXMarkerValues</code> attribute applies to a text ruler element. This attribute is not documented. The values of markers in the rulers in Apple's TextEdit and Pages applications are available using their <code>AXValue</code> attribute.
*/
@property(readonly) NSArray *AXMarkerValues; // array of NSString*

/*
 @property AXMarkerGroupUIElement
 @abstract A group element containing the receiving text ruler's markers.
 @result A PFUIElement object.
 @discussion The <code>AXMarkerGroupUIElement</code> attribute applies to a text ruler element. It is not documented. The rulers in Apple's TextEdit and Pages applications do not use this attribute.
*/
@property(readonly) PFUIElement *AXMarkerGroupUIElement;

/*
 @property AXUnits
 @abstract A technical, non-localized string representing the receiving text ruler's units of measurement.
 @result An NSString object.
 @discussion The <code>AXUnits</code> attribute applies to a text ruler element, returning a string describing its units of measurement.
 
 The result can be compared to the string constants <code>NSAccessibilityInchesUnitValue</code> ("AXInchesUnit"), <code>NSAccessibilityCentimetersUnitValue</code> ("AXCentimentersUnit"), <code>NSAccessibilityPointsUnitValue</code> ("AXPointsUnit"), <code>NSAccessibilityPicasUnitValue</code> ("AXPicasUnit"), and <code>NSAccessibilityUnknownUnitValue</code> ("AXUnknownUnit").
 
 See also @link AXUnitDescription AXUnitDescription@/link.
*/
@property(readonly) NSString *AXUnits; // AXInchesUnit, AXCentimetersUnit, AXPointsUnit, AXPicasUnit, or AXUnknownUnit

/*
 @property AXUnitDescription
 @abstract A localized string describing the receiving text ruler's units of measurement, suitable for display or speech.
 @result An NSString object.
 @discussion The <code>AXUnitDescription</code> attribute applies to a text ruler element, returning a localized string describing its units of measurement.
 
 See also @link AXUnits AXUnits@/link.
*/
@property(readonly) NSString *AXUnitDescription;

/*
 @property AXMarkerType
 @abstract A technical, non-localized string designating the receiving text ruler marker's type.
 @result An NSString object.
 @discussion The <code>AXMarkerType</code> attribute applies to a text ruler marker, such as a left tab stop marker or the head indent marker, returning a string describing its type.
 
 The result can be compared to the string constants <code>NSAccessibilityLeftTabStopMarkerTypeValue</code> ("AXLeftTabStopMarkerType"), <code>NSAccessibilityRightTabStopMarkerTypeValue</code> ("AXRightTabStopMarkerType"), <code>NSAccessibilityCenterTabStopMarkerTypeValue</code> ("AXCenterTabStopMarkerType"), <code>NSAccessibilityDecimalTabStopMarkerTypeValue</code> ("AXDecimalTabStopMarkerType"), <code>NSAccessibilityHeadIndentMarkerTypeValue</code> ("AXHeadIndentMarkerType"), <code>NSAccessibilityTailIndentMarkerTypeValue</code> ("AXTailIndentMarkerType"), <code>NSAccessibilityFirstLineIndentMarkerTypeValue</code> ("AXFirstLineIndentMarkerType"), <code>NSAccessibilityUnknownMarkerTypeValue</code> ("AXUnknownMarkerType").
 
 See also @link AXMarkerTypeDescription AXMarkerTypeDescription@/link.
*/
@property(readonly) NSString *AXMarkerType; // AXLeftTabStopMarkerType, AXRightTabStopMarkerType, AXCenterTabStopMarkerType, AXDecimalTabStopMarkerType, AXHeadIndentMarkerType, AXTailIndentMarkerType, AXFirstLineIndentMarkerType, or AXUnknownMarkerType

/*
 @property AXMarkerTypeDescription
 @abstract A localized string describing the receiving text ruler marker, suitable for display or speech.
 @result An NSString object.
 @discussion The <code>AXMarkerTypeDescription</code> attribute applies to a text ruler marker, such as a left tab stop marker or the head indent marker, returning a localized string describing its type.
*/
@property(readonly) NSString *AXMarkerTypeDescription;

#pragma mark search field-specific attributes
/*
 @group ATTRIBUTES - search field-specific
*/

/*
 @property AXSearchButton
 @abstract The search button element in the receiving search field.
 @result A PFUIElement object.
 @discussion The <code>AXSearchButton</code> attribute applies to a search field, returning its search button.
*/
@property(readonly) PFUIElement *AXSearchButton;

/*
 @property AXSearchMenu
 @abstract The search pop-up menu element in the receiving search field.
 @result A PFUIElement object.
 @discussion The <code>AXSearchMenu</code> attribute applies to a search field, returning its search menu, or <code>nil</code> if the receiving search field does not have a search menu.
*/
@property(readonly) PFUIElement *AXSearchMenu;

/*
 @property AXClearButton
 @abstract The clear button element in the receiving search field.
 @result A PFUIElement object with a role of <code>AXButton</code>, or <code>nil</code> if the receiving search field does not have a clear button.
 @discussion The <code>AXClearButton</code> attribute applies to a search field, returning its clear button.
 
 An <code>AXClearButton</code> attribute is required for all <code>AXSearchField</code> elements.

*/
@property(readonly) PFUIElement *AXClearButton;

#pragma mark stepper-specific attributes
/*
 @group ATTRIBUTES - stepper-specific
*/

/*
 @property AXIncrementor
 @abstract A stepper UI element.
 @result A PFUIElement object.
 @discussion The <code>AXIncrementor</code> attribute returns a stepper, such as the stepper in a clock element.
*/
@property(readonly) PFUIElement *AXIncrementor;

/*
 @property AXDecrementButton
 @abstract The receiving stepper's decrement button.
 @result A PFUIElement object with a role of <code>AXButton</code> and a subrole of <code>AXDecrementArrow</code>.
 @discussion The <code>AXDecrementButton</code> attribute applies to a stepper, returning its up arrow button.
 
 See also @link AXIncrementButton AXIncrementButton@/link.
*/
@property(readonly) PFUIElement *AXDecrementButton;

/*
 @property AXIncrementButton
 @abstract The receiving stepper's increment button.
 @discussion The <code>AXIncrementButton</code> attribute applies to a stepper, returning its down arrow button.
 @result A PFUIElement object.
 
 See also @link AXDecrementButton AXDecrementButton@/link.
*/
@property(readonly) PFUIElement *AXIncrementButton;

#pragma mark miscellaneous or role-specific attributes
/*
 @group ATTRIBUTES - miscellaneous or role-specific
*/

/*
 @property AXHorizontalScrollBar
 @abstract The receiving scroller's horizontal scroll bar element.
 @result A PFUIElement object.
 @discussion The <code>AXHorizontalScrollBar</code> attribute returns the horizontal scroll bar of the receiving scroll view.
 
 See also @link AXVerticalScrollBar AXVerticalScrollBar@/link.
*/
@property(readonly) PFUIElement *AXHorizontalScrollBar;

/*
 @property AXVerticalScrollBar
 @abstract The receiving scroller's vertical scroll bar element.
 @discussion The <code>AXVerticalScrollBar</code> attribute returns the vertical scroll bar of the receiving scroll view.
 @result A PFUIElement object.
 
 See also @link AXHorizontalScrollBar AXHorizontalScrollBar@/link.
*/
@property(readonly) PFUIElement *AXVerticalScrollBar;

/*
 @property AXOrientation
 @abstract A technical, non-localized string designating the receiver's orientation on the screen.
 @result An NSString object.
 @discussion The <code>AXOrientation</code> attribute applies to elements having a vertical or horizontal orientation, such as scroll bars and split views.
 
 The result can be compared to the string constants <code>NSAccessibilityHorizontalOrientationValue</code> ("AXVerticalOrientation"), <code>NSAccessibilityVerticalOrientationValue</code> ("AXHorizontalOrientation", and <code>NSAccessibiliityUnknownOrientationValue</code> ("AXUnknownOrientation". NSAccessibilityUnknownOrientationValue was added to the NSAccessibility protocol in Mac OS X 10.6 Snow Leopard to match the existing kAXUnknownOrientationValue constant. In OS X v10.10 Yosemite and newer
 
*/
@property(readonly) NSString *AXOrientation; // AXHorizontalOrientation, AXVerticalOrientation, or AXUnknownOrientation

/*
 @property AXEdited
 @abstract Whether the receiver has been edited since it was last saved.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXEdited</code> attribute typically applies to a document, indicating whether it is dirty. An element is edited if a user has made changes to it that have not yet been saved.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
*/
@property(readonly) NSNumber *AXEdited; // -boolValue to extract BOOL

/*
 @property AXTabs
 @abstract The receiving tab group's tab elements.
 @result An array of PFUIElement objects.
 @discussion The <code>AXTabs</code> attribute applies to a tab group element, returning its tabs. The role of the tabs is <code>AXRadioButton</code>.
*/
@property(readonly) NSArray *AXTabs; // array of PFUIElement*

/*
 @property AXOverflowButton
 @abstract The receiving toolbar's overflow button element.
 @result A PFUIElement object.
 @discussion The <code>AXOverflowButton</code> attribute applies to a toolbar, returning its overflow button.
*/
@property(readonly) PFUIElement *AXOverflowButton;

/*
 @property AXFilename
 @abstract The receiver's file name.
 @result An NSString object, or <code>nil</code> if the receiver does not have a filename attribute.
 @discussion The <code>AXFilename</code> attribute returns the file name associated with the receiver. For example, the Finder's finder item element recognizes the AXFilename attribute.
 
 See also @link AXDocument AXDocument @/link and @link AXURL AXURL@/link.
*/
@property(readonly) NSString *AXFilename;

/*
 @property AXExpanded
 @abstract Whether the receiving UI element is expanded.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXExpanded</code> attribute applies to expandable UI elements such as disclosure triangles, pop-up menus and combo boxes, indicating whether they are expanded or collapsed.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
 
 To expand the receiver, call <code>-setAXExpanded:</code> passing an NSNumber object encoding <code>YES</code>; to collapse it, an NSNumber object encoding <code>NO</code>.
*/
@property(readwrite, retain) NSNumber *AXExpanded; // -boolValue to extract BOOL

/*
 @property AXSelected
 @abstract Whether the receiver is selected.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXSelected</code> attribute indicates whether a user-selectable element is selected in the user interface.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
 
 To select the receiver, call <code>-setAXSelected:</code> passing an NSNumber object encoding <code>YES</code>; to deselect it, an NSNumber object encoding <code>NO</code>.
*/
@property(readwrite, retain) NSNumber *AXSelected; // -boolValue to extract BOOL

/*
 @property AXSplitters
 @abstract The splitter bar elements in the receiving split group.
 @result An array of PFUIElement objects.
 @discussion The splitter bar elements are also children of the split group.
*/
@property(readonly) NSArray *AXSplitters; // array of PFUIElement*

/*
 @property AXContents
 @abstract The main contents of the receiver.
 @result An array of PFUIElement objects.
 @discussion The returned UI elements are the main informational elements contained by the receiver, omitting children that represent functional elements such as scrollbars. For example, the contents of a scroll area may be a text entry area or a table but would omit the scroll area's scroll bar or ruler, and the contents of a tab group would omit the tabs (radio buttons) themselves.
 
 The <code>AXContents</code> attribute is required for certain elements that have elements containing substantive information as well as functional elements such as scroll areas and tab groups.
 
 See also @link AXNextContents AXNextContents @/link and @link AXPreviousContents AXPreviousContents@/link.
*/
@property(readonly) NSArray *AXContents; // array of PFUIElement*; main elements, e.g., scrollable elements of a scroll area, tab view without the tabs

/*
 @property AXNextContents
 @abstract The contents of the subview following the splitter bar in a split group or a similar divider.
 @result An array of PFUIElement objects.
 @discussion The <code>AXNextContents</code> attribute returns all of the elements contained in the subview following a divider element, such as a splitter bar in a split group.
*/
@property(readonly) id AXNextContents; // array of PFUIElement*; main elements

/*
 @property AXPreviousContents
 @abstract The contents of the subview preceding the splitter bar in a split group or a similar divider.
 @result An array of PFUIElement objects.
 @discussion The <code>AXPreviousContents</code> attribute returns all of the elements contained in the subview preceding a divider element, such as a splitter bar in a split group.
*/
@property(readonly) id AXPreviousContents; // array of PFUIElement*; main elements

/*
 @property AXDocument
 @abstract The receiver's path as a file URL.
 @result An NSString object, or <code>nil</code> if the receiving window has not been saved or does not have a document.
 @discussion The <code>AXDocument</code> attribute applies to an element that displays a file, such as a window element, returning its file path as a file URL.
 
 See also @link AXURL AXURL @/link and @link AXFilename AXFilename@/link.
*/
@property(readonly) NSString *AXDocument; // URL of document as string

/*
 @property AXURL
 @abstract The receiver's associated file URL.
 @result An NSURL object representing the file URL associated with the receiver, or <code>nil</code> for elements that don't have a file URL such as a volume.
 @discussion The <code>AXURL</code> attribute returns the file URL associated with the receiver. For example, the Finder's finder item element recognizes the AXURL attribute.
 
 See also @link AXDocument AXDocument @/link and @link AXFilename AXFilename@/link>.
*/
@property(readonly) NSURL *AXURL; // array of PFUIElement*

/*
 @property AXContainsProtectedContent
 @abstract Whether the receiver contains protected content. Introduced in OS X v10.9 Mavericks.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXContainsProtectedContent</code> attribute indicates whether the receiver contains protected, or confidential, content. If it does, its value will be reported as <code>nil</code>.
*/
@property(readonly) NSNumber *AXContainsProtectedContent; // -boolValue to extract BOOL

/*
 @property AXAlternateUIVisible
 @abstract Whether the current element has triggered some alternate UI to appear. Introduced in OS X v10.10 Yosemite.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXAlternateUIVisible</code> attribute indicates whether the current element has triggered some alternate UI to appear.
 
 To indicate that the receiving alternate UI element is visible, call <code>-setAXAlternateUIVisible:</code> passing an NSNumber object encoding <code>YES</code>.
*/
@property(readwrite, retain) NSNumber *AXAlternateUIVisible; // -boolValue to extract BOOL

/*
 @property AXElementBusy
 @abstract Whether the current element is busy. Introduced in OS X v10.10 Yosemite.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXElementBusy</code> attribute indicates whether the current element is busy, for example, in the process of updating or loading and new information will be available when the busy state completes.
 
 To indicate that the receiving element is busy or not busy, call <code>-setAXElementBusy:</code> passing an NSNumber object encoding <code>YES</code> or <code>NO</code>. Any change in an element's busy state should trigger the <code>AXElementBusyChanged</code> notification.
*/
@property(readwrite, retain) NSNumber *AXElementBusy; // -boolValue to extract BOOL

#pragma mark Dock attributes
/*
 @group ATTRIBUTES - Dock
*/

/*
 @property AXIsApplicationRunning
 @abstract Whether the application represented by the receiving application dock item is running.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXIsApplicationRunning</code> attribute indicates whether the application represented by the receiving application dock item is running.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
*/
@property(readonly) NSNumber *AXIsApplicationRunning; // -boolValue to extract BOOL

#pragma mark system-wide attributes
/*
 @group ATTRIBUTES - system-wide
*/

/*
 @property AXFocusedApplication
 @abstract The receiving system-wide UI element's focused application element. 
 @result A PFApplicationUIElement object.
 @discussion The <code>AXFocusedApplication</code> attribute identifies the application element having keyboard focus; that is, the application having a UI element that is frontmost among all active applications.
 
 See @link AXFocused AXFocused @/link for more information.
*/
@property(readonly) PFApplicationUIElement *AXFocusedApplication;

#pragma mark grid attributes
/*
 @group ATTRIBUTES - grid
*/

/*
 @property AXRowCount
 @abstract The number of rows in the receiving grid element.
 @result An NSNumber object encoding an integer value, or <code>nil</code> if the receiver does not have a row count attribute.
 @discussion The <code>AXRowCount</code> attribute applies to grids, returning the number of rows in the table.
 
 Use NSNumber's <code>-intValue</code> method or similar to extract the integer value of the result.
 
 See also @link AXColumnCount AXColumnCount@/link.
*/
@property(readonly) NSNumber *AXRowCount; // -intValue or similar to extract integer

/*
 @property AXColumnCount
 @abstract The number of columns in the receiving grid element.
 @result An NSNumber object encoding an integer value, or <code>nil</code> if the receiver does not have a column count attribute.
 @discussion The <code>AXColumnCount</code> attribute applies to grids, returning the number of columns in the table.
 
 Use NSNumber's <code>-intValue</code> method or similar to extract the integer value of the result.
 
 A <code>AXColumnCount</code> attribute is required for all <code>AXGrid</code> elements.
 
 See also @link AXRowCount AXRowCount@/link.
*/
@property(readonly) NSNumber *AXColumnCount; // -intValue or similar to extract integer

/*
 @property AXOrderedByRow
 @abstract Whether the receiving grid element is ordered by rows.
 @result An NSNumber object encoding a BOOL value.
 @discussion The <code>AXOrderedByRow</code> attribute indicates whether the receiving grid element is ordered by rows.
 
 Use NSNumber's <code>-boolValue</code> method to extract the BOOL value of the result.
*/
@property(readonly) NSNumber *AXOrderedByRow; // -boolValue to extract BOOL

#pragma mark Parameterized Attributes

#pragma mark text suite parameterized attributes
/*
 @group PARAMETERIZED ATTRIBUTES - text suite
*/

/*
 @method AXLineForIndex:
 @abstract Returns the line index of the line containing the character at <code>characterIndex</code> in the receiving text object.
 @param characterIndex An NSNumber encoding an integer value that is the index of a character in the receiving text object, counting all characters from the beginning of the text object even if it flows across multiple text views.
 @result An NSNumber object encoding an integer value.
 @discussion This is a parameterized attribute method to read text objects. Given a character index in the receiving text object, it returns the index of the line of text in which the character appears.
 
 This is a common way to use the parameterized text methods: start with a character index, typically obtained by reading the screen to obtain a reference to the glyph under the mouse pointer. See @link AXRangeForPosition: @/link for information on obtaining the character range of the glyph at a specified screen position. A character's index is its sequential index, counting all characters from the beginning of the text object even if it flows across multiple text containers or views such as pages or columns.
 
 It can be important in some circumstances to avoid using character indexes that fall in the middle of a glyph. See @link AXRangeForIndex: -AXRangeForIndex:@/link for information about obtaining the starting character index and length of any glyph.
 
 The result returned by this method can be ambiguous when you are attempting to determine the line index of a naked insertion point at <code>characterIndex</code>. See @link AXInsertionPointLineNumber -AXInsertionPointLineNumber@/link for information and a technique to resolve the ambiguity.
 
 See @link AXRangeForPosition: -AXRangeForPosition:@/link for information about obtaining the character index and length of a glyph on the screen for use in a screen reader application.
 
 Use NSNumber's <code>-intValue</code> method to extract the integer value of the result.
*/
- (NSNumber *)AXLineForIndex:(NSNumber *)characterIndex; // -intValue or similar to extract integer

/*
 @method AXRangeForLine:
 @abstract Returns the range of characters contained in the line of text at <code>lineIndex</code> in the receiving text object.
 @param lineIndex An NSNumber encoding an integer value that is the index of a line, counting all lines from the beginning of the text object even if it flows aross multiple text views.
 @result An NSValue object encoding an NSRange structure.
 @discussion This is a parameterized attribute method to read text objects. Given a line index in the receiving text object, it returns the range of characters contained in the line of text.
 
 A screen reader typically obtains the line index by first obtaining the character index under the mouse pointer and then calling @link AXLineForIndex: AXLineForIndex:@/link.
   
 Use NSValue's <code>-rangeValue</code> method to extract the NSRange structure of the result.
*/
- (NSValue *)AXRangeForLine:(NSNumber *)lineIndex; // -rangeValue to extract NSRange

/*
 @method AXStringForRange:
 @abstract Returns the text of the characters in <code>characterRange</code> in the receiving text object, as a string.
 @param characterRange An NSValue object encoding an NSRange structure describing a range of characters in the receiving text object, counting all characters from the beginning of the text object even if it flows across multiple text views.
 @result An NSString object.
 @discussion This is a parameterized attribute method to read text objects. Given a character range in the receiving text object, it returns the characters in that range as an NSString object.
 
 The <code>characterRange</code> parameter can be any range of characters in a text object. To obtain a character range for a specific line in a text object, see @link AXRangeForLine: AXRangeForLine:@/link. To obtain a character range for a style run in a text object, see @link AXStyleRangeForIndex: AXStyleRangeForIndex:@/link.
 
 See also @link AXRTFForRange: AXRTFForRange: @/link and @link AXAttributedStringForRange: -AXAttributedStringForRange:@/link.
*/
- (NSString *)AXStringForRange:(NSValue *)characterRange;

/*
 @method AXRangeForPosition:
 @abstract Returns the range of characters in the receiving text object composing the single glyph located at <code>glyphPosition</code>, a point on the screen in top-left relative screen coordinates.
 @param glyphPosition An NSValue object encoding an NSPoint structure describing a position on the screen in top-left relative screen coordinates that is within a single glyph.
 @result An NSValue object encoding an NSRange structure.
 @discussion This method is based on the premise that a glyph may be composed of one or more characters. It can be important to avoid using character indexes that fall in the middle of a glyph.
 
 Use this method to obtain the character range of a single glyph based on its position on the screen, for example, the character range of the glyph under the mouse. See @link elementAtPoint:withDelegate:error: -elementAtPoint:withDelegate:error: @/link for information about obtaining the current position of the mouse. In a screen reader application, use the resulting character range returned by this method as a route into the other parameterized methods which take a character index, character range, or (indirectly from the character index) line index as input. The location field of the character range is the character index of the first character of the glyph, and the length field of the character range is the number of characters to the next glyph.
 
 Use NSValue's <code>-rangeValue</code> method to extract the NSRange structure of the result.
*/
- (NSValue *)AXRangeForPosition:(NSValue *)glyphPosition; // -rangeValue to extract NSRange

/*
 @method AXRangeForIndex:
 @abstract Returns the range of characters in the receiving text object composing the single glyph that contains <code>characterIndex</code>.
 @param characterIndex An NSNumber encoding an integer value that is the index of a character in the receiving text object within a single glyph, counting all characters from the beginning of the text object even if it flows across multiple text views.
 @result An NSValue object encoding an NSRange structure.
 @discussion This is a parameterized attribute method to read text objects. Given a character index in the receiving text object, it returns the range of characters in the receiving text object composing the glyph.
 
 This method is based on the premise that a glyph may be composed of one or more characters.
 
 Use this method to obtain the character range of a single glyph based on its position within the receiving text view.
 
 Use NSValue's <code>-rangeValue</code> method to extract the NSRange structure of the result.
*/
- (NSValue *)AXRangeForIndex:(NSNumber *)characterIndex; // -rangeValue to extract NSRange

/*
 @method AXBoundsForRange:
 @abstract Returns the bounds on the screen enclosing the characters in <code>characterRange</code> in the receiving text object in screen coordinates.
 @param characterRange An NSValue object encoding an NSRange structure describing a range of characters in the receiving text object, counting all characters from the beginning of the text object even if it flows across multiple text views.
 @result An NSValue object encoding an NSRect struct.
 @discussion This is a parameterized attribute method to read text objects. Given a character range in the receiving text object, it returns the bounds on the screen enclosing the characters.
 
 The <code>characterRange</code> parameter can be any range of characters in a text object. To obtain a character range for a specific line in a text object, see @link AXRangeForLine: AXRangeForLine:@/link. To obtain a character range for a style run in a text object, see @link AXStyleRangeForIndex: AXStyleRangeForIndex:@/link.
 
 In the case of a range of characters that encompasses more than one line, the returned bounds encloses all of the characters in the range, in all lines.
 
 Use NSValue's <code>-rectValue</code> method to extract the NSRect structure of the result.
*/
- (NSValue *)AXBoundsForRange:(NSValue *)characterRange; // -rectValue to extract NSRect

/*
 @method AXRTFForRange:
 @abstract Returns the text of the characters in <code>characterRange</code> in the receiving text object, as RTF data.
 @param characterRange An NSValue object encoding an NSRange structure describing a range of characters in the receiving text object, counting all characters from the beginning of the text object even if it flows across multiple text views.
 @result An NSData object containing the RTF data of the receiving text object in <code>characterRange</code>.
 @discussion This is a parameterized attribute method to read text objects. Given a character range in the receiving text object, it returns the characters in that range as an NSData object, including formatting.
 
 The <code>characterRange</code> parameter can be any range of characters in a text object. To obtain a character range for a specific line in a text object, see @link AXRangeForLine: AXRangeForLine:@/link. To obtain a character range for a style run in a text object, see @link AXStyleRangeForIndex: AXStyleRangeForIndex:@/link.
 
 See also @link AXStringForRange: AXStringForRange: @/link and @link AXAttributedStringForRange: -AXAttributedStringForRange:@/link.
*/
- (NSData *)AXRTFForRange:(NSValue *)characterRange; // AXAscendingSortDirection, AXDescendingSortDirection, or AXUnknownSortDirection
/*
 @method AXAttributedStringForRange:
 @abstract Returns the text of the characters in <code>characterRange</code> in the receiving text object, as an accessibility attributed string.
 @param characterRange An NSValue object encoding an NSRange structure describing a range of characters in the receiving text object, counting all characters from the beginning of the text object even if it flows across multiple text views.
 @result An NSAttributedString object containing the attributed string of the receiving text object in <code>characterRange</code>.
 @discussion This is a parameterized attribute method to read text objects. Given a character range in the receiving text object, it returns the characters in that range as an NSAttributedString object, including formatting.
 
 The text attributes contained in the dictionary object associated with the attributed string returned by this method are not intended for visual display, but instead to provide information that is useful in the accessibility context. They differ from the text attributes in a standard text system NSAttributedString object and in its CFAttributedString counterpart. The keys are prefixed with "AX" instead of "NS", as in "AXFont" instead of "NSFont", and their contents may differ from the contents of their standard text system analogs. They are declared in the AXTextAttributedString.h header.
 
 The accessibility attributed string returned by this method therefore cannot be displayed in a text view without first converting it to a standard attributed string. See @link attributedStringFromAXAttributedString: -attributedStringFromAXAttributedString:@/link for more information, and use that method to effect the conversion.
  
 The <code>characterRange</code> parameter can be any range of characters in a text object. To obtain a character range for a specific line in a text object, see @link AXRangeForLine: AXRangeForLine:@/link. To obtain a character range for a style run in a text object, see @link AXStyleRangeForIndex: AXStyleRangeForIndex:@/link.
 
 See also @link AXStringForRange: AXStringForRange: @/link and @link AXRTFForRange: AXRTFForRange:@/link.
*/
- (NSAttributedString *)AXAttributedStringForRange:(NSValue *)characterRange;

/*
 @method AXStyleRangeForIndex:
 @abstract Returns the range of characters in the receiving text object composing a single style run that contains <code>characterIndex</code>.
 @param characterIndex An NSNumber encoding an integer value that is the index of a character in the receiving text object, counting all characters from the beginning of the text object even if it flows across multiple text views.
 @result An NSValue object encoding an NSRange structure.
 @discussion This is a parameterized attribute method to read text objects. Given a character index in the receiving text object, it returns the range of characters in the receiving text object composing the style run.
 
 Use NSValue's <code>-rangeValue</code> method to extract the NSRange structure of the result.
*/
- (NSValue *)AXStyleRangeForIndex:(NSNumber *)characterIndex; // -rangeValue to extract NSRange

#pragma mark cell-based table parameterized attributes
/*
 @group PARAMETERIZED ATTRIBUTES - cell-based table
*/

/*
 @method AXCellForColumnAndRow:
 @abstract Returns the cell in the receiving cell-based table or outline at the intersection of the column and row in <code>columnAndRowArray</code>.
 @param columnAndRowArray An NSArray object containing two NSNumber objects encoding integer values that are the column index and the row index (in that order) in the receiving table or outline.
 @result A PFUIElement object with a role of AXCell.
 @discussion This is a parameterized attribute method to read cell-based table and outline objects. Given an array containing the column index and the row index in the receiving table or outline object, it returns the cell object. It returns the correct cell object even if the cell spans multiple columns or rows. This method is available, for example, in Apple's Numbers application.
 
 An <code>AXCellForColumnAndRow</code> attribute is required for cell-based tables and outlines.
*/
- (PFUIElement *)AXCellForColumnAndRow:(NSArray *)columnAndRowArray; // UI element for cell at specified column and row; the parameter is a 2-element array of NSNumber

#pragma mark layout area parameterized attributes
/*
 @group PARAMETERIZED ATTRIBUTES - layout area
*/

/*
 @method AXLayoutPointForScreenPoint:
 @abstract Returns the point in the layout area coordinate system corresponding to <code>screenPoint</code> in top-left relative screen coordinates.
 @param screenPoint An NSValue object encoding an NSPoint structure describing a position on the screen in top-left relative screen coordinates that is within a layout area.
 @result An NSValue object encoding an NSPoint structure.
 @discussion This is a parameterized attribute method to read cell-based table objects. Given a point on the screen in top-left relative screen coordinates, it returns the corresponding point in the receiving layout area in the layout area's coordinate system. This method is available, for example, in Apple's Numbers application.
 
 The units of measurement used in the layout area may differ from those used on the screen. See @link AXHorizontalUnits AXHorizontalUnits @/link and @link AXVerticalUnits AXVerticalUnits@/link.
 
 Use NSValue's <code>-pointValue</code> method to extract the NSPoint structure of the result.

 See also @link AXScreenPointForLayoutPoint: AXScreenPointForLayoutPoint:@/link.
*/
- (NSValue *)AXLayoutPointForScreenPoint:(NSValue *)screenPoint; // -pointValue to extract NSPoint; in screen coordinates

/*
 @method AXLayoutSizeForScreenSize:
 @abstract Returns the size in the layout area coordinate system corresponding to <code>screenSize</code> in screen coordinates.
 @param screenSize An NSValue object encoding an NSSize structure describing the size on the screen in screen coordinates of a layout area.
 @result An NSValue object encoding an NSSize structure.
 @discussion This is a parameterized attribute method to read layout area objects. Given the size on the screen in screen coordinates, it returns the corresponding size of the receiving layout area in the layout area's coordinate system. This method is available, for example, in Apple's Numbers application.
 
 The units of measurement used in the layout area may differ from those used on the screen. See @link AXHorizontalUnits AXHorizontalUnits @/link and @link AXVerticalUnits AXVerticalUnits@/link.
 
 Use NSValue's <code>-sizeValue</code> method to extract the NSSize structure of the result.

 See also @link AXScreenPointForLayoutPoint: AXScreenPointForLayoutPoint:@/link.
*/
- (NSValue *)AXLayoutSizeForScreenSize:(NSValue *)screenSize; // -sizeValue to extract NSSize; in screen coordinates

/*
 @method AXScreenPointForLayoutPoint:
 @abstract Returns the point in top-left relative screen coordinates corresponding to <code>layoutPoint</code> in the layout area coordinate system.
 @param layoutPoint An NSValue object encoding an NSPoint structure describing a position in the receiving layout area in layout area coordinates.
 @result An NSValue object encoding an NSPoint structure.
 @discussion This is a parameterized attribute method to read cell-based table objects. Given a point in the receiving layout area in the layout area's coordinate system, it returns the corresponding point on the screen in top-left relative screen coordinates.
 
 The units of measurement used in the layout area may differ from those used on the screen. See @link AXHorizontalUnits AXHorizontalUnits @/link and @link AXVerticalUnits AXVerticalUnits@/link.

 Use NSValue's <code>-pointValue</code> method to extract the NSPoint structure of the result.

 See also @link AXScreenSizeForLayoutSize: AXScreenSizeForLayoutSize:@/link.
*/
- (NSValue *)AXScreenPointForLayoutPoint:(NSValue *)layoutPoint; // -pointValue to extract NSPoint; in layout area coordinates

/*
 @method AXScreenSizeForLayoutSize:
 @abstract Returns the size in screen coordinates corresponding to <code>layoutSize</code>in the layout area coordinate system.
 @param layoutSize An NSValue object encoding an NSSize structure describing the size of the receiving layout area in the layout area's coordinate system.
 @result An NSValue object encoding an NSSize structure.
 @discussion This is a parameterized attribute method to read layout area objects. Given the size of the receiving layout area in the layout area's coordinate systemon, it returns the corresponding size on the screen in screen coordinates.
 
 The units of measurement used in the layout area may differ from those used on the screen. See @link AXHorizontalUnits AXHorizontalUnits @/link and @link AXVerticalUnits AXVerticalUnits@/link.
 
 Use NSValue's <code>-sizeValue</code> method to extract the NSSize structure of the result.

 See also @link AXLayoutSizeForScreenSize: AXLayoutSizeForScreenSize:@/link.
*/
- (NSValue *)AXScreenSizeForLayoutSize:(NSValue *)layoutSize; // -sizeValue to extract NSSize; in layout area coordinates

@end

#pragma mark -
/*!
 @protocol PFUIElementDelegate
 @abstract A formal protocol declaring delegate messages sent by PFUIElement.
 @discussion The PFUIElementDelegate protocol implements two optional delegate methods, @link PFUIElementWasDestroyed: PFUIElementWasDestroyed: @/link and @link PFUIElementReportError: PFUIElementReportError:@/link.
 
 Clients that implement either of these delegate methods must declare that they adopt this formal protocol.
 
 The PFUIElement.h header declares <code>PFUIElementWasDestroyedNotification</code>, an external notification posted when a UI element is destroyed in the user interface. Any client object can register to receive this notification. An object that implements the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method is automatically registered to receive this notification.
*/
@protocol PFUIElementDelegate<NSObject>

// Clients that implement either of these delegate methods must declare that they adopt this formal protocol.

@optional
/*!
 @group OPTIONAL DELEGATE METHODS
*/

/*!
 @method PFUIElementWasDestroyed:
 @abstract Sent when a UI element is destroyed.
 @param notification The notification parameter is always <code>PFUIElementWasDestroyedNotification</code>.
 @discussion Sent from the default notification center immediately after the receiving UI element is destroyed in the running application (for example, when a window closes).
 
 The notification parameter is always <code>PFUIElementWasDestroyedNotification</code>. You can retrieve the destroyed PFUIElement object by sending <code>-object</code> to <code>notification</code>. Beware the warnings about using destroyed PFUIElement objects described in @link elementRef -elementRef@/link.
 
 If this method is implemented in the client, the client is automatically registered for <code>PFUIElementWasDestroyedNotification</code> when you set the element's delegate. Other classes can register with the default notification center to observe <code>PFUIElementWasDestroyedNotification</code> without implementing this delegate method.
 
 If this element was registered by a client to be observed for accessibility notifications, destruction of this element will automatically unregister it for all registered notifications and update the observers' list of current registrations. This happens after this delegate message is sent and after <code>PFUIElementWasDestroyedNotification</code> is posted, to ensure that a client registered to receive <code>NSAccessibilityUIElementDestroyedNotification</code> from this element can respond before the registrations array is updated. (This means that implementors of this delegate method and observers of this notification must be aware that the current registrations array does not yet reflect the destruction of this element. If your delegate or notification method needs an up-to-date registrations array, you can force the registrations array to update by sending @link //apple_ref/occ/instm/PFObserver/unregisterForAllNotificationsFromElement:. -unregisterForAllNotificationsFromElement:@/link.)
 
 Some UI elements do not report when their container is destroyed, such as a Cocoa window's title and grow area elements. The @link exists -exists@/link method depends on this notification, so it will report that such elements still exist in the running application. See @link exists -exists@/link for techniques that can be used to deal with this issue.
 
 You can always safely get a destroyed UI element's @link pid -pid@/link, and you can always test destroyed PFUIElement objects for equality using <code>-isEqual:</code> or @link isEqualToElement: -isEqualToElement:@/link. You can also get information that was cached when a destroyed PFUIElement object was created, by sending @link elementInfo -elementInfo@/link, and the notification's <code>userInfo</code> dictionary also contains this information. See @link elementInfo -elementInfo@/link for information about the objects that are cached in the dictionary and their keys. The <code>userInfo</code> dictionary is autoreleased; the client must retain it if it is used.
 
 A PFApplicationUIElement object never reports when it is destroyed (that is, when the application quits). It does not send this delegate message and it does not post the corresponding notification.
 
 See also @link //apple_ref/occ/instm/PFUIElement/delegate -delegate@/link and @link //apple_ref/occ/instm/PFUIElement/setDelegate: -setDelegate:@/link.
*/
- (void)PFUIElementWasDestroyed:(NSNotification *)notification;

/*!
 @method PFUIElementReportError:
 @abstract Sent when an operation on the receiving UI element generates an accessibility error.
 @param error An NSError object containing information about the error.
 @discussion See @link //apple_ref/occ/instm/PFUIElement/delegate -delegate@/link and @link //apple_ref/occ/instm/PFUIElement/setDelegate: -setDelegate:@/link.
*/
- (void)PFUIElementReportError:(NSError *)error;

@end

#pragma mark -
/*!
 @class PFApplicationUIElement
 @abstract A concrete class representing the root application UI element of a running application or the system-wide UI element.
 @discussion A PFApplicationUIElement object represents the root application UI element of a running application or the system-wide UI element. It is a subclass of PFUIElement and inherits all of PFUIElement's functionality (except that it does not respond to the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method). It offers additional features for root-level UI elements (applications and the system-wide UI element).
 
 See the PFUIElement class description for more information.
 
 A client application may subclass PFApplicationUIElement. If it does so, it must register its Class object with PFApplicationUIElement so that PFUIElement methods that allocate and initialize a new root application UI Element will know what class of object to instantiate. 
 
 To register itself, the subclass must override +[NSObject load] like this:
 
 + (void)load {
 [PFApplicationUIElement registerApplicationUIElementClass:self];
 }

 Clients should not subclass PFUIElement, because they would lose access to PFApplicationUIElement. Instead, they should use categories or similar techniques to add functionality to PFUIElement (and, for that matter, to PFApplicationUIElement).
 
 Before a client application can make use of this class, the "Enable access for assistive devices" setting in the Accessibility (or Universal Access) pane of System Preferences must be turned on, or the client application must be made a trusted process through use of the accessibility API's <code>AXMakeProcessTrusted()</code> function. Authentication may be required.
*/
@interface PFApplicationUIElement : PFUIElement {
	// Subclass for application UI elements

	@private
	NSNumber *pidNumber;
}

#pragma mark SUBCLASS SUPPORT
/*
 A client application may subclass PFApplicationUIElement. If it does so, it must register its Class object with PFApplicationUIElement so that PFUIElement methods that allocate and initialize a new root application UI Element will know what class of object to instantiate. 
 
 Clients should not subclass PFUIElement, because they would lose access to PFApplicationUIElement. Instead, they should use categories or similar techniques to add functionality to PFUIElement (and, for that matter, to PFApplicationUIElement).

 To register itself, the subclass must override +[NSObject load] like this:

    + (void)load {
        [PFApplicationUIElement registerApplicationUIElementClass:self];
    }
*/

+ (void)registerApplicationUIElementClass:(Class)appClass;
+ (Class)applicationUIElementClass;

#pragma mark INITIALIZATION
/*!
 @methodgroup INITIALIZATION
*/

#pragma mark Initializers
// The designated initializer for a PFApplicationUIElement object representing a root application UI element is -initWithPid:delegate:. The designated initializer for a PFApplicationUIElement object representing the system-wide application UI element is -initSystemWideWithDelegate:.

/*!
 @method initWithPath:delegate:
 @abstract Initializes and returns a newly allocated PFApplicationUIElement object with <code>fullPath</code>, the full path to the target application's bundle or file.
 @param fullPath An NSString object giving the full path to the application.
 @param aDelegate An object that may implement the optional @link PFUIElementReportError: -PFUIElementReportError: @/link delegate method. May be <code>nil</code>.
 @result A PFApplicationUIElement object, or <code>nil</code> if there is no application at <code>fullPath</code>.
 @discussion Use this convenience initializer to initialize a newly-allocated root UI element object for a specific application; for example, to browse a specific application's user interface without regard to whether it is frontmost or where on the screen its UI elements may be located.
 
 A PFApplicationUIElement object does not participate in PFUIElement's delegate and notification mechanisms regarding destroyed UI elements. It does not report when it is destroyed (that is, when the application quits). See the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method and PFUIElement's @link elementInfo -elementInfo @/link for details regarding these mechanisms.
 
 See PFUIElement's @link initWithElementRef:delegate: -initWithElementRef:delegate: @/link for important information.
 
 See also @link initWithURL:delegate: -initWithURL:delegate: @/link, @link initWithBundleIdentifier:delegate: -initWithBundleIdentifier:delegate: @/link, @link initWithPid:delegate: -initWithPid:delegate: @/link and @link initSystemWideWithDelegate: -initSystemWideWithDelegate:@/link.
*/
- (id)initWithPath:(NSString *)fullPath delegate:(id)aDelegate; // convenience initializer

/*!
 @method initWithURL:delegate:
 @abstract Initializes and returns a newly allocated PFApplicationUIElement object with <code>url</code>, the file URL for the target application's bundle or file.
 @param url An NSURL object referencing the application.
 @param aDelegate An object that may implement the optional @link PFUIElementReportError: -PFUIElementReportError: @/link delegate method. May be <code>nil</code>.
 @result A PFApplicationUIElement object, or <code>nil</code> if there is no application at <code>url</code>.
 @discussion Use this convenience initializer to initialize a newly-allocated root UI element object for a specific application; for example, to browse a specific application's user interface without regard to whether it is frontmost or where on the screen its UI elements may be located.
 
 A PFApplicationUIElement object does not participate in PFUIElement's delegate and notification mechanisms regarding destroyed UI elements. It does not report when it is destroyed (that is, when the application quits). See the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method and PFUIElement's @link elementInfo -elementInfo @/link for details regarding these mechanisms.
 
 See PFUIElement's @link initWithElementRef:delegate: -initWithElementRef:delegate: @/link for important information.
 
 See also @link initWithPath:delegate: -initWithPath:delegate: @/link, @link initWithBundleIdentifier:delegate: -initWithBundleIdentifier:delegate: @/link, @link initWithPid:delegate: -initWithPid:delegate: @/link and @link initSystemWideWithDelegate: -initSystemWideWithDelegate:@/link.
*/
- (id)initWithURL:(NSURL *)url delegate:(id)aDelegate; // convenience initializer

/*!
 @method initWithBundleIdentifier:delegate:
 @abstract Initializes and returns a newly allocated PFApplicationUIElement object with <code>bundleIdentifier</code>, the target application's bundle identifier.
 @param bundleIdentifier An NSString object, the application bundle's identifier.
 @param aDelegate An object that may implement the optional @link PFUIElementReportError: -PFUIElementReportError: @/link delegate method. May be <code>nil</code>.
 @result A PFApplicationUIElement object, or <code>nil</code> if there is no application bundle with a bundle identifier of <code>bundleIdentifier</code>.
 @discussion Use this convenience initializer to initialize a newly-allocated root UI element object for a specific application; for example, to browse a specific application's user interface without regard to whether it is frontmost or where on the screen its UI elements may be located.
 
 A PFApplicationUIElement object does not participate in PFUIElement's delegate and notification mechanisms regarding destroyed UI elements. It does not report when it is destroyed (that is, when the application quits). See the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method and PFUIElement's @link elementInfo -elementInfo @/link for details regarding these mechanisms.
 
 See PFUIElement's @link initWithElementRef:delegate: -initWithElementRef:delegate: @/link for important information.
 
 See also @link initWithPath:delegate: -initWithPath:delegate: @/link, @link initWithURL:delegate: -initWithURL:delegate: @/link, @link initWithPid:delegate: -initWithPid:delegate: @/link and @link initSystemWideWithDelegate: -initSystemWideWithDelegate:@/link.
*/
- (id)initWithBundleIdentifier:(NSString *)bundleIdentifier delegate:(id)aDelegate; // convenience initializer

/*!
 @method initWithPid:delegate:
 @abstract The designated initializer for the PFApplicationUIElement class for a specific application.
 @param pid An integer, the running application's BSD Unix process identification number.
 @param aDelegate An object that may implement the optional @link PFUIElementReportError: -PFUIElementReportError: @/link delegate method. May be <code>nil</code>.
 @result A PFApplicationUIElement object whose UNIX process identifier is pid.
 @discussion Initializes and returns a newly allocated PFApplicationUIElement object with <code>pid</code>, the target application's BSD Unix process identification number (PID).
 
 Use this designated initializer to initialize a newly-allocated root UI element object for a specific application; for example, to browse a specific application's user interface without regard to whether it is frontmost or where on the screen its UI elements may be located.
 
 A PFApplicationUIElement object does not participate in PFUIElement's delegate and notification mechanisms regarding destroyed UI elements. It does not report when it is destroyed (that is, when the application quits). See the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method and PFUIElement's @link elementInfo -elementInfo @/link for details regarding these mechanisms.
 
 See PFUIElement's @link initWithElementRef:delegate: -initWithElementRef:delegate:@/link for important information.
 
 See also @link initWithPath:delegate: -initWithPath:delegate: @/link, @link initWithURL:delegate: -initWithURL:delegate: @/link, @link initWithBundleIdentifier:delegate: -initWithBundleIdentifier:delegate: @/link and @link initSystemWideWithDelegate: -initSystemWideWithDelegate:@/link.
*/
- (id)initWithPid:(pid_t)pid delegate:(id)aDelegate; // designated initializer for specific application

// Initializes and returns a newly allocated PFApplicationUIElement object. The delegate parameter value may be nil if the client does not wish to track accessibility errors.
/*!
 @method initSystemWideWithDelegate:
 @abstract The designated initializer for the PFApplicationUIElement class for a system-wide element.
 @param delegate An object that may implement the optional @link PFUIElementReportError: -PFUIElementReportError: @/link delegate method. May be <code>nil</code>.
 @result A system-wide PFApplicationUIElement object.
 @discussion Initializes and returns a newly allocated system-wide PFApplicationUIElement object.
 
 Use this designated initializer to initialize a newly-allocated root UI element object for the entire system, representing all running applications collectively. Use this method, for example, to obtain the element currently having systemwide focus without first determining which application is active.
 
 A PFApplicationUIElement object does not participate in PFUIElement's delegate and notification mechanisms regarding destroyed UI elements. See the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed:@/link delegate method and PFUIElement's @link elementInfo -elementInfo@/link for details regarding these mechanisms.
 
 See PFUIElement's @link initWithElementRef:delegate: -initWithElementRef:delegate: @/link method for important information.
 
 See also @link initWithPath:delegate: -initWithPath:delegate: @/link, @link initWithURL:delegate: -initWithURL:delegate: @/link, @link initWithBundleIdentifier:delegate: -initWithBundleIdentifier:delegate: @/link and @link initWithPid:delegate: -initWithPid:delegate: @/link.
*/
- (id)initSystemWideWithDelegate:(id)delegate; // designated initializer for system

#pragma mark Factory Convenience Methods

/*!
 @method applicationUIElementWithPath:delegate:
 @abstract Convenience class method creates and returns an initialized PFApplicationUIElement object with <code>fullPath</code>, the full path to the target application's bundle or file.
 @param fullPath An NSString object giving the full path to the application.
 @param aDelegate An object that may implement the optional @link PFUIElementReportError: -PFUIElementReportError: @/link delegate method. May be <code>nil</code>.
 @result An autoreleased PFApplicationUIElement object, or <code>nil</code> if there is no application at <code>fullPath</code>.
 @discussion Use this convenience method to create and return an initialized root UI element object for a specific application; for example, to browse a specific application's user interface without regard to whether it is frontmost or where on the screen its UI elements may be located.
 
 A PFApplicationUIElement object does not participate in PFUIElement's delegate and notification mechanisms regarding destroyed UI elements. It does not report when it is destroyed (that is, when the application quits). See the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method and PFUIElement's @link elementInfo -elementInfo @/link for details regarding these mechanisms.
 
 See PFUIElement's @link initWithElementRef:delegate: -initWithElementRef:delegate: @/link for important information.
 
 See also @link applicationUIElementWithURL:delegate: +applicationUIElementWithURL:delegate: @/link, @link applicationUIElementWithBundleIdentifier:delegate: +applicationUIElementWithBundleIdentifier:delegate: @/link, @link applicationUIElementWithPid:delegate: +applicationUIElementWithPid:delegate:@/link and @link systemWideUIElementWithDelegate: +systemWideUIElementWithDelegate:@/link.
*/
+ (id)applicationUIElementWithPath:(NSString *)fullPath delegate:(id)aDelegate;

/*!
 @method applicationUIElementWithURL:delegate:
 @abstract Convenience class method creates and returns an initialized PFApplicationUIElement object with <code>url</code>, the file URL for the target application's bundle or file.
 @param url An NSURL object referencing the application.
 @param aDelegate An object that may implement the optional @link PFUIElementReportError: -PFUIElementReportError: @/link delegate method. May be <code>nil</code>.
 @result An autoreleased PFApplicationUIElement object, or <code>nil</code> if there is no application at <code>url</code>.
 @discussion Use this convenience method to create and return an initialized root UI element object for a specific application; for example, to browse a specific application's user interface without regard to whether it is frontmost or where on the screen its UI elements may be located.
 
 A PFApplicationUIElement object does not participate in PFUIElement's delegate and notification mechanisms regarding destroyed UI elements. It does not report when it is destroyed (that is, when the application quits). See the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method and PFUIElement's @link elementInfo -elementInfo @/link for details regarding these mechanisms.
 
 See PFUIElement's @link initWithElementRef:delegate: -initWithElementRef:delegate: @/link for important information.
 
 See also @link applicationUIElementWithPath:delegate: +applicationUIElementWithPath:delegate: @/link, @link applicationUIElementWithBundleIdentifier:delegate: +applicationUIElementWithBundleIdentifier:delegate: @/link, @link applicationUIElementWithPid:delegate: +applicationUIElementWithPid:delegate:@/link and @link systemWideUIElementWithDelegate: +systemWideUIElementWithDelegate:@/link.
*/
+ (id)applicationUIElementWithURL:(NSURL *)url delegate:(id)aDelegate;

/*!
 @method applicationUIElementWithBundleIdentifier:delegate:
 @abstract Convenience class method creates and returns an initialized PFApplicationUIElement object with <code>bundleIdentifier</code>, the target application's bundle identifier.
 @param bundleIdentifier An NSString object, the application bundle's identifier.
 @param aDelegate An object that may implement the optional @link PFUIElementReportError: -PFUIElementReportError: @/link delegate method. May be <code>nil</code>.
 @result An autoreleased PFApplicationUIElement object, or <code>nil</code> if there is no application bundle with a bundle identifier of <code>bundleIdentifier</code>.
 @discussion Use this convenience method to create and return an initialized root UI element object for a specific application; for example, to browse a specific application's user interface without regard to whether it is frontmost or where on the screen its UI elements may be located.
 
 A PFApplicationUIElement object does not participate in PFUIElement's delegate and notification mechanisms regarding destroyed UI elements. It does not report when it is destroyed (that is, when the application quits). See the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method and PFUIElement's @link elementInfo -elementInfo @/link for details regarding these mechanisms.
 
 See PFUIElement's @link initWithElementRef:delegate: -initWithElementRef:delegate: @/link for important information.
 
 See also @link applicationUIElementWithPath:delegate: +applicationUIElementWithPath:delegate: @/link, @link applicationUIElementWithURL:delegate: +applicationUIElementWithURL:delegate: @/link, @link applicationUIElementWithPid:delegate: +applicationUIElementWithPid:delegate:@/link and @link systemWideUIElementWithDelegate: +systemWideUIElementWithDelegate:@/link.
*/
+ (id)applicationUIElementWithBundleIdentifier:(NSString *)bundleIdentifier delegate:(id)aDelegate;

/*!
 @method applicationUIElementWithPid:delegate:
 @abstract Convenience class method creates and returns an initialized PFApplicationUIElement object with <code>pid</code>, the UNIX process identifier for the application.
 @param pid An integer, the running application's BSD Unix process identification number.
 @param aDelegate An object that may implement the optional @link PFUIElementReportError: -PFUIElementReportError: @/link delegate method. May be <code>nil</code>.
 @result An autoreleased PFApplicationUIElement object, or <code>nil</code> if there is no application at <code>url</code>.
 @discussion Use this convenience method to create and return an initialized root UI element object for a specific application; for example, to browse a specific application's user interface without regard to whether it is frontmost or where on the screen its UI elements may be located.
 
 A PFApplicationUIElement object does not participate in PFUIElement's delegate and notification mechanisms regarding destroyed UI elements. It does not report when it is destroyed (that is, when the application quits). See the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method and PFUIElement's @link elementInfo -elementInfo @/link for details regarding these mechanisms.
 
 See PFUIElement's @link initWithElementRef:delegate: -initWithElementRef:delegate: @/link for important information.
 
 See also @link applicationUIElementWithPath:delegate: +applicationUIElementWithPath:delegate: @/link, @link applicationUIElementWithURL:delegate: +applicationUIElementWithURL:delegate: @/link, @link applicationUIElementWithBundleIdentifier:delegate: +applicationUIElementWithBundleIdentifier:delegate: @/link and @link systemWideUIElementWithDelegate: +systemWideUIElementWithDelegate:@/link.
*/
+ (id)applicationUIElementWithPid:(pid_t)pid delegate:(id)aDelegate;

/*!
 @method systemWideUIElementWithDelegate:
 @abstract Convenience class method creates and returns an initialized system-wide PFApplicationUIElement object.
 @param aDelegate An object that may implement the optional @link PFUIElementReportError: -PFUIElementReportError: @/link delegate method. May be <code>nil</code>.
 @result An autoreleased system-wide PFApplicationUIElement object.
 @discussion Use this convenience method to create and return an initialized UI element object for the entire system, representing all running applications collectively. Use this method, for example, to obtain the element currently having systemwide focus without first determining which application is active.
 
 A PFApplicationUIElement object does not participate in PFUIElement's delegate and notification mechanisms regarding destroyed UI elements. It does not report when it is destroyed (that is, when the application quits). See the @link PFUIElementWasDestroyed: -PFUIElementWasDestroyed: @/link delegate method and PFUIElement's @link elementInfo -elementInfo @/link for details regarding these mechanisms.
 
 See PFUIElement's @link initWithElementRef:delegate: -initWithElementRef:delegate: @/link for important information.

 See also @link applicationUIElementWithPath:delegate: +applicationUIElementWithPath:delegate: @/link, @link applicationUIElementWithURL:delegate: +applicationUIElementWithURL:delegate: @/link, @link applicationUIElementWithBundleIdentifier:delegate: +applicationUIElementWithBundleIdentifier:delegate: @/link and @link applicationUIElementWithPid:delegate: +applicationUIElementWithPid:delegate:@/link.
*/
+ (id)systemWideUIElementWithDelegate:(id)aDelegate;

#pragma mark ACCESSOR METHODS
/*!
 @methodgroup ACCESSOR METHODS
*/

/*!
 @method pidNumber
 @abstract Returns the BSD Unix process identification number (PID) of the receiver.
 @result An NSNumber object encoding an integer value.
 @discussion It is sometimes useful to know the process identification number of an application. For example, it can be used with a variety of Carbon functions to access information not otherwise readily available in Cocoa.
 
 See also the @link pid -pid @/link method in the PFUIElement class, which returns the PID as an integer.
 
 Use NSNumber's <code>-intValue</code> method to extract the integer value of the result.
*/
- (NSNumber *)pidNumber; // -intValue extracts integer

#pragma mark ELEMENT UTILITIES
/*!
 @methodgroup ELEMENT UTILITIES
*/

/*!
 @method elementAtPoint:
 @abstract Creates and returns a PFUIElement object representing the visible UI element located at <code>point</code> on the screen belonging to the receiving application UI element.
 @param point An NSPoint struct using top-left relative screen coordinates.
 @result The PFUIElement object at <code>point</code>, or <code>nil</code> if the application does not own it.
 @discussion Use this method to read the screen, based, for example, on the current location of the mouse.
 
 Use the PFUIElement class method @link elementAtPoint:withDelegate:error: +elementAtPoint:withDelegate:error:@/link instead of this instance method to get a PFUIElement object representing the UI element located at <code>point</code> without regard to what application owns it.
 
 See PFUIElement's @link initWithElementRef:delegate: -initWithElementRef:delegate:@/link: method for important information.
*/
- (PFUIElement *)elementAtPoint:(NSPoint)point;

/*!
 @method typeCharacters:keyCode:withModifierFlags:
 @abstract Posts keyboard events to the application represented by the receiver, whether or not it is the active application. DEPRECATED in OS X 10.9 Mavericks.
 @param characters An NSString object representing a Unicode character that can be generated by a single key press without modifier keys (Shift excepted).
 @param virtualKey The virtual key code provided by NSEvent's <code>-keyCode</code> method.
 @param flags An integer; either 0 for no modifier keys, or one or more of the first five key mask constants listed in NSEvent.h (combined using the C bitwise OR operator, if more than one) for modifier keys.
 @discussion This method is deprecated in OS X 10.9 Mavericks and newer because Apple has deprecated the function on which it relies, <code>AXUIElementPostKeyboardEvent()</code> in AXUIElement.h. The similar <code>CGPostKeyboardEvent()</code> function in CGRemoteOperation.h was deprecated in Mac OS X 10.6 Snow Leopard. Developers are advised to use Quartz Event Taps through the <code>CGEventCreateKeyboardEvent()</code> function in CGEvent.h or its Cocoa counterpart, <code>+[NSEvent keyEventWithType:location:modifierFlags:timestamp:windowNumber:context:characters:charactersIgnoringModifiers:isARepeat:keyCode:]</code>.
 
 In OS X 10.8 Mountain Lion and older, use this method to type one character at a time into the UI element that currently has keyboard focus in the application represented by the receiver (usually a text-based view or control such as a text view or a text field). Specifying the Command key in <code>flags</code> may be treated by the application as a keyboard shortcut for a menu item or button.
 
 Use @link typeCharactersSystemWide:keyCode:withModifierFlags: -typeCharactersSystemWide:keyCode:withModifierFlags: @/link instead of this method to type characters into the active application without first determining which application is active.
 
 The <code>characters</code> and <code>flags</code> parameters are equivalent to those used in NSEvent's <code>-charactersIgnoringModifiers</code> and <code>-modifierFlags</code> methods. The <code>virtualKey</code> parameter is the virtual key code provided by NSEvent's <code>-keyCode</code> method; it is a hardware-independent integer value provided by system resources for every known keyboard, mapped from the hardware-dependent raw key code using the current keyboard layout resource.
 
 On Roman systems, the <code>characters</code> parameter is optional and should be passed as <code>nil</code> or an empty string unless you are knowledgeable regarding the use of keyboard layouts, but it is not optional on some other systems where it is used as a hint to supplement the virtual key code during key translation. 
*/
- (void)typeCharacters:(NSString *)characters keyCode:(unsigned short)virtualKey withModifierFlags:(NSUInteger)flags;

/*!
 @method typeCharactersSystemWide:keyCode:withModifierFlags:
 @abstract Class method posts keyboard events to the active application. DEPRECATED in OS X 10.9 Mavericks.
 @param characters An NSString object representing a Unicode character that can be generated by a single key press without modifier keys (Shift excepted).
 @param virtualKey The virtual key code provided by NSEvent's <code>-keyCode</code> method.
 @param flags An unsigned integer; either 0 for no modifier keys, or one or more of the first five key mask constants listed in NSEvent.h (combined using the C bitwise OR operator, if more than one) for modifier keys.
 @discussion This method is deprecated in OS X 10.9 Mavericks and newer because Apple has deprecated the function on which it relies, <code>AXUIElementPostKeyboardEvent()</code> in AXUIElement.h. The similar <code>CGPostKeyboardEvent()</code> function in CGRemoteOperation.h was deprecated in Mac OS X 10.6 Snow Leopard. Developers are advised to use Quartz Event Taps through the <code>CGEventCreateKeyboardEvent()</code> function in CGEvent.h or its Cocoa counterpart, <code>+[NSEvent keyEventWithType:location:modifierFlags:timestamp:windowNumber:context:characters:charactersIgnoringModifiers:isARepeat:keyCode:]</code>.
 
 In OS X 10.8 Mountain Lion and older, use this method to type one character at a time into the UI element that currently has keyboard focus in the active application (usually a text-based view or control such as a text view or a text field). Specifying the Command key in flags may be treated by the application as a keyboard shortcut for a menu item or button.
 
 Use @link typeCharacters:keyCode:withModifierFlags: -typeCharacters:keyCode:withModifierFlags: @/link instead of this method to type characters into a specific application whether or not it is active.
 
 The <code>characters</code> and <code>flags</code> parameters are equivalent to those used in NSEvent's <code>-charactersIgnoringModifiers</code> and <code>-modifierFlags</code> methods. The <code>virtualKey</code> parameter is the virtual key code provided by NSEvent's <code>-keyCode</code> method; it is a hardware-independent integer value provided by system resources for every known keyboard, mapped from the hardware-dependent raw key code using the current keyboard layout resource.
 
 On Roman systems, the <code>characters</code> parameter is optional and should be passed as <code>nil</code> or an empty string unless you are knowledgeable regarding the use of keyboard layouts, but it is not optional on some other systems where it is used as a hint to supplement the virtual key code during key translation.
*/
+ (void)typeCharactersSystemWide:(NSString *)characters keyCode:(unsigned short)virtualKey withModifierFlags:(NSUInteger)flags;

@end

#pragma mark -
#pragma mark NOTIFICATIONS

extern NSString *PFUIElementWasDestroyedNotification;

//#endif

/*
 CHANGE LOG 3.1.3
 
 2011-07-01 WJC build 78
 • Added -valuesForAttribute:range:. Requires increasing the framework version to I.
 
 2011-07-09 WJC Build 82
 • Removed privateVars iVar in anticipation of an all-64 bit world.
 
 2011-08-02 WJC Build 83
 • Added new Mac OS X 10.7 Lion attribute properties AXEnhancedUserInterface (private), AXFullScreen (private), AXFullScreenButton, AXIdentifier, AXTextInputMarkedRange (private).
 • Changed attributes from readonly to readwrite based on AccessibilityDefinitions.plist: AXDisclosing, AXExpanded, AXSelected, AXSortDirection, AXVisibleCells, AXVisibleCharacterRange.
 
 2011-08-15 WJC Build 84
 • Added +actionsForRole:andSubrole:, +notificationsForRole:andSubrole: and +attributesForRole:andSubrole: class methods.
 
 2011-08-16 WJC Build 85
 • Added declaration that PFUIElement supports the NSCopying protocol.
 • Made -isDestroyed a public method.
 
 CHANGE LOG 3.2.0
 
 2012-03-28 WJC Build 89
 • Changed the version for the HeaderDoc documentation from 3.1.4 to 3.2.0 in PFUIElement.h.
 • Added +registerApplicationUIElementClass: and +applicationUIElementClass methods in new Subclass Support section in PFApplicationUIElement implementation part in PFUIElement.

 CHANGE LOG 3.3.0
 
 2013-08-04 WJC Build 92
 • Updated Deployment Target to 10.7 with permission of licensees.
 • Changed Build Settings Architectures to Standard Architectures (64-bit Intel) with permission of licensees.
 • Updated PFUIElement.h and PFObserver.h HeaderDoc headers @copyright to 2013 and @version to 3.3.0
 • Marked -[PFUIElement isAccessEnabled] as deprecated in OS X 10.9 Mavericks because the function it wraps, AXAPIEnabled(), is deprecated.
 • Added +isProcessTrustedWithOptions: to PFUIElement to cover new AXIsProcessTrustedWithOptions() function.
 • Updated HeaderDoc comments throughout and many method comments.
 • Added -[PFUIElement AXContainsProtectedContent] to the miscellaneous group in the Attributes category.
 • Added documentation to -[PFUIElement AXMenuItemCmdModifiers] to describe new mask constants introduced in OS X 10.9 Mavericks.
 • Suppressed generation of HeaderDoc documentation for individual attributes by removing the exclamation point from the comment. Documentation for attributes, as well as roles, subroles, actions and notifications, is now available in /System/Library/Accessibility/AccessibilityDefinitions.plist.
 • Marked the -typeCharacters:keyCode:withModifierFlags: and +typeCharactersSystemWide:keyCode:withModfierFlags: methods in PFUIElement as deprecated in OS X 10.9 Mavericks because the function they wrap, AXUIElementPostKeyboardEvent(), is deprecated.
 
 CHANGE LOG 3.3.1
 
 2014-03-09 WJC Build 96
 • Fixed bad HeaderDoc links.
 
 CHANGE LOG 3.4.0
 
 2014-09-10 WJC Build 97
 • Updated PFUIElement.h and PFObserver.h HeaderDoc headers @version to 3.4.0.

*/