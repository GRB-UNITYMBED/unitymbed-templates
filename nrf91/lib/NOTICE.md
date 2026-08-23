# Third-party components (vendored)

- `libmodem.a`, `include/` — Nordic Semiconductor nrfxlib v2.3.0 (nrf_modem),
  Nordic 5-Clause License: redistributable for use with Nordic Semiconductor
  devices. https://github.com/nrfconnect/sdk-nrfxlib
- `mdk/` — Nordic Semiconductor MDK (nrfx), BSD-3-Clause.
  https://github.com/NordicSemiconductor/nrfx
- `nrfx_ipc.o` — compiled from nrfx v3.5.0 `drivers/src/nrfx_ipc.c`
  (BSD-3-Clause) with -DNRF9160_XXAA -DNRF_TRUSTZONE_NONSECURE.
- `cmsis/` — ARM CMSIS_5 Core headers, Apache-2.0.
