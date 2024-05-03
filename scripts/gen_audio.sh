#!/usr/bin/env sh

set -eu

main() {
    echo "#include <stdint.h>"
    echo "const unsigned char audio[] = {"
    xxd -i <audio.ogg
    echo "};const uint64_t audio_size = sizeof(audio);"
}

main
