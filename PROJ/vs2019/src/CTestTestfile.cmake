# CMake generated Testfile for 
# Source directory: F:/code/3rdparty/PROJ/PROJ-master/src
# Build directory: F:/code/3rdparty/PROJ/vs2019/src
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(geodesic-test "F:/code/3rdparty/PROJ/vs2019/bin/Debug/geodtest.exe")
  set_tests_properties(geodesic-test PROPERTIES  _BACKTRACE_TRIPLES "F:/code/3rdparty/PROJ/PROJ-master/src/bin_geodtest.cmake;11;add_test;F:/code/3rdparty/PROJ/PROJ-master/src/bin_geodtest.cmake;0;;F:/code/3rdparty/PROJ/PROJ-master/src/CMakeLists.txt;57;include;F:/code/3rdparty/PROJ/PROJ-master/src/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(geodesic-test "F:/code/3rdparty/PROJ/vs2019/bin/Release/geodtest.exe")
  set_tests_properties(geodesic-test PROPERTIES  _BACKTRACE_TRIPLES "F:/code/3rdparty/PROJ/PROJ-master/src/bin_geodtest.cmake;11;add_test;F:/code/3rdparty/PROJ/PROJ-master/src/bin_geodtest.cmake;0;;F:/code/3rdparty/PROJ/PROJ-master/src/CMakeLists.txt;57;include;F:/code/3rdparty/PROJ/PROJ-master/src/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(geodesic-test "F:/code/3rdparty/PROJ/vs2019/bin/MinSizeRel/geodtest.exe")
  set_tests_properties(geodesic-test PROPERTIES  _BACKTRACE_TRIPLES "F:/code/3rdparty/PROJ/PROJ-master/src/bin_geodtest.cmake;11;add_test;F:/code/3rdparty/PROJ/PROJ-master/src/bin_geodtest.cmake;0;;F:/code/3rdparty/PROJ/PROJ-master/src/CMakeLists.txt;57;include;F:/code/3rdparty/PROJ/PROJ-master/src/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(geodesic-test "F:/code/3rdparty/PROJ/vs2019/bin/RelWithDebInfo/geodtest.exe")
  set_tests_properties(geodesic-test PROPERTIES  _BACKTRACE_TRIPLES "F:/code/3rdparty/PROJ/PROJ-master/src/bin_geodtest.cmake;11;add_test;F:/code/3rdparty/PROJ/PROJ-master/src/bin_geodtest.cmake;0;;F:/code/3rdparty/PROJ/PROJ-master/src/CMakeLists.txt;57;include;F:/code/3rdparty/PROJ/PROJ-master/src/CMakeLists.txt;0;")
else()
  add_test(geodesic-test NOT_AVAILABLE)
endif()
