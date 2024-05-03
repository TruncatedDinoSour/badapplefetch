#!/usr/bin/env bash

set -eu

main() {
    echo '#include <stdint.h>'
    echo 'const char*const frames[]={'

    for frame in $(ls ./frames/ | sort -n); do
        content=$(<"./frames/$frame")

        escaped_content=${content//\\/\\\\}           # escape backslashes
        escaped_content=${escaped_content//\"/\\\"}   # escape double quotes
        escaped_content=${escaped_content//$'\n'/\\n} # escape newlines

        printf '"%s",' "$escaped_content"
    done

    echo "};const uint16_t frames_size=sizeof(frames)/sizeof(frames[0]);"
}

main
