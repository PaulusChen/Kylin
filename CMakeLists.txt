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
aux_source_directory(net KYLIN_NET_SRC)

set(PROJECT_SRC_FILE
  ${KYLIN_SRC_SRC}
  ${KYLIN_UTILITY_SRC}
  ${KYLIN_NET_SRC})

link_directories(
  ${PROJECT_LIBS_DIR}/gtest/lib
  ~/.local/lib
  )

set(TEST_REGEX_ST "^.+_testing\\.(h|H|hcc|HCC|c|C|cc|CC|cpp|CPP)$")

foreach(srcFile ${PROJECT_SRC_FILE})
  if("${srcFile}" MATCHES "${TEST_REGEX_ST}")
    set(TESTING_CODE_FILE ${TESTING_CODE_FILE} ${srcFile})
    message(STATUS "TESTING FILE : " ${srcFile})
  else("${srcFile}" MATCHES "${TEST_REGEX_ST}")
    set(CODE_FILE ${CODE_FILE} ${srcFile})
    message(STATUS "CODE FILE : " ${srcFile})
  endif("${srcFile}" MATCHES "${TEST_REGEX_ST}")
endforeach(srcFile)

add_definitions("-Wno-unused-result -Wsign-compare -fwrapv -g -Wall -DDEBUG")
#add_definitions("-Wno-unused-result -Wsign-compare -fwrapv -Wall -O2")
#add_definitions("-Wall -Xlinker -export-dynamic -fstack-protector-all")

add_library(kylin ${CODE_FILE})

add_executable(kylin_testing ${TESTING_CODE_FILE})

set(PUB_LIBRARIES libgtest.a pthread dl util m curl)
target_link_libraries(kylin_testing kylin ${PUB_LIBRARIES})