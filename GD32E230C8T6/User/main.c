#include "gd32e23x.h"
#include "systick.h"
#include "pwmout.h"
#include "singlepulse.h"
#include <stdio.h>
#include "main.h"

void delay(int time)
{
    while(time--);
    
    return;
}

int main(void)
{
	rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);

	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_mode_set(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_13);
	
    systick_config();
    pwmoutInit();
    singlepulseInit();
    while(1)
    {
        /*delay(2000000);
        gpio_bit_set(GPIOC, GPIO_PIN_13);
        delay(2000000);
        gpio_bit_reset(GPIOC, GPIO_PIN_13);*/
    }
    return 0;
}