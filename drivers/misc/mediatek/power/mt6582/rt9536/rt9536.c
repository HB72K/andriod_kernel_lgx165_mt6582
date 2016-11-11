/*
 * Charging IC driver (rt9536)
 *
 * Copyright (C) 2010 LGE, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/kobject.h>
#include <linux/earlysuspend.h>
#include <linux/platform_device.h>
#include <asm/atomic.h>
#include <linux/power_supply.h>		// might need to get fuel gauge info

#include <mach/mt_gpio.h>
#include <cust_gpio_usage.h>
#include <cust_gpio_boot.h>

#include <linux/xlog.h>
#include <mach/charging.h>
#include <cust_charging.h>
#include "rt9536.h"
//<2015/1/26-davidda, [V10][Feature][Common][BSP][]Battery OTP test
typedef struct {
	kal_bool bat_exist;
	kal_bool bat_full;
	INT32 bat_charging_state;
	UINT32 bat_vol;
	kal_bool bat_in_recharging_state;
	kal_uint32 Vsense;
	kal_bool charger_exist;
	UINT32 charger_vol;
	INT32 charger_protect_status;
	INT32 ICharging;
	INT32 IBattery;
	INT32 temperature;
	INT32 temperatureR;
	INT32 temperatureV;
	UINT32 total_charging_time;
	UINT32 PRE_charging_time;
	UINT32 CC_charging_time;
	UINT32 TOPOFF_charging_time;
	UINT32 POSTFULL_charging_time;
	UINT32 charger_type;
	INT32 SOC;
	INT32 UI_SOC;
	UINT32 nPercent_ZCV;
	UINT32 nPrecent_UI_SOC_check_point;
	UINT32 ZCV;
} PMU_ChargerStruct;
//>2015/1/26-davidda

static DEFINE_MUTEX(charging_lock);

enum power_supply_type charging_ic_status;
//<2015/1/26-davidda, [V10][Feature][Common][BSP][]Battery OTP test
extern PMU_ChargerStruct BMT_status;
//>2015/1/26-davidda

kal_bool chargin_hw_init_done = KAL_FALSE; 

/* Fuction Prototype */
static void charging_ic_initialize(void);
static irqreturn_t charging_ic_interrupt_handler(int irq, void *data);

struct timer_list charging_timer;

enum power_supply_type get_charging_ic_status()
{
    return charging_ic_status;
}

// USB500 mode charging
void charging_ic_active_default(void)
{
    
    if(charging_ic_status == POWER_SUPPLY_TYPE_USB)
    {
        printk("[charger_rt9536] :: it's already %s mode!!\n", __func__);
        return;
    }

    if(charging_ic_status != POWER_SUPPLY_TYPE_BATTERY)
    {
        charging_ic_deactive();
    }

    mutex_lock(&charging_lock);

    //<2015/1/20-davidda, [V10][Feature][Common][BSP][]enable high voltage battery charging
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ZERO);
    udelay(400);
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ONE);
    udelay(400);  // about 400 us
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ZERO);
    udelay(400);  // about 400 us
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ONE);
    udelay(400);  // about 400 us
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ZERO);
    udelay(400);  // about 400 us
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ONE);
    udelay(400);  // about 400 us
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ZERO);
    udelay(400);  // about 400 us
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ONE);
    udelay(400);  // about 400 us
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ZERO);
    mdelay(2); 
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ONE);
    udelay(850);  // about 400 us
    //>2015/1/20-davidda

	

    // USB500 mode
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ZERO);
    udelay(1500);

    charging_ic_status = POWER_SUPPLY_TYPE_USB;
    //<2015/1/26-davidda, [V10][Feature][Common][BSP][]add charging current indicator
    BMT_status.ICharging = 500;
    //>2015/1/26-davidda

    mutex_unlock(&charging_lock);

    printk("[charger_rt9536] :: %s : \n", __func__);

}
EXPORT_SYMBOL(charging_ic_active_default);
// TA connection, ISET mode
void charging_ic_set_ta_mode(void)
{

    if(charging_ic_status == POWER_SUPPLY_TYPE_MAINS)
    {
        printk("[charger_rt9536] :: it's already %s mode!! : \n", __func__);
        return;
    }

    if(charging_ic_status != POWER_SUPPLY_TYPE_BATTERY)
    {
        charging_ic_deactive();
    }

    mutex_lock(&charging_lock);
    //<2015/1/20-davidda, [V10][Feature][Common][BSP][]enable high voltage battery charging
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ZERO);
    udelay(400);
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ONE);
    udelay(400);  // about 400 us
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ZERO);
    mdelay(2); 
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ONE);
    udelay(850);  // about 400 us
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ZERO);
    udelay(1500);
    //>2015/1/20-davidda
    //<2015/1/26-davidda, [V10][Feature][Common][BSP][]add charging current indicator
    BMT_status.ICharging = 750;
    //>2015/1/26-davidda
    charging_ic_status = POWER_SUPPLY_TYPE_MAINS;

    mutex_unlock(&charging_lock);

    printk("[charger_rt9536] :: %s : \n", __func__);
}

void charging_ic_set_usb_mode(void)
{
    charging_ic_active_default();
}

void charging_ic_set_factory_mode(void)
{

#if 0
    if(charging_ic_status == POWER_SUPPLY_TYPE_FACTORY)
    {
        printk("Power/Charger", "[charger_rt9536] :: it's already %s mode!! : \n", __func__);
        return;
    }
#endif /* 0 */

    //<2015/2/10-davidda, [V10][Feature][Common][BSP][]power on without battery
    if(charging_ic_status == POWER_SUPPLY_TYPE_UPS)
    {
        printk("[charger_rt9536] :: it's already %s mode!! : \n", __func__);
        return;
    }

    if(charging_ic_status != POWER_SUPPLY_TYPE_BATTERY)
    {
        charging_ic_deactive();
    }

    mutex_lock(&charging_lock);
    charging_ic_status =  POWER_SUPPLY_TYPE_UPS;
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ZERO);
    udelay(400);
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ONE);
    udelay(400);
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ZERO);
    udelay(400);
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ONE);
    udelay(400);
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ZERO);
    udelay(400);
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ONE);
    udelay(400);
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ZERO);
    udelay(1500);
    //>2015/2/10-davidda

#if 0
    charging_ic_status = POWER_SUPPLY_TYPE_FACTORY;
#endif /* 0 */

    mutex_unlock(&charging_lock);

    printk("[charger_rt9536] :: %s : \n", __func__);
}

void charging_ic_deactive(void)
{

    //<2015/1/30-davidda, [V10][Feature][Common][BSP][]Power on the phone without battery
    if(!BMT_status.bat_exist)
	return;	
    //>2015/1/30-davidda


    if(charging_ic_status == POWER_SUPPLY_TYPE_BATTERY)
    {
        printk("[charger_rt9536] :: it's already %s mode!! : \n", __func__);
        return;
    }

	

    mutex_lock(&charging_lock);

    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ONE);

    udelay(2000);

    charging_ic_status = POWER_SUPPLY_TYPE_BATTERY;
    //<2015/1/26-davidda, [V10][Feature][Common][BSP][]add charging current indicator
    BMT_status.ICharging = 0;
    //>2015/1/26-davidda
    mutex_unlock(&charging_lock);

    printk("[charger_rt9536] :: %s : \n", __func__);
}
EXPORT_SYMBOL(charging_ic_deactive);

void rt9536_charging_enable(unsigned int set_current, unsigned int enable)
{
    //<2015/2/11-davidda, [V10][Feature][Common][BSP][]support PIF jag
    //<2015/1/30-davidda, [V10][Feature][Common][BSP][]Power on the phone without battery
    if(!BMT_status.bat_exist) 
    {
        charging_ic_set_factory_mode();
	return;
    }
    else if(charging_ic_status==POWER_SUPPLY_TYPE_UNKNOWN)
    {
            charging_ic_deactive();		
    }
     //>2015/1/30-davidda
    //>2015/2/11-davidda
    
    if (enable)
    {
        if ( set_current == AC_CHARGER_CURRENT )
            charging_ic_set_ta_mode();
        else if ( set_current == USB_CHARGER_CURRENT )
            charging_ic_set_usb_mode();
        //<2015/1/26-davidda, [V10][Feature][Common][BSP][]Battery OTP test
        else if ( set_current == CHARGE_CURRENT_0_00_MA )
            charging_ic_deactive();		
        //>2015/1/26-davidda
        else
            charging_ic_active_default();

        printk("[charger_rt9536] :: %s, current(%d), enable(%d)\n", __func__, set_current, enable);
    }
    else
    {
        charging_ic_deactive();
        printk("[charger_rt9536] :: %s, enable(%d)\n", __func__, enable);        
    }
    
}

unsigned char rt9536_check_eoc_status(void)
{
    //<2015/2/9-davidda, [V10][Feature][Common][BSP][]fix issue : status not stable 
    unsigned char reg_val = 0;
    unsigned char eoc_status0, eoc_status1 = 0;

    // TO DO
    mdelay(150);
    eoc_status0 = mt_get_gpio_in(CHG_EOC_N);;
    mdelay(150);
    eoc_status1 = mt_get_gpio_in(CHG_EOC_N);;

    if( eoc_status1 && eoc_status0)
    {
        //<2015/1/26-davidda, [V10][Feature][Common][BSP][]add charging current indicator
        BMT_status.ICharging = 0; 
        //>2015/1/26-davidda
        printk("[charger_rt9536] :: (%s) eoc_status is not in charging mode(1)\n", __func__ );
        return 1;
    }
    printk("[charger_rt9536] :: (%s) eoc_status is in charging mode (0)\n", __func__ );
    return 0;
    //>2015/2/9-davidda
}

static void charging_ic_initialize(void)
{
    charging_ic_status = POWER_SUPPLY_TYPE_BATTERY;
}

static irqreturn_t charging_ic_interrupt_handler(int irq, void *data)
{
    ;
}

static void charging_timer_work(struct work_struct *work)
{
    ;
}

static int charging_ic_probe(struct platform_device *dev)
{
    int ret = 0;
	
    //<2015/1/30-davidda, [V10][Feature][Common][BSP][]Power on the phone without battery
    if(BMT_status.bat_exist)
    {
    printk("[charger_rt9536] :: charging IC Initialization BMT_status.bat_exist\n");
    mt_set_gpio_mode(CHG_EN_SET_N, CHG_EN_MODE);
    mt_set_gpio_dir(CHG_EN_SET_N, CHG_EN_DIR);
    mt_set_gpio_out(CHG_EN_SET_N, GPIO_OUT_ONE);  // charging off ; HIGH
    //mt_set_gpio_out(CHG_EN_SET_N, CHG_EN_DATA_OUT);  // charging off ; HIGH

    mt_set_gpio_mode(CHG_EOC_N, CHG_EOC_MODE);
    mt_set_gpio_dir(CHG_EOC_N, CHG_EOC_DIR);
    mt_set_gpio_pull_enable(CHG_EOC_N, CHG_EOC_PULL_ENABLE);
    mt_set_gpio_pull_select(CHG_EOC_N, CHG_EOC_PULL_SELECT);
    udelay(2000);

    charging_ic_initialize();
    }
    //>2015/1/30-davidda
    printk("[charger_rt9536] :: charging IC Initialization is done\n");


    chargin_hw_init_done = KAL_TRUE;

    
    return 0;
}

static int charging_ic_remove(struct platform_device *dev)
{
    charging_ic_deactive();

    return 0;
}

static int charging_ic_suspend(struct platform_device *dev, pm_message_t state)
{
    printk("[charger_rt9536] :: charging_ic_suspend \n");
    dev->dev.power.power_state = state;
    return 0;
}

static int charging_ic_resume(struct platform_device *dev)
{
    printk("[charger_rt9536] :: charging_ic_resume \n");
    dev->dev.power.power_state = PMSG_ON;
    return 0;
}

static struct platform_driver charging_ic_driver = {
    .probe = charging_ic_probe,
    .remove = charging_ic_remove,
//    .suspend = charging_ic_suspend,
//    .resume = charging_ic_resume,
    .driver = {
        .name = "ext_charger",
    },
};

static struct platform_device charger_ic_dev = {
	.name = "ext_charger",
	.id   = -1,
};


static int __init charging_ic_init(void)
{
    int ret=0;
    
    printk("[charger_rt9536] Charging IC Driver Init \n");
    
    ret = platform_device_register(&charger_ic_dev);
    if (ret) {
        printk("[charger_rt9536] Unable to device register(%d)\n", ret);
        return ret;
    }    
    
    return platform_driver_register(&charging_ic_driver);
}

static void __exit charging_ic_exit(void)
{
    printk("[charger_rt9536] Charging IC Driver Exit \n");
    platform_driver_unregister(&charging_ic_driver);
}

module_init(charging_ic_init);
module_exit(charging_ic_exit);
