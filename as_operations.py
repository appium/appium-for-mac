from library import applescript

def frontmost_application():
    script = '''
    tell application "Finder"
	set appPath to the path to the frontmost application
	set appName to the name of file appPath
	set appName to text 1 thru ((offset of "." in appName) - 1) of appName
    end tell'''
    return applescript(script)