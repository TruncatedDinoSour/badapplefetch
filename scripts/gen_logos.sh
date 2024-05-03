#!/usr/bin/env bash

set -eu

main() {
    echo '#include <stdint.h>'
    echo 'const char*logos[][2]={'

    for logo in $(ls ./logos/ | sort -n); do
        content=$(<"./logos/$logo")

        escaped_content=${content//'\033'/$'\033'}   # escape newlines
        escaped_content=${escaped_content//\\/\\\\}   # escape backslashes
        escaped_content=${escaped_content//\"/\\\"}   # escape double quotes
        escaped_content=${escaped_content//$'\n'/\\n} # escape newlines

        printf '{"%s","%s"},' "$logo" "$escaped_content"
    done

    echo "};const uint16_t logos_size=sizeof(logos)/sizeof(logos[0]);"
}

main
