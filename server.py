from base64 import b64encode
from bottle import Bottle, request, response, redirect
from bottle import run, static_file

import as_dom_parser
import library

app = Bottle()
local_storage = {}
app.SESSION_ID = '123456'
app.started = False

@app.route('/wd/hub/status', method='GET')
def status():
    status = {'sessionId': app.SESSION_ID if app.started else None,
              'status': 0,
              'value': {'build': {'version': 'Appium for Mac 1.0'}}}
    return status

@app.route('/wd/hub/session', method='POST')
def create_session():
    app.started = True
    redirect('/wd/hub/session/%s' % app.SESSION_ID)

@app.route('/wd/hub/session/<session_id>', method='GET')
def get_session(session_id=''):
    app_response = {'sessionId': session_id,
                'status': 0,
                'value': {"version":"5.0",
                          "webStorageEnabled":False,
                          "locationContextEnabled":False,
                          "browserName":"Mac",
                          "platform":"MAC",
                          "javascriptEnabled":True,
                          "databaseEnabled":False,
                          "takesScreenshot":True}}
    return app_response

@app.route('/wd/hub/session/<session_id>', method='DELETE')
def delete_session(session_id=''):
    app.started = False
    app_response = {'sessionId': session_id,
                'status': 0,
                'value': {}}
    return app_response

@app.route('/wd/hub/session/<session_id>/screenshot', method='GET')
def screenshot(session_id=''):
    try:
        encoded_file = b64encode(library.takeScreenshot())
        return {'sessionId': session_id, 'status': 0, 'value': encoded_file}
    except:
        response.status = 400
        return {'sessionId': session_id, 'status': 13, 'value': 'Could not take screenshot'}

@app.route('/wd/hub/session/<session_id>/source', method='GET')
def source(session_id=''):
    dom = as_dom_parser.dom_for_app('Appium')
    json = dom[0].jsonify()
    return {'sessionId': session_id, 'status': 0, 'value': json}

run(app, host='localhost', port=8080, debug=True)