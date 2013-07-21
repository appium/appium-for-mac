from selenium import webdriver

# create a webdriver instance
command_url = 'http://localhost:8080/wd/hub'

print '\nconnecting to web driver @ ' + command_url
driver = webdriver.Remote(
            command_executor='http://127.0.0.1:8080/wd/hub',
            desired_capabilities={ 'browserName': 'Mac', 'platform': 'Mac', 'version': '1.0'})

# print page_source
print driver.page_source

# quit the webdriver instance
print 'Quitting webdriver\n'
driver.quit()