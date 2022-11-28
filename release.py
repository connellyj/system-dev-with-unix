# Julia Connelly
# 605.614 System Development in the UNIX Environment
# release.py
# Script to package a project release.
# -b will create a binary release, -s a source release.
# Source release will perform a make clean prior to packaging.

import argparse
import os
import sys
import tarfile

dirname = 'homework4'

# only keep the project directory and any files in bin
def binary_filter(tar_info):
    if tar_info.name.endswith(dirname):
        return tar_info
    elif dirname + '/bin' in tar_info.name:
        return tar_info
    else:
        return None

# input is not support in Python 2        
input_func = input
if sys.version[0] < '3':
    input_func = raw_input
    
parser = argparse.ArgumentParser(description='Package ' + dirname + '.')
action = parser.add_mutually_exclusive_group(required=True)
action.add_argument('-b', action='store_true', help='Package a binary release.')
action.add_argument('-s', action='store_true', help='Package a source release.')
args = parser.parse_args()

filename = dirname
if args.b:
    val = input_func('Requested a binary release. Continue? Y/N: ')
    if not val.lower() == 'y':
        print("Exiting")
        sys.exit(0)
    hostname = input_func('Enter the system hostname: ')
    filename += "_" + hostname
else:
    val = input_func('Requested a source release. Continue? Y/N: ')
    if not val.lower() == 'y':
        print("Exiting")
        sys.exit(0)
filename += ".tar"

dir = os.path.dirname(os.path.realpath(__file__))
if os.path.exists(filename):
    print(filename + ' already exists, exiting')
    sys.exit(1)
    
if args.s:
    os.system('make clean')

print('Compressing release to ' + filename)
with tarfile.open(filename, 'w:gz') as tfile:
    if args.b:
        tfile.add(dir, dirname, filter=binary_filter)
    else:
        tfile.add(dir, dirname)
    
print('Done')
