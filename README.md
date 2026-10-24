# NativeQcomDsi

NativeQcomDsi sends DCS packets through the embedded command FIFO of a Qualcomm
DSI controller whose video scanout, PHY and clocks remain active. The caller
supplies register access and delay functions and serializes commands. Completion
is polled with a bounded timeout and the prior command engine configuration is
restored after the FIFO is cleared. The controller never reads a CPU memory
buffer for these commands.

The implementation is original BSD-2-Clause-Patent code. Hardware register and
packet-format references are Qualcomm's `techpack/display/msm/dsi/dsi_ctrl.c`,
`dsi_ctrl_hw_cmn.c`, and `dsi_ctrl_reg.h` in the Samsung SM7325 Android source:
https://github.com/Bush-cat/android_kernel_samsung_sm7325-devicetrees

The local reference checkout is `references/android_kernel_samsung_sm7325-devicetrees`.
That reference code retains its own GPL-2.0-only license. Panel command values,
brightness curves, GPIO and power coordination belong to the board package.
