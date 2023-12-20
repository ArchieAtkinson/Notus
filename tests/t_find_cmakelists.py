#!/bin/python
import os
import sys
import unittest

# Add the parent directory to the sys.path list
scripts_dir_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "scripts"))
sys.path.append(scripts_dir_path)

from find_cmakelist import find_cmake_lists_file


class TestFindCMakeListsFile(unittest.TestCase):
    def setUp(self):
        # Create a temporary directory structure for testing
        """
        temp_test_directory/
        |-- CMakeLists.txt
        |-- subdir1/
        |   |-- CMakeLists.txt
        |   |-- subdir2/
        |       |-- CMakeLists.txt
        """
        self.temp_dir = "temp_test_directory"
        os.makedirs(os.path.join(self.temp_dir, "subdir1", "subdir2"), exist_ok=True)
        with open(os.path.join(self.temp_dir, "CMakeLists.txt"), "w") as file:
            file.write("CMakeLists.txt in root directory")
        with open(os.path.join(self.temp_dir, "subdir1", "CMakeLists.txt"), "w") as file:
            file.write("CMakeLists.txt in subdir1")
        with open(os.path.join(self.temp_dir, "subdir1", "subdir2", "CMakeLists.txt"), "w") as file:
            file.write("CMakeLists.txt in subdir2")

    def tearDown(self):
        # Remove the temporary directory after testing
        os.system(f"rm -r {self.temp_dir}")

    def test_find_cmake_lists_file_in_root(self):
        result = find_cmake_lists_file(self.temp_dir)
        self.assertEqual(result, os.path.abspath(self.temp_dir))

    def test_find_cmake_lists_file_in_subdir1(self):
        result = find_cmake_lists_file(os.path.join(self.temp_dir, "subdir1"))
        self.assertEqual(result, os.path.abspath(os.path.join(self.temp_dir, "subdir1")))

    def test_find_cmake_lists_file_in_subdir2(self):
        result = find_cmake_lists_file(os.path.join(self.temp_dir, "subdir1", "subdir2"))
        self.assertEqual(result, os.path.abspath(os.path.join(self.temp_dir, "subdir1", "subdir2")))

    def test_find_cmake_lists_file_not_found(self):
        result = find_cmake_lists_file("/nonexistent/directory")
        self.assertIsNone(result)

if __name__ == "__main__":
    unittest.main()
