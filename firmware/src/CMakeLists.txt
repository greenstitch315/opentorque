add_executable(opentorque main.c)

target_link_libraries(opentorque
    pico_stdlib
    hardware_pwm
    hardware_adc
    hardware_i2c
)

pico_add_extra_outputs(opentorque)
pico_enable_stdio_usb(opentorque 1)
pico_enable_stdio_uart(opentorque 0)
