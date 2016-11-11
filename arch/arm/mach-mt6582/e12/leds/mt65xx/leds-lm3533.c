/*
 * License Terms: GNU General Public License v2
 *
 * Simple driver for National Semiconductor LM3533 LEDdriver chip *
 * 
 * based on leds-lm3533.c 
 */ 

#include <linux/module.h>
#include <leds-lm3533.h>
#include <mach/mt_gpio.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <mach/mt_pm_ldo.h>
#include <linux/delay.h>

#define LM3533_DEV_NAME        "LM3533"
#define LM3533_HW_ENABLE_PIN GPIO117
#define LM3533_USB_ENABLE_PIN GPIO119  //20141029   tracy   add  for   charge  led   
#define LM3533_I2C_SLAVE 1

static const struct i2c_device_id lm3533_i2c_id[] = {{LM3533_DEV_NAME,0},{}};
static struct i2c_board_info __initdata i2c_lm3533={ I2C_BOARD_INFO("LM3533", (0x36))};
//20141023   tracy add for  pull low  led gpio when  led  sleep start
static int sleep_flag=0;
static int sleep_flag_red=1;
static int sleep_flag_blue=1;
static int sleep_flag_green=1;
static int sleep_flag_light=0;          //20141029 tracy modify  for fix dispaly not show when plug usb
static int lm3533_init=0;
//20141023   tracy add for  pull low  led gpio when  led  sleep end
//20140422 tracy modify for backlight control++
u8 brightness_table[]=
{
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB2,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF
};
//20140422 tracy modify for backlight control--
/*----------------------------------------------------------------------------*/
static int lm3533_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id); 
static int lm3533_i2c_remove(struct i2c_client *client);
static int lm3533_hw_enable(void);
static int lm3533_sleep(int enable);         //20141029 tracy modify  for fix dispaly not show when plug usb 
/*----------------------------------------------------------------------------*/

static struct i2c_driver lm3533_i2c_driver = {
    .driver = {
        .owner          = THIS_MODULE,
        .name           = LM3533_DEV_NAME,
    },
	.probe = lm3533_i2c_probe,
	.remove = lm3533_i2c_remove,
	.id_table = lm3533_i2c_id,
};

static struct i2c_client* lm3533_client = NULL;

static int lm3533_hw_enable(void)
{
	//set HW enable ++
	mt_set_gpio_mode(LM3533_HW_ENABLE_PIN, GPIO_MODE_GPIO);
	mt_set_gpio_dir(LM3533_HW_ENABLE_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(LM3533_HW_ENABLE_PIN, GPIO_OUT_ONE);
	// set HW enable --
	//20141029   tracy   add  for   charge  led   start
    mt_set_gpio_mode(LM3533_USB_ENABLE_PIN, GPIO_MODE_GPIO);
	mt_set_gpio_dir(LM3533_USB_ENABLE_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(LM3533_USB_ENABLE_PIN, GPIO_OUT_ONE);
	//20141029   tracy   add  for   charge  led   end
	printk(KERN_EMERG "lm3533_hw_enable : %d \n",mt_get_gpio_out(LM3533_HW_ENABLE_PIN));

	return 0; // maybe we should add some debug info
	
}

static int lm3533_set_backlight_on(void)
{	
   //20140530 tracy add for rgb led++
   int  err = 0;
   u8 LM3533_data[32];
   memset(LM3533_data,0,32);    
   //20140521 tracy modify led backlight config for pdp++
   //20140925 tracy modify Backlight switching freq to 1MHz
   i2c_smbus_write_byte_data(lm3533_client, LM3533_OVP_FREQUENCY_PWM_POLARITY, 0x0D);	
	
   LM3533_data[0]=0x00;
   LM3533_data[1]=0x00;
   LM3533_data[2]=0x00;
   LM3533_data[3]=0x00;
   LM3533_data[4]=0x00;
   LM3533_data[5]=0x00;
   err = i2c_smbus_write_i2c_block_data(lm3533_client,LM3533_CONTROL_BANK_A_PWM_CONFIGURATION,6,LM3533_data);
   if(err)    
    {
        printk(KERN_NOTICE "LM3533_CONTROL_BANK_A_PWM_CONFIGURATION \n");
    };
	//20141112 tracy modify for change backlight led current seting for power consumption start
    LM3533_data[0]=0x0D;
    LM3533_data[1]=0x0D;
    LM3533_data[2]=0x00;
    LM3533_data[3]=0x00;   
    LM3533_data[4]=0x00;   
    //20141112 tracy modify for change backlight led current setting for power consumption end
    err = i2c_smbus_write_i2c_block_data(lm3533_client,LM3533_CONTROL_BANK_A_FULL_SCALE_CURRENT,5,LM3533_data);
    if(err)    
    {
        printk(KERN_NOTICE "LM3533_CONTROL_BANK_C_FULL_SCALE_CURRENT \n");
    };

    LM3533_data[0]=0x0A;
    LM3533_data[1]=0x04;
    LM3533_data[2]=0x04;
    LM3533_data[3]=0x04;

    err = i2c_smbus_write_i2c_block_data(lm3533_client,LM3533_CONTROL_BANK_A_B_BRIGHTNESS_CONFIGURATION,4,LM3533_data);//20130722 tracy add for notification led++

    if(err)    
    {
        printk(KERN_NOTICE "LM3533_CONTROL_BANK_A_B_BRIGHTNESS_CONFIGURATION \n");
    };    
    //20141023   tracy add for  pull low  led gpio when  led  sleep start
    if(lm3533_init==0)
    {
    LM3533_data[0]=0xAF;
    LM3533_data[1]=0xAF;
    LM3533_data[2]=0x00;
    LM3533_data[3]=0x00;  
    LM3533_data[4]=0x00;  
    i2c_smbus_write_i2c_block_data(lm3533_client,LM3533_BRIGHTNESS_REGISTER_A,5,LM3533_data);
     lm3533_init=1;
    }	
    else
    {
    LM3533_data[0]=0x00;
    LM3533_data[1]=0x00;
    LM3533_data[2]=0x00;
    LM3533_data[3]=0x00;  
    LM3533_data[4]=0x00;  
    i2c_smbus_write_i2c_block_data(lm3533_client,LM3533_BRIGHTNESS_REGISTER_A,5,LM3533_data);
    }
    //20141023   tracy add for  pull low  led gpio when  led  sleep end
	
	i2c_smbus_write_byte_data(lm3533_client, LM3533_CONTROL_ENABLE, 0x1F);	//backlight offset
	//20140530 tracy add for rgb led--
	//20140521 tracy modify led backlight config for pdp++
    printk(KERN_EMERG "lm3533_set_backlight_on \n");
	return 0;		
	
}

//20140422 tracy modify for backlight control++
int lm3533_backlight_control(unsigned long brightness)
{    
   int led_brightness = 0;

   led_brightness = brightness;

   if (led_brightness > 255)
     {
       printk(KERN_INFO"tracy: led_brightness exceed limitation = %d \n", led_brightness);
       led_brightness = 255;
     }
   //20141029 tracy modify  for fix dispaly not show when plug usb start
   if(led_brightness==0)
     {
       	sleep_flag_light=1;
	    lm3533_sleep(1);
     }
   else
	 {
        sleep_flag_light=0;
	    if(sleep_flag==1)
           {
               lm3533_sleep(0);
           }
	 
	 }
   //20141029 tracy modify  for fix dispaly not show when plug usb end
   i2c_smbus_write_byte_data(lm3533_client, LM3533_BRIGHTNESS_REGISTER_A,brightness_table[led_brightness]); 
   i2c_smbus_write_byte_data(lm3533_client, LM3533_BRIGHTNESS_REGISTER_B,brightness_table[led_brightness]);  //20140521 tracy modify led backlight config for pdp
  
  return 0;
}
//20140422 tracy modify for backlight control--
//20140530 tracy add for rgb led++
int lm3533_blue_brightness_control(unsigned long brightness)
{    
   int blue_brightness=0;

   blue_brightness=brightness;

   //20141023   tracy add for  pull low  led gpio when  led  sleep start
   //20141029 tracy modify  for fix dispaly not show when plug usb start
   if(blue_brightness==0)
     {
       sleep_flag_blue=1;
     }
   else
	 {
       sleep_flag_blue=0;
	   if(sleep_flag==1)
         {
           lm3533_sleep(0);
         }
	 }
	 
    //20141029 tracy modify  for fix dispaly not show when plug usb end
    //20141023   tracy add for  pull low  led gpio when  led  sleep end
	i2c_smbus_write_byte_data(lm3533_client, LM3533_BRIGHTNESS_REGISTER_C,brightness_table[blue_brightness]); 
  
    
    return 0;
}

int lm3533_red_brightness_control(unsigned long brightness)
{    
   int red_brightness=0;

   red_brightness=brightness;

   //20141023   tracy add for  pull low  led gpio when  led  sleep start
   //20141029 tracy modify  for fix dispaly not show when plug usb start
   if(red_brightness==0)
     {
       sleep_flag_red=1;
     }
   else
	 {
       sleep_flag_red=0;
	   if(sleep_flag==1)
         {
           lm3533_sleep(0);
         } 
	 }	
   //20141029 tracy modify  for fix dispaly not show when plug usb end
   //20141023   tracy add for  pull low  led gpio when  led  sleep end
   i2c_smbus_write_byte_data(lm3533_client, LM3533_BRIGHTNESS_REGISTER_E,brightness_table[red_brightness]); 

   return 0;
}

int lm3533_green_brightness_control(unsigned long brightness)
{    
   int green_brightness=0;

   green_brightness=brightness;

   //20141023   tracy add for  pull low  led gpio when  led  sleep start
   if(green_brightness==0)
      {
        sleep_flag_green=1;
      }
   else
	 {
        sleep_flag_green=0;
	    if(sleep_flag==1)
          {
            lm3533_sleep(0);
          }
	 }
   //20141023 tracy add for  pull low  led gpio when  led  sleep end
   i2c_smbus_write_byte_data(lm3533_client, LM3533_BRIGHTNESS_REGISTER_D,brightness_table[green_brightness]);
   
   return 0;
}
//20140530 tracy add for rgb led--
//20141023   tracy add for  pull low  led gpio when  led  sleep start
static int lm3533_sleep(int enable)
{
  if (enable) 
     {
	   if((sleep_flag_red==1)&&(sleep_flag_green==1)&&(sleep_flag_blue==1)&&(sleep_flag_light=1))
	  	{
	         sleep_flag=1;
		     mt_set_gpio_out(LM3533_HW_ENABLE_PIN, GPIO_OUT_ZERO);
	         printk(KERN_INFO"tracy:lm3533_sleep_check : %d \n",mt_get_gpio_out(LM3533_HW_ENABLE_PIN));
	  	}
	  else
	  	{
              sleep_flag=0;
	  	}
      }
  else
     {
       if(sleep_flag==1)
		{
		    sleep_flag=0;
		    printk(KERN_INFO"tracy:lm3533 wake up \n");
            lm3533_hw_enable();
            lm3533_set_backlight_on();	
		}
	  }
}
//20141023   tracy add for  pull low  led gpio when  led  sleep end
static int lm3533_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{

	struct lm3533_platform_data *lm3533_pdata = client->dev.platform_data;
	printk(KERN_EMERG "in lm3533_i2c_probe");	

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) 
      {
		printk(KERN_EMERG "lm3533_i2c_probe probe error !! \n");
		return -ENODEV;
	  }		

	printk(KERN_EMERG "LM3533_CURRENT_SINK_OUTPUT_CONFIGURATION1 = %d \n", i2c_smbus_read_byte_data(client, LM3533_CURRENT_SINK_OUTPUT_CONFIGURATION1));
	lm3533_client = client;
	lm3533_set_backlight_on();	
		
	return 0;	
}

static int lm3533_i2c_remove(struct i2c_client *client)
{
	return 0;
}

/*-------------------------------------------------------------*/
static int lm3533_probe(struct platform_device *pdev) 
{

	printk(KERN_EMERG "lm3533_probe \n");

	if(i2c_add_driver(&lm3533_i2c_driver))
	{
		printk(KERN_EMERG"add driver error\n");
		return -1;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static int lm3533_remove(struct platform_device *pdev)
{   
    i2c_del_driver(&lm3533_i2c_driver);
    return 0;
}

static struct platform_driver lm3533_led_driver = {
	.probe      = lm3533_probe,
	.remove     = lm3533_remove,    
	.driver     = {
		.name  = "leds-lm3533",
		.owner = THIS_MODULE,
	}
};

static int __init lm3533_module_init(void)
{

	/*change defult voltage++*/
	if(!hwPowerOn(MT6323_POWER_LDO_VIO28, 3000, "LM3533"))
		printk(KERN_EMERG "init power VIO28 volt fail \n");
	if(!hwPowerOn(MT6323_POWER_LDO_VIO18, 1800, "LM3533"))	
		printk(KERN_EMERG "init power VIO18  volt fail \n");
	/*change defult voltage--*/
	
	lm3533_hw_enable();
	
	i2c_register_board_info(LM3533_I2C_SLAVE, &i2c_lm3533, 1);  // 1 device
		printk(KERN_EMERG "register board info \n");

	if(platform_driver_register(&lm3533_led_driver))
	{
		printk(KERN_ERR "failed to register lm3533_led_driver");
		return -ENODEV;
	}
	
	printk(KERN_NOTICE "lm3533_module_init  \n");
	
	return 0;
}

static void __exit lm3533_module_exit(void)
{
	platform_driver_unregister(&lm3533_led_driver);
}

module_init(lm3533_module_init);
module_exit(lm3533_module_exit);

MODULE_DESCRIPTION("Back Light driver for LM3533");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("EdisonShih@Arimacomm");

