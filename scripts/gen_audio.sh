#!/usr/bin/env sh

set -eu

main() {
    echo "#include <stdint.h>"
    echo "const unsigned char audio[] = {"
    od -An -vtu1 <audio.ogg | awk '{for (i=1; i<=NF; i++) printf "0x%s, ", $i}'
    echo "};"
    echo "const uint64_t audio_size = sizeof(audio);"
}

main
