#include "notes.h"

#define NOTE_SCALE_MULTIPLIER 1.0
#define NOTE_Y_OFFSET_PERIOD 32
#define NOTE_Y_OFFSET_AMPLITUDE 4
#define NOTE_THETA_PERIOD 16
#define NOTE_THETA_AMPLITUDE 0.1
#define NOTE_SCALE_PERIOD 0.1
#define NOTE_SCALE_AMPLITUDE 0.05
#define NOTE_ANIMATION_BOUNCE 0x1
#define NOTE_ANIMATION_ROTATE 0x2
#define NOTE_ANIMATION_PULSE 0x4


static note_ll_t notes;
static sprite_t* note_sprites[PLAYER_MAX];

void notes_init(void) {
	char temptext[64];
	for (uint8_t i=0; i<PLAYER_MAX; i++) {
		// Load the note sprites
		sprintf(temptext, "rom:/hydraharmonics/note-%i.ci4.sprite", i);
		note_sprites[i] = sprite_load(temptext);
		// Set how many notes are left per player
		notes.notes_left[i] = NOTES_PER_PLAYER;
	}
	// Init notes
	notes.start = notes.end = NULL;
}

PlyNum note_get_free(void) {
	uint8_t i, status;
	// Check if we've depleted them
	status = 0;
	for (i=0; i<PLAYER_MAX; i++) {
		if (notes.notes_left[i]) {
			status = 1;
		}
	}
	// Depleted, let's just pull some random ones now
	if (!status) {
		return rand()%PLAYER_MAX;
	}
	// Pick a random player to spawn a note for
	do {
		status = rand()%PLAYER_MAX;
	} while (!notes.notes_left[status]);
	notes.notes_left[status]--;
	return status;
}

void notes_add (void) {
	note_t* note = malloc(sizeof(note_t));
	uint32_t random = rand();
	if (note != NULL) {
		// Rearrange the pointers
		if (notes.end == NULL) {
			notes.start = note;
		} else {
			notes.end->next = note;
		}
		notes.end = note;

		// Set the note's starting  values
		if (random%3 == 0) {
			note->state = STATE_UP;
		} else if (random%3 == 1) {
			note->state = STATE_UP;
		} else {
			note->state = STATE_DOWN;
		}
		note->player = note_get_free();
		note->sprite = note_sprites[note->player];
		note->x = display_get_width();
		note->y = PADDING_TOP + note->state*HYDRA_HEAD_HEIGHT + 8;
		note->anim_offset = random % UINT8_MAX;
		note->blitparms = (rdpq_blitparms_t){
			.theta = 0,
			.scale_x = 0,
			.scale_y = 0,
			.cx = note->sprite->width/2,
			.cy = note->sprite->height/2,
		};
		note->scale = 0;
		note->y_offset = 0;
		note->next = NULL;
	}
}

note_t* notes_get_first(void) {
	return notes.start;
}

void notes_move (void) {
	note_t* current = notes.start;
	while (current != NULL) {
		current->x -= NOTE_SPEED;
		current->y_offset = sin(current->x / NOTE_Y_OFFSET_PERIOD) * NOTE_Y_OFFSET_AMPLITUDE;
		current->blitparms.theta = sin((current->x - current->anim_offset) / NOTE_THETA_PERIOD) * NOTE_THETA_AMPLITUDE;
		current->blitparms.scale_x = 1 + sin((current->x + current->anim_offset) / NOTE_SCALE_PERIOD) * NOTE_SCALE_AMPLITUDE;
		current->blitparms.scale_y = 1 + sin((current->x + current->anim_offset) / NOTE_SCALE_PERIOD) * NOTE_SCALE_AMPLITUDE;
		current = current->next;
	}
}

void notes_draw (void) {
	note_t* current = notes.start;
	while (current != NULL) {
		rdpq_sprite_blit(
			current->sprite,
			current->x,
			current->y + current->y_offset,
			&current->blitparms
		);
		current = current->next;
	}
}

uint16_t notes_get_remaining (notes_remaining_t type) {
	uint16_t remaining = 0;
	uint8_t i;
	note_t* current = notes.start;
	// Get unspawned notes
	for (i=0; i<PLAYER_MAX && (type & NOTES_GET_REMAINING_UNSPAWNED); i++) {
		remaining += notes.notes_left[i];
	}
	// Get spawned notes
	while (current != NULL && (type & NOTES_GET_REMAINING_SPAWNED)) {
		remaining++;
		current = current->next;
	}
	return remaining;
}

void notes_destroy (note_t* dead_note) {
	note_t* current = notes.start;
	note_t* prev = NULL;
	while (current != NULL) {
		if (current == dead_note) {
			if (current == notes.start) {
				notes.start = current->next;
			}
			if (current == notes.end) {
				notes.end = prev;
			}
			if (prev) {
				prev->next = current->next;
			}
			free(dead_note);
			return;
		}
		prev = current;
		current = current->next;
	}
}

void notes_destroy_all (void) {
	note_t* current = notes.start;
	note_t* next = NULL;
	while (current != NULL) {
		next = current->next;
		notes_destroy(current);
		current = next;
	}
}

void notes_clear (void) {
	uint8_t i;
	for (i=0; i<PLAYER_MAX; i++) {
		sprite_free(note_sprites[i]);
	}
	notes_destroy_all();
}
