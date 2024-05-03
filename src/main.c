#define _POSIX_C_SOURCE 199309L

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "fps.h"

#include "stats.h"
#include "distro.h"

extern const char *const frames[];
extern const uint16_t frames_size;

#ifdef ENABLE_SOUND
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <mpv/client.h>

extern const unsigned char audio[];
extern const uint64_t audio_size;

static void *play_mpv(void *mpv) {
    while (1) {
        mpv_event *event = mpv_wait_event((mpv_handle *)mpv, -1);

        if (event->event_id == MPV_EVENT_SHUTDOWN ||
            event->event_id == MPV_EVENT_END_FILE)
            break;
    }

    mpv_terminate_destroy((mpv_handle *)mpv);

    return NULL;
}
#endif /* ENABLE_SOUND */

int main(const int argc, const char *const argv[]) {
    uint16_t frame;

    struct timespec ts;
    OsInfo os = {0};

    ts.tv_sec  = 0;
    ts.tv_nsec = 1000000000 / FPS;

#ifdef ENABLE_SOUND
    pthread_t thread_id;
    int fd;
    const char *cmd[3];
    char mpv_filename[64];
    mpv_event *event;
    mpv_handle *mpv = mpv_create();

    if (!mpv) {
        fputs("failed to create mpv context\n", stderr);
        return 1;
    }

    mpv_set_option_string(mpv, "msg-level", "all=no");

    if (mpv_initialize(mpv) < 0) {
        fputs("failed to initialize mpv\n", stderr);
        return 1;
    }
#endif /* ENABLE_SOUND */

    srand((unsigned int)time(NULL));

    OsInfo_init(&os);

    if (argc > 1)
        os.logo = get_distro_logo(argv[1]);

#ifdef ENABLE_SOUND
    snprintf(mpv_filename, 64, ".%u.ogg", getpid());

    fd = open(mpv_filename, O_WRONLY | O_CREAT);
    write(fd, audio, audio_size);
    close(fd);

    chmod(mpv_filename, 0700);

    cmd[0] = "loadfile";
    cmd[1] = mpv_filename;
    cmd[2] = NULL;

    mpv_command(mpv, cmd);

    while (1) {
        event = mpv_wait_event((mpv_handle *)mpv, -1);

        if (event->event_id == MPV_EVENT_FILE_LOADED)
            break;
    }

    pthread_create(&thread_id, NULL, &play_mpv, mpv);

    remove(mpv_filename);
#endif /* ENABLE_SOUND */

    fputs("\033[H\033[J", stdout);

    for (frame = 0; frame < frames_size; ++frame) {
        puts("\033[H");
        stats_print_beside(&os, 77, frame);

        puts("\033[H");
        puts(frames[frame]);

        fflush(stdout);

        if (frame % FPS == 0)
            OsInfo_update(&os);

        nanosleep(&ts, NULL);
    }

#ifdef ENABLE_SOUND
    pthread_join(thread_id, NULL);
#endif /* ENABLE_SOUND */

    return 0;
}
