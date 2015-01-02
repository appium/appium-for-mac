from selenium import webdriver
from random import randint

def numToString(num):
  if num == 0:
    return "_NS:9"
  elif num == 1:
    return "_NS:32"
  elif num == 2:
    return "_NS:43"
  elif num == 3:
    return "_NS:54"
  elif num == 4:
    return "_NS:65"
  elif num == 5:
    return "_NS:76"
  elif num == 6:
    return "_NS:87"
  elif num == 7:
    return "_NS:98"
  elif num == 8:
    return "_NS:109"
  elif num == 9:
    return "_NS:120"
  else:
    return ""

print 'Starting the WebDriver session'
driver = webdriver.Remote( command_executor='http://localhost:4622/wd/hub', desired_capabilities={'platform':'Mac'})

print 'Opening the "Calculator" app'
driver.get("Calculator")

print 'Selecting the 1st Window'
driver.switch_to_window("1")

rand1 = randint(0,100)
rand2 = randint(0,100)

print 'Finding Some Elements...'
eclear = driver.find_element_by_id('_NS:751')
eplus = driver.find_element_by_id('_NS:131')
eequals = driver.find_element_by_id('_NS:698')

print 'Clearing the calculator'
eclear.click()

print 'Entering the first number'
for num in str(rand1):
  n = numToString(int(num))
  print str(num) + ' --> ' + str(n)
  driver.find_element_by_id(n).click()

print 'Clicking the "+" button'
eplus.click()

print 'Entering the second number'
for num in str(rand2):
  n = numToString(int(num))
  print str(num) + ' --> ' + str(n)
  driver.find_element_by_id(n).click()

print 'Clicking the "=" button'
eequals.click()

#print 'Page Source'
#source = driver.page_source
#print source

print 'Reading result from screen'
#result_text_element = driver.find_element_by_xpath('//AXStaticText[@AXDescription="main display"]')
result_text_element = driver.find_element_by_id('_NS:490')
print '****'
print result_text_element
answer = result_text_element.text

if int(answer) == (rand1 + rand2):
  print 'Correct Result: ' + answer
else:
  print 'Incorect Result: ' + answer

# quit the webdriver instance
print 'Quitting webdriver\n'
driver.quit()
