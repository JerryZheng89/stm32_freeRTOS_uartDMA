# FreeRTOS 实验 -- 串口DMA 发送和接收
[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat-square)](https://github.com/JerryZheng/stm32_freeRTOS_uartDMA)

MCU: STM32F103ZET6

基础代码由**STM32CubeMX**生成

## 开发环境和工具
- MacOS Monterey
- STM32CubeMX
- VS Code
    - PlatformIO
- [stm32pio](https://github.com/ussserrr/stm32pio)(Python 脚本软件)
- STLink

## 实现的功能
DMA 方式实现发送和接收串口数据, 最大程度节约CPU资源, 并保证最出色的并发能力, 不丢包, 打印不错位, 包与包之间最小发送间隔仅为0.5ms

task: 

- 2个 LED BLink task, 负责LED灯闪烁, 并向串口发送LED灯状态
- uart receive task, 负责将收到的不定长数据包,转发出去


API:

- debug_printf
- ST_LOGI 红色字体, 头部有TICK时间, 文件名, 函数名, 行号等

```
ST_LOGI("LED1 OFF")

[3267002](Core/Src/freertos.c vLED_1_Task@139): LED1 OFF
```
```c
debug_printf("\e[0mLED2 OFF%d\r\n", i % 10);
```


## Maintainers
[@JerryZheng89](https://github.com/JerryZheng89).

## License
[MIT](LICENSE) © Jerry Zheng