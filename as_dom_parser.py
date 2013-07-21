import re
from library import applescript

class PyDomElement(object):
    def __init__(self, parent, as_type, identifier):
        self.as_type = as_type
        self.identifier = identifier
        self.parent = parent
        self.children = []
    def __repr__(self):
        my_repr =  self.as_type + ' ' + self.identifier
        if self.parent != None:
            return my_repr + ' of ' + repr(self.parent)
        else:
            return my_repr

    def __eq__(self, other):
        if  other == None:
            return False
        if self.as_type != other.as_type:
            return False
        if self.identifier != other.identifier:
            return False
        if self.parent != other.parent:
            return False
        if self.children != other.children:
            return False
        return True

    def jsonify(self):
        child_json = '['
        for child in self.children:
            child_json = child_json + child.jsonify() + ','
        if child_json.endswith(','):
            child_json = child_json[:-1]
        child_json = child_json + ']'
        return '{"type":"%s","id":"%s","children":%s}' % (self.as_type.replace('"', '\\"'), self.identifier.replace('"', '\\"'), child_json)

def parse_dom(content):
    as_element_regex = '([^"]+\s)+("[^"]*"|\d+)'
    py_dom = []
    # split the dom by items
    as_dom = content.split(',')
    as_dom_size = len(as_dom)
    index = 0
    while index < as_dom_size:
        as_dom_item = as_dom[index][:]
        while as_dom_item.count('"') % 2 == 1:  # it's a comma inside a string
            # check for malformed applescript dump
            if (index + 1 >= as_dom_size):
                raise Exception('Invalid AppleScript Record')
            # append the next item because it is part of the same element
            index = index + 1
            as_dom_item = as_dom_item + as_dom[index][:]

        # split the elements in the dom item
        as_dom_elements = as_dom_item.split(' of ') # bug here if " of " is in id or type of dom element
        as_dom_elements.reverse()
        parent = None
        next_py_dom_level = py_dom
        for as_dom_element in as_dom_elements:
            match = re.search(as_element_regex, as_dom_element.strip())
            if match != None:
                # read the values
                as_type = match.group(1).strip()
                identifier = match.group(2).strip()

                # check if object already exists in the dom
                py_dom_object = None
                if parent != None:
                    for py_dom_item in parent.children:
                        if py_dom_item.as_type == as_type and py_dom_item.identifier == identifier:
                            py_dom_object = py_dom_item
                            break

                # create a new object if its new to the dom
                if py_dom_object == None:
                    py_dom_object = PyDomElement(parent, as_type, identifier)

                # check if it already exists
                alreadyExists = False
                for py_dom_item in next_py_dom_level:
                    if py_dom_item.as_type == as_type and py_dom_item.identifier == identifier:
                        alreadyExists = True
                        py_dom_object = py_dom_item
                        break

                # add it to the dom if it does not already exist
                if not alreadyExists:
                    next_py_dom_level.append(py_dom_object)

                # set this element as the parent of the next element
                parent = py_dom_object
                next_py_dom_level = parent.children

        # increment and continue the loop
        index = index + 1
    return py_dom

def dom_for_app(appName):
    script = 'tell application "System Events" to tell process "%s" to return entire contents as text' % appName
    domText = applescript(script).split('{', 1)[1].split('}', 1)[0]
    return parse_dom(domText)

def test():
    dom = dom_for_app('Appium')
    print dom
#test()

