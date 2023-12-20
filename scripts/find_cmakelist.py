#!/bin/python
import os
import sys
import unittest

def find_cmake_lists_file(directory):
    while directory and directory != "/":
        cmake_lists_path = os.path.join(directory, "CMakeLists.txt")
        if os.path.exists(cmake_lists_path):
            return os.path.abspath(directory)
        directory = os.path.dirname(directory)
    return None

if __name__ == "__main__":
    current_directory = os.getcwd()
    result = find_cmake_lists_file(current_directory)
    if result:
        print(f"{result}")
    else:
        print("CMakeLists.txt not found in any parent directory.")
