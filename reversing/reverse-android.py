import sys
import subprocess
import os.path
from subprocess import call

if len(sys.argv) < 2:
    print("usage: reverse-apk.py <apk file>")
    sys.exit()

apk_file = sys.argv[1]
apk_name = apk_file.split('.')[0]

if not os.path.isfile(apk_file):
    print("File does not exist")
    sys.exit()

# Get JAR file
print("Extracting JAR file from apk...\n")
os.system("sh dex2jar.sh "+apk_file+" > /dev/null")

# Extract the JAR file:
print("Extracting .class files from apk...\n")
os.system("mv "+apk_name+"_dex2jar.jar app.zip")
os.system("unzip app.zip -d "+apk_name+" > /dev/null")
os.system("rm app.zip")

# Decompile .class to .java
path = apk_name+"/com"
path = os.path.normpath(path)
res = []
for root,dirs,files in os.walk(path, topdown=True):
    depth = root[len(path) + len(os.path.sep):].count(os.path.sep)
    if depth == 0:
        # We're currently two directories in, so all subdirs have depth 3
        res += [os.path.join(root, d) for d in dirs]
        dirs[:] = [] # Don't recurse any deeper

for directory in res:
    print(str(res.index(directory)+1)+". "+directory.split(apk_name+"/com/")[1])

number = int(input("Which path to decompile? [Number]: "))
while number > len(res) and number > 0:
    print("That option does not exist")
    number = int(input("Which path to decompile? [Number]: "))


subprocess.call(['/bin/bash', '-i', '-c', "cd "+res[number-1]+" && decompileandroid"])
os.system("cd "+res[number-1]+" && mv src ../../../src")
os.system("rm -r "+apk_name)
subprocess.call(['/bin/bash', '-i', '-c', "apktool d "+apk_file])
os.system("cd src/com && mv "+res[number-1]+" ../../"+res[number-1])
