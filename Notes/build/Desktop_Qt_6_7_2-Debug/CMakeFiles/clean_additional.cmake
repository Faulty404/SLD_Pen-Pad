# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Notes_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Notes_autogen.dir/ParseCache.txt"
  "Notes_autogen"
  )
endif()
