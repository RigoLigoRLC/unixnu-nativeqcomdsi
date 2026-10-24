// SPDX-License-Identifier: BSD-2-Clause-Patent
#include "QcomDsi.h"

int qcom_dsi_dcs(struct QcomDsi *dsi, const uint8_t *bytes, size_t count)
{
    if (!dsi || !bytes || !count || count > 60) {
        return -22;
    }

    uint32_t control = dsi->read(dsi->context, 0x004);
    if ((control & 3) != 3) {
        return -19;
    }
    uint32_t dma = dsi->read(dsi->context, 0x03c);
    uint32_t trigger = dsi->read(dsi->context, 0x084);
    uint32_t schedule = dsi->read(dsi->context, 0x100);
    uint32_t pattern = dsi->read(dsi->context, 0x15c);
    uint32_t interrupt = dsi->read(dsi->context, 0x110);
    uint32_t packet[16] = {0};
    size_t words;
    if (count == 1) {
        packet[0] = 0x80050000u | bytes[0];
        words = 1;
    } else {
        packet[0] = 0xc0390000u | (uint32_t)count;
        for (size_t i = 0; i < count; ++i) {
            packet[1 + i / 4] |= (uint32_t)bytes[i] << ((i % 4) * 8);
        }
        words = 1 + (count + 3) / 4;
    }

    // The FIFO carries embedded headers and payload without a DMA allocation.
    dsi->write(dsi->context, 0x004, control | 4);
    dsi->write(dsi->context, 0x084, (trigger & ~0x1000fu) | 4);
    // The first front-porch line follows the inherited active vertical range.
    uint32_t line = (dsi->read(dsi->context, 0x028) >> 16) + 1;
    dsi->write(dsi->context, 0x100, (schedule & ~0xffffu) | 0x10000000u | (line & 0xffff));
    dsi->write(dsi->context, 0x110, (interrupt & 0xaaaaaaaau) | 1);
    dsi->write(dsi->context, 0x15c, 0x30006);
    for (size_t i = 0; i < words; ++i) {
        dsi->write(dsi->context, 0x17c, packet[i]);
    }
    if (words & 1) {
        dsi->write(dsi->context, 0x17c, 0);
    }
    dsi->write(dsi->context, 0x03c, (dma & ~0xc0000000u) | 0x14000000u);
    dsi->write(dsi->context, 0x04c, (uint32_t)words * 4);
    dsi->write(dsi->context, 0x090, 1);

    int result = -60;
    for (unsigned i = 0; i < 2000; ++i) {
        dsi->last_interrupt = dsi->read(dsi->context, 0x110);
        if (dsi->last_interrupt & 1) {
            result = 0;
            break;
        }
        dsi->delay(dsi->context, 100);
    }
    dsi->last_status = dsi->read(dsi->context, 0x008);
    dsi->write(dsi->context, 0x110, (interrupt & 0xaaaaaaaau) | 1);
    dsi->write(dsi->context, 0x15c, 0);
    dsi->write(dsi->context, 0x1ec, 1);
    dsi->delay(dsi->context, 1);
    dsi->write(dsi->context, 0x1ec, 0);
    dsi->write(dsi->context, 0x15c, pattern);
    dsi->write(dsi->context, 0x03c, dma);
    dsi->write(dsi->context, 0x084, trigger);
    dsi->write(dsi->context, 0x100, schedule);
    dsi->write(dsi->context, 0x004, control);
    return result;
}
