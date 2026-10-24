// SPDX-License-Identifier: BSD-2-Clause-Patent
#pragma once
#include <stddef.h>
#include <stdint.h>

/* QcomDsi sends short DCS commands through an already running video controller.
 * The board serializes callers and retains the display clocks, PHY and rails.
 */
struct QcomDsi {
    void *context;
    uint32_t (*read)(void *, uint32_t);
    void (*write)(void *, uint32_t, uint32_t);
    void (*delay)(void *, unsigned);
    uint32_t last_status;
    uint32_t last_interrupt;
};

#ifdef __cplusplus
extern "C" {
#endif
int qcom_dsi_dcs(struct QcomDsi *, const uint8_t *, size_t);
#ifdef __cplusplus
}
#endif
