#include <mt_gpio_base.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/ioctl.h>
#include "cust_gpio_usage.h"
#include <mach/mt_gpio.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
/*
mt_set_gpio_mode( GPIO_CTP_RST_PIN, GPIO_MODE_00 );
mt_set_gpio_dir( GPIO_CTP_RST_PIN, GPIO_DIR_OUT );
mt_set_gpio_out( GPIO_CTP_RST_PIN, GPIO_OUT_ONE );
*/

static int gpio_id_probe(struct platform_device *pdev);

static void gpio_func_rset(void) {
    mt_set_gpio_mode(GPIO7 | 0x80000000, GPIO_MODE_GPIO);
    mt_set_gpio_dir(GPIO7 | 0x80000000, GPIO_DIR_IN);
    mt_set_gpio_mode(GPIO8 | 0x80000000, GPIO_MODE_GPIO);
    mt_set_gpio_dir(GPIO8 | 0x80000000, GPIO_DIR_IN);
    mt_set_gpio_mode(GPIO9 | 0x80000000, GPIO_MODE_GPIO);
    mt_set_gpio_dir(GPIO9 | 0x80000000, GPIO_DIR_IN);
    mt_set_gpio_mode(GPIO4 | 0x80000000, GPIO_MODE_GPIO);
    mt_set_gpio_dir(GPIO4 | 0x80000000, GPIO_DIR_IN);
    mt_set_gpio_mode(GPIO118 | 0x80000000, GPIO_MODE_GPIO);
    mt_set_gpio_dir(GPIO118 | 0x80000000, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO118 | 0x80000000, GPIO_PULL_DISABLE);
    mt_set_gpio_mode(GPIO119 | 0x80000000, GPIO_MODE_GPIO);
    mt_set_gpio_dir(GPIO119 | 0x80000000, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO119 | 0x80000000, GPIO_PULL_DISABLE);
}
static ssize_t show_gpio_id_status(struct device *dev, struct device_attribute *attr, char *buf) {
    int gpio_stat[4] = {0};
    uint8_t ret = 0;
    gpio_func_rset();
    gpio_stat[1] = mt_get_gpio_in(GPIO7 | 0x80000000);
    gpio_stat[2] = mt_get_gpio_in(GPIO8 | 0x80000000);
    gpio_stat[3] = mt_get_gpio_in(GPIO9 | 0x80000000);
    gpio_stat[0] = mt_get_gpio_in(GPIO4 | 0x80000000);
    if (gpio_stat[3] == 0 && gpio_stat[2] == 0 && gpio_stat[1] == 0 && gpio_stat[0] == 1) {
        ret = sprintf(buf, "%s\n\r", "X160");
    }
    else if (gpio_stat[3] == 1 && gpio_stat[2] == 1 && gpio_stat[1] == 0 && gpio_stat[0] == 1) {
        ret = sprintf(buf, "%s\n\r", "X170fTV");
    }
    else if (gpio_stat[3] == 1 && gpio_stat[2] == 1 && gpio_stat[1] == 1 && gpio_stat[0] == 1) {
        ret = sprintf(buf, "%s\n\r", "X160t");
    }
    else if (gpio_stat[3] == 0 && gpio_stat[2] == 1 && gpio_stat[1] == 0 && gpio_stat[0] == 0) {
        ret = sprintf(buf, "%s\n\r", "X165g");
    }
    else if (gpio_stat[3] == 0 && gpio_stat[2] == 1 && gpio_stat[1] == 0 && gpio_stat[0] == 1) {
        ret = sprintf(buf, "%s\n\r", "X170g");
    }
    else if (gpio_stat[3] == 0 && gpio_stat[2] == 0 && gpio_stat[1] == 1 && gpio_stat[0] == 0) {
        ret = sprintf(buf, "%s\n\r", "X165j");
    }
    else if (gpio_stat[3] == 1 && gpio_stat[2] == 0 && gpio_stat[1] == 1 && gpio_stat[0] == 1) {
        ret = sprintf(buf, "%s\n\r", "X155");
    }
    else if (gpio_stat[3] == 1 && gpio_stat[2] == 0 && gpio_stat[1] == 1 && gpio_stat[0] == 0) {
        ret = sprintf(buf, "%s\n\r", "X150");
    }
    else {
        ret = sprintf(buf, "%s,%d%d%d%d\n\r", "not configure", gpio_stat[3],gpio_stat[2],gpio_stat[1],gpio_stat[0]);
    }
    return ret + 1;
}

static ssize_t show_pstage_id_status(struct device *dev, struct device_attribute *attr, char *buf) {
    uint8_t len = 0;
    int gpio_stat[2] = {0};
    gpio_func_rset();
    gpio_stat[0] = mt_get_gpio_in(GPIO118 | 0x80000000);
    gpio_stat[1] = mt_get_gpio_in(GPIO119 | 0x80000000);
    if (gpio_stat[1] == 1 && gpio_stat[0] == 1) {
        len = sprintf(buf, "%s\n\r", "002");         
    }
    else if (gpio_stat[1] == 0 && gpio_stat[0] == 1) {
        len = sprintf(buf, "%s\n\r", "003"); 
    }
    else if (gpio_stat[1] == 1 && gpio_stat[0] == 0) {
        len = sprintf(buf, "%s\n\r", "004"); 
    }
    else if (gpio_stat[1] == 0 && gpio_stat[0] == 0) {
        len = sprintf(buf, "%s\n\r", "005"); 
    }
    return len + 1;
}

DRIVER_ATTR(gpio_id, 0644, show_gpio_id_status, NULL);
DRIVER_ATTR(pstage_id, 0644, show_pstage_id_status, NULL);
static struct driver_attribute *gpio_id_attr[] = {
    &driver_attr_gpio_id,
    &driver_attr_pstage_id,
};

static int gpio_id_remove(struct platform_driver *pdev) {
    return 0;
}

static struct platform_driver gpio_pltform_drv = {
    .probe = gpio_id_probe,
    .remove = gpio_id_remove,
    .driver = {
        .name = "gpio_id",
        .owner = THIS_MODULE,
    }
};

static struct platform_device gpio_pltform_dev = {
    .name = "gpio_id",
};

static int gpio_id_probe(struct platform_device *pdev) {
    int err = 0;
    uint8_t loop = 0;
    printk("%s\n\r", __func__);
    for (loop = 0; loop < sizeof(gpio_id_attr) / sizeof(gpio_id_attr[0]); loop++) {
        err = driver_create_file(&(gpio_pltform_drv.driver), gpio_id_attr[loop]);
        if (err) {
            printk(KERN_ERR "gpio id driver create file failed\n\r");
            return err;
        }
    }
    printk("gpio id probe success\n\r");
    return 0;
}

static int __init gpio_id_init(void) {
    printk("%s\n\r", __func__);
    platform_device_register(&gpio_pltform_dev);
    platform_driver_register(&gpio_pltform_drv);
    return 0;
}

static void __exit gpio_id_exit(void) {
    platform_device_unregister(&gpio_pltform_dev);
    platform_driver_unregister(&gpio_pltform_drv);
}

module_init(gpio_id_init);
module_exit(gpio_id_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("lever.ni@arimatech.com.cn");
MODULE_DESCRIPTION("add show modem auto detect gpio status");