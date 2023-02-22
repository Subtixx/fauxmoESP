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

LIBS=${LIBS:-"ottowinter/ESPAsyncTCP-esphome esphome/AsyncTCP-esphome ottowinter/ESPAsyncWebServer-esphome"}

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
BOARD_OPTS=$(for b in $BOARDS; do echo -n "--board $b "; done)

LIB_DEPS=$(for l in $LIBS; do echo -n "-l=$l "; done)

cd "$DIR/.."

export PLATFORMIO_EXTRA_SCRIPTS="pre:lib/ci/ci-flags.py"

for d in $EXAMPLES ; do
  echo "*** building example $d for $BOARDS ***"
  pio pkg install --global --no-save $LIB_DEPS
  pio ci $BOARD_OPTS --lib="ci" --lib="src" "examples/$d/$d.ino"
done