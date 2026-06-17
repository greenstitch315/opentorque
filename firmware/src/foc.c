#include <math.h>
#include "foc.h"

static void clarke(float ia, float ib, float ic, float *alpha, float *beta) {
    *alpha = ia;
    *beta = (ia + 2.0f * ib) / sqrtf(3.0f);
}

static void park(float alpha, float beta, float angle, float *d, float *q) {
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    *d =  alpha * cos_a + beta * sin_a;
    *q = -alpha * sin_a + beta * cos_a;
}

static void inv_park(float d, float q, float angle, float *alpha, float *beta) {
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    *alpha = d * cos_a - q * sin_a;
    *beta  = d * sin_a + q * cos_a;
}

static void svm(float alpha, float beta, float *da, float *db, float *dc) {
    /* Space vector modulation — placeholder, outputs normalised 0..1 duties */
    *da = 0.5f + alpha * 0.5f;
    *db = 0.5f + (-0.5f * alpha + (sqrtf(3.0f) / 2.0f) * beta) * 0.5f;
    *dc = 0.5f + (-0.5f * alpha - (sqrtf(3.0f) / 2.0f) * beta) * 0.5f;
}

void foc_update(foc_state_t *s, float angle, float ia, float ib, float ic) {
    float alpha, beta;
    clarke(ia, ib, ic, &alpha, &beta);
    park(alpha, beta, angle, &s->id, &s->iq);

    /* PI controllers for id/iq go here */

    float out_alpha, out_beta;
    inv_park(s->vd, s->vq, angle, &out_alpha, &out_beta);
    svm(out_alpha, out_beta, &s->duty_a, &s->duty_b, &s->duty_c);
}
