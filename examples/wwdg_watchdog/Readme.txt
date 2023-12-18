1) in "platformio.ini"
  - add used libraries from "../common"
  - add supported boards as [env] 

2) in "src/stm8s_it.c" implement used ISR handlers

3) in "src/stm8s_conf.h" comment out unused peripherals. This step is optional, it only shortens compile time
