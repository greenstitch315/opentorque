#include "hardware/adc.h"
#include "adc.h"

/* INA180A3 gain = 100 V/V, shunt = 1 mΩ → 0.1 V/A
   RP2040 ADC ref = 3.3 V, 12-bit → 3.3/4096 V/count */
#define COUNTS_TO_AMPS  (3.3f / 4096.0f / 0.1f)

#define ADC_CH_A  0
#define ADC_CH_B  1
#define ADC_CH_C  2

void adc_init_current(void) {
    adc_init();
    adc_gpio_init(26 + ADC_CH_A);
    adc_gpio_init(26 + ADC_CH_B);
    adc_gpio_init(26 + ADC_CH_C);
}

void adc_read_currents(float *ia, float *ib, float *ic) {
    adc_select_input(ADC_CH_A); *ia = adc_read() * COUNTS_TO_AMPS;
    adc_select_input(ADC_CH_B); *ib = adc_read() * COUNTS_TO_AMPS;
    adc_select_input(ADC_CH_C); *ic = adc_read() * COUNTS_TO_AMPS;
}
