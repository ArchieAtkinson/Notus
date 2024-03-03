#!/bin/python
import os
import sys
import unittest

def find_root_dir(directory):
    while directory and directory != "/":
        prj_conf_path = os.path.join(directory, "prj.conf")
        if os.path.exists(prj_conf_path):
            return os.path.abspath(directory)
        directory = os.path.dirname(directory)
    return None

if __name__ == "__main__":
    current_directory = os.getcwd()
    result = find_root_dir(current_directory)
    if result:
        print(f"{result}")
    else:
        print("prj.conf not found in any parent directory.")
