from selenium import webdriver
from time import sleep

print 'Starting the WebDriver session'
driver = webdriver.Remote( command_executor='http://localhost:4622/wd/hub', desired_capabilities={'platform':'Mac'})

print 'Opening the "TextEdit" app'
driver.get("TextEdit")

print 'Finding the Text Area'
text_field = driver.find_element_by_id('First Text View')

print 'Typing some text'
text_field.send_keys('http://appium.io')

sleep(1)
text_field_text = text_field.text
print 'Got Text: ' + text_field_text

if text_field_text == 'http://appium.io':
  print 'Text was entered correctly'
else:
  print 'Text was not entered correctly'

# quit the webdriver instance
print 'Quitting webdriver\n'
driver.quit()
