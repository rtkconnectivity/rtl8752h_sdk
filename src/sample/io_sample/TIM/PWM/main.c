/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
**********************************************************************************************************
* @file     main.c
* @brief    PWM demo and complementary output demo of PWM.
* @details
* @author   yuan
* @date     2019-02-18
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_tim.h"
#include "trace.h"
#include "platform_utils.h"

/* Defines --------------------------------------------------------------*/
/** Define pwm output pin.
  * P0_1 is connected to LED0 on the EVB.
  */
#define PWM_OUT_PIN             P0_1
/** Define pwm complementary output pin.
  * P0_2 is connected to LED1 on the EVB.
  */
#define PWM_OUT_P_PIN           P0_2
#define PWM_OUT_N_PIN           P2_2


/** Define TIM num and pinmux of PWM output.
  * Timer2 to Timer5 all have PWM output function.
  * But only Timer2 support PWM complementary output function, other timers do not support it.
  */
#define PWM_TIMER_NUM           TIM2
#define PWM_OUT_PIN_PINMUX      TIM_PWM2//timer_pwm2
#define PWM_OUT_P_PIN_PINMUX    PWM2_P
#define PWM_OUT_N_PIN_PINMUX    PWM2_N

/* Config PWM_PERIOD and PWM_DUTY_CYCLE */
#define PWM_PERIOD              1000000 //uint:us
#define PWM_DUTY_CYCLE          50      //uint:percent

/* PWM_HIGH_COUNT =  */
#define PWM_HIGH_COUNT          (((PWM_PERIOD)*((PWM_DUTY_CYCLE*40)/100))-1)    //PWM CLOCK = 40000000
#define PWM_LOW_COUNT           (((PWM_PERIOD)*(((100-PWM_DUTY_CYCLE)*40)/100))-1)

#define PWM_DEAD_ZONE_CLOCK_SOURCE_5M   0
#define PWM_DEAD_ZONE_BYPASS            0
#define PWM_COMPL_OUTPUT_EM_STOP_TEST   0

/** Config PWM_DEAD_ZONE_TIME. Max 8ms/32k, 51us/5M.
  * When dead time is set, PWM2_P and PWM2_N will
  * delay one dead time longer than PWM2 at high output level.
  * When the dead_zone time is longer than the high-level time of PWM,
  * PWM_P will always output low-level.
  * When the dead_zone time is longer than the low-level time of PWM,
  * PWM_N will always output low-level.
  */
#if PWM_DEAD_ZONE_CLOCK_SOURCE_5M
#define PWM_DEAD_ZONE_TIME      4 //uint:us
#define PWM_DEAD_ZONE_SIZE      ((PWM_DEAD_ZONE_TIME)*5-1)
#else
#define PWM_DEAD_ZONE_TIME      5 //uint:ms
#define PWM_DEAD_ZONE_SIZE      ((PWM_DEAD_ZONE_TIME)*32-1)
#endif

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
*/
void board_pwm_init(void)
{
    Pad_Config(PWM_OUT_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(PWM_OUT_P_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pad_Config(PWM_OUT_N_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);

    /* Normal mode */
    Pinmux_Config(PWM_OUT_PIN, PWM_OUT_PIN_PINMUX);
    /* Deadzone mode */
    Pinmux_Config(PWM_OUT_P_PIN, PWM_OUT_P_PIN_PINMUX);
    Pinmux_Config(PWM_OUT_N_PIN, PWM_OUT_N_PIN_PINMUX);

}

/**
  * @brief  Initialize tim peripheral.
  * @param  No parameter.
  * @return void
*/
void driver_pwm_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_TIMER, APBPeriph_TIMER_CLOCK, ENABLE);
#if PWM_DEAD_ZONE_CLOCK_SOURCE_5M
    RCC_ClockSrc5MCmd();
#endif

    TIM_TimeBaseInitTypeDef TIM_InitStruct;

    TIM_StructInit(&TIM_InitStruct);
    TIM_InitStruct.TIM_Mode             = TIM_Mode_UserDefine;
    TIM_InitStruct.TIM_PWM_En           = PWM_ENABLE;
    TIM_InitStruct.TIM_PWM_High_Count   = PWM_HIGH_COUNT;
    TIM_InitStruct.TIM_PWM_Low_Count    = PWM_LOW_COUNT;
    TIM_InitStruct.PWM_Stop_State_P     = PWM_STOP_AT_HIGH;
    TIM_InitStruct.PWM_Stop_State_N     = PWM_STOP_AT_LOW;
    TIM_InitStruct.PWMDeadZone_En       = DEADZONE_ENABLE;  //enable to use pwn p/n output
    TIM_InitStruct.PWM_Deazone_Size     = PWM_DEAD_ZONE_SIZE;
    TIM_TimeBaseInit(TIM2, &TIM_InitStruct);

#if PWM_DEAD_ZONE_CLOCK_SOURCE_5M
    TIM_PWMChangeDZClockSrc((PWM_TypeDef *)(&TIMER_PWM2_CR), ENABLE);
#endif

    TIM_Cmd(TIM2, ENABLE);

#if PWM_DEAD_ZONE_BYPASS
    TIM_PWMDZBypassCmd((PWM_TypeDef *)(&TIMER_PWM2_CR), ENABLE);
#endif
}

/**
  * @brief  Demo code of operation about PWM.
  * @param  No parameter.
  * @return void
  */
void pwm_demo(void)
{
    /* Configure PAD and pinmux firstly! */
    board_pwm_init();

    /* Initialize TIM peripheral */
    driver_pwm_init();
}

/**
  * @brief    Entry of app code
  * @return   int (To avoid compile warning)
  */
int main(void)
{
    extern uint32_t random_seed_value;
    srand(random_seed_value);
    pwm_demo();

    while (1)
    {
#if PWM_COMPL_OUTPUT_EM_STOP_TEST
        platform_delay_ms(2000);
        //PWM complementary output emergency stop.
        TIM_PWMComplOutputEMCmd(PWM2, ENABLE);
        platform_delay_ms(2000);
        //Resume PWM complementary output.
        TIM_PWMComplOutputEMCmd(PWM2, DISABLE);
#else

#endif
    }
}

/******************* (C) COPYRIGHT 2019 Realtek Semiconductor Corporation *****END OF FILE****/
