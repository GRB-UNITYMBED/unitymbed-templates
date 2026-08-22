# UnityMbed STM32 (auto-detect)

Generic bare-metal STM32 starter. `unitymbed init` probes the board over
ST-Link and rewrites CPU/flash/RAM/openocd target for the exact part it finds;
the detected identity is stored under `"stm32"` in `unitymbed.json` so the AI
can use its native STM32 knowledge. Works standalone as a plain make project.
