/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
static char sendBuffer[100];
static char receive_buff[BUFFER_SIZE];
static char test_dmarx_buff[BUFFER_SIZE];

QueueHandle_t xPrintQueue;

extern TaskHandle_t xUartRxTaskHandle;

void vUART_RX_Task(void *pvParameters);
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{
    /* USER CODE BEGIN USART1_Init 0 */

    /* USER CODE END USART1_Init 0 */

    /* USER CODE BEGIN USART1_Init 1 */

    /* USER CODE END USART1_Init 1 */
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART1_Init 2 */
    xPrintQueue = xQueueCreate(50, sizeof(char *));

    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    //设置DMA传输，讲串口1的数据搬运到recvive_buff中，
    //每次255个字节
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)&receive_buff, BUFFER_SIZE);     
    /* USER CODE END USART1_Init 2 */
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (uartHandle->Instance == USART1) {
        /* USER CODE BEGIN USART1_MspInit 0 */

        /* USER CODE END USART1_MspInit 0 */
        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USART1 DMA Init */
        /* USART1_TX Init */
        hdma_usart1_tx.Instance = DMA1_Channel4;
        hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_tx.Init.Mode = DMA_NORMAL;
        hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(uartHandle, hdmatx, hdma_usart1_tx);

        /* USART1_RX Init */
        hdma_usart1_rx.Instance = DMA1_Channel5;
        hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_rx.Init.Mode = DMA_NORMAL;
        hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(uartHandle, hdmarx, hdma_usart1_rx);

        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspInit 1 */

        /* USER CODE END USART1_MspInit 1 */
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{
    if (uartHandle->Instance == USART1) {
        /* USER CODE BEGIN USART1_MspDeInit 0 */

        /* USER CODE END USART1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);

        /* USART1 DMA DeInit */
        HAL_DMA_DeInit(uartHandle->hdmatx);
        HAL_DMA_DeInit(uartHandle->hdmarx);

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspDeInit 1 */

        /* USER CODE END USART1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

int logi_printf(uint32_t tickTime, const char *file, const char *function, int line, char *format, ...)
{
    int ret;
    va_list arg;
    char *buffer, *logBuffer;


    buffer = (char *)pvPortMalloc(100);
    logBuffer = (char *)pvPortMalloc(100);
    memset(buffer, 0, 100);
    va_start(arg, format);
    ret = vsprintf(buffer, format, arg);
    va_end(arg);

    sprintf(logBuffer, "\e[30;31m[%ld](%s %s@%d): %s\r\n", tickTime, file, function, line, buffer);
    vPortFree(buffer);

    if (xQueueSend(xPrintQueue, &logBuffer, 10) != pdTRUE)
        vPortFree(logBuffer);

    return ret;
}

int debug_printf(char *format, ...)
{
    int ret;
    va_list arg;
    char *debug_buf;


    debug_buf = (char *)pvPortMalloc(100);
    memset(debug_buf, 0, 100);
    va_start(arg, format);
    ret = vsprintf(debug_buf, format, arg);
    va_end(arg);

    if (xQueueSend(xPrintQueue, &debug_buf, 10) != pdTRUE)
        vPortFree(debug_buf);

    return ret;
}

void vLOG_Task(void *pvParameters)
{
    char *buffer;
    int ret = 0;
    for (;;) {
        if (xQueueReceive(xPrintQueue, &buffer, portMAX_DELAY)) {
            while (huart1.gState != HAL_UART_STATE_READY); 

            ret = strlen(buffer);
            memset(sendBuffer, 0, 100);
            memcpy(sendBuffer, buffer, ret);
            
            vPortFree(buffer);

            HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&sendBuffer, ret);
        }
    }
}

void USAR_UART_IDLECallback(UART_HandleTypeDef *huart)
{
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    HAL_UART_DMAStop(huart);                                                     //停止本次DMA传输
    uint8_t data_length  = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);   //计算接收到的数据长度

    memset(test_dmarx_buff, 0, BUFFER_SIZE);
    memcpy(test_dmarx_buff, receive_buff, data_length);
    vTaskNotifyGiveFromISR(xUartRxTaskHandle, &xHigherPriorityTaskWoken);
  
    HAL_UART_Receive_DMA(huart, (uint8_t*)&receive_buff, BUFFER_SIZE);          //重启开始DMA传输 每次255字节数据

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)  {
        if (RESET != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)) { //判断是否是空闲中断
            __HAL_UART_CLEAR_IDLEFLAG(huart);       //清楚空闲中断标志（否则会一直不断进入中断）
            USAR_UART_IDLECallback(huart);          //调用中断处理函数
        }
    }
}

void vUART_RX_Task(void *pvParameters)
{
    char *buffer;
    for ( ;; ) {
        if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) {
            buffer = pvPortMalloc(BUFFER_SIZE);
            memcpy(buffer, test_dmarx_buff, BUFFER_SIZE);
            xQueueSend(xPrintQueue, &buffer, 10);
        }
    }
}
/* USER CODE END 1 */
