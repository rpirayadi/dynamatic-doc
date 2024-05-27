#!/bin/bash

source "$1"/tools/dynamatic/scripts/utils.sh

# ============================================================================ #
# Variable definitions
# ============================================================================ #

# Script arguments
DYNAMATIC_DIR=$1
OUTPUT_DIR=$2
KERNEL_NAME=$3

# Generated directories/files
HDL_DIR="$OUTPUT_DIR/hdl"

# Shortcuts
COMP_DIR="$OUTPUT_DIR/comp"
LSQ_JAR="$DYNAMATIC_DIR/bin/generators/lsq-generator.jar"

# ============================================================================ #
# HDL writing flow
# ============================================================================ #

# Reset output directory
rm -rf "$HDL_DIR" && mkdir -p "$HDL_DIR"

# Convert DOT graph to VHDL
"$DYNAMATIC_DIR/bin/export-vhdl" $KERNEL_NAME "$COMP_DIR/$KERNEL_NAME.dot" \
  "$HDL_DIR"
exit_on_fail "Failed to convert DOT to VHDL" "Converted DOT to VHDL"

# Generate LSQs
for lsq_config in $HDL_DIR/lsq*_config.json; do
  # Skip non-existent files (the loop will iterate over
  # $HDL_DIR/lsq*_config.json if there are no LSQs)
  if [ ! -e "$lsq_config" ]; then continue; fi

  # Run the LSQ generator
  java -jar -Xmx7G "$LSQ_JAR" --target-dir "$HDL_DIR" --spec-file \
    "$lsq_config" > /dev/null
  exit_on_fail "Failed to generate LSQs"
done

echo_info "HDL generation succeeded"
