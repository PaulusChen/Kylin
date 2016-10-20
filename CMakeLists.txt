#Added by chenpeng 2016-10-18

cmake_minimum_required(VERSION 2.8.11)

project(Kylin)

set(PROJECT_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

include_directories(Kylin
  ${PROJECT_LIBS_DIR}/gtest
  ~/.local/include
  )

aux_source_directory(utility KYLIN_UTILITY_SRC)
aux_source_directory(src KYLIN_SRC_SRC)

set(KYLIN_SRC ${KYLIN_SRC_SRC} ${KYLIN_UTILITY_SRC})

link_directories(
  ${PROJECT_LIBS_DIR}/gtest/lib
  ~/.local/lib
  )

set(TEST_REGEX_ST "^.+_testing\\.(h|H|hcc|HCC|c|C|cc|CC|cpp|CPP)$")

foreach(srcFile ${KYLIN_SRC})
  if("${srcFile}" MATCHES "${TEST_REGEX_ST}")
    set(KYLIN_TESTING_FILE ${KYLIN_TESTING_FILE} ${srcFile})
    message(STATUS "TESTING FILE : " ${srcFile})
  else("${srcFile}" MATCHES "${TEST_REGEX_ST}")
    set(KYLIN_CODE_FILE ${KYLIN_CODE_FILE} ${srcFile})
    message(STATUS "CODE FILE : " ${srcFile})
  endif("${srcFile}" MATCHES "${TEST_REGEX_ST}")
endforeach(srcFile)

add_definitions("-Wno-unused-result -Wsign-compare -fwrapv -g -Wall -DDEBUG")
#add_definitions("-Wno-unused-result -Wsign-compare -fwrapv -Wall -O2")
#add_definitions("-Wall -Xlinker -export-dynamic -fstack-protector-all")

add_library(kylin ${KYLIN_CODE_FILE})

add_executable(kylin_testing ${KYLIN_TESTING_FILE})

set(PUB_LIBRARIES libgtest.a pthread dl util m curl)
target_link_libraries(kylin_testing kylin ${PUB_LIBRARIES})