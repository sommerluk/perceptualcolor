#!/bin/bash

# Run doxygen, but only show errors
doxygen > /dev/null

CODEDIRECTORIES="include src test"

# Search for some patterns that should not be used in the source code
# Do not use the “code” and “endcode” tags for Doxygen documentation.
# Use @snippet instead!
grep --fixed-strings --recursive "\\code" $CODEDIRECTORIES
grep --fixed-strings --recursive "\\endcode" $CODEDIRECTORIES
grep --fixed-strings --recursive "@code" $CODEDIRECTORIES
grep --fixed-strings --recursive "@endcode" $CODEDIRECTORIES
