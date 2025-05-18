#ifndef FREQUENCY_GENERATOR_H
#define FREQUENCY_GENERATOR_H

#include "arm_math.h"
#include <stdio.h>

#define MAX_Q31 0x7FFFFFFF

extern int next_wave_id;

typedef enum {
    WAVE_SINE,
    WAVE_TRIANGLE,
    WAVE_SQUARE,
	WAVE_SAWTOOTH
} WaveType;

typedef struct {
	int id;
	float amplitude;
	int frequency;
	WaveType type;

	uint32_t phase_acc;
	uint32_t phase_inc;
} Wave;

typedef struct WaveNode {
    Wave wave;
    struct WaveNode *next;
} WaveNode;

WaveNode* add_wave(WaveNode *head, int frequency, float amplitude, int sample_rate, WaveType type);
WaveNode* remove_wave(WaveNode *head, int id);
void free_waves(WaveNode *head);
q31_t get_single_wave_by_id(WaveNode *head, int wave_id);
q31_t mix_generated_waves(WaveNode *head);
q31_t mix_waves(WaveNode *head, q31_t *input_wave);

#endif
