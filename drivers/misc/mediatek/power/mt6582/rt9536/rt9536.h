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

#ifndef __LINUX_RT9536_CHARGER_H
#define __LINUX_RT9536_CHARGER_H
//<2015/1/20-davidda, [V10][Feature][Common][BSP][]enable rt9536
#if 1
#define CHG_EN_SET_N                 (GPIO45| 0x80000000) //   (GPIO45| 0x80000000) //(GPIO118 | 0x80000000) //GPIO_SWCHARGER_EN_PIN
#define CHG_EN_MODE                     GPIO_MODE_00   //GPIO_SWCHARGER_EN_PIN_M_GPIO
#define CHG_EN_DIR                        GPIO45_DIR // GPIO118_DIR
#define CHG_EN_DATA_OUT            GPIO45_DATAOUT // GPIO118_DATAOUT
 //#define CHG_EN_PULL_ENABLE        GPIO35_PULLEN
 //#define CHG_EN_PULL_SELECT        GPIO35_PULL
 #define CHG_EOC_N                        (GPIO43| 0x80000000)//(GPIO119 | 0x80000000) //GPIO_EINT_CHG_STAT_PIN
 #define CHG_EOC_MODE                 GPIO_MODE_00 //GPIO_EINT_CHG_STAT_PIN_M_GPIO
 #define CHG_EOC_DIR                   GPIO43_DIR// GPIO119_DIR
 #define CHG_EOC_PULL_ENABLE  GPIO43_PULLEN// GPIO119_PULLEN
 #define CHG_EOC_PULL_SELECT   GPIO43_PULL//  GPIO119_PULL
#else
#define CHG_EN_SET_N                   (GPIO118 | 0x80000000) //GPIO_SWCHARGER_EN_PIN
#define CHG_EN_MODE                     GPIO_MODE_00   //GPIO_SWCHARGER_EN_PIN_M_GPIO
#define CHG_EN_DIR                        GPIO118_DIR
#define CHG_EN_DATA_OUT            GPIO0_DATAOUT // GPIO118_DATAOUT
 //#define CHG_EN_PULL_ENABLE        GPIO35_PULLEN
 //#define CHG_EN_PULL_SELECT        GPIO35_PULL
 #define CHG_EOC_N                        (GPIO119 | 0x80000000) //GPIO_EINT_CHG_STAT_PIN
 #define CHG_EOC_MODE                  GPIO_MODE_00 //GPIO_EINT_CHG_STAT_PIN_M_GPIO
 #define CHG_EOC_DIR                     GPIO119_DIR
 #define CHG_EOC_PULL_ENABLE    GPIO119_PULLEN
 #define CHG_EOC_PULL_SELECT     GPIO119_PULL
#endif
//>2015/1/20-davidda

/* Function Prototype */
//enum power_supply_type get_charging_ic_status(void);

extern void charging_ic_active_default(void);
//void charging_ic_set_ta_mode(void);
//void charging_ic_set_usb_mode(void);
extern void charging_ic_deactive(void);
//void charging_ic_set_factory_mode(void);
extern void rt9536_charging_enable(unsigned int set_current, unsigned int enable);
extern unsigned char rt9536_check_eoc_status(void);

#endif /* __LINUX_MAX8971_CHARGER_H */
