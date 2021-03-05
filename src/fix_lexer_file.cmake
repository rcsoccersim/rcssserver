#!/usr/bin/cmake

# This file contains the fix for the legacy code

if(NOT DEFINED GENERATED_FILE_PATH)
    message(FATAL_ERROR "Missing arguments for the fix_lexer_file script: GENERATED_FILE_PATH")
elseif(NOT DEFINED CORRECT_HEADER_NAME)
    message(FATAL_ERROR "Missing arguments for the fix_lexer_file script: CORRECT_HEADER_NAME")
elseif(NOT DEFINED OUTPUT_FILE_PATH)
    message(FATAL_ERROR "Missing arguments for the fix_lexer_file script: OUTPUT_FILE_PATH")
endif()

# Replace #include <FlexLexer.h> in generated lexer files
file(READ ${GENERATED_FILE_PATH} FILE_CONTENTS)
string(REPLACE
    "#include <FlexLexer.h>" # String to match
    "#include \"${CORRECT_HEADER_NAME}\"" # With what to replace
    FILE_CONTENTS # Where to put the result
    "${FILE_CONTENTS}" # Input
)
file(WRITE ${OUTPUT_FILE_PATH} "${FILE_CONTENTS}")
