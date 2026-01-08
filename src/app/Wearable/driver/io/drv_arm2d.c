/*
 * Copyright (c) 2022
 *
 */

#include <string.h>
#include "drv_tim.h"
#include "trace.h"
#include "os_sync.h"
#include "drv_lcd.h"


uint32_t SystemCoreClock = 90000000;
void (*tim5_indicate)(void) = NULL;
void (*tim6_indicate)(void) = NULL;



void drv_tim5_start(void)
{
    TIM_Cmd(TIM5, ENABLE);
}

void drv_tim5_stop(void)
{
    TIM_Cmd(TIM5, DISABLE);
    TIM_INTConfig(TIM5, DISABLE);
}

void drv_tim5_reset(void)
{

}

void drv_tim5_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_StructInit(&TIM_InitStruct);

    TIM_InitStruct.TIM_Mode = TIM_Mode_FreeRun;
    TIM_InitStruct.TIM_SOURCE_DIV = TIM_CLOCK_DIVIDER_40;
    TIM_InitStruct.TIM_Period = 0xffffffff ;
    TIM_TimeBaseInit(TIM5, &TIM_InitStruct);

    /*  Enable TIMER IRQ  */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIMER5_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    TIM_ClearINT(TIM5);
    TIM_INTConfig(TIM5, ENABLE);
}



uint32_t drv_tim5_read(void)// unit: 1us
{
    //return TIM_GetCurrentValue(TIM5);
    return (~TIM_GetCurrentValue(TIM5));
}



void drv_tim5_set_indicate(void (*tim_ind)(void))
{
    tim5_indicate = tim_ind;
}


static uint64_t arm2d_count = 0;
void Timer5_Handler(void)
{
    arm2d_count++;
    TIM_ClearINT(TIM5);
    TIM_Cmd(TIM5, DISABLE);
    TIM_Cmd(TIM5, ENABLE);
    DBG_DIRECT("Timer5_Handler line = %d", __LINE__);
}


void hw_tim_init(void)
{
    DBG_DIRECT("hw_tim_init");
    drv_tim5_init();
    drv_tim5_start();
}

int32_t Disp0_DrawBitmap(int16_t x,
                         int16_t y,
                         int16_t width,
                         int16_t height,
                         const uint8_t *bitmap)
{
    extern void drv_lcd_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h);
    drv_lcd_set_window(x, y, width, height);
    drv_lcd_start_transfer((uint8_t *)bitmap, width * height);
    drv_lcd_transfer_done();
    return 0;
}

uint32_t arm_2d_helper_get_reference_clock_frequency(void)
{
    return 1000000;
}
int64_t arm_2d_helper_get_system_timestamp(void)
{
    //DBG_DIRECT("low = %d, total = %d", drv_tim5_read(), (uint64_t)drv_tim5_read() + (arm2d_count<<32));
    return (uint64_t)drv_tim5_read() + (arm2d_count << 32);
}

#include "os_mem.h"
void *malloc(size_t size)
{
    return os_mem_alloc(RAM_TYPE_DATA_ON, size);
}

void free(void *ptr)
{
    os_mem_free(ptr);
}
/************** end of file ********************/
