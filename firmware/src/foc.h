#pragma once

typedef struct {
    float id, iq;       // d/q axis currents
    float vd, vq;       // d/q axis voltages
    float duty_a, duty_b, duty_c;
} foc_state_t;

void foc_update(foc_state_t *s, float angle, float ia, float ib, float ic);
