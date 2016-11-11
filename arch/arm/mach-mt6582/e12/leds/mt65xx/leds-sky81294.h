/*
 * leds-lp3944.h - platform data structure for lp3944 led controller
 *
 * Copyright (C) 2009 Antonio Ospite <ospite@studenti.unina.it>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef __LINUX_LEDS_SKY81294_H
#define __LINUX_LEDS_SKY81294_H

#define SKY81294_LEDS_MAX 3
//20140815 tracy modify for change flashled start
#define SKY81294_FLASH_CURRENT 0x00
#define SKY81294_FLASH_TIME 0x01
#define SKY81294_MOVIE_CURRENT 0x02
#define SKY81294_CONTROL1 0x03
#define SKY81294_CONTROL2 0x04
#define SKY81294_LINKING_TIME 0x05
//20140815 tracy modify for change flashled start
enum sky81294_type {
	SKY81294_LED_TYPE_NONE,
	SKY81294_LED_TYPE_LED,
};


struct sky81294_led {
	char *name;
	enum sky81294_type type;
};



struct sky81294_platform_data {
	struct sky81294_led leds[SKY81294_LEDS_MAX];
	u8 leds_size;
};

extern int  sky81294_set_flashlight_control(unsigned long brightness);

#endif /* __LINUX_LEDS_SKY81294_H */
