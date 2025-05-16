
#include <stdlib.h>
#include "frequency_generator.h"

int next_wave_id = 0;

//----------------------------------------LINKED LIST-------------------------------------//

WaveNode* add_wave(WaveNode *head, int frequency, float amplitude, int sample_rate, WaveType type) {
    WaveNode *new_node = (WaveNode *)malloc(sizeof(WaveNode));
    if (!new_node) {
        perror("malloc failed");
        return head;
    }

    new_node->wave.id = next_wave_id++;
    new_node->wave.frequency = frequency;
    new_node->wave.amplitude = amplitude;
    new_node->wave.type = type;
    new_node->wave.phase_acc = 0;
    new_node->wave.phase_inc = (uint32_t)(((uint64_t)frequency << 32) / sample_rate);
    new_node->next = head;

    return new_node;
}

WaveNode* remove_wave(WaveNode *head, int id) {
    WaveNode *current = head;
    WaveNode *prev = NULL;

    while (current != NULL) {
        if (current->wave.id == id) {
            if (prev) {
            	prev->next = current->next;
            } else {
                head = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }
    printf("Wave with id %d not found\n", id);
    return head;
}

void free_waves(WaveNode *head) {
    WaveNode *current = head;
    while (current != NULL) {
        WaveNode *next = current->next;
        free(current);
        current = next;
    }
}

//---------------------------CREATE WAVE--------------------------------------//

q31_t generate_wave_sample(Wave *wave) {
	static q31_t sample;
	wave->phase_acc += wave->phase_inc;
	q31_t amp_q31 = (q31_t)((wave->amplitude / 100.f) * MAX_Q31);

	switch (wave->type) {
	case WAVE_SINE: {
		q31_t phase_q31 = (q31_t)(wave->phase_acc >> 1);
		q31_t sine = arm_sin_q31(phase_q31);
		int64_t scaled = ((int64_t)sine * amp_q31) >> 31;
		sample = (q31_t)__SSAT(scaled, 31);
		break;
	}
	case WAVE_SQUARE: {
		sample = (wave->phase_acc & 0x80000000)? amp_q31 : -amp_q31;
		break;
	}
	case WAVE_TRIANGLE: {
		uint32_t phase = wave->phase_acc;
		q31_t triangle;
		if (phase & 0x80000000) {
			triangle = ((~phase) << 1) - MAX_Q31;
		} else {
			triangle = (phase << 1) - MAX_Q31;
		}

		int64_t scaled = ((int64_t)triangle * amp_q31) >> 31;
		sample = (q31_t)__SSAT(scaled, 31);
		break;
	}
	case WAVE_SAWTOOTH: {
		q31_t saw = ((q31_t)(wave->phase_acc >> 1)) - MAX_Q31;
		int64_t scaled = ((int64_t)saw * amp_q31) >> 31;
		sample = (q31_t)__SSAT(scaled, 31);
		break;
	}
	default:
		return 0;
	}
	return sample;
}

//-------------------------------------------GET_WAVE--------------------------------------------------//

q31_t get_single_wave_sample_by_id(WaveNode *head, int wave_id) {
	WaveNode *node = head;

	while(node != NULL) {
		if(node->wave.id == wave_id) {
			return generate_wave_sample(&node->wave);
		}
		node = node->next;
	}
	return 0;
}

q31_t mix_waves_sample(WaveNode *head) {
    if (head == NULL) return 0;

    int64_t sum = 0;
    int count = 0;
    WaveNode *node = head;

    while (node != NULL) {
        q31_t sample = generate_wave_sample(&node->wave);
        sum += sample;
        count++;
        node = node->next;
    }

    if (count == 0) return 0;
    sum /= count;
    return (q31_t)__SSAT(sum, 31);
}
