#ifndef AUDIO_EFFECTS_H
#define AUDIO_EFFECTS_H

#include "arm_math.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define EFFECT_BUFFER_SIZE 96000

typedef struct {
    arm_fir_instance_q31 fir;
    q31_t *input_buffer;
    q31_t *output_buffer;
    q31_t *state_buffer;
    uint32_t num_taps;
    uint32_t block_size;
    uint32_t idx;
    uint8_t initialized;
} FIRFilter;

typedef enum {
    FILTER_NONE,
    FILTER_FIR
} FilterType;

typedef struct {
    FilterType type;
    FIRFilter fir;
} AudioFilter;

typedef enum {
    PREDEFINED_FILTER_NONE,
    PREDEFINED_FILTER_LOWPASS,
    PREDEFINED_FILTER_HIGHPASS,
	PREDEFINED_FILTER_CUSTOM
} PredefinedFilterType;

int enable_predefined_fir_filter(PredefinedFilterType filter_type);


int enable_fir_filter(const q31_t *coeffs, uint32_t num_taps, uint32_t block_size);
void disable_filter(void);

typedef enum {
    EFFECT_NONE,
    EFFECT_DELAY,
    EFFECT_ECHO
} EffectType;

int audio_effects_init(uint32_t sample_rate);
void audio_effects_free(void);

int enable_delay(float delay_ms);
int enable_echo(float delay_ms, float gain);
int disable_effect();

q31_t process_effect(q31_t input_sample); // gebruikt de actieve effect
EffectType get_active_effect(void);

#endif
