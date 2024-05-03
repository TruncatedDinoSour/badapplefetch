#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>

#include "fps.h"

#include "stats.h"
#include "distro.h"

static void printf_b(uint64_t art_line, const char *format, ...)
    __attribute__((format(printf, 2, 3)));

#define BUFFER_SIZE  1024
#define NUM_PACKAGES 11
#define pfb(...)     printf_b(art_line, __VA_ARGS__)

#ifndef PATH
#define PATH "/usr/bin/"
#endif /* PATH */

#define duration(t) (t) / 3600, ((t) % 3600) / 60, (t) % 60

extern const unsigned short frames_size;

static const char *const releases[] = {"/etc/os-release", "/etc/lsb-release",
                                       "/etc/release"};

static char *pkg_commands[NUM_PACKAGES][8] = {
    {PATH "q", "qlist", "-I", NULL},
    {PATH "dnf", "list", "installed", NULL},
    {PATH "dpkg-query", "-f", "${binary:Package}\\n", "-W", NULL},
    {PATH "nix-store", "-q", "--requisites", "/run/current-system/sw", NULL},
    {PATH "pacman", "-Qq", NULL},
    {PATH "rpm", "-qa", "--last", NULL},
    {PATH "xbps-query", "-l", NULL},
    {PATH "bonsai", "list", NULL},
    {PATH "apk", "info", NULL},
    {PATH "pkg", "list-installed", NULL},
    {PATH "snap", "list", NULL},
};

static const char *const quotes[] = {
    "Your heart is a beacon of love and kindness.",
    "Unexpected joy is coming your way. Keep smiling!",
    "Adventure awaits you around the corner.",
    "Beauty surrounds you because you create it.",
    "Expect the unexpected and cherish the predictable.",
    "The stars shine brighter when you smile.",
    "Life is sweet, savor every moment.",
    "Your laugh is music to others' ears.",
    "Kindness is a currency that can cover any debt.",
    "A journey of a thousand miles starts with a single step.",
    "The best is yet to come!",
    "Your spirit is as boundless as the ocean.",
    "Happiness is a journey, not a destination.",
    "You will accomplish great things in unique ways.",
    "Dream, explore, discover!",
    "Your journey will lead you to hidden treasures.",
};

static const char *quote = NULL;

static void printf_b(uint64_t art_line, const char *format, ...) {
    va_list args;
    va_start(args, format);

    printf("\033[%zuC\033[J ", art_line);
    vprintf(format, args);
    printf("\033[1B\r");

    va_end(args);
}

char *stat_get_name(void) {
    uint64_t idx;
    char buf[BUFFER_SIZE];
    char *pretty_name = NULL, *pos, *start, *end;
    FILE *file        = NULL;

    for (idx = 0; idx < sizeof(releases) / sizeof(releases[0]); ++idx) {
        file = fopen(releases[idx], "r");
        if (file != NULL)
            break;
    }

    if (file == NULL)
        return NULL;

    while (fgets(buf, sizeof(buf), file)) {
        pos = strstr(buf, "PRETTY_NAME");

        if (!pos)
            continue;

        pos += 11; /* len("PRETTY_NAME") */

        if (*pos != '=')
            continue;

        pos++;

        if (*pos == '\'' || *pos == '\"') {
            start = pos + 1;
            end   = strchr(start, *pos);
        } else {
            start = pos;

            for (end = pos; *end && *end != '\n'; end++)
                ;
        }

        if (!end || end == start)
            continue;

        pretty_name = malloc((size_t)(end - start + 1));

        if (pretty_name) {
            memcpy(pretty_name, start, (size_t)(end - start));
            pretty_name[(size_t)(end - start)] = '\0';
        }

        break;
    }

    fclose(file);

    return pretty_name;
}

char *stat_get_shell(void) {
    char *shell = getenv("SHELL");
    return shell == NULL && *shell != '\0' ? shell : strrchr(shell, '/') + 1;
}

uint64_t stat_get_package_count(void) {
    uint64_t count = 0;
    uint64_t idx, jdx;

    pid_t cpid;
    int pipefd[2];
    char buf[BUFFER_SIZE];

    for (idx = 0; idx < NUM_PACKAGES; ++idx) {
        if (pipe(pipefd))
            return 0;

        cpid = fork();

        if (cpid == -1)
            return 0;
        else if (cpid == 0) {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            execve(pkg_commands[idx][0], pkg_commands[idx], NULL);
            _exit(2);
        } else {
            close(pipefd[1]);

            while (read(pipefd[0], buf, BUFFER_SIZE) > 0)
                for (jdx = 0; jdx < BUFFER_SIZE; ++jdx)
                    if (buf[jdx] == '\n')
                        ++count;

            close(pipefd[0]);
            wait(NULL);
        }
    }

    return count;
}

uint64_t stat_get_uptime(void) {
    uint64_t uptime = 0.0;
    FILE *f;

    f = fopen("/proc/uptime", "r");

    if (f == NULL)
        return 0;

    fscanf(f, "%zu", &uptime);

    if (ferror(f)) {
        fclose(f);
        return 0;
    }

    fclose(f);

    return uptime;
}

double stat_get_cpu_freq(const uint8_t core_count) {
    double sum_freq = 0.0;
    FILE *fp_core;

    char freq_file[128];
    char line[128];

    uint8_t core;
    double core_freq;

    for (core = 0; core < core_count; ++core) {
        snprintf(freq_file, 128,
                 "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_cur_freq",
                 core);

        fp_core = fopen(freq_file, "r");

        if (fp_core != NULL) {
            if (fgets(line, sizeof(line), fp_core)) {
                core_freq = strtod(line, NULL);
                sum_freq += core_freq;
            }

            fclose(fp_core);
        }
    }

    return sum_freq / core_count;
}

unsigned char Memory_init(Memory *memory) {
    uint64_t value;
    char line[256];
    FILE *fp_m = fopen("/proc/meminfo", "r");

    if (fp_m == NULL)
        return 1;

    while (fgets(line, sizeof(line), fp_m))
        if (sscanf(line, "MemTotal: %lu kB", &value) == 1)
            memory->total = value;
        else if (sscanf(line, "MemAvailable: %lu kB", &value) == 1)
            memory->used = memory->total - value;

    fclose(fp_m);
    return 0;
}

unsigned char CPU_init(CPU *cpu) {
    FILE *fp_cpu = fopen("/proc/cpuinfo", "r");
    char line[256];

    if (fp_cpu == NULL)
        return 1;

    cpu->core_count = 0;

    while (fgets(line, sizeof(line), fp_cpu))
        if (strncmp(line, "model name", 10) == 0) {
            strncpy(cpu->model, strchr(line, ':') + 2, 64);
            cpu->model[strcspn(cpu->model, "\n")] = 0;
        } else if (strncmp(line, "processor", 9) == 0 && cpu->core_count < 255)
            cpu->core_count += 1;

    fclose(fp_cpu);

    cpu->freq = stat_get_cpu_freq(cpu->core_count);

    return 0;
}

unsigned char OsInfo_init(OsInfo *os) {
    os->name = stat_get_name();

    if (uname(&os->uts) != 0)
        return 1;

    os->pw        = getpwuid(geteuid());
    os->pkg_count = stat_get_package_count();
    os->shell     = stat_get_shell();
    os->term      = getenv("TERM");
    os->uptime    = stat_get_uptime();
    os->logo      = get_distro_logo(os->name);
    Memory_init(&os->memory);
    CPU_init(&os->cpu);

    return 0;
}

unsigned char OsInfo_update(OsInfo *os) {
    os->uptime = stat_get_uptime();
    Memory_init(&os->memory);
    CPU_init(&os->cpu);
    return 0;
}

void OsInfo_delete(OsInfo *os) { free(os->name); }

#define stats_info(label, format, ...) \
    pfb(label " \033[1m~\033[0m \033[37m" format "\033[0m", __VA_ARGS__);

void stats_print_beside(const OsInfo *os,
                        const uint64_t art_line,
                        const uint16_t cur_frame) {
    char header[512] = {0};
    char line[512]   = {0};
    uint16_t idx, jdx;
    long double used, total;

    /* calc info */

    used  = (long double)os->memory.used / (1024.0 * 1024.0);
    total = (long double)os->memory.total / (1024.0 * 1024.0);

    /* print header */

    snprintf(header, 512, "%s\033[34m@\033[0m%s", os->pw->pw_name,
             os->uts.nodename);

    pfb("%s", header);
    pfb(" ");

    /* print info */

    stats_info("OS", "%s %s", os->name, os->uts.machine);
    stats_info("Kernel", "%s %s", os->uts.sysname, os->uts.release);
    stats_info("Packages", "%zu", os->pkg_count);
    stats_info("Terminal", "%s", os->term);
    stats_info("Shell", "%s", os->shell);
    stats_info("Uptime", "%02zu:%02zu:%02zu", duration(os->uptime));
    stats_info("Memory", "%.2LfGiB/%.2LfGiB (%.2Lf%%)", used, total,
               used / total * 100.0);
    stats_info("CPU", "%s (%u) @ %.2fGHz", os->cpu.model, os->cpu.core_count,
               os->cpu.freq / 1000.0 / 1000.0);

    /* print other stuff */

    pfb(" ");
    pfb("Made with \033[91m<3\033[0m by Ari Archer "
        "<\033[1m\033[36mari@ari.lt\033[0m> under GPL-3.0-or-later: "
        "\033[4mhttps://ari.lt/gh/badapplefetch\033[0m");

    /* print duration */

    pfb(" ");
    pfb("Current frame (at %u FPS): %u/%u (%.2f%%)", FPS, cur_frame,
        frames_size, (double)cur_frame / frames_size * 100.0);
    pfb("%02u:%02u:%02u / %02u:%02u:%02u", duration(cur_frame / FPS),
        duration(frames_size / FPS));

    /* print logo */

    pfb(" ");

    for (idx = 0, jdx = 0; os->logo[idx] && jdx < 512; ++idx) {
        if (os->logo[idx] == '\n' || os->logo[idx] == '\0') {
            line[jdx] = '\0';
            pfb("%s\033[0m", line);
            jdx = 0;
            continue;
        }

        line[jdx++] = os->logo[idx];
    }

    if (idx)
        pfb("%s\033[0m", line);

    /* quote */

    if (quote == NULL || rand() % 256 == 0)
        quote = quotes[((size_t)rand() % (sizeof(quotes) / sizeof(quotes[0])))];

    pfb(" ");
    pfb("%s", quote);

    /* flush output */

    fflush(stdout);
}
