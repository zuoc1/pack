#include "singlepulse.h"

#include "gd32e23x.h"
#include "gd32e23x_adc.h"
#include "gd32e23x_crc.h"
#include "gd32e23x_dbg.h"
#include "gd32e23x_dma.h"
#include "gd32e23x_exti.h"
#include "gd32e23x_fmc.h"
#include "gd32e23x_gpio.h"
#include "gd32e23x_syscfg.h"
#include "gd32e23x_i2c.h"
#include "gd32e23x_fwdgt.h"
#include "gd32e23x_pmu.h"
#include "gd32e23x_rcu.h"
#include "gd32e23x_rtc.h"
#include "gd32e23x_spi.h"
#include "gd32e23x_timer.h"
#include "gd32e23x_usart.h"
#include "gd32e23x_wwdgt.h"
#include "gd32e23x_misc.h"
#include "gd32e23x_cmp.h"
#include <stdio.h>

/* configure the GPIO ports */
void gpio_config(void);
/* configure the TIMER peripheral */
void timer_config(void);

/**
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gpio_config(void)
{
    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /*configure PA6/PA7(TIMER2 CH0/CH1) as alternate function*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_6);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_6);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_7);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_7);
}

/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer_config(void)
{
    /* -----------------------------------------------------------------------
    TIMER2 configuration: single pulse mode
    the external signal is connected to TIMER2 CH0 pin(PA0) and the falling 
    edge is used as active edge.
    the single pulse signal is output on TIMER2 CH1 pin(PA1).

    the TIMER2CLK frequency is set to systemcoreclock, the prescaler is 36, 
    so the TIMER2 counter clock is 2MHz.

    single pulse value = (TIMER2_Period - TIMER2_Pulse) / TIMER2 counter clock
                       = (65535 - 11535) / 2MHz = 27 ms.
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;
    timer_ic_parameter_struct timer_icinitpara;
    /* enable the peripherals clock */
    rcu_periph_clock_enable(RCU_TIMER2);

    /* deinit a TIMER */
    timer_deinit(TIMER2);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER2 configuration */
    timer_initpara.prescaler         = 72 * 50 - 1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 15999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER2, &timer_initpara);

    /* auto-reload preload disable */
    timer_auto_reload_shadow_disable(TIMER2);
    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* CH1 configuration in OC PWM mode 1 */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER2, TIMER_CH_1, &timer_ocinitpara);

    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_1, 4000);
    /* configure TIMER channel output compare mode */
    timer_channel_output_mode_config(TIMER2, TIMER_CH_1, TIMER_OC_MODE_PWM1);
    /* configure TIMER channel output shadow function */
    timer_channel_output_shadow_config(TIMER2, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* initialize TIMER channel input parameter struct */
    timer_channel_input_struct_para_init(&timer_icinitpara);
    /* TIMER2 CH0 input capture configuration */
    timer_icinitpara.icpolarity  = TIMER_IC_POLARITY_FALLING;
    timer_icinitpara.icselection = TIMER_IC_SELECTION_DIRECTTI;
    timer_icinitpara.icprescaler = TIMER_IC_PSC_DIV1;
    timer_icinitpara.icfilter    = 0x00;
    timer_input_capture_config(TIMER2, TIMER_CH_0, &timer_icinitpara);

    /* single pulse mode selection */
    timer_single_pulse_mode_config(TIMER2, TIMER_SP_MODE_SINGLE);

    /* slave mode selection : event mode */
    /* TIMER2 input trigger : external trigger connected to CI0 */
    timer_input_trigger_source_select(TIMER2, TIMER_SMCFG_TRGSEL_CI0FE0);
    timer_slave_mode_select(TIMER2, TIMER_SLAVE_MODE_EVENT);
}

void singlepulseInit(void)
{
    /* configure the GPIO ports */
    gpio_config();
    /* configure the TIMER peripheral */
    timer_config();

    while (1);
}