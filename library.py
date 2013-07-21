import os
import subprocess
import tempfile

def run_command(command):
    p = subprocess.Popen(command,
                         stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT)
    return iter(p.stdout.readline, b'')

def applescript(cmd):
    tmpFile = tempfile.mktemp('.applescript','cmd', tempfile.tempdir)
    with open(tmpFile, 'w') as f:
        f.write(cmd)
    output = ''
    for line in run_command(['/usr/bin/osascript', tmpFile]):
        output = output + line
    os.remove(tmpFile)
    return output

def takeScreenshot():
    tmpFile = tempfile.mktemp('.png','capture',tempfile.tempdir)
    subprocess.call(['/usr/sbin/screencapture', tmpFile])
    response = None
    with open(tmpFile, 'r') as f:
        response = f.read()
    os.remove(tmpFile)
    return response