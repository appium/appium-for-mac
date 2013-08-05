from selenium import webdriver
from random import randint

print 'Starting the WebDriver session'
driver = webdriver.Remote( command_executor='http://localhost:8080/wd/hub', desired_capabilities={'platform':'Mac'})

print 'Opening the "Calculator" app'
driver.get("Calculator")

rand1 = randint(0,100)
rand2 = randint(0,100)

print 'Finding Some Elements...'
eclear = driver.find_element_by_name('C')
eplus = driver.find_element_by_name('+')
eequals = driver.find_element_by_name('=')

print 'Clearing the calculator'
eclear.click()

print 'Entering the first number'
for num in str(rand1):
  driver.find_element_by_name(num).click()

print 'Clicking the "+" button'
eplus.click()

print 'Entering the second number'
for num in str(rand2):
  driver.find_element_by_name(num).click()

print 'Clicking the "=" button'
eequals.click()

print 'Page Source'
source = driver.page_source
print source

print 'Reading result from screen'
els = driver.find_elements_by_tag_name('AXGroup')
result_text_element = els[0].find_element_by_tag_name('AXStaticText')
answer = result_text_element.text

if int(answer) == (rand1 + rand2):
  print 'Correct Result: ' + answer
else:
  print 'Incorect Result: ' + answer

# quit the webdriver instance
print 'Quitting webdriver\n'
driver.quit()
