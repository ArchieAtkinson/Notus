#!/bin/python
import os
import sys
import unittest

# Add the parent directory to the sys.path list
scripts_dir_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "scripts"))
sys.path.append(scripts_dir_path)

from find_root import find_root_dir  # Replace 'your_script' with the name of your script

class TestFindRootDir(unittest.TestCase):
    def setUp(self):
        # Create a temporary directory structure for testing
        """
        temp_test_directory/
        |-- prj.conf
        |-- subdir1/
        |   |-- subdir2/
        """
        self.temp_dir = "temp_test_directory"
        os.makedirs(os.path.join(self.temp_dir, "subdir1", "subdir2"), exist_ok=True)
        with open(os.path.join(self.temp_dir, "prj.conf"), "w") as file:
            file.write("prj.conf in root directory")

    def tearDown(self):
        # Remove the temporary directory after testing
        os.system(f"rm -r {self.temp_dir}")

    def test_find_root_dir_in_root(self):
        result = find_root_dir(self.temp_dir)
        self.assertEqual(result, os.path.abspath(self.temp_dir))

    def test_find_root_dir_in_subdir1(self):
        result = find_root_dir(os.path.join(self.temp_dir, "subdir1"))
        self.assertEqual(result, os.path.abspath(self.temp_dir))

    def test_find_root_dir_in_subdir2(self):
        result = find_root_dir(os.path.join(self.temp_dir, "subdir1", "subdir2"))
        self.assertEqual(result, os.path.abspath(self.temp_dir, "subdir1"))

    def test_find_root_dir_not_found(self):
        result = find_root_dir("/nonexistent/directory")
        self.assertIsNone(result)

if __name__ == "__main__":
    unittest.main()