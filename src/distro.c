#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "distro.h"

extern const char *logos[][2];
extern const uint16_t logos_size;

static int strncasecmp_custom(const char *s1, const char *s2, size_t n) {
    if (n == 0)
        return 0;

    while (n-- != 0 && tolower(*s1) == tolower(*s2)) {
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;
        s1++;
        s2++;
    }

    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

const char *get_distro_logo(const char *name) {
    uint16_t logo;

    for (logo = 0; logo < logos_size; ++logo)
        if (strncasecmp_custom(name, logos[logo][0], strlen(logos[logo][0])) ==
            0)
            return logos[logo][1];

    return "\033[37m,d88b.d88b,\n"
           "\033[31m88888888888\n"
           "\033[32m`Y8888888Y´\n"
           "\033[33m  `Y888Y´\n"
           "\033[34m    `Y´";
}
