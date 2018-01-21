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
clickElement()

#View Menu
clickElement()

#Scientific Menu Item
clickElement()

#Pi
clickElement()

#x3
clickElement()

#ex
clickElement()

#10x
clickElement()

time.sleep(3)

#closeButton
clickElement()


