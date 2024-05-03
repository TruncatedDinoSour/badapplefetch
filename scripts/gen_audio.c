#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(void) {
    int fd;
    off_t size, idx;
    unsigned char *buf;

    fd   = open("audio.ogg", O_RDONLY);
    size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    buf = malloc(size);

    read(fd, buf, size);

    puts("#include <stdint.h>\nconst unsigned char audio[]={");

    for (idx = 0; idx < size; ++idx)
        printf("%u,", buf[idx]);

    puts("};const uint64_t audio_size=sizeof(audio);");

    free(buf);

    return 0;
}
