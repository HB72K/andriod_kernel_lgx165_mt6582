/*
 * License Terms: GNU General Public License v2
 *
 * Simple driver for National Semiconductor LM3533 LEDdriver chip *
 * 
 * based on leds-lm3533.c 
 */ 

#include <linux/module.h>
#include <leds-sky81294.h>
#include <mach/mt_gpio.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <mach/mt_pm_ldo.h>
#include <linux/delay.h>

#define SKY81294_DEV_NAME        "SKY81294"
#define SKY81294_STROBE_PIN GPIO12
#define SKY81294_I2C_SLAVE 1

//20141008 tracy modify for flashled keep light on start
static const struct i2c_device_id sky81294_i2c_id[] = {{SKY81294_DEV_NAME,0},{}};
static struct i2c_board_info __initdata i2c_sky81294={ I2C_BOARD_INFO("SKY81294", (0x37))};
//20141008 tracy modify for flashled keep light on end


/*----------------------------------------------------------------------------*/
static int sky81294_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id); 
static int sky81294_i2c_remove(struct i2c_client *client);
static int sky81294_hw_enable(void);
/*----------------------------------------------------------------------------*/

static struct i2c_driver sky81294_i2c_driver = {
    .driver = {
        .owner          = THIS_MODULE,
        .name           = SKY81294_DEV_NAME,
    },
	.probe = sky81294_i2c_probe,
	.remove = sky81294_i2c_remove,
	.id_table = sky81294_i2c_id,
};

static struct i2c_client* sky81294_client = NULL;

static int sky81294_hw_enable(void)
{
	//set STROBE enable ++
	mt_set_gpio_mode(SKY81294_STROBE_PIN, GPIO_MODE_GPIO);
	mt_set_gpio_dir(SKY81294_STROBE_PIN, GPIO_DIR_OUT);
	
	// set STROBE enable --

	printk(KERN_EMERG "sky81294_hw_enable : %d \n",mt_get_gpio_out(SKY81294_STROBE_PIN));

	return 0; // maybe we should add some debug info
	
}

int sky81294_set_flashlight_control(unsigned long brightness)
{	
   int flash_brightness;
 //20141008 tracy modify for flashled keep light on start
   flash_brightness=brightness;
  printk(KERN_EMERG "sky81294_ flash_brightness=%d !! \n",  flash_brightness);
  //20141027 tracy add for change flashlight setting for add flashled brightness start
   if( flash_brightness==255)
   	{
   	        printk(KERN_EMERG "sky81294_movie_mode !! \n");
		i2c_smbus_write_byte_data(sky81294_client,0x02, 0x03);	
		i2c_smbus_write_byte_data(sky81294_client,0x03, 0x01);	
	
   	}
   else  if( flash_brightness==200)
    	{
    	    printk(KERN_EMERG "sky81294_flash_mode !! \n");
             i2c_smbus_write_byte_data(sky81294_client,0x00, 0x0D);	
             i2c_smbus_write_byte_data(sky81294_client,0x01, 0x05);	
	     i2c_smbus_write_byte_data(sky81294_client,0x03, 0x02);	

    	}
    //20141027 tracy add for change flashlight setting for add flashled brightness end
 else
 	{
             i2c_smbus_write_byte_data(sky81294_client,0x03, 0x00);	
 	}
//20141008 tracy modify for flashled keep light on end

}

static int sky81294_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{

	struct sky81294_platform_data *sky81294_pdata = client->dev.platform_data;
	printk(KERN_EMERG "in sky81294_i2c_probe");	

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) 
      {
		printk(KERN_EMERG "sky81294_i2c_probe probe error !! \n");
		return -ENODEV;
	}		
	
	//20141008 tracy modify for flashled keep light on start
	sky81294_client = client;
	//20141008 tracy modify for flashled keep light on end

	return 0;	
}

static int sky81294_i2c_remove(struct i2c_client *client)
{
	return 0;
}

/*-------------------------------------------------------------*/
static int sky81294_probe(struct platform_device *pdev) 
{
	//struct acc_hw *hw = get_cust_acc_hw();
	//GSE_FUN();

	printk(KERN_EMERG "sky81294_probe \n");

	if(i2c_add_driver(&sky81294_i2c_driver))
	{
		printk(KERN_EMERG"add driver error\n");
		return -1;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static int sky81294_remove(struct platform_device *pdev)
{   
    i2c_del_driver(&sky81294_i2c_driver);
    return 0;
}

static struct platform_driver sky81294_led_driver = {
	.probe      = sky81294_probe,
	.remove     = sky81294_remove,    
	.driver     = {
		.name  = "leds-sky81294",
		.owner = THIS_MODULE,
	}
};

static int __init sky81294_module_init(void)
{

	/*change defult voltage++*/
	if(!hwPowerOn(MT6323_POWER_LDO_VIO28, 3000, "SKY81294"))
		printk(KERN_EMERG "init power VIO28 volt fail \n");
	if(!hwPowerOn(MT6323_POWER_LDO_VIO18, 1800, "SKY81294"))	
		printk(KERN_EMERG "init power VIO18  volt fail \n");
	/*change defult voltage--*/
	
	sky81294_hw_enable();
	
	i2c_register_board_info(SKY81294_I2C_SLAVE, &i2c_sky81294, 1);  // 1 device
		printk(KERN_EMERG "register board info \n");

	if(platform_driver_register(&sky81294_led_driver))
	{
		printk(KERN_ERR "failed to register sky81294_led_driver");
		return -ENODEV;
	}
	
	printk(KERN_NOTICE "sky81294_module_init  \n");
	
	return 0;
}

static void __exit sky81294_module_exit(void)
{
	platform_driver_unregister(&sky81294_led_driver);
}

module_init(sky81294_module_init);
module_exit(sky81294_module_exit);

MODULE_DESCRIPTION("Flash Light driver for SKY81294");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("TractChui@Arimacomm");

