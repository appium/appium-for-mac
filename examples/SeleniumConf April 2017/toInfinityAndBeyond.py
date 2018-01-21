# coding=utf-8

from selenium import webdriver
from selenium.webdriver import ActionChains
from selenium.webdriver.common.keys import Keys
from random import randint
import time
import utilities

driver = utilities.startWebDriverSession('http://localhost:4622/wd/hub')

driver.get("Calculator")

driver.add_cookie({'name': 'mouse_speed', 'value': 40})

def clickElement(xPath):
    element = driver.find_element_by_xpath(xPath)
    ActionChains(driver).move_to_element(element).click().perform()

#AC
clickElement("/AXApplication[@AXTitle='Calculator']/AXWindow[@AXIdentifier='_NS:477' and @AXSubrole='AXStandardWindow']/AXGroup[@AXIdentifier='_NS:696']/AXButton[@AXIdentifier='_NS:752']")

#View Menu
clickElement("/AXApplication[@AXTitle='Calculator']/AXMenuBar[0]/AXMenuBarItem[@AXTitle='View']")

#Scientific Menu Item
clickElement("/AXApplication[@AXTitle='Calculator']/AXMenuBar[0]/AXMenuBarItem[@AXTitle='View']/AXMenu[0]/AXMenuItem[@AXTitle='Scientific']")

#Pi
clickElement("/AXApplication[@AXTitle='Calculator']/AXWindow[@AXIdentifier='_NS:477' and @AXSubrole='AXStandardWindow']/AXGroup[@AXIdentifier='_NS:145']/AXButton[@AXIdentifier='_NS:317']")

#x3
clickElement("/AXApplication[@AXTitle='Calculator']/AXWindow[@AXIdentifier='_NS:477' and @AXSubrole='AXStandardWindow']/AXGroup[@AXIdentifier='_NS:145']/AXButton[@AXIdentifier='_NS:252']")

#ex
clickElement("/AXApplication[@AXTitle='Calculator']/AXWindow[@AXIdentifier='_NS:477' and @AXSubrole='AXStandardWindow']/AXGroup[@AXIdentifier='_NS:145']/AXButton[@AXTitle='e to the x' and @AXIdentifier='_NS:416']")

#10x
clickElement("/AXApplication[@AXTitle='Calculator']/AXWindow[@AXIdentifier='_NS:477' and @AXSubrole='AXStandardWindow']/AXGroup[@AXIdentifier='_NS:145']/AXButton[@AXTitle='ten to the x' and @AXIdentifier='_NS:361']")

time.sleep(3)

#closeButton
clickElement("/AXApplication[@AXTitle='Calculator']/AXWindow[@AXIdentifier='_NS:477' and @AXSubrole='AXStandardWindow']/AXButton[@AXSubrole='AXCloseButton']")


