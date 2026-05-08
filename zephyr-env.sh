#!/usr/bin/env bash
# source this from zsh/bash to activate Zephyr build env in any terminal:
#   source ~/dev/zephyr/zephyr-env.sh
ZEPHYR_WORKSPACE="$( cd -- "$( dirname -- "${BASH_SOURCE[0]:-${(%):-%N}}" )" &> /dev/null && pwd )"
export ZEPHYR_BASE="$ZEPHYR_WORKSPACE/zephyr"
export ZEPHYR_SDK_INSTALL_DIR="/Users/dkkang/zephyr-sdk-1.0.1"
# shellcheck disable=SC1091
source "$ZEPHYR_WORKSPACE/.venv/bin/activate"
echo "Zephyr env active: $ZEPHYR_BASE"
echo "  west: $(command -v west)"
echo "  SDK : $ZEPHYR_SDK_INSTALL_DIR"
