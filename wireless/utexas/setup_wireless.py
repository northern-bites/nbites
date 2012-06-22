#!/usr/bin/env python

import os, shutil, subprocess

def runRemoteCommand(ip,command):
  subprocess.check_call('ssh nao@%s "%s"' % (ip,command),shell=True)

def copyFile(ip,filename,dest=''):
  subprocess.check_call('scp %s nao@%s:%s' % (filename,ip,dest),shell=True)

def copyFileAndMove(ip,filename,dest):
  tmpDest = os.path.basename(filename) + '.tmp'
  copyFile(ip,filename,tmpDest)
  return 'mv %s %s\n' % (tmpDest,dest)

def removeTempDir(d):
  shutil.rmtree(d)

def makeTempDir():
  d = os.path.join(os.path.expandvars('./tmp'))
  os.mkdir(d)
  return d

def filloutID(name,teamID,robotID,subnet,d):
  with open('./%s' % name,'r') as f:
    contents = f.read()
  contents = contents.replace('${TEAM_NUM}',teamID)
  contents = contents.replace('${ID}',robotID)
  contents = contents.replace('${BASE}',subnet)
  with open(os.path.join(d,name),'w') as f:
    f.write(contents)

def filloutField(name,field,d):
  with open('./%s' % name,'r') as f:
    contents = f.read()
  contents = contents.replace('${FIELD}',field)
  with open(os.path.join(d,name),'w') as f:
    f.write(contents)

def setupNetworking(ip,teamID,robotID,field,subnet,d):
  filloutID('utwired',teamID,robotID,subnet,d)
  filloutID('utwireless',teamID,robotID,subnet,d)
  filloutField('wpa_supplicant.conf',field,d)

  script = '#!/bin/bash\n\n'
  script += copyFileAndMove(ip,os.path.join(d,'wpa_supplicant.conf'),'/etc/wpa_supplicant.conf')
  script += copyFileAndMove(ip,os.path.join(d,'utwired'),'/etc/init.d/utwired')
  script += copyFileAndMove(ip,os.path.join(d,'utwireless'),'/etc/init.d/utwireless')
  script += 'chmod +x /etc/init.d/utwired\n'
  script += 'rc-config add utwireless boot\n'
  script += 'rc-config add utwired boot\n'
  script += 'chmod +x /etc/init.d/utwireless\n'
  script += 'rc-config delete connman boot\n'
  script += 'killall wpa_supplicant\n'
  script += '/etc/init.d/utwireless stop\n'
  script += '/etc/init.d/utwireless start\n'

  scriptName = 'setupScript.sh'
  scriptPath = os.path.join(d,scriptName)
  

  with open(scriptPath,'w') as f:
    f.write(script)
  copyFile(ip,scriptPath)
  print 'Moving a bunch of stuff around, removing connman, etc.'
  print 'You need to put in the password "root" after it prompts for the password'
  subprocess.check_call('ssh -t nao@%s "su -c \'bash %s\'"' % (ip,scriptName),shell=True)
  runRemoteCommand(ip,'rm %s' % scriptName)

if __name__ == '__main__':
  import sys
  usage = 'setup_wireless.py currentIP teamID robotID field desiredWiredSubnet(in the form 11.0.1'
  helpStrings = ['-h','--help']
  args = sys.argv[1:]
  for s in helpStrings:
    if s in args:
      print usage
      sys.exit(0)
  if len(args) != 5:
    print usage
    sys.exit(1)
  currentIP = args[0]
  teamID = args[1]
  robotID = args[2]
  field = args[3]
  subnet = args[4]
  try:
    d = makeTempDir()
    print 'setting up networking'
    setupNetworking(currentIP,teamID,robotID,field,subnet,d)
  finally:
    removeTempDir(d)
