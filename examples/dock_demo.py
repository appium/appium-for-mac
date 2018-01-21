# coding=utf-8

from selenium import webdriver
from selenium.webdriver import ActionChains
from selenium.webdriver.common.keys import Keys
from random import randint
import time
import utilities

global driver

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


sysPrefSearchField = 0
magChkbx = 0
magSlider = 0
showHideChkbx = 0
positionBtn = 0
minimizePopUp = 0

def openDockSettings():
    print 'openDockSettings'
    utilities.openSystemPreferencesPane(driver, "Dock")

def configureDockSettings():
    print 'configureDockSettings'
#    magChkbx = driver.find_element_by_xpath(basicGroupPath + "/AXButton[@AXDescription='clear']")
#    magSlider = driver.find_element_by_xpath(basicGroupPath + "/AXButton[@AXDescription='add']")
#    showHideChkbx = driver.find_element_by_xpath(basicGroupPath + "/AXButton[@AXDescription='equals']")
#    positionBtn = driver.find_element_by_xpath(resultGroupPath + "/AXStaticText[@AXDescription='main display']")
#    minimizePopUp = driver.find_element_by_xpath(resultGroupPath + "/AXStaticText[@AXDescription='main display']")
#     utilities.selectMenuItemFromMenuBar(driver, "System Preferences", ["Quit System Preferences"], "")


def restoreDockSettings():
    print 'restoreDockSettings'
#     openDockSettings()
#    magChkbx = driver.find_element_by_xpath(basicGroupPath + "/AXButton[@AXDescription='clear']")
#    magSlider = driver.find_element_by_xpath(basicGroupPath + "/AXButton[@AXDescription='add']")
#    showHideChkbx = driver.find_element_by_xpath(basicGroupPath + "/AXButton[@AXDescription='equals']")
#    positionBtn = driver.find_element_by_xpath(resultGroupPath + "/AXStaticText[@AXDescription='main display']")
#    minimizePopUp = driver.find_element_by_xpath(resultGroupPath + "/AXStaticText[@AXDescription='main display']")
    utilities.selectMenuItemFromMenuBar(driver, "System Preferences", ["Quit System Preferences"], "")

def performDemo():
    print 'Starting the Dock demo'
    try:
        utilities.selectMenuItemFromDock(driver, "System Preferences", ["Time Machine"], "")
#         openDockSettings()
#         configureDockSettings()
    except Exception as e:
        print 'Exception in dock_demo: ', e
#     finally:
#         restoreDockSettings()
#         utilities.selectMenuItemFromMenuBar(driver, "System Preferences", ["Quit System Preferences"], "")

performDemo()