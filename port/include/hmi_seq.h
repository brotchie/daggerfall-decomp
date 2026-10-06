/* hmi_seq.h: HMI SOS 4.0's MIDI song player as FALL.EXE links it, for the native build
   (port/host/hmi_seq.c): Daggerfall's "HMI-MIDISONG061595" songs played on one MIDI driver
   at 120 ticks a second. */
#ifndef HMI_SEQ_H
#define HMI_SEQ_H

#include <stddef.h>

struct hmi_song;

/* the driver the songs play on: its device id (0xA009, 0xA002 ...) and its SendData */
void hmi_set_driver(unsigned device, void (*send)(void *ctx, unsigned status, unsigned d1,
                                                  unsigned d2), void *ctx);
/* the driver's channel table back to its start (sosMIDIInitDriver), as SOS resets it */
void hmi_driver_reset(void);

/* a song from its file bytes (copied); NULL when it is not an HMI song. *ready is 0 when no
   track matches the driver (sosMIDIInitSong's error 0x1A) */
struct hmi_song *hmi_song_new(const unsigned char *data, size_t len, int *ready);
void hmi_song_free(struct hmi_song *s);
/* a song's size from its bytes alone (sosMIDIInitSong is given no length) */
size_t hmi_song_size(const unsigned char *data);
void hmi_song_start(struct hmi_song *s);
void hmi_song_stop(struct hmi_song *s);
int hmi_song_playing(const struct hmi_song *s);

/* sosMIDISetMasterVolume (0-127) */
void hmi_set_master_volume(unsigned v);

/* one sequencer callback (120 Hz) for every playing song */
void hmi_tick(void);
#define HMI_TICK_HZ 120

#endif
