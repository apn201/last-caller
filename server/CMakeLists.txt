cmake_minimum_required(VERSION 3.20)
project(tinybbs_server LANGUAGES C CXX)

# The shared core builds against Linux Arduino look-alikes. HTTP/WebSocket and
# Telnet bindings are added in phases 3 and 4.
find_path(MBEDTLS_INCLUDE_DIR mbedtls/sha256.h REQUIRED)
find_library(MBEDCRYPTO_LIBRARY mbedcrypto REQUIRED)
find_path(CIVETWEB_INCLUDE_DIR civetweb.h REQUIRED)
find_library(CIVETWEB_LIBRARY civetweb REQUIRED)

add_library(tinybbs-miniz STATIC platform/miniz_tinfl.c)
target_include_directories(tinybbs-miniz PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/platform)

add_executable(tinybbs-phase1-smoke phase1_smoke.cpp)
target_compile_features(tinybbs-phase1-smoke PRIVATE cxx_std_17)
target_include_directories(tinybbs-phase1-smoke PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/platform
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
  ${MBEDTLS_INCLUDE_DIR}
)
target_link_libraries(tinybbs-phase1-smoke PRIVATE tinybbs-miniz ${MBEDCRYPTO_LIBRARY})
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(tinybbs-phase1-smoke PRIVATE pthread)
endif()

# Like the ESP32 firmware, the server is one translation unit: bbs_core.h
# defines its deliberately file-local state and main.cpp includes http.cpp.
add_executable(tinybbs-server main.cpp)
target_compile_features(tinybbs-server PRIVATE cxx_std_17)
target_include_directories(tinybbs-server PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/platform
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
  ${MBEDTLS_INCLUDE_DIR}
  ${CIVETWEB_INCLUDE_DIR}
)
target_link_libraries(tinybbs-server PRIVATE tinybbs-miniz ${MBEDCRYPTO_LIBRARY} ${CIVETWEB_LIBRARY})
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(tinybbs-server PRIVATE pthread)
endif()

# ---- Property and logic tests (queue item Q12) ----------------------------------------
# Host tests of the shared logic headers (src/core/*_logic.h). They include no Arduino
# or Linux headers, so they build and run the same here as on the stick's PC. Each test
# that takes card files gets the card's own, so a broken story file fails the build's
# `ctest`. The last test is the story lint (tools/story_lint.py, needs python3).
enable_testing()
set(TB_STORY ${CMAKE_CURRENT_SOURCE_DIR}/../sd/BBS/STORY)
file(GLOB TB_SCENES ${TB_STORY}/SCENES/*.INI)
set(TB_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../tools/test)
function(tb_logic_test name)
  add_executable(${name} ${TB_TEST_DIR}/${name}.cpp)
  target_compile_features(${name} PRIVATE cxx_std_17)
  target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/core)
  target_compile_options(${name} PRIVATE -Wall -Wextra)
  add_test(NAME ${name} COMMAND ${name} ${ARGN})
endfunction()
tb_logic_test(egg_logic_test)
tb_logic_test(power_logic_test)
tb_logic_test(modinfo_logic_test)
tb_logic_test(haunt_logic_test ${TB_STORY}/TRIGGERS.INI)
tb_logic_test(manifest_logic_test ${TB_STORY}/MANIFEST.INI)
tb_logic_test(ghost_logic_test ${TB_STORY}/GHOSTS.INI ${TB_STORY}/SYSOP.INI)
tb_logic_test(scene_logic_test ${TB_SCENES})
tb_logic_test(shell_logic_test ${TB_STORY}/SHELL.INI)
tb_logic_test(story_property_test ${TB_STORY}/TRIGGERS.INI ${TB_STORY}/MANIFEST.INI)
find_program(TB_PYTHON NAMES python3 python)
if(TB_PYTHON)
  add_test(NAME story_lint
           COMMAND ${TB_PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/../tools/story_lint.py --card ${CMAKE_CURRENT_SOURCE_DIR}/../sd/BBS)
endif()
