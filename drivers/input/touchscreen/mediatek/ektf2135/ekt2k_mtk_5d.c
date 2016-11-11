/* touchscreen/ektf2k_kthread_mtk.c - ELAN EKTF2K touchscreen driver
 * for MTK65xx serial platform.
 *
 * Copyright (C) 2012 Elan Microelectronics Corporation.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * 2011/12/06: The first release, version 0x0001
 * 2012/2/15:  The second release, version 0x0002 for new bootcode
 * 2012/5/8:   Release version 0x0003 for china market
 *             Integrated 2 and 5 fingers driver code together and
 *             auto-mapping resolution.
 * 2012/8/24:  MTK version
 * 2013/2/1:   Release for MTK6589/6577/6575/6573/6513 Platform
 *             For MTK6575/6573/6513, please disable both of ELAN_MTK6577 and MTK6589DMA.
 *                          It will use 8+8+2 received packet protocol
 *             For MTK6577, please enable ELAN_MTK6577 and disable MTK6589DMA.
 *                          It will use Elan standard protocol (18bytes of protocol).
 *             For MTK6589, please enable both of ELAN_MTK6577 and MTK6589DMA.
 * 2013/5/15   Fixed MTK6589_DMA issue.
 */

//#define SOFTKEY_AXIS_VER
//#define ELAN_TEN_FINGERS
#define MTK6589_DMA
#define ELAN_MTK6577
//#define ELAN_BUTTON
//#define TPD_HAVE_BUTTON

#ifdef ELAN_TEN_FINGERS
#define PACKET_SIZE		44		/* support 10 fingers packet */
#else
//<<Mel - 4/10, Modify firmware support 2 fingers. 
//#define PACKET_SIZE		8 		/* support 2 fingers packet  */
//20140619 tracy modify for  firmware support 5 fingers. 
#define PACKET_SIZE		18			/* support 5 fingers packet  */
//>>Mel - 4/10, Modify firmware support 2 fingers. 
#endif

#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/earlysuspend.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/rtpm_prio.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/miscdevice.h>
#include <linux/hrtimer.h>

#include <linux/dma-mapping.h>

#include <mach/mt_pm_ldo.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_boot.h>
#ifndef TPD_NO_GPIO
#include "cust_gpio_usage.h"
#endif

// for linux 2.6.36.3
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <asm/ioctl.h>

//dma
#include <linux/dma-mapping.h>
//20140903 tracy add for fix 5" touch
#include <linux/wakelock.h>  

//#include "ekth3250.h"
#include "tpd.h"
//#include "mach/eint.h"

//#include "tpd_custom_ekth3250.h"
#include "ektf2k_mtk.h"

#include <cust_eint.h>
//20140901 tracy add for tp replace psensor start
/*
#include <linux/hwmsensor.h>
#include <linux/hwmsen_dev.h>
#include <linux/sensors_io.h>
*/
//20140901 tracy add for tp replace psensor end
//#define ELAN_DEBUG
//20140917 tracy add touch replace ps define start
//20141030 tracy close touch replace ps define 
#define ELAN_PS 0
#include <linux/hwmsensor.h>
#include <linux/hwmsen_dev.h>
#include <linux/sensors_io.h>
//20140917 tracy add touch replace ps define end

#define PWR_STATE_DEEP_SLEEP	0
#define PWR_STATE_NORMAL		1
#define PWR_STATE_MASK			BIT(3)

#define CMD_S_PKT			0x52
#define CMD_R_PKT			0x53
#define CMD_W_PKT			0x54

#define HELLO_PKT			0x55
#define FIVE_FINGERS_PKT			0x5D
#define   MTK_FINGERS_PKT                  0x6D    /** 2 Fingers: 5A 5 Fingers 5D, 10 Fingers: 62 **/

#define TWO_FINGERS_PKT      0x5A
#define MTK_FINGERS_PKT       0x6D
#define TEN_FINGERS_PKT	0x62

#define RESET_PKT			0x77
#define CALIB_PKT			0xA8


#define TPD_OK 0
//#define HAVE_TOUCH_KEY

#define LCT_VIRTUAL_KEY

#ifdef MTK6589_DMA
static uint8_t *gpDMABuf_va = NULL;
static uint32_t gpDMABuf_pa = NULL;
#endif

//20140917 tracy add touch replace ps define start
#if ELAN_PS
int ckt_tp_replace_ps_mod_on(void);
int ckt_tp_replace_ps_mod_off(void);
int  ckt_tp_replace_ps_enable(int enable);
u16  ckt_get_tp_replace_ps_value(void);
int ckt_tp_replace_ps_state= 0;
int ckt_tp_replace_ps_close = 0;
#endif
//20140917 tracy add touch replace ps define end
//20140901 tracy add for tp replace psensor end
//20140722 tracy add for ESD -start
#define ESD_CHECK 
#ifdef ESD_CHECK
	static int have_interrupts = 0;
	static struct workqueue_struct *esd_wq = NULL;
	static struct delayed_work esd_work;
	static unsigned long delay = 2*HZ;

    //declare function
    static void elan_touch_esd_func(struct work_struct *work);
#endif
//20140722 tracy add for esd -end

#ifdef TPD_HAVE_BUTTON
#define TPD_KEY_COUNT           3
#define TPD_KEYS                { KEY_MENU, KEY_HOMEPAGE, KEY_BACK}
#define TPD_KEYS_DIM            {{107,1370,109,TPD_BUTTON_HEIGH},{365,1370,109,TPD_BUTTON_HEIGH},{617,1370,102,TPD_BUTTON_HEIGH}}

static int tpd_keys_local[TPD_KEY_COUNT] = TPD_KEYS;
static int tpd_keys_dim_local[TPD_KEY_COUNT][4] = TPD_KEYS_DIM;
#endif

//<<Mel - 4/10, Add pin define. 
#define CUST_EINT_POLARITY_LOW         0
#define CUST_EINT_TOUCH_PANEL_SENSITIVE        1
//Mel - 4/10, Add pin define>>. 

// modify
#define SYSTEM_RESET_PIN_SR 	135

//Add these Define

#define IAP_PORTION            	1
#define PAGERETRY  10//20141105 tracy add for FW Update start
#define IAPRESTART 5
#define CMD_54001234	   0


// For Firmware Update 
#define ELAN_IOCTLID	0xD0
#define IOCTL_I2C_SLAVE	_IOW(ELAN_IOCTLID,  1, int)
#define IOCTL_MAJOR_FW_VER  _IOR(ELAN_IOCTLID, 2, int)
#define IOCTL_MINOR_FW_VER  _IOR(ELAN_IOCTLID, 3, int)
#define IOCTL_RESET  _IOR(ELAN_IOCTLID, 4, int)
#define IOCTL_IAP_MODE_LOCK  _IOR(ELAN_IOCTLID, 5, int)
#define IOCTL_CHECK_RECOVERY_MODE  _IOR(ELAN_IOCTLID, 6, int)
#define IOCTL_FW_VER  _IOR(ELAN_IOCTLID, 7, int)
#define IOCTL_X_RESOLUTION  _IOR(ELAN_IOCTLID, 8, int)
#define IOCTL_Y_RESOLUTION  _IOR(ELAN_IOCTLID, 9, int)
#define IOCTL_FW_ID  _IOR(ELAN_IOCTLID, 10, int)
#define IOCTL_ROUGH_CALIBRATE  _IOR(ELAN_IOCTLID, 11, int)
#define IOCTL_IAP_MODE_UNLOCK  _IOR(ELAN_IOCTLID, 12, int)
#define IOCTL_I2C_INT  _IOR(ELAN_IOCTLID, 13, int)
#define IOCTL_RESUME  _IOR(ELAN_IOCTLID, 14, int)
#define IOCTL_POWER_LOCK  _IOR(ELAN_IOCTLID, 15, int)
#define IOCTL_POWER_UNLOCK  _IOR(ELAN_IOCTLID, 16, int)
#define IOCTL_FW_UPDATE  _IOR(ELAN_IOCTLID, 17, int)
#define IOCTL_BC_VER  _IOR(ELAN_IOCTLID, 18, int)
#define IOCTL_2WIREICE  _IOR(ELAN_IOCTLID, 19, int)
#define IOCTL_GET_UPDATE_PROGREE	_IOR(CUSTOMER_IOCTLID,  2, int)


#define CUSTOMER_IOCTLID	0xA0
#define IOCTL_CIRCUIT_CHECK  _IOR(CUSTOMER_IOCTLID, 1, int)
//20141227 tracy modify for main/second source FW Update for user mode
#define Touch_Detect_Pin GPIO9 


extern struct tpd_device *tpd;

uint8_t RECOVERY=0x00;
int FW_VERSION=0x00;
int X_RESOLUTION=0x00; // Please fill the right resolution if resolution mapping error. 
int Y_RESOLUTION=0x00; // Please fill the right resolution if resolution mapping error.
int FW_ID=0x00;
int BC_VERSION = 0x00;
int work_lock=0x00;
int power_lock=0x00;
int circuit_ver=0x01;
int button_state = 0;
int updatefw_flag=0;    //20141030 tracy add for avoid update FW  fail
/*++++i2c transfer start+++++++*/
//<<Mel - 4/10, Modify I2C slave address to 0x15. 
//int file_fops_addr=0x10;
int file_fops_addr=0x15;
//Mel - 4/10, Modify I2C slave address to 0x15>>. 
/*++++i2c transfer end+++++++*/
int tpd_down_flag=0;
//20140917 tracy add touch replace ps define start
#if ELAN_PS
int ps_enable_flag=1;
#endif
//20140917 tracy add touch replace ps define end

struct i2c_client *i2c_client = NULL;
struct task_struct *thread = NULL;
//20140903 tracy add for fix 5" touch start
struct task_struct *thread_update = NULL;
//20140903 tracy add for fix 5" touch end

static DECLARE_WAIT_QUEUE_HEAD(waiter);
static inline int elan_ktf2k_ts_parse_xy(uint8_t *data,
			uint16_t *x, uint16_t *y);
#if 0
extern void mt65xx_eint_unmask(unsigned int line);
extern void mt65xx_eint_mask(unsigned int line);
extern void mt65xx_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 mt65xx_eint_set_sens(kal_uint8 eintno, kal_bool sens);
extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);
#endif

static int tpd_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int tpd_detect(struct i2c_client *client, int kind, struct i2c_board_info *info);
static int tpd_remove(struct i2c_client *client);
static int touch_event_handler(void *unused);
//20140903 tracy add for fix 5" touch start
static int touch_update_handler(void *unused);
//20140903 tracy add for fix 5" touch end


static int tpd_flag = 0;
//20141227 tracy modify for main/second source FW Update for user mode 
static int Touch_Source;
#if 0
static int key_pressed = -1;

struct osd_offset{
	int left_x;
	int right_x;
	unsigned int key_event;
};

static struct osd_offset OSD_mapping[] = { // Range need define by Case!
	{35, 290,  KEY_MENU},	//menu_left_x, menu_right_x, KEY_MENU
	{303, 467, KEY_HOME},	//home_left_x, home_right_x, KEY_HOME
	{473, 637, KEY_BACK},	//back_left_x, back_right_x, KEY_BACK
	{641, 905, KEY_SEARCH},	//search_left_x, search_right_x, KEY_SEARCH
};
#endif 

#if IAP_PORTION
uint8_t ic_status=0x00;	//0:OK 1:master fail 2:slave fail
int update_progree=0;
//<<Mel - 4/10, Modify I2C slave address to 0x15. 
uint8_t I2C_DATA[3] = {/*0x10*/0x15, 0x20, 0x21};/*I2C devices address*/  
//Mel - 4/10, Modify I2C slave address to 0x15>>. 
int is_OldBootCode = 0; // 0:new 1:old



/*The newest firmware, if update must be changed here*/

//20141216  tracy modify for touch main/second source detection start
//20141230  tracy modify for update touch main/second source FW start
//20150113  tracy modify for update touch main/second source FW 5528/5558 start
static uint8_t file_fw_data_truly[] = {
#include "E12_5528.i"
};
static uint8_t file_fw_data_liano[] = {
#include "E12_555c.i"  
};

static uint8_t file_fw_data_biel[] = {
#include "E12_5558.i"  
};
//20141230  tracy modify for update touch main/second source FW end
//20150113  tracy modify for update touch main/second source FW to 5528/5558 end
static uint8_t *file_fw_data=file_fw_data_truly;
//20141216  tracy modify for touch main/second source detection end
enum
{
	PageSize		= 132,
	//PageNum		        = 249,   //20141105 tracy add for FW Update 
	ACK_Fail		= 0x00,
	ACK_OK			= 0xAA,
	ACK_REWRITE		= 0x55,
};

enum
{
	E_FD			= -1,
};
#endif
//20141105 tracy add for FW Update 
int PageNum=0;
//Add 0821 start
static const struct i2c_device_id tpd_id[] = 
{
	{ "ektf2k_mtk", 0 },
	{ }
};

#ifdef ELAN_MTK6577
	static struct i2c_board_info __initdata i2c_tpd={ I2C_BOARD_INFO("ektf2k_mtk", (/*0x20*/0x2a>>1))};
#else
//<<Mel - 4/10, Modify I2C slave address to 0x15. 
	unsigned short force[] = {0, /*0x20*/0x2a, I2C_CLIENT_END, I2C_CLIENT_END};
//Mel - 4/10, Modify I2C slave address to 0x15>>. 
	static const unsigned short *const forces[] = { force, NULL };
	//static struct i2c_client_address_data addr_data = { .forces = forces, };
#endif

static struct i2c_driver tpd_i2c_driver =
{
    .driver = {
        .name = "ektf2k_mtk",
        .owner = THIS_MODULE,
    },
    .probe = tpd_probe,
    .remove =  tpd_remove,
    .id_table = tpd_id,
    .detect = tpd_detect,
    //.address_data = &addr_data,
};
//Add 0821 end



struct elan_ktf2k_ts_data {
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct workqueue_struct *elan_wq;
	struct work_struct work;
	struct early_suspend early_suspend;
	int intr_gpio;
// Firmware Information
	int fw_ver;
	int fw_id;
	int bc_ver;
	int x_resolution;
	int y_resolution;
// For Firmare Update 
	struct miscdevice firmware;
	struct hrtimer timer;
	struct wake_lock wakelock; //20140903 tracy add for fix 5" touch 
};

static struct elan_ktf2k_ts_data *private_ts;
static int __fw_packet_handler(struct i2c_client *client);
static int elan_ktf2k_ts_rough_calibrate(struct i2c_client *client);
static int tpd_resume(struct i2c_client *client);

#if IAP_PORTION
int Update_FW_One(/*struct file *filp,*/ struct i2c_client *client, int recovery);
static int __hello_packet_handler(struct i2c_client *client);
int IAPReset();
#endif


#ifdef MTK6589_DMA

static int elan_i2c_dma_recv_data(struct i2c_client *client, uint8_t *buf,uint8_t len)
{
	int rc;
	uint8_t *pReadData = 0;
	unsigned short addr = 0;
	pReadData = gpDMABuf_va;
	addr = client->addr ;
	client->addr |= I2C_DMA_FLAG;	
  if(!pReadData){
		printk("[elan] dma_alloc_coherent failed!\n");
		return -1;
  }
	rc = i2c_master_recv(client, gpDMABuf_pa, len);
	printk("[elan] elan_i2c_dma_recv_data rc=%d!\n",rc);
//	copy_to_user(buf, pReadData, len);
	return rc;
}

static int elan_i2c_dma_send_data(struct i2c_client *client, uint8_t *buf,uint8_t len)
{
	int rc;
	unsigned short addr = 0;
	uint8_t *pWriteData = gpDMABuf_va;
	addr = client->addr ;
	client->addr |= I2C_DMA_FLAG;	
	
  if(!pWriteData){
		printk("[elan] dma_alloc_coherent failed!\n");
		return -1;
  }
 // copy_from_user(pWriteData, ((void*)buf), len);

	rc = i2c_master_send(client, gpDMABuf_pa, len);
	client->addr = addr;
	printk("[elan] elan_i2c_dma_send_data rc=%d!\n",rc);
	return rc;
}
#endif
// For Firmware Update 
int elan_iap_open(struct inode *inode, struct file *filp){ 

	printk("[ELAN]into elan_iap_open\n");
		if (private_ts == NULL)  printk("private_ts is NULL~~~");
		
	return 0;
}

int elan_iap_release(struct inode *inode, struct file *filp){    
	return 0;
}

static ssize_t elan_iap_write(struct file *filp, const char *buff, size_t count, loff_t *offp){  
    int ret;
    char *tmp;

    printk("[ELAN]into elan_iap_write\n");
//20140722 tracy add for ESD -start
    #ifdef ESD_CHECK  
       have_interrupts = 1;
    #endif
//20140722 tracy add for ESD -end
    if (count > 8192)
        count = 8192;

    tmp = kmalloc(count, GFP_KERNEL);
    
    if (tmp == NULL)
        return -ENOMEM;

#ifdef MTK6589_DMA    
    if (copy_from_user(gpDMABuf_va, buff, count)) {
        return -EFAULT;
    }
    ret = elan_i2c_dma_send_data(private_ts->client, tmp, count);
#else
    if (copy_from_user(tmp, buff, count)) {
        return -EFAULT;
    }    
    ret = i2c_master_send(private_ts->client, tmp, count);
#endif    
    kfree(tmp);
    return (ret == 1) ? count : ret;

}

ssize_t elan_iap_read(struct file *filp, char *buff, size_t count, loff_t *offp){    
    char *tmp;
    int ret;  
    long rc;
//20140722 tracy add for ESD -start
    #ifdef ESD_CHECK  //0604
       have_interrupts = 1;
    #endif
//20140722 tracy add for ESD -end

    printk("[ELAN]into elan_iap_read\n");
    if (count > 8192)
        count = 8192;

    tmp = kmalloc(count, GFP_KERNEL);

    if (tmp == NULL)
        return -ENOMEM;
#ifdef MTK6589_DMA
    ret = elan_i2c_dma_recv_data(private_ts->client, tmp, count);
    if (ret >= 0)
        rc = copy_to_user(buff, gpDMABuf_va, count);    
#else    
    ret = i2c_master_recv(private_ts->client, tmp, count);
    if (ret >= 0)
        rc = copy_to_user(buff, tmp, count);    
#endif  

    kfree(tmp);

    //return ret;
    return (ret == 1) ? count : ret;
	
}

static long elan_iap_ioctl(/*struct inode *inode,*/ struct file *filp,    unsigned int cmd, unsigned long arg){

	int __user *ip = (int __user *)arg;
	printk("[ELAN]into elan_iap_ioctl\n");
	printk("cmd value %x\n",cmd);
	
	switch (cmd) {        
		case IOCTL_I2C_SLAVE: 
			private_ts->client->addr = (int __user)arg;
			private_ts->client->addr &= I2C_MASK_FLAG; 
			private_ts->client->addr |= I2C_ENEXT_FLAG;
			//file_fops_addr = 0x15;
			break;   
		case IOCTL_MAJOR_FW_VER:            
			break;        
		case IOCTL_MINOR_FW_VER:            
			break;        
		case IOCTL_RESET:

	   		mt_set_gpio_mode( GPIO_CTP_RST_PIN, GPIO_CTP_RST_PIN_M_GPIO );
 			mt_set_gpio_dir( GPIO_CTP_RST_PIN, GPIO_DIR_OUT );
    			mt_set_gpio_out( GPIO_CTP_RST_PIN, GPIO_OUT_ONE );
			mdelay(10);
		//	#if !defined(EVB)
    				mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ZERO);
		//	#endif
		        mdelay(10);
    			mt_set_gpio_out( GPIO_CTP_RST_PIN, GPIO_OUT_ONE );

			break;
		case IOCTL_IAP_MODE_LOCK:
			if(work_lock==0)
			{
				printk("[elan]%s %x=IOCTL_IAP_MODE_LOCK\n", __func__,IOCTL_IAP_MODE_LOCK);
				work_lock=1;
				disable_irq(CUST_EINT_TOUCH_PANEL_NUM);
				mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
				//cancel_work_sync(&private_ts->work);
//20140722 tracy add for ESD -start
		#ifdef ESD_CHECK	
          cancel_delayed_work_sync(&esd_work);
        #endif	
//20140722 tracy add for ESD -end
			}
			break;
		case IOCTL_IAP_MODE_UNLOCK:
			if(work_lock==1)
			{			
				work_lock=0;
				enable_irq(CUST_EINT_TOUCH_PANEL_NUM);
				mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
//20140722 tracy add for ESD -start
		#ifdef ESD_CHECK  
          queue_delayed_work(esd_wq, &esd_work, delay);	
        #endif
//20140722 tracy add for ESD -end
			}
			break;
		case IOCTL_CHECK_RECOVERY_MODE:
			return RECOVERY;
			break;
		case IOCTL_FW_VER:
			__fw_packet_handler(private_ts->client);
			return FW_VERSION;
			break;
		case IOCTL_X_RESOLUTION:
			__fw_packet_handler(private_ts->client);
			return X_RESOLUTION;
			break;
		case IOCTL_Y_RESOLUTION:
			__fw_packet_handler(private_ts->client);
			return Y_RESOLUTION;
			break;
		case IOCTL_FW_ID:
			__fw_packet_handler(private_ts->client);
			return FW_ID;
			break;
		case IOCTL_ROUGH_CALIBRATE:
			return elan_ktf2k_ts_rough_calibrate(private_ts->client);
		case IOCTL_I2C_INT:
			put_user(mt_get_gpio_in(GPIO_CTP_EINT_PIN),ip);
			printk("[elan]GPIO_CTP_EINT_PIN = %d\n", mt_get_gpio_in(GPIO_CTP_EINT_PIN));

			break;	
		case IOCTL_RESUME:
			tpd_resume(private_ts->client);
			break;	
		case IOCTL_CIRCUIT_CHECK:
			return circuit_ver;
			break;
		case IOCTL_POWER_LOCK:
			power_lock=1;
			break;
		case IOCTL_POWER_UNLOCK:
			power_lock=0;
			break;
#if IAP_PORTION		
		case IOCTL_GET_UPDATE_PROGREE:
			update_progree=(int __user)arg;
			break; 

		case IOCTL_FW_UPDATE:
			//RECOVERY = IAPReset(private_ts->client);
			RECOVERY=0;
			Update_FW_One(private_ts->client, RECOVERY);
#endif
		case IOCTL_BC_VER:
			__fw_packet_handler(private_ts->client);
			return BC_VERSION;
			break;
		default:            
			break;   
	}       
	return 0;
}

struct file_operations elan_touch_fops = {    
        .open =         elan_iap_open,    
        .write =        elan_iap_write,    
        .read = 	elan_iap_read,    
        .release =	elan_iap_release,    
	.unlocked_ioctl=elan_iap_ioctl, 
 };
#if IAP_PORTION
int EnterISPMode(struct i2c_client *client, uint8_t  *isp_cmd)
{
	char buff[4] = {0};
	int len = 0;
	
	len = i2c_master_send(private_ts->client, isp_cmd,  sizeof(isp_cmd));
	if (len != sizeof(buff)) {
		printk("[ELAN] ERROR: EnterISPMode fail! len=%d\r\n", len);
		return -1;
	}
	else
		printk("[ELAN] IAPMode write data successfully! cmd = [%2x, %2x, %2x, %2x]\n", isp_cmd[0], isp_cmd[1], isp_cmd[2], isp_cmd[3]);
	return 0;
}

int ExtractPage(struct file *filp, uint8_t * szPage, int byte)
{
	int len = 0;

	len = filp->f_op->read(filp, szPage,byte, &filp->f_pos);
	if (len != byte) 
	{
		printk("[ELAN] ExtractPage ERROR: read page error, read error. len=%d\r\n", len);
		return -1;
	}

	return 0;
}

int WritePage(uint8_t * szPage, int byte)
{
	int len = 0;

	len = i2c_master_send(private_ts->client, szPage,  byte);
	if (len != byte) 
	{
		printk("[ELAN] ERROR: write page error, write error. len=%d\r\n", len);
		return -1;
	}

	return 0;
}

int GetAckData(struct i2c_client *client)
{
	int len = 0;

	char buff[2] = {0};
	
	len=i2c_master_recv(private_ts->client, buff, sizeof(buff));
	if (len != sizeof(buff)) {
		printk("[ELAN] ERROR: read data error, write 50 times error. len=%d\r\n", len);
		return -1;
	}

	printk("[ELAN] GetAckData:%x,%x\n",buff[0],buff[1]);
	if (buff[0] == 0xaa/* && buff[1] == 0xaa*/) 
		return ACK_OK;
	else if (buff[0] == 0x55 && buff[1] == 0x55)
		return ACK_REWRITE;
	else
		return ACK_Fail;

	return 0;
}

void print_progress(int page, int ic_num, int j)
{
	int i, percent,page_tatol,percent_tatol;
	char str[256];
	str[0] = '\0';
	for (i=0; i<((page)/10); i++) {
		str[i] = '#';
		str[i+1] = '\0';
	}
	
	page_tatol=page+249*(ic_num-j);
	percent = ((100*page)/(249));
	percent_tatol = ((100*page_tatol)/(249*ic_num));

	if ((page) == (249))
		percent = 100;

	if ((page_tatol) == (249*ic_num))
		percent_tatol = 100;		

	printk("\rprogress %s| %d%%", str, percent);
	
	if (page == (249))
		printk("\n");
}
/* 
* Restet and (Send normal_command ?)
* Get Hello Packet
*/
int  IAPReset()
{
			int res;

	   		mt_set_gpio_mode( GPIO_CTP_RST_PIN, GPIO_CTP_RST_PIN_M_GPIO );
 			mt_set_gpio_dir( GPIO_CTP_RST_PIN, GPIO_DIR_OUT );
    			mt_set_gpio_out( GPIO_CTP_RST_PIN, GPIO_OUT_ONE );
			mdelay(10);
		//	#if !defined(EVB)
    			mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ZERO);
	//		#endif
	   		mdelay(10);
    			mt_set_gpio_out( GPIO_CTP_RST_PIN, GPIO_OUT_ONE );
			return 1;

#if 0
	printk("[ELAN] read Hello packet data!\n"); 	  
	res= __hello_packet_handler(client);
	return res;
#endif 
}

/* Check Master & Slave is "55 aa 33 cc" */
int CheckIapMode(void)
{
	char buff[4] = {0},len = 0;
	//WaitIAPVerify(1000000);
	//len = read(fd, buff, sizeof(buff));
	len=i2c_master_recv(private_ts->client, buff, sizeof(buff));
	if (len != sizeof(buff)) 
	{
		printk("[ELAN] CheckIapMode ERROR: read data error,len=%d\r\n", len);
		return -1;
	}
	else
	{
		
		if (buff[0] == 0x55 && buff[1] == 0xaa && buff[2] == 0x33 && buff[3] == 0xcc)
		{
			printk("[ELAN] CheckIapMode is 55 aa 33 cc\n");
			return 0;
		}
		else// if ( j == 9 )
		{
			printk("[ELAN] Mode= 0x%x 0x%x 0x%x 0x%x\r\n", buff[0], buff[1], buff[2], buff[3]);
			printk("[ELAN] ERROR:  CheckIapMode error\n");
			return -1;
		}
	}
	printk("\n");	
}

int Update_FW_One(struct i2c_client *client, int recovery)
{
	int res = 0,ic_num = 1;
	int iPage = 0, rewriteCnt = 0; //rewriteCnt for PAGE_REWRITE
	int i = 0;
	uint8_t data;

	int restartCnt = 0, checkCnt = 0; // For IAP_RESTART
	//uint8_t recovery_buffer[4] = {0};
	int byte_count;
	uint8_t *szBuff = NULL;
	int curIndex; //20141105 tracy add for FW Update start
#if CMD_54001234
	uint8_t isp_cmd[] = {0x54, 0x00, 0x12, 0x34};	 //54 00 12 34
#else
	uint8_t isp_cmd[] = {0x45, 0x49, 0x41, 0x50};	 //45 49 41 50
#endif
	uint8_t recovery_buffer[4] = {0};

IAP_RESTART:	 //20141105 tracy add for FW Update start
//20140903 tracy add for fix 5" touch start
	mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
	#ifdef ESD_CHECK	//0822
	cancel_delayed_work_sync(&esd_work);
  #endif
	power_lock = 1;
	//wake_lock(&private_ts->wakelock);
	work_lock=1;  //0822
//20140903 tracy add for fix 5" touch end
    updatefw_flag = 1;   //20141030   tracy  add  for  avoid update FW  fail
	data=I2C_DATA[0];//Master
	dev_dbg(&client->dev, "[ELAN] %s: address data=0x%x \r\n", __func__, data);
	//20141105 tracy add for FW Update 
         curIndex =0 ; 

//	if(recovery != 0x80)
//	{
	printk("[ELAN] Firmware upgrade....\n");

		IAPReset();
	        mdelay(20);	

		res = EnterISPMode(private_ts->client, isp_cmd);	 //enter ISP mode

	//res = i2c_master_recv(private_ts->client, recovery_buffer, 4);   //55 aa 33 cc 
	//printk("[ELAN] recovery byte data:%x,%x,%x,%x \n",recovery_buffer[0],recovery_buffer[1],recovery_buffer[2],recovery_buffer[3]);			

        mdelay(10);
#if 1  //20140903 tracy add for fix 5" touch 
		//Check IC's status is IAP mode(55 aa 33 cc) or not
		res = CheckIapMode();	 //Step 1 enter ISP mode
		if (res == -1) //CheckIapMode fail
		{	
			checkCnt ++;
			if (checkCnt >= 5)
			{
				printk("[ELAN] ERROR: CheckIapMode %d times fails!\n", IAPRESTART);
				goto Upd_Fail;    //20140903 tracy add for fix 5" touch 
			}
			else
			{
				printk("[ELAN] CheckIapMode retry %dth times! And restart IAP~~~\n\n", checkCnt);
				goto IAP_RESTART;
			}
		}
		else
			printk("[ELAN]  CheckIapMode ok!\n");
#endif
//	} else
//		printk("[ELAN] Firmware upgrade recovery mode !\n");
	// Send Dummy Byte	
	printk("[ELAN] send one byte data:%x,%x",private_ts->client->addr,data);
	res = i2c_master_send(private_ts->client, &data,  sizeof(data));
	if(res!=sizeof(data))
	{
		printk("[ELAN] dummy error code = %d\n",res);
	}	
	  mdelay(30);
//201219 tracy modify for main/second source FW Update start
	PageNum = (int)(sizeof(file_fw_data_liano)/sizeof(uint8_t)/PageSize); 
         printk("[ELAN] file_fw_data PageNum=%d\n", PageNum);
//201219 tracy modify for main/second source FW Update end

	// Start IAP
	for( iPage = 1; iPage <= PageNum; iPage++ ) 
	{
PAGE_REWRITE:
#if 1 
		// 8byte mode
		//szBuff = fw_data + ((iPage-1) * PageSize); 
		for(byte_count=1;byte_count<=17;byte_count++)
		{
			if(byte_count!=17)
			{		
	//			printk("[ELAN] byte %d\n",byte_count);	
	//			printk("curIndex =%d\n",curIndex);
				szBuff = file_fw_data + curIndex;
				curIndex =  curIndex + 8;

				//ioctl(fd, IOCTL_IAP_MODE_LOCK, data);
				res = WritePage(szBuff, 8);
			}
			else
			{
	//			printk("byte %d\n",byte_count);
	//			printk("curIndex =%d\n",curIndex);
				szBuff = file_fw_data + curIndex;
				curIndex =  curIndex + 4;
				//ioctl(fd, IOCTL_IAP_MODE_LOCK, data);
				res = WritePage(szBuff, 4); 
			}
		} // end of for(byte_count=1;byte_count<=17;byte_count++)
#endif 
#if 0 // 132byte mode		
		szBuff = file_fw_data + curIndex;
		curIndex =  curIndex + PageSize;
		res = WritePage(szBuff, PageSize);
#endif
#if 1  //20140903 tracy add for fix 5" touch 
		if(iPage==249 || iPage==1)
		{
			mdelay(70); 			 
		}
		else
		{
			mdelay(70); 			 
		}
#endif	
		res = GetAckData(private_ts->client);

		if (ACK_OK != res) 
		{
			mdelay(70);  //20141024   tracy  modify for  improve touch  download
			printk("[ELAN] ERROR: GetAckData fail! res=%d\r\n", res);
			if ( res == ACK_REWRITE ) 
			{
				rewriteCnt = rewriteCnt + 1;
				if (rewriteCnt == PAGERETRY)
				{
					printk("[ELAN] ID 0x%02x %dth page ReWrite %d times fails!\n", data, iPage, PAGERETRY);
					goto Upd_Fail;          //20140903 tracy add for fix 5" touch 
				}
				else
				{
					printk("[ELAN] ---%d--- page ReWrite %d times!\n",  iPage, rewriteCnt);
					curIndex = curIndex - PageSize;
					goto PAGE_REWRITE;
				}
			}
			else
			{
				restartCnt = restartCnt + 1;
				if (restartCnt >= 5)
				{
					printk("[ELAN] ID 0x%02x ReStart %d times fails!\n", data, IAPRESTART);
					goto Upd_Fail;    //20140903 tracy add for fix 5" touch 
					//return -1;
				}
				else
				{
					printk("[ELAN] ===%d=== page ReStart %d times!\n",  iPage, restartCnt);
					goto IAP_RESTART;
				}
			}
		}
		else
		{       printk("  data : 0x%02x ",  data);  
			rewriteCnt=0;
			print_progress(iPage,ic_num,i);
		}

		mdelay(10);
	} // end of for(iPage = 1; iPage <= PageNum; iPage++)
//20140903 tracy add for fix 5" touch start

	printk("[ELAN] read Hello packet data!\n");
	mdelay(100); //////
	res= __hello_packet_handler(client);
	if (res > 0)
		printk("[ELAN] Update ALL Firmware successfully!\n");


	__fw_packet_handler(private_ts->client);
	
	work_lock=0;
	printk("0822 show_fw_update mt_eint_unmask\n");
	mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
	//wake_unlock(&private_ts->wakelock);
#ifdef ESD_CHECK  //0822
	queue_delayed_work(esd_wq, &esd_work, delay);	
#endif
	updatefw_flag = 0;   //20141030   tracy  add  for  avoid update FW  fail
	
	return 0;
	
	Upd_Fail:
	
	work_lock=0;
	printk("FAIL 0822 show_fw_update mt_eint_unmask\n");
	mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
	//wake_unlock(&private_ts->wakelock);
#ifdef ESD_CHECK  //0822
	queue_delayed_work(esd_wq, &esd_work, delay);	
#endif
	goto IAP_RESTART;	 //20141105 tracy add for FW Update start
	
	return -1;
//20140903 tracy add for fix 5" touch end
}

#endif
// End Firmware Update


#if 0
static void elan_ktf2k_ts_early_suspend(struct early_suspend *h);
static void elan_ktf2k_ts_late_resume(struct early_suspend *h);
#endif

static ssize_t elan_ktf2k_gpio_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret = 0;
	struct elan_ktf2k_ts_data *ts = private_ts;

	//ret = gpio_get_value(ts->intr_gpio);
	ret = mt_get_gpio_in(GPIO_CTP_EINT_PIN);
	printk(KERN_DEBUG "GPIO_TP_INT_N=%d\n", ts->intr_gpio);
	sprintf(buf, "GPIO_TP_INT_N=%d\n", ret);
	ret = strlen(buf) + 1;
	return ret;
}

static DEVICE_ATTR(gpio, S_IRUGO, elan_ktf2k_gpio_show, NULL);

static ssize_t elan_ktf2k_vendor_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;
	struct elan_ktf2k_ts_data *ts = private_ts;

	sprintf(buf, "%s_x%4.4x\n", "ELAN_KTF2K", ts->fw_ver);
	ret = strlen(buf) + 1;
	return ret;
}
#if 0
static DEVICE_ATTR(vendor, S_IRUGO, elan_ktf2k_vendor_show, NULL);

static struct kobject *android_touch_kobj;

static int elan_ktf2k_touch_sysfs_init(void)
{
	int ret ;

	android_touch_kobj = kobject_create_and_add("android_touch", NULL) ;
	if (android_touch_kobj == NULL) {
		printk(KERN_ERR "[elan]%s: subsystem_register failed\n", __func__);
		ret = -ENOMEM;
		return ret;
	}
	ret = sysfs_create_file(android_touch_kobj, &dev_attr_gpio.attr);
	if (ret) {
		printk(KERN_ERR "[elan]%s: sysfs_create_file failed\n", __func__);
		return ret;
	}
	ret = sysfs_create_file(android_touch_kobj, &dev_attr_vendor.attr);
	if (ret) {
		printk(KERN_ERR "[elan]%s: sysfs_create_group failed\n", __func__);
		return ret;
	}
	return 0 ;
}

static void elan_touch_sysfs_deinit(void)
{
	sysfs_remove_file(android_touch_kobj, &dev_attr_vendor.attr);
	sysfs_remove_file(android_touch_kobj, &dev_attr_gpio.attr);
	kobject_del(android_touch_kobj);
}	
#endif


static int __elan_ktf2k_ts_poll(struct i2c_client *client)
{
	struct elan_ktf2k_ts_data *ts = i2c_get_clientdata(client);
	int status = 0, retry = 10;

	do {
		//status = gpio_get_value(ts->intr_gpio);
		status = mt_get_gpio_in(GPIO_CTP_EINT_PIN);
		printk("[elan]: %s: status = %d\n", __func__, status);
		retry--;
		//mdelay(200); //0403 modify
		mdelay(10);
	} while (status == 1 && retry > 0);

	printk( "[elan]%s: poll interrupt status %s\n",
			__func__, status == 1 ? "high" : "low");
	return (status == 0 ? 0 : -ETIMEDOUT);
}

static int elan_ktf2k_ts_poll(struct i2c_client *client)
{
	return __elan_ktf2k_ts_poll(client);
}

static int elan_ktf2k_ts_get_data(struct i2c_client *client, uint8_t *cmd,
			uint8_t *buf, size_t size)
{
	int rc;

	dev_dbg(&client->dev, "[elan]%s: enter\n", __func__);

	if (buf == NULL)
		return -EINVAL;


	if ((i2c_master_send(client, cmd, 4)) != 4) {
		dev_err(&client->dev,
			"[elan]%s: i2c_master_send failed\n", __func__);
		return -EINVAL;
	}


	rc = elan_ktf2k_ts_poll(client);
	if (rc < 0)
		return -EINVAL;
	else {

		if (i2c_master_recv(client, buf, size) != size ||
		    buf[0] != CMD_S_PKT)
			return -EINVAL;
	}

	return 0;
}

static int __hello_packet_handler(struct i2c_client *client)
{
	int rc;
	uint8_t buf_recv[8] = { 0 };
	//uint8_t buf_recv1[4] = { 0 };

//	mdelay(1500);

	rc = elan_ktf2k_ts_poll(client);
	if (rc < 0) {
		printk( "[elan] %s: Int poll failed!\n", __func__);
		RECOVERY=0x80;
		return RECOVERY;	
	}
//20140903 tracy add for fix 5" touch start
	rc = i2c_master_recv(client, buf_recv, 8);  

	printk("[elan] %s: Hello Packet %2x:%2X:%2x:%2x:%2x:%2x:%2x:%2x\n", __func__, buf_recv[0], buf_recv[1], buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7]);
//20140903 tracy add for fix 5" touch end
/*  Received 8 bytes data will let TP die on old firmware on ektf21xx carbon player and MK5     
    rc = i2c_master_recv(client, buf_recv, 8);
	printk("[elan] %s: hello packet %2x:%2X:%2x:%2x:%2x:%2X:%2x:%2x\n", __func__, buf_recv[0], buf_recv[1], buf_recv[2], buf_recv[3] , buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7]);
*/
	if(buf_recv[0]==0x55 && buf_recv[1]==0x55 && buf_recv[2]==0x80 && buf_recv[3]==0x80)
	{
	//20141216  tracy modify for touch main/second source detection start
		   if(buf_recv[4]==0x9b && buf_recv[5]==0x04)
		   	{
		      file_fw_data = file_fw_data_truly;
		   	}
		   else if(buf_recv[4]==0x9b && buf_recv[5]==0xfb) 
		   	{
		      file_fw_data = file_fw_data_liano;
		   	}
		   else if(buf_recv[4]==0x64 && buf_recv[5]==0x04) 
		   	{
		      file_fw_data = file_fw_data_biel;
		   	}
		   else if(buf_recv[4]==0x64 && buf_recv[5]==0xfb)
		   	{
		      file_fw_data = file_fw_data_biel;
		   	}
		   else {
		        printk("[ELAN] Recovery Mode, Sensor Opt Error: %x, %x", buf_recv[4], buf_recv[5]);	
		   }
	//20141216  tracy modify for touch main/second source detection end   
        RECOVERY=0x80;
        return RECOVERY;
    }else{
	mdelay(300); //0141024   tracy  modify for  improve touch  download
	rc = i2c_master_recv(client, buf_recv, 4);
	printk("[elan] %s: Calibration Packet %2x:%2X:%2x:%2x\n", __func__, buf_recv[0], buf_recv[1], buf_recv[2], buf_recv[3]);
	}
	
	return 0;
}

static int __fw_packet_handler(struct i2c_client *client)
{
	int rc;
	int major, minor;
	uint8_t cmd[] = {CMD_R_PKT, 0x00, 0x00, 0x01};/* Get Firmware Version*/
	uint8_t cmd_x[] = {0x53, 0x60, 0x00, 0x00}; /*Get x resolution*/
	uint8_t cmd_y[] = {0x53, 0x63, 0x00, 0x00}; /*Get y resolution*/
	uint8_t cmd_id[] = {0x53, 0xf0, 0x00, 0x01}; /*Get firmware ID*/
	uint8_t cmd_bc[] = {CMD_R_PKT, 0x10, 0x00, 0x01};/* Get BootCode Version*/ //0403 modify
	uint8_t buf_recv[4] = {0};

printk( "[elan] %s: n", __func__);
// Firmware version
	rc = elan_ktf2k_ts_get_data(client, cmd, buf_recv, 4);
	if (rc < 0)
		return rc;
	major = ((buf_recv[1] & 0x0f) << 4) | ((buf_recv[2] & 0xf0) >> 4);
	minor = ((buf_recv[2] & 0x0f) << 4) | ((buf_recv[3] & 0xf0) >> 4);
//	ts->fw_ver = major << 8 | minor;
    private_ts->fw_ver = major << 8 | minor;  //20140903 tracy add for fix 5" touch 
	FW_VERSION = major << 8 | minor;
// Firmware ID
	rc = elan_ktf2k_ts_get_data(client, cmd_id, buf_recv, 4);
	if (rc < 0)
		return rc;
	major = ((buf_recv[1] & 0x0f) << 4) | ((buf_recv[2] & 0xf0) >> 4);
	minor = ((buf_recv[2] & 0x0f) << 4) | ((buf_recv[3] & 0xf0) >> 4);
    private_ts->fw_id = major << 8 | minor;  //20140903 tracy add for fix 5" touch 
	//ts->fw_id = major << 8 | minor;
	FW_ID = major << 8 | minor;
// X Resolution
	rc = elan_ktf2k_ts_get_data(client, cmd_x, buf_recv, 4);
	if (rc < 0)
		return rc;
	minor = ((buf_recv[2])) | ((buf_recv[3] & 0xf0) << 4);
	//ts->x_resolution =minor;
    private_ts->x_resolution =minor;    //20140903 tracy add for fix 5" touch 
	X_RESOLUTION = minor;
	
// Y Resolution	
	rc = elan_ktf2k_ts_get_data(client, cmd_y, buf_recv, 4);
	if (rc < 0)
		return rc;
	minor = ((buf_recv[2])) | ((buf_recv[3] & 0xf0) << 4);
	//ts->y_resolution =minor;
	private_ts->y_resolution =minor;   //20140903 tracy add for fix 5" touch 
	Y_RESOLUTION = minor;

// Bootcode version
	rc = elan_ktf2k_ts_get_data(client, cmd_bc, buf_recv, 4);
	if (rc < 0)
		return rc;
	major = ((buf_recv[1] & 0x0f) << 4) | ((buf_recv[2] & 0xf0) >> 4);
	minor = ((buf_recv[2] & 0x0f) << 4) | ((buf_recv[3] & 0xf0) >> 4);
	//ts->bc_ver = major << 8 | minor;
    private_ts->bc_ver = major << 8 | minor;  //20140903 tracy add for fix 5" touch 
	BC_VERSION = major << 8 | minor;
	
	printk( "[elan] %s: firmware version: 0x%4.4x\n",
			__func__, FW_VERSION);
	printk( "[elan] %s: firmware ID: 0x%4.4x\n",
			__func__, FW_ID);
	printk( "[elan] %s: x resolution: %d, y resolution: %d, LCM x resolution: %d, LCM y resolution: %d\n",
			__func__, (int)X_RESOLUTION, (int)Y_RESOLUTION,(int)LCM_X_MAX,(int)LCM_Y_MAX);
	printk( "[elan] %s: bootcode version: 0x%4.4x\n",
			__func__, BC_VERSION);
	return 0;
}

static inline int elan_ktf2k_ts_parse_xy(uint8_t *data,
			uint16_t *x, uint16_t *y)
{
	*x = *y = 0;

	*x = (data[0] & 0xf0);
	*x <<= 4;
	*x |= data[1];

	*y = (data[0] & 0x0f);
	*y <<= 8;
	*y |= data[2];

	return 0;
}

static int elan_ktf2k_ts_setup(struct i2c_client *client)
{
	int rc;
   
	rc = __hello_packet_handler(client);
	printk("[elan] hellopacket's rc = %d\n",rc);

	mdelay(20);  //20141119 Tracy modify for improve touch update FW
	if (rc != 0x80){
	    rc = __fw_packet_handler(client);
	    if (rc < 0)
		    printk("[elan] %s, fw_packet_handler fail, rc = %d", __func__, rc);
		  else
	    	printk("[elan] %s: firmware checking done.\n", __func__);
			/* Check for FW_VERSION, if 0x0000 means FW update fail! */
	    if ( FW_VERSION == 0x00)
	    {
				rc = 0x80;
				printk("[elan] FW_VERSION = %d, last FW update fail\n", FW_VERSION);
	    }
	}
	return rc; /* Firmware need to be update if rc equal to 0x80(Recovery mode)   */
}

static int elan_ktf2k_ts_rough_calibrate(struct i2c_client *client){
      uint8_t cmd[] = {CMD_W_PKT, 0x29, 0x00, 0x01};

	//dev_info(&client->dev, "[elan] %s: enter\n", __func__);
	printk("[elan] %s: enter\n", __func__);
	dev_info(&client->dev,
		"[elan] dump cmd: %02x, %02x, %02x, %02x\n",
		cmd[0], cmd[1], cmd[2], cmd[3]);

	if ((i2c_master_send(client, cmd, sizeof(cmd))) != sizeof(cmd)) {
		dev_err(&client->dev,
			"[elan] %s: i2c_master_send failed\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static int elan_ktf2k_ts_set_power_state(struct i2c_client *client, int state)
{
	uint8_t cmd[] = {CMD_W_PKT, 0x50, 0x00, 0x01};

	dev_dbg(&client->dev, "[elan] %s: enter\n", __func__);

	cmd[1] |= (state << 3);

	dev_dbg(&client->dev,
		"[elan] dump cmd: %02x, %02x, %02x, %02x\n",
		cmd[0], cmd[1], cmd[2], cmd[3]);

	if ((i2c_master_send(client, cmd, sizeof(cmd))) != sizeof(cmd)) {
		dev_err(&client->dev,
			"[elan] %s: i2c_master_send failed\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static int elan_ktf2k_ts_get_power_state(struct i2c_client *client)
{
	int rc = 0;
	uint8_t cmd[] = {CMD_R_PKT, 0x50, 0x00, 0x01};
	uint8_t buf[4], power_state;

	rc = elan_ktf2k_ts_get_data(client, cmd, buf, 4);
	if (rc)
		return rc;

	power_state = buf[1];
	dev_dbg(&client->dev, "[elan] dump repsponse: %0x\n", power_state);
	power_state = (power_state & PWR_STATE_MASK) >> 3;
	dev_dbg(&client->dev, "[elan] power state = %s\n",power_state == PWR_STATE_DEEP_SLEEP ? "Deep Sleep" : "Normal/Idle");

	return power_state;
}

static int elan_ktf2k_read_block(struct i2c_client *client, u8 addr, u8 *data, u8 len)
{
    int err;
    u8 beg = addr; 
    struct i2c_msg msgs[2] = {
        {
            .addr = client->addr,    .flags = 0,
            .len = 1,                .buf= &beg
        },
        {
            .addr = client->addr,    .flags = I2C_M_RD,
            .len = len,             .buf = data,
            .ext_flag = I2C_DMA_FLAG,
        }
    };
   
    if (!client)
        return -EINVAL;

    err = i2c_transfer(client->adapter, msgs, sizeof(msgs)/sizeof(msgs[0]));
    if (err != len) {
        printk("[elan] elan_ktf2k_read_block err=%d\n", err);
        err = -EIO;
    } else {
		printk("[elan] elan_ktf2k_read_block ok\n");
        err = 0;    /*no error*/
    }
    return err;


}


static int elan_ktf2k_ts_recv_data(struct i2c_client *client, uint8_t *buf)
{
	int rc, bytes_to_recv=PACKET_SIZE;
	uint8_t *pReadData = 0;
	unsigned short addr = 0;

	if (buf == NULL)
		return -EINVAL;
	memset(buf, 0, bytes_to_recv);

//#ifdef ELAN_MTK6577
#ifdef MTK6589_DMA
	addr = client->addr ;
	client->addr |= I2C_DMA_FLAG;
	pReadData = gpDMABuf_va;
  if(!pReadData){
		printk("[elan] dma_alloc_coherent failed!\n");
  }
	rc = i2c_master_recv(client, gpDMABuf_pa, bytes_to_recv);
	copy_to_user(buf, pReadData, bytes_to_recv);
	client->addr = addr;
	#ifdef ELAN_DEBUG
	printk("[elan_debug] %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15],buf[16], buf[17]);
	#endif
	
#else	
	rc = i2c_master_recv(client, buf, 8);
	if (rc != 8)
		printk("[elan_debug] The first package error.\n");
	printk("[elan_recv] %x %x %x %x %x %x %x %x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
	mdelay(1);
	
	if (buf[0] == FIVE_FINGERS_PKT){    //for five finger
		rc = i2c_master_recv(client, buf+ 8, 8);	
		if (rc != 8)
			printk("[elan_debug] The second package error.\n");
		printk("[elan_recv] %x %x %x %x %x %x %x %x\n", buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
		rc = i2c_master_recv(client, buf+ 16, 2);
		if (rc != 2)
			printk("[elan_debug] The third package error.\n");
		mdelay(1);
		printk("[elan_recv] %x %x \n", buf[16], buf[17]);
	}
#endif	
	
	return rc;
}
#if 0  //20140903 tracy add for fix 5" touch start
//#ifdef SOFTKEY_AXIS_VER //SOFTKEY is reported via AXI
static void elan_ktf2k_ts_report_data(struct i2c_client *client, uint8_t *buf)
{
	//struct elan_ktf2k_ts_data *ts = i2c_get_clientdata(client);
	struct input_dev *idev = tpd->dev;
	uint16_t x, y;
	uint16_t fbits=0;
	uint8_t i, num, reported = 0;
	uint8_t idx, btn_idx;
	int finger_num;
	int limitY = ELAN_Y_MAX -100; // limitY need define by Case!
/* for 10 fingers	*/
	if (buf[0] == TEN_FINGERS_PKT){
	    	finger_num = 10;
	    	num = buf[2] & 0x0f; 
	    	fbits = buf[2] & 0x30;	
		    fbits = (fbits << 4) | buf[1]; 
	    	idx=3;
		    btn_idx=33;
      }
// for 5 fingers	
	else if ((buf[0] == MTK_FINGERS_PKT) || (buf[0] == FIVE_FINGERS_PKT)){
	    	finger_num = 5;
	    	num = buf[1] & 0x07; 
        fbits = buf[1] >>3;
	    	idx=2;
	    	btn_idx=17;
	}else{
// for 2 fingers      
		finger_num = 2;
		num = buf[7] & 0x03; 
		fbits = buf[7] & 0x03;
		idx=1;
		btn_idx=7;
	}

	switch (buf[0]) {
//<<Mel - 4/10, Add 0x78 packet. 
		case 0x78 :  // chip may reset due to watch dog
			//printk(KERN_EMERG "!!!!!!!tp chip check event\n");
		break;
//Mel - 4/10, Add 0x78 packet>>. 
		case MTK_FINGERS_PKT:
		case TWO_FINGERS_PKT:
		case FIVE_FINGERS_PKT:	
		case TEN_FINGERS_PKT:
			//input_report_key(idev, BTN_TOUCH, 1);
			if (num == 0)
			{
				//dev_dbg(&client->dev, "no press\n");
				if(key_pressed < 0){
					input_report_abs(idev, ABS_MT_TOUCH_MAJOR, 0);
					input_report_abs(idev, ABS_MT_WIDTH_MAJOR, 0);
					input_mt_sync(idev);
					
					if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
          {   
          	tpd_button(x, y, 0);  
          }
          TPD_EM_PRINT(x, y, x, y, 0, 0);
					}
				else{
					//dev_err(&client->dev, "[elan] KEY_RELEASE: key_code:%d\n",OSD_mapping[key_pressed].key_event);
					input_report_key(idev, OSD_mapping[key_pressed].key_event, 0);
					key_pressed = -1;
				}
			}
			else 
			{			
				//dev_dbg(&client->dev, "[elan] %d fingers\n", num);                        
				//input_report_key(idev, BTN_TOUCH, 1);
				for (i = 0; i < finger_num; i++) 
				{	
					if ((fbits & 0x01)) 
					{
						elan_ktf2k_ts_parse_xy(&buf[idx], &x, &y);  
						//elan_ktf2k_ts_parse_xy(&buf[idx], &y, &x);
						//x = X_RESOLUTION-x;	 
						//y = Y_RESOLUTION-y; 
#if 1
	if(X_RESOLUTION > 0 && Y_RESOLUTION > 0)
	{
		x = ( x * LCM_X_MAX )/X_RESOLUTION;
		y = ( y * LCM_Y_MAX )/Y_RESOLUTION;
	}
	else
	{
		x = ( x * LCM_X_MAX )/ELAN_X_MAX;
		y = ( y * LCM_Y_MAX )/ELAN_Y_MAX;
	}
#endif 		 
						printk("[elan_debug SOFTKEY_AXIS_VER] %s, x=%d, y=%d\n",__func__, x , y);
									     
						if (!((x<=0) || (y<=0) || (x>=X_RESOLUTION) || (y>=Y_RESOLUTION))) 
						{   
							if ( y < limitY )
						     	{
			    					input_report_abs(idev, ABS_MT_TRACKING_ID, i);
								input_report_abs(idev, ABS_MT_TOUCH_MAJOR, 8);
								input_report_abs(idev, ABS_MT_POSITION_X, x);
								input_report_abs(idev, ABS_MT_POSITION_Y, y);
								input_mt_sync(idev);
								if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
                {   
                	tpd_button(x, y, 1);  
                }
                TPD_EM_PRINT(x, y, x, y, i-1, 1);
							}
							else
							{
							    	int i=0;
							    	for(i=0;i<4;i++)
							    	{
							    		if((x > OSD_mapping[i].left_x) && (x < OSD_mapping[i].right_x))
							    		{
										//dev_err(&client->dev, "[elan] KEY_PRESS: key_code:%d\n",OSD_mapping[i].key_event);
										//printk("[elan] %d KEY_PRESS: key_code:%d\n", i, OSD_mapping[i].key_event);
							    			input_report_key(idev, OSD_mapping[i].key_event, 1);
							    			key_pressed = i;
							    		}
							    	}
							}
							reported++;
							
					  	} // end if border
					} // end if finger status
				  	fbits = fbits >> 1;
				  	idx += 3;
				} // end for
			}

			if (reported)
				input_sync(idev);
			else 
			{
				input_mt_sync(idev);
				input_sync(idev);
			}

			break;
	   	default:
				dev_err(&client->dev,
					"[elan] %s: unknown packet type: %0x\n", __func__, buf[0]);
				break;
	} // end switch
	return;
}

//#else //SOFTKEY is reported via BTN bit
#endif //20140903 tracy add for fix 5" touch end
static void elan_ktf2k_ts_report_data(struct i2c_client *client, uint8_t *buf)
{
	/*struct elan_ktf2k_ts_data *ts = i2c_get_clientdata(client);*/
	struct input_dev *idev = tpd->dev;
	uint16_t x, y;
	uint16_t fbits=0;
	uint8_t i, num, reported = 0;
	uint8_t idx, btn_idx;
	int finger_num;
/* for 10 fingers	*/
//20140917 tracy add touch replace ps define start
#if ELAN_PS
	hwm_sensor_data *sensor_data;

	//face close turn off lcd led
	if(buf[0] ==0xFA && buf[1] ==0xCE && buf[2] ==0xAA && buf[3] ==0xAA){
		ckt_tp_replace_ps_close=0;
		printk("[elan-ps] elan_ktf2k_ts_report_data - face away\n");
	}

	//face away turn on lcd led
	if(buf[0] ==0xFA && buf[1] ==0xCE && buf[2] ==0x55 && buf[3] ==0x55){
		ckt_tp_replace_ps_close=1;
		printk("[elan-ps] elan_ktf2k_ts_report_data - face close \n");
	}

#endif
//20140917 tracy add touch replace ps define end

	if (buf[0] == TEN_FINGERS_PKT){
	    	finger_num = 10;
	    	num = buf[2] & 0x0f; 
	    	fbits = buf[2] & 0x30;	
		    fbits = (fbits << 4) | buf[1]; 
	    	idx=3;
		    btn_idx=33;
      }
// for 5 fingers	
	else if ((buf[0] == MTK_FINGERS_PKT) || (buf[0] == FIVE_FINGERS_PKT)){
	    	finger_num = 5;
	    	num = buf[1] & 0x07; 
        fbits = buf[1] >>3;
	    	idx=2;
	    	btn_idx=17;
	}else{
// for 2 fingers      
		finger_num = 2;
		num = buf[7] & 0x03; 
		fbits = buf[7] & 0x03;
		idx=1;
		btn_idx=7;
	}
		
	switch (buf[0]) {
//<<Mel - 4/10, Add 0x78 packet. 
		case 0x78 :  // chip may reset due to watch dog
			//printk(KERN_EMERG "!!!!!!!tp chip check event\n");
		break;
//Mel - 4/10, Add 0x78 packet>>. 
		case MTK_FINGERS_PKT:
		case TWO_FINGERS_PKT:
		case FIVE_FINGERS_PKT:	
		case TEN_FINGERS_PKT:
			//input_report_key(idev, BTN_TOUCH, 1);
			if (num == 0)
			{
				dev_dbg(&client->dev, "no press\n");
				#ifdef ELAN_DEBUG
				printk("tp button_state0 = %x\n",button_state);
        printk("tp buf[btn_idx] = %x KEY_MENU=%x KEY_HOME=%x KEY_BACK=%x KEY_SEARCH =%x\n",buf[btn_idx], KEY_MENU, KEY_HOME, KEY_BACK, KEY_SEARCH);
				#endif
	#ifdef ELAN_BUTTON
						
					switch (buf[btn_idx]) {
				    	case ELAN_KEY_BACK:
						printk("KEY back 1\n");
									#ifndef LCT_VIRTUAL_KEY
			                        input_report_key(idev, KEY_BACK, 1);
									#else
									input_report_key(idev, BTN_TOUCH, 1);
									input_report_abs(idev, ABS_MT_TOUCH_MAJOR, 8);
									input_report_abs(idev, ABS_MT_POSITION_X, 617);
									input_report_abs(idev, ABS_MT_POSITION_Y, 1360);
									#endif
			            button_state = KEY_BACK;
						break;
						
					case ELAN_KEY_HOME:
						printk("KEY home 1\n");
									#ifndef LCT_VIRTUAL_KEY
			                        input_report_key(idev, KEY_HOMEPAGE, 1);
									#else
									input_report_key(idev, BTN_TOUCH, 1);
									input_report_abs(idev, ABS_MT_TOUCH_MAJOR, 8);
									input_report_abs(idev, ABS_MT_POSITION_X, 365);
									input_report_abs(idev, ABS_MT_POSITION_Y, 1360);
									#endif
			                        button_state = KEY_HOMEPAGE;
						break;
						
					case ELAN_KEY_MENU:
						printk("KEY menu 1\n");
									#ifndef LCT_VIRTUAL_KEY
			                        input_report_key(idev, KEY_MENU, 1);
									#else
									input_report_key(idev, BTN_TOUCH, 1);
									input_report_abs(idev, ABS_MT_TOUCH_MAJOR, 8);
									input_report_abs(idev, ABS_MT_POSITION_X, 107);
									input_report_abs(idev, ABS_MT_POSITION_Y, 1360);
									#endif
			                        button_state = KEY_MENU;
							break;
				
			     // TOUCH release
						default: 		
				  			printk("[ELAN ] test tpd up\n");
								input_report_key(idev, BTN_TOUCH, 0);
								input_report_abs(idev, ABS_MT_TOUCH_MAJOR, 0);
								input_report_abs(idev, ABS_MT_WIDTH_MAJOR, 0);
								input_mt_sync(idev);
                tpd_down_flag = 0;
               break;
				    }
								  
                //input_sync(idev);     
#endif		      
					if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
          {   
          	tpd_button(x, y, 0);  
          }
          TPD_EM_PRINT(x, y, x, y, 0, 0);
				
		}
			else 
			{			
				//dev_dbg(&client->dev, "[elan] %d fingers\n", num);                        
				input_report_key(idev, BTN_TOUCH, 1);
				for (i = 0; i < finger_num; i++) 
				{	
					if ((fbits & 0x01)) 
					{
						elan_ktf2k_ts_parse_xy(&buf[idx], &x, &y);  
						//elan_ktf2k_ts_parse_xy(&buf[idx], &y, &x); 
						#if 1
                        if(X_RESOLUTION > 0 && Y_RESOLUTION > 0)
                        {
                            x = ( x * LCM_X_MAX )/X_RESOLUTION;
                            y = ( y * LCM_Y_MAX )/Y_RESOLUTION;
                        }
                        else
                        {
                            x = ( x * LCM_X_MAX )/ELAN_X_MAX;
                            y = ( y * LCM_Y_MAX )/ELAN_Y_MAX;
                        }
						#endif 		 

						//x = ( x * LCM_X_MAX )/ELAN_X_MAX;
						//y = ( y * LCM_Y_MAX )/ELAN_Y_MAX;
						#ifdef ELAN_DEBUG
						printk("[elan_debug  BTN bit] %s, x=%d, y=%d\n",__func__, x , y);
						#endif
						//x = LCM_X_MAX-x;	 
						//y = Y_RESOLUTION-y;			     
						if (!((x<=0) || (y<=0) || (x>=LCM_X_MAX) || (y>=LCM_Y_MAX))) 
						{   
							input_report_key(idev, BTN_TOUCH, 1);
							input_report_abs(idev, ABS_MT_TRACKING_ID, i);
							input_report_abs(idev, ABS_MT_TOUCH_MAJOR, 8);
							input_report_abs(idev, ABS_MT_POSITION_X, x);
							input_report_abs(idev, ABS_MT_POSITION_Y, y);
							input_mt_sync(idev);
							reported++;
							tpd_down_flag=1;
							if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
              {   
              	tpd_button(x, y, 1);  
              }
              TPD_EM_PRINT(x, y, x, y, i-1, 1);
					  } // end if border
					} // end if finger status
				  	fbits = fbits >> 1;
				  	idx += 3;
				} // end for
			}
			if (reported)
				input_sync(idev);
			else 
			{
				input_mt_sync(idev);
				input_sync(idev);
			}
			break;
	   	default:
					printk("[elan] %s: unknown packet type: %0x\n", __func__, buf[0]);
				break;
	} // end switch
	return;
}
//#endif
static void elan_ktf2k_ts_work_func(struct work_struct *work)
{
	int rc;
	struct elan_ktf2k_ts_data *ts =
	container_of(work, struct elan_ktf2k_ts_data, work);
	uint8_t buf[PACKET_SIZE] = { 0 };

//		if (gpio_get_value(ts->intr_gpio))
		if (mt_get_gpio_in(GPIO_CTP_EINT_PIN))
		{
			//enable_irq(ts->client->irq);
			printk("[elan]: Detected Jitter at INT pin. \n");
			mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
			return;
		}
	
		rc = elan_ktf2k_ts_recv_data(ts->client, buf);
 
		if (rc < 0)
		{
			//enable_irq(ts->client->irq);
			printk("[elan] elan_ktf2k_ts_recv_data Error, Error code %d \n", rc);
			mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
			return;
		}

		//printk("[elan] %2x,%2x,%2x,%2x,%2x,%2x\n",buf[0],buf[1],buf[2],buf[3],buf[5],buf[6]);
		elan_ktf2k_ts_report_data(ts->client, buf);

		//enable_irq(ts->client->irq);
		mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);

	return;
}

static irqreturn_t elan_ktf2k_ts_irq_handler(int irq, void *dev_id)
{
	struct elan_ktf2k_ts_data *ts = dev_id;
	struct i2c_client *client = ts->client;

	dev_dbg(&client->dev, "[elan] %s\n", __func__);
	mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
 	tpd_flag = 1;
 	wake_up_interruptible(&waiter);
	return IRQ_HANDLED;
}

static int elan_ktf2k_ts_register_interrupt(struct i2c_client *client)
{
	struct elan_ktf2k_ts_data *ts = i2c_get_clientdata(client);
	int err = 0;

	err = request_irq(client->irq, elan_ktf2k_ts_irq_handler,
											IRQF_TRIGGER_LOW, client->name, ts);
	if (err)
		dev_err(&client->dev, "[elan] %s: request_irq %d failed\n",
				__func__, client->irq);

	return err;
}
//20140903 tracy add for fix 5" touch start
static int touch_update_handler(void *unused)
{
	Update_FW_One(private_ts->client, RECOVERY);
	return 0;
}
//20140903 tracy add for fix 5" touch end
static int touch_event_handler(void *unused)
{
	int rc;
	uint8_t buf[PACKET_SIZE] = { 0 };

	int touch_state = 3;
//	int button_state = 0;
	unsigned long time_eclapse;
	struct sched_param param = { .sched_priority = RTPM_PRIO_TPD };
	sched_setscheduler(current, SCHED_RR, &param);
	int last_key = 0;
	int key;
	int index = 0;
	int i =0;

	do
	{
		mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
		enable_irq(CUST_EINT_TOUCH_PANEL_NUM);
		set_current_state(TASK_INTERRUPTIBLE);
		wait_event_interruptible(waiter, tpd_flag != 0);
		tpd_flag = 0;
		set_current_state(TASK_RUNNING);
		disable_irq(CUST_EINT_TOUCH_PANEL_NUM);
		mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
		rc = elan_ktf2k_ts_recv_data(private_ts->client, buf);

		if (rc < 0)
		{
			printk("[elan] rc<0\n");
	
			continue;
		}

		elan_ktf2k_ts_report_data(/*ts*/private_ts->client, buf);

	}while(!kthread_should_stop());

	return 0;
}

static int tpd_detect(struct i2c_client *client, int kind, struct i2c_board_info *info)
{
    strcpy(info->type, TPD_DEVICE);
    return 0;
}

static void tpd_eint_interrupt_handler(void)
{
//    printk("[elan]TPD int\n");
//20140722 tracy add for ESD -start
    #ifdef ESD_CHECK	 
		   have_interrupts = 1;
    #endif
//20140722 tracy add for ESD -end
    tpd_flag = 1;
    wake_up_interruptible(&waiter);
}

//20140917 tracy add touch replace ps define start
#if ELAN_PS
s32 tpd_ps_operate(void *self, u32 command, void *buff_in, s32 size_in,
			          void *buff_out, s32 size_out, s32 *actualout)
{
		s32 err = 0;
		s32 value;
		hwm_sensor_data *sensor_data;

		switch (command)
		{
			case SENSOR_DELAY:
				if ((buff_in == NULL) || (size_in < sizeof(int)))
				{
					//TPD_DEBUG("Set delay parameter error!");
					err = -EINVAL;
				}
				// Do nothing
				break;
			case SENSOR_ENABLE:
		printk("[elan-ps]tracy:enter SENSOR_ENABLE!!!!\n");
				if ((buff_in == NULL) || (size_in < sizeof(int)))
				{
					//TPD_DEBUG("Enable sensor parameter error!");
					err = -EINVAL;
				}
				else
				{
			printk("[elan-ps]ps_enable_flag=%d!!!!\n", ps_enable_flag);
			if (ps_enable_flag){  //0828 ps_enable_flag
			    value = *(int *)buff_in;
			    printk("[elan-ps]tracy:the value of enable is %d \n", value);
			    ckt_tp_replace_ps_enable(value);
			    err=0;
			}
					//printk("[elan]tracy:the value of err is %d !!!!\n", err);
				}
				break;
			case SENSOR_GET_DATA:
		//printk("[elan]tracy:enter SENSOR_GET_DATA!!!!\n");
				if ((buff_out == NULL) || (size_out < sizeof(hwm_sensor_data)))
				{
					printk("Get sensor data parameter error!");
					err = -EINVAL;
				}
				else
				{
			
					sensor_data = (hwm_sensor_data *)buff_out;
			//mdelay(2000);
			if( ckt_tp_replace_ps_close==1)
			{
				sensor_data->values[0] = 0;
				printk("[elan-ps]tracy:enter SENSOR_GET_DATA sensor_data=0\n");
			}
			else
			{
				sensor_data->values[0] = 1;
				printk("[elan-ps]tracy:enter SENSOR_GET_DATA sensor_data=1\n");
			}
					sensor_data->value_divide = 1;
					sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
			
				}
				break;
			default:
				//TPD_DEBUG("proxmy sensor operate function no this parameter %d!\n", command);
				err = -1;
				break;
		}
		return err;
}
#endif
//20140917 tracy add touch replace ps define end

static int tpd_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int err = 0;
	int fw_err = 0;
	int New_FW_ID;	
	int New_FW_VER;	
	int i;
	int retval = TPD_OK;
	static struct elan_ktf2k_ts_data ts;
//20140917 tracy add touch replace ps define start
#if ELAN_PS
	struct hwmsen_object obj_ps;
	int tp_err;
#endif
//20140917 tracy add touch replace ps define end

//20141216  tracy modify for touch main/second source detection start
  uint8_t sensor_recv[4] = {0};
  uint8_t get_sensor_cmd[4] = {0x53, 0xd3, 0x00, 0x01};
  int rc =0 ;
//20141216  tracy modify for touch main/second source detection end
	client->addr |= I2C_ENEXT_FLAG;

	
	printk("[elan] %s:client addr is %x, TPD_DEVICE = %s\n",__func__,client->addr,TPD_DEVICE);
	printk("[elan] %s:I2C_WR_FLAG=%x,I2C_MASK_FLAG=%x,I2C_ENEXT_FLAG =%x\n",__func__,I2C_WR_FLAG,I2C_MASK_FLAG,I2C_ENEXT_FLAG);
	client->timing =  100;

	printk("[elan]%x=IOCTL_I2C_INT\n",IOCTL_I2C_INT);
	printk("[elan]%x=IOCTL_IAP_MODE_LOCK\n",IOCTL_IAP_MODE_LOCK);
	printk("[elan]%x=IOCTL_IAP_MODE_UNLOCK\n",IOCTL_IAP_MODE_UNLOCK);

#if 1
	//client->timing = 400;
	i2c_client = client;
	private_ts = &ts;
	private_ts->client = client;
	//private_ts->addr = 0x2a;
#endif

//20140827 Tracy Modify Power VGP1 configuration to 3V start
      hwPowerOn(MT6323_POWER_LDO_VGP1, VOL_3000, "TP");
      hwPowerOn(MT6323_POWER_LDO_VGP1, VOL_1800, "TP_ENT");
      printk("[elan] MT6323_POWER_LDO_VGP1,VOL_3000\n");
//20140827 Tracy Modify Power VGP1 configuration to 3V end. 
	msleep(10);
#if 0
	/*LDO enable*/
	mt_set_gpio_mode(GPIO_CTP_EN_PIN, GPIO_CTP_EN_PIN_M_GPIO);
	mt_set_gpio_dir(GPIO_CTP_EN_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_CTP_EN_PIN, GPIO_OUT_ZERO);
	msleep(50);
	mt_set_gpio_out(GPIO_CTP_EN_PIN, GPIO_OUT_ONE);
#endif
	printk("[elan] ELAN enter tpd_probe_ ,the i2c addr=0x%x\n", client->addr);
	//printk("GPIO43 =%d,GPIO_CTP_EINT_PIN =%d,GPIO_DIR_IN=%d,CUST_EINT_TOUCH_PANEL_NUM=%d\n",GPIO43,GPIO_CTP_EINT_PIN,GPIO_DIR_IN,CUST_EINT_TOUCH_PANEL_NUM);

// Reset Touch Pannel
    mt_set_gpio_mode( GPIO_CTP_RST_PIN, GPIO_CTP_RST_PIN_M_GPIO );
    mt_set_gpio_dir( GPIO_CTP_RST_PIN, GPIO_DIR_OUT );
    mt_set_gpio_out( GPIO_CTP_RST_PIN, GPIO_OUT_ONE );
    mdelay(10);
//#if !defined(EVB)
    mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ZERO);
//#endif
    mdelay(10);
    mt_set_gpio_out( GPIO_CTP_RST_PIN, GPIO_OUT_ONE );
// End Reset Touch Pannel	

    mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_EINT);
    mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
    mt_set_gpio_pull_enable(GPIO_CTP_EINT_PIN, GPIO_PULL_ENABLE);
    mt_set_gpio_pull_select(GPIO_CTP_EINT_PIN, GPIO_PULL_UP);
    msleep( 200 );

#ifdef HAVE_TOUCH_KEY
int retry;
	for(retry = 0; retry <3; retry++)
	{
		input_set_capability(tpd->dev,EV_KEY,tpd_keys_local[retry]);
	}
#endif

// Setup Interrupt Pin
/* //0403 mark
	mt_set_gpio_mode(GPIO_CTP_EINT_PIN, GPIO_CTP_EINT_PIN_M_EINT);
	mt_set_gpio_dir(GPIO_CTP_EINT_PIN, GPIO_DIR_IN);
	mt_set_gpio_pull_enable(GPIO_CTP_EINT_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO_CTP_EINT_PIN, GPIO_PULL_UP);
*/
     
	mt_eint_set_sens(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_SENSITIVE);
	mt_eint_set_hw_debounce(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_CN);
//<<Mel - 4/10, Modify Interrupt triger to falling. 
      mt_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, /*EINTF_TRIGGER_LOW*/EINTF_TRIGGER_FALLING, tpd_eint_interrupt_handler, 1);
	//mt65xx_eint_registration(CUST_EINT_TOUCH_PANEL_NUM, CUST_EINT_TOUCH_PANEL_DEBOUNCE_EN, CUST_EINT_POLARITY_LOW, tpd_eint_interrupt_handler, 1);
//Mel - 4/10, Modify Interrupt triger to falling>>. 
	mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
	//msleep(100); //0403 mark
// End Setup Interrupt Pin	
	tpd_load_status = 1;

#ifdef MTK6589_DMA    
    gpDMABuf_va = (u8 *)dma_alloc_coherent(NULL, 4096, &gpDMABuf_pa, GFP_KERNEL);
    if(!gpDMABuf_va){
		printk(KERN_INFO "[elan] Allocate DMA I2C Buffer failed\n");
    }
#endif



mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
	fw_err = elan_ktf2k_ts_setup(client);
	if (fw_err < 0) {
		printk(KERN_INFO "[elan] No Elan chip inside\n");
	}
mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);

//20140721 tracy modify for touch point incorrect++
/*RESET RESOLUTION: tmp use ELAN_X_MAX & ELAN_Y_MAX*/ 
	printk("[elan] RESET RESOLUTION\n");
	input_set_abs_params(tpd->dev, ABS_X, 0,  ELAN_X_MAX, 0, 0);
	input_set_abs_params(tpd->dev, ABS_Y, 0,  ELAN_Y_MAX, 0, 0);
	input_set_abs_params(tpd->dev, ABS_MT_POSITION_X, 0, ELAN_X_MAX, 0, 0);
	input_set_abs_params(tpd->dev, ABS_MT_POSITION_Y, 0, ELAN_Y_MAX, 0, 0);
//20140721 tracy modify for touch point incorrect--


	#ifndef LCT_VIRTUAL_KEY
	set_bit( KEY_BACK,  tpd->dev->keybit );
    set_bit( KEY_HOMEPAGE,  tpd->dev->keybit );
    set_bit( KEY_MENU,  tpd->dev->keybit );
	#endif
	

	thread = kthread_run(touch_event_handler, 0, TPD_DEVICE);

	if(IS_ERR(thread))
	{
		retval = PTR_ERR(thread);
		printk(TPD_DEVICE "[elan]  failed to create kernel thread: %d\n", retval);
	}

	printk("[elan]  ELAN Touch Panel Device Probe %s\n", (retval < TPD_OK) ? "FAIL" : "PASS");
// Firmware Update
	// MISC
  	ts.firmware.minor = MISC_DYNAMIC_MINOR;
  	ts.firmware.name = "elan-iap";
  	ts.firmware.fops = &elan_touch_fops;
  	ts.firmware.mode = S_IRWXUGO; 
   	
  	if (misc_register(&ts.firmware) < 0)
  		printk("[elan] misc_register failed!!");
  	else
  	  printk("[elan] misc_register finished!!"); 
// End Firmware Update	

//20140722 tracy add for ESD -start
#ifdef ESD_CHECK 
	INIT_DELAYED_WORK(&esd_work, elan_touch_esd_func);
	esd_wq = create_singlethread_workqueue("esd_wq");	
	if (!esd_wq) {
		retval = -ENOMEM;
	}
	
	queue_delayed_work(esd_wq, &esd_work, delay);
#endif
//20140722 tracy add for ESD -end
//20140917 tracy add touch replace ps define start
#if ELAN_PS
	obj_ps.polling = 1; 
	//0--interrupt mode;1--polling mode;
	printk("[elan_debug]touch_proxi_register\n");
	obj_ps.sensor_operate = tpd_ps_operate;
	//   printk("[elan]hwmsen_attach\n");
	if ((tp_err = hwmsen_attach(ID_PROXIMITY, &obj_ps)))
	{
		TPD_DEBUG("hwmsen attach fail, return:%d.", tp_err);
	}
#endif
//20140917 tracy add touch replace ps define end

#if IAP_PORTION

 //for firmware auto-upgrade            
    //20141020 tracy modify check FW to force update FW 550X when phone FW  is 5532 start
    //20140917 tracy modify check FW when update FW for speedup booting start 
   
      printk("RECOVERY==0x%x!!",RECOVERY);  //20141024   tracy  modify for  improve touch  download
      //201219 tracy modify for main/second source FW Update start
    //20141227 tracy modify for main/second source FW Update for user mode start
      #if 0
	//20141024   tracy  modify for  improve touch  download	
	if(RECOVERY==0x80)
	{
	      printk("[elan]RECOVERY==0x80!!\n");
	thread_update = kthread_run(touch_update_handler, 0, TPD_DEVICE);
	      if(IS_ERR(thread_update))
	      {
		    retval = PTR_ERR(thread_update);
		     printk(TPD_DEVICE "[elan]  failed to create kernel thread_update: %d\n", retval);
	       }
	}
	else
	{
	     //20141216  tracy modify for touch main/second source detection start
	     //20141218 tracy add for update biel FW
	     if(!(FW_ID == 0x049b || FW_ID == 0xfb9b || FW_ID == 0x0464|| FW_ID == 0xfb64))  
        {
          printk("[elan]ID not correct, write default truly Firmware !!\n");
          thread_update = kthread_run(touch_update_handler, 0, TPD_DEVICE);
	      if(IS_ERR(thread_update))
	      {
		    retval = PTR_ERR(thread_update);
		     printk(TPD_DEVICE "[elan]  failed to create kernel thread_update: %d\n", retval);
	      }
	     }
	 else
       {
         printk("ID correct!!\n");
         disable_irq(CUST_EINT_TOUCH_PANEL_NUM);
    	 mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);
	 #ifdef ESD_CHECK	
             cancel_delayed_work_sync(&esd_work);
         #endif
	       rc = elan_ktf2k_ts_get_data(client, get_sensor_cmd, sensor_recv, 4);
	 enable_irq(CUST_EINT_TOUCH_PANEL_NUM);
	 mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
	 #ifdef ESD_CHECK  
         queue_delayed_work(esd_wq, &esd_work, delay);	
     #endif
	       if (rc < 0){
		        printk("[elan] Can't get Sensor Option Data.");
	       } 
	       else
	       {
	      printk("[elan] get sensor option: 0x%x%x!!!!\n", sensor_recv[2],sensor_recv[3]);
           	if(sensor_recv[2]==0x9b && sensor_recv[3]==0x04)
           	  {
					printk("Sensor option: 0x9b04\n");
		           file_fw_data = file_fw_data_truly;
           	  }   
		    else if(sensor_recv[2]==0x9b && sensor_recv[3]==0xfb) 
		      {
					printk("Sensor option: 0x9bfb\n");
		           file_fw_data = file_fw_data_liano;
		      }   
		    else if(sensor_recv[2]==0x64 && sensor_recv[3]==0x04)
		      {
					printk("Sensor option: 0x6404\n");
		           file_fw_data = file_fw_data_biel;   
		      }
			else if(sensor_recv[2]==0x64 && sensor_recv[3]==0xfb) //20141218 tracy add for update biel FW
			  {
					printk("Sensor option: 0x64fb\n");
		           file_fw_data = file_fw_data_biel; 
			  }
		  New_FW_ID = file_fw_data[0x7D67]<<8  | file_fw_data[0x7D66] ;        
	      New_FW_VER = file_fw_data[0x7D65]<<8  | file_fw_data[0x7D64] ;
	      //20140625 tracy modify for read currect FW version--
	      printk(" FW_ID=0x%x,   New_FW_ID=0x%x \n",  FW_ID, New_FW_ID);   	       
	      printk(" FW_VERSION=0x%x,   New_FW_VER=0x%x \n",  FW_VERSION  , New_FW_VER); 
	       }  
           
         if (FW_VERSION != New_FW_VER)
         {
           printk("FW Version is not the same, go to Update!!");
           thread_update = kthread_run(touch_update_handler, 0, TPD_DEVICE);
	      if(IS_ERR(thread_update))
	      {
		    retval = PTR_ERR(thread_update);
		     printk(TPD_DEVICE "[elan]  failed to create kernel thread_update: %d\n", retval);
	      }
	     }
		 //20141216  tracy modify for touch main/second source detection end
		 //201219 tracy modify for main/second source FW Update end
       }    
	}   	
	//20141119 tracy add for improve touch FW update end
	
	 //20141030 tracy modify  for force update FW end
	//20141024   tracy  modify for  improve touch  download
    //20141020 tracy modify check FW to force update FW 550X when phone FW  is 5532 end
    //20140917 tracy modify check FW when update FW for speedup booting end
    #else
   mt_set_gpio_mode(Touch_Detect_Pin, GPIO_MODE_GPIO);
   mt_set_gpio_dir(Touch_Detect_Pin, GPIO_DIR_IN);
   Touch_Source=mt_get_gpio_in(Touch_Detect_Pin);
   if(Touch_Source==0)
           	  {
					printk("Sensor option: 0x9b04\n");
		           file_fw_data = file_fw_data_truly;
           	  }   
		    else 
		      {
					printk("Sensor option: 0x64fb\n");
		           file_fw_data = file_fw_data_biel; 
		      }
		printk("FW Version is not the same, go to Update!!");
           thread_update = kthread_run(touch_update_handler, 0, TPD_DEVICE);
	      if(IS_ERR(thread_update))
	      {
		    retval = PTR_ERR(thread_update);
		     printk(TPD_DEVICE "[elan]  failed to create kernel thread_update: %d\n", retval);
	      }
	#endif
	//20141227 tracy modify for main/second source FW Update for user mode end
#endif


    
    return 0;

}

//20140917 tracy add touch replace ps define start
#if ELAN_PS
int ckt_tp_replace_ps_mod_on(void)
{
	int res = 0;
	char buf_on[] = {0x54, 0xC1, 0x00, 0x01};
	ckt_tp_replace_ps_close = 0;
	res = i2c_master_send(i2c_client, buf_on, sizeof(buf_on));
	if (res != sizeof(buf_on)) {
		printk("[elan-ps] turn on face mod faild\n");
		ckt_tp_replace_ps_state=0;
		return 0;
	} else {
		ckt_tp_replace_ps_state=1;
		printk("[elan-ps] turn on face mod ok\n");
		return 1;
	}
}

int ckt_tp_replace_ps_mod_off(void)
{
#if 1
	char buf_off[] = {0x54, 0xC0, 0x00, 0x01};
	ssize_t ret = 0;
	ckt_tp_replace_ps_close = 0;
	ret = i2c_master_send(i2c_client, buf_off, sizeof(buf_off));
	if (ret != sizeof(buf_off)) {	
		ckt_tp_replace_ps_state=1;
		printk("[elan-ps] turn off face mod faild\n");
		return 0;
	} else {
	       ckt_tp_replace_ps_state=0;
		printk("[elan-ps] turn off face mod ok\n");
		return 1;
	}
#else
	mt_set_gpio_mode(GPIO_CTP_RST_PIN, 0);
	mt_set_gpio_dir(GPIO_CTP_RST_PIN, 1);
	mt_set_gpio_out(GPIO_CTP_RST_PIN, 0);
	msleep(30);

	// for enable/reset pin
	mt_set_gpio_mode(GPIO_CTP_RST_PIN, 0);
	mt_set_gpio_dir(GPIO_CTP_RST_PIN, 1);
	mt_set_gpio_out(GPIO_CTP_RST_PIN, 1);
	msleep(300);
	ckt_tp_replace_ps_close = 0;
	ckt_tp_replace_ps_state=0;
	printk("[elan-ps] turn off face mod ok\n");
	return 1;
#endif
}
int  ckt_tp_replace_ps_enable(int enable)
{
  if (!updatefw_flag){   //20141030   tracy  add  for  avoid update FW  fail
if (enable)
	{
	if(1==ckt_tp_replace_ps_mod_on())
		{
		printk("[elan-ps]open the ps mod successful\n");
	    return 1;
		}
	else
		{
		printk("[elan-ps]open the ps mod fail\n");
	    return 0;
		}
	
	}
else
	{
	if(1==ckt_tp_replace_ps_mod_off())
		{
		printk("[elan-ps]close the ps mod successful\n");
	    return 1;
		}
	else
		{
		printk("[elan-ps]close the ps mod fail\n");
	    return 0;
		}
	      
	}
} //20141030   tracy  add  for  avoid update FW  fail
}
u16  ckt_get_tp_replace_ps_value(void)
	{
	if(1==ckt_tp_replace_ps_close)
		{
		printk("[elan-ps]ckt_get_tp_replace_ps_value 500\n");
		return 500;
		}
	else
		{
		printk("[elan-ps]ckt_get_tp_replace_ps_value 100\n");
		return 100;
		}
	
	}
#endif
//20140917 tracy add touch replace ps define end
#ifdef ESD_CHECK  
static void elan_touch_esd_func(struct work_struct *work)
{	
	int res;	
	printk("[elan esd] %s: enter.......\n", __FUNCTION__); 		/* elan_dlx */
	
	if(have_interrupts == 1){
		printk("[elan esd] %s: had interrup not need check\n", __func__);
	}
	else{

	  mt_set_gpio_mode(GPIO_CTP_RST_PIN, 0);
    mt_set_gpio_dir(GPIO_CTP_RST_PIN, 1);
    mt_set_gpio_out(GPIO_CTP_RST_PIN, 0);
    msleep(10);

            // for enable/reset pin
    mt_set_gpio_mode(GPIO_CTP_RST_PIN, 0); 
    mt_set_gpio_dir(GPIO_CTP_RST_PIN, 1);
    mt_set_gpio_out(GPIO_CTP_RST_PIN, 1);
    msleep(100);
	}
	
	have_interrupts = 0;	
	queue_delayed_work(esd_wq, &esd_work, delay);
	printk("[elan esd] %s: exit.......\n", __FUNCTION__); 		/* elan_dlx */
}
#endif
//20140722 tracy add for ESD -end

static int tpd_remove(struct i2c_client *client)

{
    printk("[elan] TPD removed\n");
    
	#ifdef MTK6589_DMA    
    if(gpDMABuf_va){
        dma_free_coherent(NULL, 4096, gpDMABuf_va, gpDMABuf_pa);
        gpDMABuf_va = NULL;
        gpDMABuf_pa = NULL;
    }
	#endif    

    return 0;
}


static int tpd_suspend(struct i2c_client *client, pm_message_t message)
{
    int retval = TPD_OK;
    static char data = 0x3;
    uint8_t cmd[] = {CMD_W_PKT, 0x50, 0x00, 0x01};
//20140917 tracy add touch replace ps define start

	#if ELAN_PS
	if(ckt_tp_replace_ps_state == 1)
	{
		printk("[elan-ps] %s: can't enter sleep mode when tp replace ps\n", __func__);
		return;
	}
	#endif    
//20140917 tracy add touch replace ps define end
    
    printk("[elan] TP enter into sleep mode\n");
    if ((i2c_master_send(private_ts->client, cmd, sizeof(cmd))) != sizeof(cmd)) 
    {
			printk("[elan] %s: i2c_master_send failed\n", __func__);
			return -retval;
    }
    mt_eint_mask(CUST_EINT_TOUCH_PANEL_NUM);

//20140722 tracy add for ESD -start
#ifdef ESD_CHECK	
	cancel_delayed_work_sync(&esd_work);
#endif    
//20140722 tracy add for ESD -end
    return retval;
}


static int tpd_resume(struct i2c_client *client)
{
    int retval = TPD_OK;
    uint8_t cmd[] = {CMD_W_PKT, 0x58, 0x00, 0x01};
    printk("[elan] %s wake up\n", __func__);
//20140722 tracy add for ESD -start
//20140917 tracy add touch replace ps define start
#if ELAN_PS
    if(ckt_tp_replace_ps_state == 1)
    {
        //ckt_tp_replace_ps_state = 0;
        printk(TPD_DEVICE "[elan-ps] %s: can't enter sleep mode when tp replace ps\n", __func__);
        return;
    }
#endif
//20140917 tracy add touch replace ps define end
#ifdef ESD_CHECK
	queue_delayed_work(esd_wq, &esd_work, delay);	
#endif
//20140722 tracy add for ESD -end
//20141230 tracy add for improve touch wake up process start
#if 1//samhuang	
	// Reset Touch Pannel
    mt_set_gpio_mode( GPIO_CTP_RST_PIN, GPIO_CTP_RST_PIN_M_GPIO );
    mt_set_gpio_dir( GPIO_CTP_RST_PIN, GPIO_DIR_OUT );
    mt_set_gpio_out( GPIO_CTP_RST_PIN, GPIO_OUT_ONE );
    mdelay(10);
    mt_set_gpio_out(GPIO_CTP_RST_PIN, GPIO_OUT_ZERO);
    //20141029 tracy add delay fix hang when wakeup start	
    mdelay(50);
    mt_set_gpio_out( GPIO_CTP_RST_PIN, GPIO_OUT_ONE );
    //20141219 tracy modify for add touch cabilation 
    mdelay(300);
    //20141029 tracy add delay fix hang when wakeup end
    //#else 
    if ((i2c_master_send(private_ts->client, cmd, sizeof(cmd))) != sizeof(cmd)) 
    {
		printk("[elan] %s: i2c_master_send failed\n", __func__);
		return -retval;
    }

    msleep(200);
#endif
//20141230 tracy add for improve touch wake up process end

    mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
    return retval;
}

static int tpd_local_init(void)
{
    printk("[elan]: I2C Touchscreen Driver init\n");
    if(i2c_add_driver(&tpd_i2c_driver) != 0)
    {
        printk("[elan]: unable to add i2c driver.\n");
        return -1;
    }
    
    if(tpd_load_status == 0) 
    {
         printk("ektf3248 add error touch panel driver.\n");
         i2c_del_driver(&tpd_i2c_driver);
         return -1;
    }    

#ifdef TPD_HAVE_BUTTON
#ifdef LCT_VIRTUAL_KEY
    tpd_button_setting(TPD_KEY_COUNT, tpd_keys_local, tpd_keys_dim_local);// initialize tpd button data
 #endif
#endif
#if (defined(TPD_WARP_START) && defined(TPD_WARP_END))
    TPD_DO_WARP = 1;
    memcpy(tpd_wb_start, tpd_wb_start_local, TPD_WARP_CNT * 4);
    memcpy(tpd_wb_end, tpd_wb_start_local, TPD_WARP_CNT * 4);
#endif
#if (defined(TPD_HAVE_CALIBRATION) && !defined(TPD_CUSTOM_CALIBRATION))
         memcpy(tpd_calmat, tpd_def_calmat_local, 8*4);
         memcpy(tpd_def_calmat, tpd_def_calmat_local, 8*4);         
#endif 
    printk("end %s, %d\n", __FUNCTION__, __LINE__);
    tpd_type_cap = 1;
    return 0;
}


static struct tpd_driver_t tpd_device_driver =
{
    .tpd_device_name = "ektf2k_mtk",       
    .tpd_local_init = tpd_local_init,
    .suspend = tpd_suspend,
    .resume = tpd_resume,
#ifdef TPD_HAVE_BUTTON
    .tpd_have_button = 1,
#else
    .tpd_have_button = 0,
#endif
};

static int __init tpd_driver_init(void)
{
    printk("[elan]: Driver Verison MTK0005 for MTK65xx serial\n");
#ifdef ELAN_MTK6577		
		printk("[elan] Enable ELAN_MTK6577\n");
		i2c_register_board_info(0, &i2c_tpd, 1);
#endif		
    if(tpd_driver_add(&tpd_device_driver) < 0)
    {
        printk("[elan]: %s driver failed\n", __func__);
    }
    return 0;
}


static void __exit tpd_driver_exit(void)
{
    printk("[elan]: %s elan touch panel driver exit\n", __func__);
    tpd_driver_remove(&tpd_device_driver);
}

module_init(tpd_driver_init);
module_exit(tpd_driver_exit);




