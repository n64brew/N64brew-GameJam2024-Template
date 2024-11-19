#ifndef sequence_introduction_INPUT_H
#define sequence_introduction_INPUT_H

#include <libdragon.h>

// Libdragon Logo
extern bool sequence_introduction_libdragon_logo_started;
extern bool sequence_introduction_libdragon_logo_finished;
extern float sequence_introduction_libdragon_logo_elapsed;

// Mallard Logo
extern bool sequence_introduction_mallard_logo_started;
extern bool sequence_introduction_mallard_logo_finished;
extern float sequence_introduction_mallard_logo_elapsed;

// Paragraphs
extern bool sequence_introduction_paragraphs_started;
extern bool sequence_introduction_paragraphs_finished;
extern bool sequence_introduction_paragraph_fade_out_started;
extern bool sequence_introduction_paragraph_fade_out_finished;
extern float sequence_introduction_paragraph_fade_out_elapsed;
extern bool sequence_introduction_current_paragraph_finished;
extern int sequence_introduction_current_paragraph;
extern int sequence_introduction_current_paragraph_drawn_characters;
extern int sequence_introduction_current_paragraph_speed;
extern char *sequence_introduction_current_paragraph_string;

extern bool sequence_introduction_finished;

// Music
extern int sequence_introduction_currentXMPattern;

void sequence_introduction_process_controller(float deltatime);

#endif // sequence_introduction_INPUT_H
