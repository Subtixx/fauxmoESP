#!/bin/bash
#
# compile examples with platformio for various boards. This script
# is usually run by the CI, but can also be run locally. Only dependency
# is platformio.
#
# usage:
#   [BOARDS=boards] [EXAMPLES=examples] ./ci-compile
#
# e.g.
#  $ ./compile-ci
#         - compile all board/examples combinations
#
#  $ BOARDS="esp32 esp01" EXAMPLES=Blink ./compile-ci
#         - compile only Blink example for the esp32 and esp8266 platforms
#
set -eou pipefail

# List of examples that will be compiled by default
EXAMPLES=${EXAMPLES:-"Basic External_Server"}

# list of boards to compile for by default
BOARDS=${BOARDS:-"esp32dev esp01"}

LIBS=${LIBS:-"ottowinter/ESPAsyncTCP-esphome esphome/AsyncTCP-esphome ottowinter/ESPAsyncWebServer-esphome luc-github/ESP32SSDP bblanchon/ArduinoJson"}

PROJECT_OPTIONS=${PROJECT_OPTIONS:-"build_unflags=-std=gnu++11 build_flags=-std=gnu++14 build_flags=-Wall build_flags=-Wno-unused-function"}

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BOARD_OPTS=$(for b in $BOARDS; do echo -n "--board $b "; done)

LIB_DEPS=$(for l in $LIBS; do echo -n "-l=$l "; done)

PROJECT_OPTS=$(for l in $PROJECT_OPTIONS; do echo -n "--project-option=$l "; done)

cd "$DIR/.."

rm -f messages.txt

echo "*** generating compile_commands.json ***"
pio run -t compiledb > /dev/null 2>> messages.txt

if [ $? -ne 0 ]; then
  echo "Failed to generate compile_commands.json"
fi

for d in $EXAMPLES ; do
  echo "*** installing dependencies for $d ***"
  pio pkg install --global --no-save $LIB_DEPS > /dev/null 2>> messages.txt

  if [ $? -ne 0 ]; then
    echo "Failed to install dependencies for $d"
  fi

  echo "*** building example $d for $BOARDS ***"
  pio ci $BOARD_OPTS $PROJECT_OPTS --lib="ci" --lib="src" "examples/$d/$d.ino" > /dev/null 2>> messages.txt

  if [ $? -ne 0 ]; then
    echo "Failed to build example $d for $BOARDS"
  fi

  echo "*** building example $d for $BOARDS with ArduinoJson ***"
  pio ci $BOARD_OPTS $PROJECT_OPTS --project-option="build_flags=-DUSE_ARDUINO_JSON" --lib="ci" --lib="src" "examples/$d/$d.ino" > /dev/null 2>> messages.txt

  if [ $? -ne 0 ]; then
    echo "Failed to build example $d for $BOARDS with ArduinoJson"
  fi
done

# Run extract_messages.py
echo "*** running extract_messages.py ***"
python3 scripts/extract_messages.py messages.txt annotations.json