# CMake generated Testfile for 
# Source directory: /home/marc_gorgy/lab-cs2-project
# Build directory: /home/marc_gorgy/lab-cs2-project/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(AppLogicTests "/home/marc_gorgy/lab-cs2-project/build/test_applogic")
set_tests_properties(AppLogicTests PROPERTIES  _BACKTRACE_TRIPLES "/home/marc_gorgy/lab-cs2-project/CMakeLists.txt;112;add_test;/home/marc_gorgy/lab-cs2-project/CMakeLists.txt;0;")
add_test(ServerLogicTests "/home/marc_gorgy/lab-cs2-project/build/test_server_logic")
set_tests_properties(ServerLogicTests PROPERTIES  _BACKTRACE_TRIPLES "/home/marc_gorgy/lab-cs2-project/CMakeLists.txt;129;add_test;/home/marc_gorgy/lab-cs2-project/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
