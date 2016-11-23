from selenium import webdriver
from selenium.webdriver import ActionChains

defaultImplicitTimeout_sec = 20


#   Convenience method that catches exceptions.
#   returns: found element, or None
def findElementByXPath(driver, xPath):
    print "findElementByXPath: " + xPath
    returnedElement = None
    try:
        returnedElement = driver.find_element_by_xpath(xPath)
    finally:
        print "findElementByXPath returnedElement: " + str(returnedElement)
        return returnedElement


def menuBarAXPath(applicationTitle):
    return "/AXApplication[@AXTitle='" + applicationTitle + "']/AXMenuBar"

# Utility method to select a menu item from the application menu bar.
# driver: a WebDriver instance
# applicationTitle: the application title string, e.g. "Calculator"
# menuItemTitles: an array of strings
#   minimum (to click in the menu bar):     [menuBarItemTitle]
#   other (to select menu bar menu items):  [menuBarItemTitle, menuItemTitle, subMenuItemTitle, ...]
# lastItemOperation: optional string and boolean operator to match one additional attribute of the final menu item
#   If unwanted, use an empty string.
#   for example:
#       selectMenuItem(driver, "Calculator", ["View", "Show Thousands Separators"], " and @AXMenuItemMarkChar!=''")
#   for example:
#       selectMenuItem(driver, "Calculator", ["File", "Quit Calculator"], " or @AXTitle='Quit and Close All Windows'")
# returns: absolute AXPath string of the selected item
def selectMenuItem(driver, applicationTitle, menuItemTitles, lastItemOperation):
    if len(menuItemTitles) < 1:
        return ""
    
    # When selecting menu items, we don't need to wait. 
    driver.implicitly_wait(1)
    
    elementAXPath = ""
    
    # Start with the menu bar item which has its own role.
    menuBarItemAXPath = menuBarAXPath(applicationTitle) + "/AXMenuBarItem[@AXTitle='" + menuItemTitles[0] + "']"
    menuBarItem = findElementByXPath(driver, menuBarItemAXPath)
    if menuBarItem == None:
        return ""
    actions = ActionChains(driver)
    actions.move_to_element_with_offset(menuBarItem, 20, 10)
    actions.click().perform()

    # The menu is now showing. If we don't find and click the last menu item, the menu will remain open.
    # In that case we will have to click the menu bar item again, to hide the menu before returning.

    # Now iterate through the menuItems and subMenuItems.
    parentAXPath = menuBarItemAXPath
    parentElement = menuBarItem
    
    lastIndex = len(menuItemTitles)
    for titleIndex in range(1, lastIndex):
        elementAXPath = parentAXPath + "/AXMenu/AXMenuItem[@AXTitle='" + menuItemTitles[titleIndex] + "'"
        
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
        if element.location['x'] <= parentElement.location['x']:
            # The (sub)menu item is either to the left or under its parent menu item. 
            # Move the mouse to the left of its parent menu item first.
            # This avoids passing over other menu items, which can change the highlighted menu item.
            actions.move_to_element_with_offset(parentElement, 1, 10)
            actions.move_to_element_with_offset(element, 20, 10)
        else:
            # The (sub)menu item is to the right of its parent menu item. 
            # Move the mouse to the right of its parent menu item first.
            # This avoids passing over other menu items, which can change the highlighted menu item.
            actions.move_to_element_with_offset(parentElement, parentElement.size['width'] - 1, 10)
            actions.move_to_element_with_offset(element, 20, 10)
        actions.click().perform()
        
        parentAXPath = elementAXPath
        parentElement = element
    if elementAXPath == "":
        # Since we didn't click the element, click in the menu bar item to close the menu.
        ActionChains(driver).click(menuBarItem).perform()
    
    driver.implicitly_wait(defaultImplicitTimeout_sec)
    return elementAXPath
