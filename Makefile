LIBS :=
DEP :=
ifeq ($(NOQA),)
	CFLAGS += -std=c99 -Wall -Wextra -Wpedantic -pedantic -Wshadow -Werror -Wconversion -Wformat -Wuninitialized -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes -Wredundant-decls -Wfloat-equal -Wcast-qual -Wnested-externs -Wvla -Winline -Wmissing-format-attribute -Wmissing-noreturn -pedantic-errors
endif

ifeq ($(YT),)
	YT := yt-dlp
endif

ifeq ($(FPS),)
	FPS := 24
endif

ifneq ($(LIBMPV),)
	LIBS += -lmpv -lpthread
	CFLAGS += -DENABLE_SOUND
	DEP += audio.c
endif

CFLAGS += -DFPS=$(FPS)

SRC_DIR := src
OBJ_DIR := obj

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

badapplefetch: $(OBJ_FILES) frames.c logos.c $(DEP)
	$(CC) -o $@ $(CFLAGS) $(F_CFLAGS) $^ $(LIBS) $(LDFLAGS) $(F_LDFLAGS)

frames.c: frames scripts/gen_frames.sh
	bash ./scripts/gen_frames.sh >frames.c

logos.c: scripts/gen_logos.sh
	mkdir -p logos/
	bash ./scripts/gen_logos.sh >logos.c

audio.c: audio.ogg scripts/gen_audio.c
	$(CC) -o audio_generator scripts/gen_audio.c -Ofast
	./audio_generator >audio.c

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h | $(OBJ_DIR)
	$(CC) -c -o $@ $(CFLAGS) $< $(LDFLAGS)

video.mp4:
	$(YT) -f "bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]" -o "video.mp4" https://www.youtube.com/watch?v=FtutLA63Cp8 --verbose --geo-bypass -R infinite --console-title --concurrent-fragments 100 --rm-cache-dir
	ffmpeg -i video.mp4 -r $(FPS) -c:v libx264 -crf 17 -c:a copy output.mp4
	mv -fv output.mp4 video.mp4

frames: video.mp4
	rm -rf frames/ images/
	mkdir -p frames/ images/
	ffmpeg -i video.mp4 -vf fps=$(FPS) -f image2 images/%d
	for image in `ls images/`; do \
		jp2a --width=77 --height=25 images/$$image >frames/$$image; \
	done

audio.ogg: video.mp4
	ffmpeg -i video.mp4 -vn -acodec libvorbis audio.ogg

clean:
	rm -rf badapplefetch $(OBJ_DIR) frames.c logos.c images audio.ogg audio.c

purge: clean
	rm -rf video.mp4 frames

strip:
	strip --remove-section=.note.gnu.gold-version --remove-section=.note --remove-section=.gnu.version --remove-section=.eh_frame --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --strip-symbol=__gmon_start__ --remove-section=.comment --remove-section=.eh_frame_ptr --strip-unneeded --strip-all --strip-debug --merge-notes --strip-dwo --discard-all --discard-locals --verbose badapplefetch
