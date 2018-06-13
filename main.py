'''
Created on 16.05.2018

@author: fpawlowski
'''

import os, sys
import platform
import win32file
import win32con
from ntsecuritycon import FILE_LIST_DIRECTORY
from argparse import ArgumentParser

ACTIONS = {
    1 : "Created",
    2 : "Deleted",
    3 : "Updated",
    4 : "Renamed from something",
    5 : "Renamed to something"
}
FILE_LIST_DIRECTORY = 0x0001

def watch_directory(args):
    params = parse_args(args)
    if params['path'] is None:
        print('Path is None')
        return
    path_to_watch = fix_filepath(params['path'])
    hDir = win32file.CreateFile (
        path_to_watch,
        FILE_LIST_DIRECTORY,
        win32con.FILE_SHARE_READ | win32con.FILE_SHARE_WRITE | win32con.FILE_SHARE_DELETE,
        None,
        win32con.OPEN_EXISTING,
        win32con.FILE_FLAG_BACKUP_SEMANTICS,
    None
    )
    while 1:
        #
        # ReadDirectoryChangesW takes a previously-created
        # handle to a directory, a buffer size for results,
        # a flag to indicate whether to watch subtrees and
        # a filter of what changes to notify.
        #
        results = win32file.ReadDirectoryChangesW (
            hDir,
            1024,
            True,
            win32con.FILE_NOTIFY_CHANGE_FILE_NAME |
             win32con.FILE_NOTIFY_CHANGE_DIR_NAME |
             win32con.FILE_NOTIFY_CHANGE_ATTRIBUTES |
             win32con.FILE_NOTIFY_CHANGE_SIZE |
             win32con.FILE_NOTIFY_CHANGE_LAST_WRITE |
             win32con.FILE_NOTIFY_CHANGE_SECURITY,
            None,
            None
        )
        for action, file in results:
            full_filename = os.path.join (path_to_watch, file)
            print full_filename, ACTIONS.get (action, "Unknown")

def fix_filepath(path):
    '''Fixes the path under windows'''
    if platform.system() == 'Windows':
        if "/" in path:
            path_split = path.split("/")
            path = os.sep.join(path_split)
        if path.startswith('.'):
            abs_path = os.path.abspath(os.path.dirname(__file__))
            path = os.path.join(abs_path, path)
    return path
        

def parse_args(argv):
    '''Parses input arguments'''
    print('Command line arguments are:')
    print(argv)
    
    parser = ArgumentParser()
    parser.add_argument('--path', default=None, help='Path of the directory that should be monitored')
    
    parsed_args = parser.parse_args(argv)
    params = vars(parsed_args)
    return params

if __name__ == '__main__':
    #params = parse_args(sys.argv[1:])
    watch_directory(params)
