#pragma once

void encoder_init(void);
float encoder_read_angle(void);  /* returns angle in radians 0..2π */
