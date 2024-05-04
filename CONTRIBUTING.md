# Contribution guidelines

This document defines the contribution guidelines of this project.

## Adding a new logo

If you want to add a new logo, please keep these things in mind:

-   The file name is case-insensitive, but it should be lowercase
-   The file name defines the distribution of the OS
-   The file name is matched case-insensitively and with a starts with function, meaning a string like "arch" would match "Arch Linux"
-   In the content of the logo only the `\033` escape (ANSI escape) is interpreted. Everything else is treated as literal
-   The logo must be small

## Changing/adding code

-   Please stick to the formatting, use autoformatters (such as clang-format)
-   Test your code
-   Stick to the C99 standard
-   Your code must also compile and fully work with the following flags set:

```sh
export STRIP=llvm-strip
export STRIPFLAGS='--strip-debug --strip-sections --strip-unneeded -T --remove-section=.note.gnu.gold-version --remove-section=.note --strip-all --discard-locals --remove-section=.gnu.version --remove-section=.eh_frame --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --strip-symbol=__gmon_start__ --strip-all-gnu --remove-section=.comment --remove-section=.eh_frame_ptr --discard-all'
export CC=clang
export CFLAGS='-Wpedantic -flto=full -fno-trapping-math -fstrict-aliasing -fno-math-errno -fno-stack-check -fno-strict-overflow -funroll-loops -fno-stack-protector -fvisibility-inlines-hidden -mfancy-math-387 -fomit-frame-pointer -fstrict-overflow -Wshadow -fuse-ld=lld -s -fno-exceptions -D_FORTIFY_SOURCE=0 -Wall -Wextra -fno-signed-zeros -fno-strict-aliasing -pedantic -Ofast -fvisibility=hidden -ffast-math -funsafe-math-optimizations -std=c99 -fno-asynchronous-unwind-tables -Werror -fdiscard-value-names -femit-all-decls -fmerge-all-constants -fno-use-cxa-atexit -fno-use-init-array -march=native -mtune=native -pedantic-errors'
export LIBMPV=1
```

`NOQA` must not be set.

```sh
make purge
make -j$(nproc --all)
make strip
./badapplefetch  # Should run the whole length of the song without crashing
```
