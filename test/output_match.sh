#!/bin/bash

# This script match string contents of the error output of a test and the expected outcome and thus conclude the outcome of the test:
#       PASSED: if the content of the 2 files match
#       FAILED: if the content of the 2 files do not match
#       SKIPPED: if the expected outcome file is not found

FORMAT_RESET="\x1b[0m";
BOLD_ON="\x1b[1m";
FORMAT_GREEN_TEXT="\x1b[32;1m";
FORMAT_RED_TEXT="\x1b[31;1m";
FORMAT_YELLOW_TEXT="\x1b[33;1m";
FORMAT_GREY_TEXT="\x1b[90m";

error_output=$(cat $2); 
if [ ! -f "$3" ]
then
        echo -e "${FORMAT_GREY_TEXT}File: ${FORMAT_RESET}${BOLD_ON}$1${FORMAT_RESET}${FORMAT_GREY_TEXT}, Result: ${FORMAT_YELLOW_TEXT}SKIPPED${FORMAT_RESET}"
else
        expected_output=$(cat $3);
        if [ "$error_output" == "$expected_output" ];
        then 
                echo -e "${FORMAT_GREY_TEXT}File: ${FORMAT_RESET}${BOLD_ON}$1${FORMAT_RESET}${FORMAT_GREY_TEXT}, Result: ${FORMAT_GREEN_TEXT}PASSED${FORMAT_RESET}"
        else
                echo -e "${FORMAT_GREY_TEXT}File: ${FORMAT_RESET}${BOLD_ON}$1${FORMAT_RESET}${FORMAT_GREY_TEXT}, Result: ${FORMAT_RED_TEXT}FAILED${FORMAT_RESET}"
        fi;

fi

