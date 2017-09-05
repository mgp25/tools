import os
import glob
import platform
from plistlib import load

###############################
#    iPhone Pairing Detector  #
#            mgp25            #
#      github.com/mgp25       #
#          mgp25.com          #
###############################

path_macos = '/private/var/db/lockdown'
path_win = '%programdata%/Apple/Lockdown'
path = ''

if platform.system() == 'Windows':
    path = path_win
elif platform.system() == 'Darwin':
    path = path_macos

os.chdir(path)
for file in glob.glob('*.plist'):
    with open(file, 'rb') as fp:
        plist = load(fp)
        if 'WiFiMACAddress' in plist.keys():
            print('Device found! UDID: ' + plist['HostID'])
            print('WiFi Mac Address: ' + plist['WiFiMACAddress'] + '\n')

print('SystemBUID: ' + plist['SystemBUID'])
