# coding=utf-8

from selenium import webdriver
from selenium.webdriver import ActionChains
from selenium.webdriver.common.keys import Keys
from random import randint
import time
import utilities

kLineHeight = 33 # Monaco Regular 24
kLineStartInsetX = 3 # inset from the textView left border, for selecting lines
welcomeLineY = 20
welcomeX = 300
messageLineY = 80
global headerWindow
global bodyWindowL
global bodyWindowR
global commentsWindow

print 'Starting the WebDriver session'

defaultLoopDelay_sec = 1.00
defaultCommandDelay_sec = 0.100
defaultImplicitTimeout_sec = 3.000
defaultMouseSpeed = 50
defaultScreenShotOnError = False
defaultGlobalDiagnosticsDirectory = '~/Desktop/'
defaultCookies = [
    {'name': 'loop_delay', 'value': defaultLoopDelay_sec},
    {'name': 'command_delay', 'value':defaultCommandDelay_sec },
    {'name': 'implicit_timeout', 'value': defaultImplicitTimeout_sec},
    {'name': 'mouse_speed', 'value': defaultMouseSpeed},
    {'name': 'screen_shot_on_error', 'value': defaultScreenShotOnError},
    {'name': 'global_diagnostics_directory', 'value': defaultGlobalDiagnosticsDirectory}
]
desiredCapabilities = {'platform': 'Mac', 'cookies': defaultCookies}
# desiredCapabilities = {'platform':'Mac', 'commandDelay':50, 'loopDelay':1000, 'implicitTimeout':utilities.defaultImplicitTimeout_sec * 1000, 'mouseMoveSpeed':50, "screenShotOnError":1}
driver = webdriver.Remote( command_executor='http://localhost:4622/wd/hub', desired_capabilities=desiredCapabilities)

def setFonts():
    window = utilities.findElementByXPath(driver, "/AXApplication[@AXTitle='TextEdit']/AXWindow[@AXTitle='Fonts']")
    if window == None:
        print "No fonts window!"
        return False
    driver.switch_to.window
    driver.set_window_position(100, 100, "current")
    driver.set_window_size(600, 400, "current")
    
    # Bring Fonts to the front and make it the key window. We need to click in each table and type-select rows.
    element = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[@AXTitle='Fonts']/AXStaticText[@AXValue='Fonts']")
    ActionChains(driver).click(element).perform()
    
    # Select font by clicking in tables and typing: Fixed Width, Monaco, Regular, 24
    element = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[@AXTitle='Fonts']/AXSplitGroup[0]/AXScrollArea[0]/AXTable[0]")
    ActionChains(driver).move_to_element_with_offset(element, element.size['width']/2, 2).click().send_keys("Fixed Width").perform()
    
    element = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[@AXTitle='Fonts']/AXSplitGroup[0]/AXScrollArea[2]/AXTable[0]")
    ActionChains(driver).move_to_element_with_offset(element, element.size['width']/2, 2).click().send_keys("Monaco").perform()
    
    element = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[@AXTitle='Fonts']/AXSplitGroup[0]/AXScrollArea[1]/AXTable[0]")
    ActionChains(driver).move_to_element_with_offset(element, element.size['width']/2, 2).click().send_keys("Regular").perform()
    
    element = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[@AXTitle='Fonts']/AXSplitGroup[0]/AXGroup[0]/AXScrollArea[0]/AXTable[0]")
    ActionChains(driver).move_to_element_with_offset(element, element.size['width']/2, 2).click().send_keys("24").perform()
    
    element = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[@AXTitle='Fonts']/AXButton[@AXSubrole='AXCloseButton']")
    ActionChains(driver).click(element).perform()
    return True

def setPreferences():
    # Open the window and switch to New Document
    ActionChains(driver).key_down(Keys.COMMAND).send_keys(",").key_up(Keys.COMMAND).perform()
    prefsWindow = "/AXApplication/AXWindow[@AXTitle='Preferences']"
    tabGroup = prefsWindow + "/AXTabGroup[0]"
    utilities.findElementByXPath(driver, tabGroup + "/AXRadioButton[@AXTitle='New Document']").click()
    
    utilities.findElementByXPath(driver, tabGroup + "/AXRadioGroup[0]/AXRadioButton[@AXTitle='Rich text']").click()
    # Is it already the font we want?
    if not utilities.existsElementByXPath(driver, tabGroup + "/AXStaticText[@AXValue='Monaco 24']"):
        utilities.findElementByXPath(driver, tabGroup + "/AXButton[1]").click() # Rich text Change... button
        if setFonts() == False:
            return False
    
    # Turn off spell check. It keeps changing "scripters" into "scriptures".
    acCheckbox = utilities.findElementByXPath(driver, tabGroup + "/AXCheckBox[@AXTitle='Check spelling as you type ']")
    if acCheckbox.get_attribute("AXValue") == "1":
        ActionChains(driver).click(acCheckbox).perform()    

    # Turn off auto-correct. It keeps changing "scripters" into "scriptures".
    acCheckbox = utilities.findElementByXPath(driver, tabGroup + "/AXCheckBox[@AXTitle='Correct spelling automatically']")
    if acCheckbox.get_attribute("AXValue") == "1":
        ActionChains(driver).click(acCheckbox).perform()

    ActionChains(driver).key_down(Keys.COMMAND).send_keys("w").key_up(Keys.COMMAND).perform()
    
    return True

# Set up a new window with Rich Text and monospaced font.
# Returns: True if the window was created and set up correctly.
def createWindow(x, y, width, height):
    ActionChains(driver).key_down(Keys.COMMAND).send_keys("n").key_up(Keys.COMMAND).perform()
#    time.sleep(0.5)
    
    window = utilities.findElementByXPath(driver, "/AXApplication[@AXTitle='TextEdit']/AXWindow[0]")
    if window == None:
        print "No front window!"
        return False
    driver.switch_to.window
    
    driver.set_window_position(x, y, "current")
    driver.set_window_size(width, height, "current")
    
    return True

def dragWordInTextView(view, viewOffsetX, viewOffsetY, dragOffsetX, dragOffsetY):
    ActionChains(driver).move_to_element_with_offset(view, viewOffsetX, viewOffsetY).double_click().perform()
    time.sleep(0.5) # avoid unwanted triple-click
    
    # Save current mouse speed
    print driver.get_cookies()
    savedMouseCookie = driver.get_cookie('mouse_speed')
    print savedMouseCookie
    driver.add_cookie({'name': 'mouse_speed', 'value': 10})
    print driver.get_cookies()
    
    ActionChains(driver).click_and_hold().perform()
    time.sleep(1.5) # give time for the dragging session to start (cursor == arrow)
    ActionChains(driver).move_by_offset(dragOffsetX, dragOffsetY).release().perform()

    # Restore mouse speed
    driver.add_cookie(savedMouseCookie)

# Replace a message line. 
def replaceLineInTextView(view, newText, lineY):
    driver.get("TextEdit")
    # drag-select the line (Selenium can't triple-click, dang it!)
    ActionChains(driver).move_to_element_with_offset(view, kLineStartInsetX, lineY).click_and_hold().move_by_offset(0, kLineHeight).release().perform()
    ActionChains(driver).send_keys(newText).perform()

def setupTextEdit():
    driver.get("TextEdit")
    
    ## Close all floating windows.
    driver.implicitly_wait(0.0)
    while True:
        floatWindowCloseButton = utilities.findElementByXPath(driver, "/AXApplication[@AXTitle='TextEdit']/AXWindow[@AXSubrole='AXFloatingWindow']/AXButton[@AXSubrole='AXCloseButton']");
        if floatWindowCloseButton != None:
            floatWindowCloseButton.click()
        else:
            break
    
    driver.implicitly_wait(utilities.defaultImplicitTimeout_sec)
    
    if setPreferences() == False:
        quit()

def createHeaderWindow():
    global headerWindow
    # Create a header window
    if createWindow(0, 0, 600, 200) == False:
        quit()
    headerWindow = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]")

def createBodyWindowL():
    global bodyWindowL
    # Create a body window
    if createWindow(0, 200, 700, 500) == False:
        quit()
    bodyWindowL = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]")

def createBodyWindowR():
    global bodyWindowR
    # Create a body window
    if createWindow(701, 200, 700, 500) == False:
        quit()
    bodyWindowR = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]")

def createCommentsWindow():
    global commentsWindow
    # Create a window to display demo comments
    if createWindow(601, 0, 600, 200) == False:
        quit()
    commentsWindow = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]")

# Given a window, clicks in its text view, clears the text view.
# Returns: the text view
def clickAndClearWindowTextView(window):
    print "clickAndClearWindowTextView: " + window.get_attribute("AXTitle")
    action = ActionChains(driver).click(window)
    action.key_down(Keys.COMMAND).send_keys("a").key_up(Keys.COMMAND).send_keys(Keys.BACKSPACE).perform()
    textView = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]/AXScrollArea[0]/AXTextArea[@AXIdentifier='First Text View']")
    return textView

def showWelcomeText(window):
    textView = clickAndClearWindowTextView(window)
    
    # Type an intro with an improperly-ordered word, then drag the word to its proper location.
    ActionChains(driver).send_keys("to AppiumForMac Welcome!!" + Keys.RETURN).perform()
    time.sleep(1)
    ActionChains(driver).send_keys(Keys.RETURN + "oops, sorry, hang on...").perform()
    time.sleep(1)

    # Drag the word "Welcome" to the beginning of the title line.
    dragWordInTextView(textView, welcomeX, welcomeLineY, kLineStartInsetX - welcomeX, 0)
    time.sleep(1)
    replaceLineInTextView(textView, "Much better!", messageLineY)
    time.sleep(1)

def advertiseScripterFeatures():
    textView = clickAndClearWindowTextView(bodyWindowL)
    
    replaceLineInTextView(textView, "AppiumForMac has several powerful features for scripters" + Keys.RETURN, messageLineY)
    time.sleep(0.5)
   
    # Start a list.
    utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]/AXGroup[0]/AXMenuButton[1]").click()
    utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]/AXGroup[0]/AXMenuButton[1]/AXMenu[0]/AXMenuItem[@AXTitle='✓']").click()
    time.sleep(0.2)

    # native 

    ActionChains(driver).send_keys("native keyboard and mouse events" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("absolute AXPaths for fast execution" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("AXPath recorder for fast script creation" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("implicit timeouts" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("press fn key to cancel active sessions" + Keys.RETURN).perform()

    ActionChains(driver).send_keys("new configurable properties (modifiable while running!)" + Keys.RETURN + Keys.TAB).perform()
    ActionChains(driver).send_keys("implicit_timeout" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("loop_delay" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("command_delay" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("mouse_speed" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("global_diagnostics_directory" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("screen_shot_on_error" + Keys.RETURN).perform()
    ActionChains(driver).send_keys(Keys.RETURN + Keys.RETURN).perform()

def advertiseDeveloperFeatures():
    textView = clickAndClearWindowTextView(bodyWindowR)
    
    replaceLineInTextView(textView, "AppiumForMac has a powerful new coding pattern for project contributors" + Keys.RETURN, messageLineY)
    time.sleep(0.5)

    # Start a list.
    utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]/AXGroup[0]/AXMenuButton[1]").click()
    utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]/AXGroup[0]/AXMenuButton[1]/AXMenu[0]/AXMenuItem[@AXTitle='✓']").click()
    time.sleep(0.2)

    ActionChains(driver).send_keys("easy to add new handlers" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("each handler is a simple code block" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("common per-request processing is automatic:" + Keys.RETURN).perform()
    ActionChains(driver).send_keys(Keys.TAB).perform()
    ActionChains(driver).send_keys("elements validated" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("common errors handled" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("implied timeout" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("command delay" + Keys.RETURN).perform()
    ActionChains(driver).send_keys("loop delay" + Keys.RETURN).perform()

# Convenience method to close TextEdit windows and click Don't Save if needed.
def closeDemoWindows():
    try:
        windows = [headerWindow, bodyWindowL, bodyWindowR, commentsWindow]
        for thisWindow in windows:
            textView = clickAndClearWindowTextView(thisWindow)
            ActionChains(driver).key_down(Keys.COMMAND).send_keys("w").key_up(Keys.COMMAND).perform()
            dontSaveButton = utilities.existsElementByXPath(driver, "/AXApplication/AXWindow[0]/AXSheet[0]/AXButton[@AXTitle='Don’t Save']")
            if dontSaveButton:
                dontSaveButton.click()
            else:
                continue
    except Exception as e:
        print e

# Development hack to close TextEdit windows and click Don't Save until there are no more windows.
# lastWindowtitle: the name of the window that will stop this method when it is in front. Supply "" to close all non-minimized windows.
def closeWithoutSaving(lastWindowtitle):
    while True:
        frontWindow = utilities.findElementByXPath(driver, "/AXApplication/AXWindow[0]")
        if frontWindow == None:
            print "frontWindow == None"
            break
        fwTitle = frontWindow.get_attribute("AXTitle")
        print fwTitle
        if fwTitle == lastWindowtitle:
            break
        print fwTitle
        ActionChains(driver).key_down(Keys.COMMAND).send_keys("w").key_up(Keys.COMMAND).perform()
        dontSaveButton = utilities.existsElementByXPath(driver, "/AXApplication/AXWindow[0]/AXSheet[0]/AXButton[@AXTitle='Don’t Save']")
        if dontSaveButton:
            dontSaveButton.click()
        else:
            continue
            
def setupTextEdit():
    driver.get("TextEdit")
    createHeaderWindow()
#     createBodyWindowL()
#     createBodyWindowR()
#     createCommentsWindow()

try:
# The demo starts here.
    driver.get("TextEdit")
    
    setupTextEdit()
#     createAllWindows()
    
    showWelcomeText(headerWindow)
#     advertiseScripterFeatures()
#    advertiseDeveloperFeatures()
    
#     demoMouseSpeed()
#     demoMenuBar()
#     demoPopupMenu()
#     demoNativeModifiersInMenus()
#     demoDockItemMenu()
#     demoAXPathRecording()
#     demoCalculatorWithNativeMouse()
#     demoCalculatorWithAXElements()
#     demoCalculatorWithNativeKeys()
#     demoDiagnosticsDirectory()
#     demoScreenShotOnError()
finally:
    driver.get("TextEdit")
#     utilities.closeWithoutSaving(driver, "empty.py")



# quit the webdriver instance
print 'Quitting the WebDriver session'
driver.quit()

