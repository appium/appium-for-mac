#coding=utf-8

from selenium import webdriver
from selenium.webdriver import ActionChains
from selenium.webdriver.common.keys import Keys
import time

defaultLoopDelay_sec = 1.00
defaultCommandDelay_sec = 0.100
defaultImplicitTimeout_sec = 5.000
defaultMouseSpeed = 40
defaultScreenShotOnError = True
defaultGlobalDiagnosticsDirectory = '~/Desktop/'
defaultCookies = [
    {'name': 'loop_delay', 'value': defaultLoopDelay_sec},
    {'name': 'command_delay', 'value':defaultCommandDelay_sec },
    {'name': 'implicit_timeout', 'value': defaultImplicitTimeout_sec},
    {'name': 'mouse_speed', 'value': defaultMouseSpeed},
    {'name': 'screen_shot_on_error', 'value': True},
    {'name': 'global_diagnostics_directory', 'value': defaultGlobalDiagnosticsDirectory}
]
defaultCookies = [
    {'name': 'loop_delay', 'value': defaultLoopDelay_sec},
    {'name': 'command_delay', 'value':defaultCommandDelay_sec },
    {'name': 'implicit_timeout', 'value': defaultImplicitTimeout_sec},
    {'name': 'mouse_speed', 'value': defaultMouseSpeed}
]
desiredCapabilities = {'platform': 'Mac', 'cookies': defaultCookies}

# Convenience method to start up a web driver session.
# Returns: WebDriver instance
def startWebDriverSession(hostURL):
    capabilities = desiredCapabilities
    print 'Starting the WebDriver session at ' + hostURL + 'capabilities:'
    #print capabilities
    try:
        driver = webdriver.Remote( command_executor=hostURL, desired_capabilities=capabilities)
    except Exception as e:
        print 'Exception in startWebDriverSession:', e
        print 'Make sure AppiumForMac is running.'
        return;

    return driver

# Convenience method to start up a web driver session.
# Returns: WebDriver instance
def startWebDriverSessionCookies(hostURL, cookies):
    desiredCapabilities = {'platform': 'Mac', 'cookies': cookies}
    capabilities = desiredCapabilities
    print 'Starting the WebDriver session at ' + hostURL + 'capabilities:'
    #print capabilities
    try:
        driver = webdriver.Remote( command_executor=hostURL, desired_capabilities=capabilities)
    except Exception as e:
        print 'Exception in startWebDriverSession:', e
        print 'Make sure AppiumForMac is running.'
        return;
    
    return driver
# Convenience method that catches exceptions.
def clickElement(element, useNativeEvents):
    if useNativeEvents > 0:
        # move and click the mouse like a user
        actions = ActionChains(driver)
        actions.click(element)
        actions.perform()
    else:
        # use the traditional accessibility action
        element.click()

#   Convenience method to check existence immediately without using implicit timeout.
#   returns: found element, or None
def existsElementByXPath(driver, xPath):
    returnedElement = None
    try:
        driver.implicitly_wait(0)
        returnedElement = driver.find_element_by_xpath(xPath)
    finally:
        driver.implicitly_wait(defaultImplicitTimeout_sec)
        return returnedElement

#   Convenience method that catches exceptions.
#   returns: found element, or None
def findElementByXPath(driver, xPath):
    returnedElement = None
    try:
        returnedElement = driver.find_element_by_xpath(xPath)
    finally:
        return returnedElement


def menuBarAXPath(applicationTitle):
    if len(applicationTitle) > 0:
        return "/AXApplication[@AXTitle='" + applicationTitle + "']/AXMenuBar"
    else:
        return "/AXApplication/AXMenuBar"

# Private: you should use selectMenuItemFromMenuBar, selectMenuItemFromDock or selectMenuItemFromPopUpButton. 
# Utility method to select a menu item (or submenu items) from a menu. 
#   *** The menu must be showing on screen already before calling this method. ***
# driver: a WebDriver instance
# parentUIElement: the AXMenu (or some other PFUIElement with AXMenuItem children)
# parentAXPath: absolute AXPath of the AXMenu (or some other PFUIElement with AXMenuItem children)
# menuItemTitles: an array of strings  [menuItemTitle (, subMenuItemTitle ...)]
# lastItemOperation: optional string and boolean operator to match one additional attribute of the final menu item
#   If unwanted, use an empty string.
#   e.g. to select a menu item, but only if it has a checkmark:    " and @AXMenuItemMarkChar!=''"
#   e.g. to select a menu item that might has an alternate title:  " or @AXTitle='Quit and Close All Windows'")
# returns: absolute AXPath string of the selected item, or "" if not found
def selectMenuItemFromUIElement(driver, parentAXPath, menuItemTitles, lastItemOperation):
    if len(menuItemTitles) < 1:
        return ""
    
    # When selecting menu items from a menu, we don't need to wait very long for elements to appear.
    driver.implicitly_wait(0.5)
    
    parentUIElement = findElementByXPath(driver, parentAXPath)    
    elementAXPath = ""
    
    # Iterate through the menuItems and subMenuItems.
    lastIndex = len(menuItemTitles)
    for titleIndex in range(0, lastIndex):
        # The parent may be an AXMenu, or it may be something like AXFocusedElement,
        # so don't add AXMenu until we get into sub-menus.
        if titleIndex > 0:
            parentAXPath = parentAXPath + "/AXMenu"

        elementAXPath = parentAXPath + "/AXMenuItem[@AXTitle='" + menuItemTitles[titleIndex] + "'"
        
        # Append the lastItemOperation to the last element
        if titleIndex == lastIndex - 1 and type(lastItemOperation) is str and len(lastItemOperation) > 0:
            elementAXPath = elementAXPath + lastItemOperation
        
        elementAXPath = elementAXPath + "]"
        element = findElementByXPath(driver, elementAXPath)
        if element == None:
            elementAXPath = ""
            break
        
        eSize = element.size
        eLocation = element.location
        actions = ActionChains(driver)
        if element.location['x'] <= parentUIElement.location['x']:
            # The (sub)menu item is either to the left or under its parent menu item. 
            # Move the mouse to the left of its parent menu item first.
            # This avoids passing over other menu items, which can change the highlighted menu item.
            # (Don't do this on the first menu item, because our caller already did it.)
            if titleIndex > 0:
                actions.move_to_element_with_offset(parentUIElement, 1, 10)
            actions.move_to_element_with_offset(element, 20, 10)
        else:
            # The (sub)menu item is to the right of its parent menu item. 
            # Move the mouse to the right of its parent menu item first.
            # This avoids passing over other menu items, which can change the highlighted menu item.
            # (Don't do this on the first menu item, because our caller already did it.)
            if titleIndex > 0:
                actions.move_to_element_with_offset(parentUIElement, parentUIElement.size['width'] - 1, 10)
            actions.move_to_element_with_offset(element, 20, 10)
        actions.click().perform()
        
        parentAXPath = elementAXPath
        parentUIElement = element

    driver.implicitly_wait(defaultImplicitTimeout_sec)
    
    # If we did not select from the visible menu, press esc to cancel menu selection.
    if len(elementAXPath) == 0:
        #print "len(elementAXPath) == 0. Pressing escape."
        ActionChains(driver).send_keys(Keys.ESCAPE).perform()
    
    return elementAXPath


# Convenience method to select a menu item from the menu bar.
# driver: a WebDriver instance
# menuBarItemTitle: (required) the menu bar item title string, e.g. "View", or "File"
# menuItemTitles: an array of strings, can be empty
# lastItemOperation: optional string and boolean operator to match one additional attribute of the final menu item
#   If unwanted, use an empty string.
# returns: absolute AXPath string of the selected item, or "" if not found.
#
# e.g. to click the menu bar item (or simply find the menu bar item element and click it!):
#   selectMenuItemFromMenuBar(driver, "Help", [], "")
# e.g. to select a menu item every time:
#   selectMenuItemFromMenuBar(driver, "View", ["Show Thousands Separators"], "")
# e.g. to select a menu item, but only if it has a checkmark:
#   selectMenuItemFromMenuBar(driver, "View", ["Show Thousands Separators"], " and @AXMenuItemMarkChar!=''")
# e.g. to select a menu item that might have an alternate title:
#   selectMenuItemFromMenuBar(driver, "File", ["Quit Calculator"], " or @AXTitle='Quit and Close All Windows'")
#
def selectMenuItemFromMenuBar(driver, menuBarItemTitle, menuItemTitles, lastItemOperation):    
    elementAXPath = ""
    
    # Start with the menu bar item which has its own role.
    menuBarItemAXPath = menuBarAXPath("") + "/AXMenuBarItem[@AXTitle='" + menuBarItemTitle + "']"
    menuBarUIElement = findElementByXPath(driver, menuBarItemAXPath)
    if menuBarUIElement == None:
        return ""
    ActionChains(driver).move_to_element_with_offset(menuBarUIElement, 20, 10).click().perform()
    
    menuBarItemMenuAXPath = menuBarItemAXPath + "/AXMenu"
    menuBarMenuUIElement = findElementByXPath(driver, menuBarItemMenuAXPath)
    
    # The menu is now showing. If we don't find and click the last menu item, the menu will remain open.
    # In that case we will have to click the menu bar item again, to hide the menu before returning.
    
    # Now iterate through the menuItems and subMenuItems.
    elementAXPath = selectMenuItemFromUIElement(driver, menuBarItemMenuAXPath, menuItemTitles, lastItemOperation)
        
    return elementAXPath


# Convenience method to select a menu item from a dock item contextual menu.
# driver: a WebDriver instance
# dockItemTitle: the dock item title string, e.g. "Calculator"
# menuItemTitles: an array of one or more strings, can be empty
# lastItemOperation: optional string and boolean operator to match one additional attribute of the final menu item
#   If unwanted, use an empty string.
# returns: absolute AXPath string of the selected item, or "" if not found.
#
# e.g. to click the Dock item (or simply find the dock item element and click it!):
#   selectMenuItemFromDock(driver, "Finder", [], "")
# e.g. to select a menu item every time:
#   selectMenuItemFromDock(driver, "System Preferences", ["Options", "Show in Finder"], "")
# e.g. to select a menu item, but only if it has a checkmark:
#   selectMenuItemFromDock(driver, "System Preferences", ["Options", "Keep in Dock"], " and @AXMenuItemMarkChar!=''")
# e.g. to select a menu item that might have an alternate title:
#   selectMenuItemFromDock(driver, "System Preferences", ["Hide"], " or @AXTitle='Hide Others'")
#
def selectMenuItemFromDock(driver, dockItemTitle, menuItemTitles, lastItemOperation):    
    elementAXPath = ""
    
    # Start with the dock item which has its own role. Use control-click
    # e.g. "/AXApplication[@AXTitle='Dock']/AXList[0]/AXDockItem[@AXTitle='System Preferences']/AXMenu[0]/AXMenuItem[@AXTitle='Options']/AXMenu[0]/AXMenuItem[@AXTitle='Show in Finder']"
    dockItemAXPath = "/AXApplication[@AXTitle='Dock']/AXList[0]/AXDockItem[@AXTitle='" + dockItemTitle + "']"
    dockItemAXPath = "/AXApplication[@AXTitle='Dock']/AXList[0]/AXDockItem[@AXTitle='System Preferences' and @AXSubrole='AXApplicationDockItem']"
    dockItemUIElement = findElementByXPath(driver, dockItemAXPath)
    if dockItemUIElement == None:
        return ""
    ActionChains(driver).context_click(dockItemUIElement).perform()
    
    # The dock item popup menu is now showing. If we don't find and click the last menu item, the menu will remain open.
    # In that case we will have to click the dock item again, to hide the menu before returning.

    focusedUIElementAXPath = "/AXApplication[@AXTitle='Dock']/AXFocusedUIElement"

    # Now iterate through the menuItems and subMenuItems.
    elementAXPath = selectMenuItemFromUIElement(driver, focusedUIElementAXPath, menuItemTitles, lastItemOperation)
    
    return elementAXPath


# Convenience method to select a menu item from a popUp button.
# driver: a WebDriver instance
# popUpButtonAXPath: absolute AXPath of the button
# menuItemTitles: an array of strings, can be empty
# lastItemOperation: optional string and boolean operator to match one additional attribute of the final menu item
#   If unwanted, use an empty string.
# returns: absolute AXPath string of the selected item
#
# popUpButtonAXPath = "/AXApplication[@AXTitle='System Preferences']/AXWindow[@AXTitle='Language & Region']/AXGroup[0]/AXPopUpButton[0]",
# e.g. to click the popUp (or simply find the popUp element and click it!):
#   selectMenuItemFromPopUpButton(driver, popUpButtonAXPath, [], "")
# e.g. to select a menu item every time:
#   selectMenuItemFromPopUpButton(driver, popUpButtonAXPath, ["Africa", "Cameroon"], "")
# e.g. to select a menu item, but only if it does not have a checkmark:
#   selectMenuItemFromPopUpButton(driver, popUpButtonAXPath, ["Americas", "United States"], " and @AXMenuItemMarkChar==''")
# e.g. to select a menu item that might have an alternate title:
#   selectMenuItemFromPopUpButton(driver, popUpButtonAXPath, ["Hide"], " or @AXTitle='Hide Others'")
#
def selectMenuItemFromPopUpButton(driver, popUpButtonAXPath, menuItemTitles, lastItemOperation):    
    elementAXPath = ""
    
    # Click the popup to display its menu.
    popUpButtonUIElement = findElementByXPath(driver, popUpButtonAXPath)
    if popUpButtonUIElement == None:
        return ""
    ActionChains(driver).click(popUpButtonUIElement).perform()
    
    # The popUp button menu is now showing. If we don't find and click the last menu item, the menu will remain open.
    # In that case we will have to click the dock item again, to hide the menu before returning.
    
    # Now iterate through the menuItems and subMenuItems.
    elementAXPath = selectMenuItemFromUIElement(driver, popUpButtonAXPath, menuItemTitles, lastItemOperation)
    
    return elementAXPath

# Convenience method to open System Preferences to the pane with the given title.
# If title is empty, just open the application.
def openSystemPreferencesPane(driver, pane):    
    selectMenuItemFromMenuBar(driver, "Apple", ["System Preferences…"], "")
    window = findElementByXPath(driver, "/AXApplication[@AXTitle='System Preferences']/AXWindow[0]")
    driver.switch_to.window
    driver.set_window_position(100, 100, "current")
    if pane:
        ActionChains(driver).key_down(Keys.COMMAND).send_keys('f').key_up(Keys.COMMAND).perform()
        # select any existing text before typing
        ActionChains(driver).key_down(Keys.COMMAND).send_keys('a').key_up(Keys.COMMAND).send_keys(pane + Keys.RETURN).perform()
    else:
        # Show All Preferences
        ActionChains(driver).key_down(Keys.COMMAND).send_keys('l').key_up(Keys.COMMAND).perform()

# Convenience method to close windows and click Don't Save until there are no more windows.
# lastWindow: the name of this window that will stop this method when it is in front. Supply "" to close all windows.
def closeWithoutSaving(driver):
    #while True:
    frontWindow = findElementByXPath(driver, "/AXApplication/AXWindow[0]")
    fwTitle = frontWindow.get_attribute("AXTitle")
    closeButton = existsElementByXPath(driver, "/AXApplication/AXWindow[0]/AXButton[@AXSubrole='AXCloseButton']")
    if closeButton:
        selectMenuItemFromMenuBar(driver, "File", ["Close"], "")
        time.sleep(1)
        dontSaveButton = existsElementByXPath(driver, "/AXApplication/AXWindow[0]/AXSheet[0]/AXButton[@AXTitle='Don’t Save']")
        if dontSaveButton:
            #dontSaveButton.click()
            ActionChains(driver).click(dontSaveButton).perform()
        

#def closeWithoutSaving(driver):
#    selectMenuItemFromMenuBar(driver, "TextEdit", ["Quit TextEdit"], "")
#    time.sleep(2)
#    dontSaveButton = existsElementByXPath(driver, "/AXApplication/AXWindow[0]/AXSheet[0]/AXButton[@AXTitle='Don’t Save']")
#    if dontSaveButton:
#        #dontSaveButton.click()
#        ActionChains(driver).click(dontSaveButton).perform()
    






