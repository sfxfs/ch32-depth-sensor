#pragma once

#include "debug.h"

int ms5803_init();
void ms5803_read_temp_and_press(float *temp, float *press);
