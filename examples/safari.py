from selenium import webdriver
from time import sleep

print 'Starting the WebDriver session'
driver = webdriver.Remote( command_executor='http://localhost:4622/wd/hub', desired_capabilities={'platform':'Mac'})

print 'Opening the "Safari" app'
driver.get("Safari")

print 'Finding the Text Area'
search_field = driver.find_element_by_id('WEB_BROWSER_ADDRESS_AND_SEARCH_FIELD')

print 'Typing a URL'
search_field.send_keys('http://appium.io')

sleep(1)
search_field_text = search_field.text
print 'Got Text: ' + search_field_text

if search_field_text == 'http://appium.io':
  print 'URL was entered correctly'
else:
  print 'URL was not entered correctly'

# quit the webdriver instance
print 'Quitting webdriver\n'
driver.quit()
