#!/bin/python
import os
import sys
import json
from pathlib import Path

def main(file_path):
    file_content = None
    
    with open(file_path, 'r') as f:
        file_content = f.read()

    file_content = file_content.replace('-fno-freestanding', '')
        
    with open(file_path, 'w') as f:
        f.write(file_content)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Removes the -fno-freestanding flag from the compilation database")
        print("Usage: remove_fno-freestanding.py <compilation_database_path>")
    else:
        main(sys.argv[1])
