#ifndef _STATS_H
#define _STATS_H
#include <stdint.h>
#include <sys/utsname.h>
#include <sys/utsname.h>

typedef struct {
    uint64_t total;
    uint64_t used;
} Memory;

typedef struct {
    char model[64];
    uint8_t core_count;
    double freq;
} CPU;

typedef struct {
    char *name;
    struct utsname uts;
    struct passwd *pw;
    uint64_t pkg_count;
    char *term;
    char *shell;
    uint64_t uptime;
    const char *logo;
    Memory memory;
    CPU cpu;
} OsInfo;

char *stat_get_name(void);
char *stat_get_username(void);
char *stat_get_shell(void);
uint64_t stat_get_uptime(void);
uint64_t stat_get_package_count(void);
double stat_get_cpu_freq(const uint8_t core_count);

unsigned char Memory_init(Memory *memory);
unsigned char CPU_init(CPU *cpu);

unsigned char OsInfo_init(OsInfo *os);
unsigned char OsInfo_update(OsInfo *os);
void OsInfo_delete(OsInfo *os);

void stats_print_beside(const OsInfo *os,
                        const uint64_t art_line,
                        const uint16_t cur_frame);
#endif /* _STATS_H */
