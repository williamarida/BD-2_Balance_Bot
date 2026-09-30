#pragma once
#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
void bd2_controller_start(void);
void bd2_controller_update(bool connected, int32_t x, int32_t y, uint16_t buttons);
#ifdef __cplusplus
}
#endif
