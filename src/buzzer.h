#ifndef BUZZER_H__
#define BUZZER_H__

#define DELAY_NOTE 0
#define newNote(__f, __d)        \
  (struct Note)                  \
  {                              \
    .freq = __f, .duration = __d \
  }
#define newDelayNote(_d) newNote(DELAY_NOTE, _d)

void make_sound(uint32_t freq);
void stop_sound();
void timed_sound(uint32_t freq, uint32_t duration);

struct Note
{
  int freq, duration;
};

void play_note(struct Note note);

struct Music
{
	const char *name;
	struct Note *notes;
	int size;
};

void play_music(struct Music music);

struct Music song_mario();
struct Music song_star_wars();
struct Music song_got();

#endif
