#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/time.h>
#include "kd_flashlight.h"
#include <asm/io.h>
#include <asm/uaccess.h>
#include "kd_camera_hw.h"
#include <cust_gpio_usage.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/xlog.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
#include <linux/mutex.h>
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#include <linux/semaphore.h>
#else
#include <asm/semaphore.h>
#endif
#endif

#include <linux/i2c.h>
#include <linux/leds.h>

//<2014/12/30-samhuang, porting e1-2 driver
#if defined(E12_HW)
#include <leds-sky81294.h>
#endif
//>2014/12/30-samhuang

/******************************************************************************
 * Debug configuration
******************************************************************************/
// availible parameter
// ANDROID_LOG_ASSERT
// ANDROID_LOG_ERROR
// ANDROID_LOG_WARNING
// ANDROID_LOG_INFO
// ANDROID_LOG_DEBUG
// ANDROID_LOG_VERBOSE
#define TAG_NAME "leds_strobe.c"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, arg...)    xlog_printk(ANDROID_LOG_ERROR  , TAG_NAME, KERN_INFO  "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_WARN(fmt, arg...)        xlog_printk(ANDROID_LOG_WARNING, TAG_NAME, KERN_WARNING  "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_NOTICE(fmt, arg...)      xlog_printk(ANDROID_LOG_DEBUG  , TAG_NAME, KERN_NOTICE  "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_INFO(fmt, arg...)        xlog_printk(ANDROID_LOG_INFO   , TAG_NAME, KERN_INFO  "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_TRC_FUNC(f)              xlog_printk(ANDROID_LOG_DEBUG  , TAG_NAME,  "<%s>\n", __FUNCTION__);
#define PK_TRC_VERBOSE(fmt, arg...) xlog_printk(ANDROID_LOG_VERBOSE, TAG_NAME,  fmt, ##arg)
#define PK_ERROR(fmt, arg...)       xlog_printk(ANDROID_LOG_ERROR  , TAG_NAME, KERN_ERR "%s: " fmt, __FUNCTION__ ,##arg)


#define DEBUG_LEDS_STROBE
#ifdef  DEBUG_LEDS_STROBE
    #define PK_DBG PK_DBG_FUNC
    #define PK_VER PK_TRC_VERBOSE
    #define PK_ERR PK_ERROR
#else
    #define PK_DBG(a,...)
    #define PK_VER(a,...)
    #define PK_ERR(a,...)
#endif

/******************************************************************************
 * local variables
******************************************************************************/

static DEFINE_SPINLOCK(g_strobeSMPLock); /* cotta-- SMP proection */


static u32 strobe_Res = 0;
static u32 strobe_Timeus = 0;
static BOOL g_strobe_On = 0;


static int g_timeOutTimeMs=0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37))
static DEFINE_MUTEX(g_strobeSem);
#else
static DECLARE_MUTEX(g_strobeSem);
#endif
//<2015/02/09-joubert.she-[V10][Feature][Common][BSP][][] add SKY81294 ic driver
#if defined(V10_HW)
    #if defined(SKY81294_IS_USED)
    //2015/02/09-joubert.she-[V10][Feature][Common][BSP][][]correct flashlight ic address
    #define STROBE_DEVICE_ID (0x37<<1)
    //2015/02/09-joubert.she
    #else
    #define STROBE_DEVICE_ID 0xc6 //0x63
    #endif
#else
    #define STROBE_DEVICE_ID 0xc6 //0x63
#endif
//>2015/02/09-joubert.she

static struct work_struct workTimeOut;

//<2015/01/08-samhuang, add driver for E1-2 HW
#if defined(E12_HW)
static int g_duty=0;
#define FLASH_GPIO_ENF GPIO_CAMERA_FLASH_EN_PIN
#define FLASH_GPIO_ENT GPIO_CAMERA_FLASH_MODE_PIN
#else
//#define FLASH_GPIO_ENF GPIO12
//#define FLASH_GPIO_ENT GPIO13
//2015/02/09-joubert.she-[V10][Feature][Common][BSP][][]distinguish torch and flash mode
static int g_duty=-1;
//2015/02/09-joubert.she
#define GPIO_LED_EN  GPIO12
#define GPIO_LED_TOR_EN  GPIO13
#endif
//>2015/01/08-samhuang


#define LM3648_REG_ENABLE          0x01
#define LM3648_REG_FLASH_BR        0x03
#define LM3648_REG_TORCH_BR        0x05
#define LM3648_REG_FLASH_TOUT    0x08
#define LM3648_REG_FLAG0        0x0a
#define LM3648_REG_FLAG1        0x0b





/*****************************************************************************
Functions
*****************************************************************************/
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);
extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
static void work_timeOutFunc(struct work_struct *data);

int readReg(int reg)
{
    char buf[2];
    char bufR[2];
    buf[0]=reg;
    iReadRegI2C(buf , 1, bufR,1, STROBE_DEVICE_ID);
    PK_DBG("qq reg=%x val=%x qq\n", buf[0],bufR[0]);
    return (int)bufR[0];
}

int writeReg(int reg, int data)
{
    char buf[2];
    buf[0]=reg;
    buf[1]=data;

    iWriteRegI2C(buf, 2, STROBE_DEVICE_ID);

   return 0;
}

#define DUTYNUM 16
#define TORCHDUTYNUM 4

static int isMovieMode[DUTYNUM] = {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int isTorchFlag = 0;

static int torchLED1Reg[TORCHDUTYNUM] = {31,63,95,127};//92,178,270,357
static int flashLED1Reg[DUTYNUM] = {0,0,0,0,15,19,23,27,31,35,39,43,47,51,55,59,63};//31:748,63:1500

int m_duty=0;
//<2015/02/09-joubert.she-[V10][Feature][Common][BSP][][] add SKY81294 ic driver
#if defined(V10_HW)
#define SKY81294_REG_ENABLE          0x03
#define SKY81294_REG_FLASH_BR        0x00
#define SKY81294_REG_TORCH_BR        0x02
#define SKY81294_REG_FLASH_TOUT    0x01
static int torchLED1Reg_SKY81294[TORCHDUTYNUM] = {3,7,11,15};
static int flashLED1Reg_SKY81294[DUTYNUM] = {0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
//2015/02/09-joubert.she-[V10][Feature][Common][BSP][][]distinguish torch and flash mode
int  flashEnable_SKY81294(void)
{
    PK_DBG("flashEnable_SKY81294");

      if(g_duty==0)
        {

            PK_DBG("movie mode_SKY81294");
            writeReg(SKY81294_REG_TORCH_BR, 0x03);// movie/flash mode
            writeReg(SKY81294_REG_ENABLE,0x01);// movie/flash mode

        }
    else
        {
            PK_DBG("flash mode_SKY81294");

             writeReg(SKY81294_REG_FLASH_BR, 0x0D);//flash mode
             writeReg(SKY81294_REG_FLASH_TOUT, 0x05);//flash mode
            writeReg(SKY81294_REG_ENABLE,0x02);//flash mode
                if(mt_set_gpio_out(GPIO_LED_EN,GPIO_OUT_ONE)){PK_DBG(" set gpio failed!! \n");}
        }
    return 0;

}
int  flashDisable_SKY81294(void)
{
    PK_DBG("flashDisable_SKY81294");

      if(g_duty==0)
        {

            writeReg(SKY81294_REG_ENABLE,0x00);// movie/flash mode
        }
    else
        {

            writeReg(SKY81294_REG_ENABLE,0x00);//flash mode
            if(mt_set_gpio_out(GPIO_LED_EN,GPIO_OUT_ZERO)){PK_DBG(" set gpio failed!! \n");}
        }
    return 0;

}
int setDuty_SKY81294(int duty)
{
    if(duty<0)
        duty=0;
    else if(duty>=TORCHDUTYNUM)
        duty=TORCHDUTYNUM-1;
    m_duty=duty;

    PK_DBG("setDuty_SKY81294:m_duty = %d!\n", m_duty);

    if(isMovieMode[m_duty] == 1)
    {
        isTorchFlag = 1;
        writeReg(SKY81294_REG_TORCH_BR, torchLED1Reg_SKY81294[m_duty] );
    }
    else
    {
        isTorchFlag = 0;
         writeReg(SKY81294_REG_FLASH_BR, flashLED1Reg_SKY81294[m_duty] );
    }

    return 0;
}
#endif
//>2015/02/09-joubert.she
//2015/02/09-joubert.she
//*********************************************************************************************************************************************************//
//*******************************************************************LM3648 ST************************************************************************//
int flashEnable_LM3648(void)
{
    int temp;

    PK_DBG("flashEnable_LM3648\n");

    writeReg(LM3648_REG_FLASH_TOUT, 0x1F);

    if(isTorchFlag == 0)
    {
        writeReg(LM3648_REG_ENABLE, 0x0B);//torch mode open
    }
    else
    {
        writeReg(LM3648_REG_ENABLE, 0x0F);//flash mode open
        //2015/02/09-joubert.she-[V10][Feature][Common][BSP][][]disable enable gpio of ic
        #if defined(V10_HW)
        if(mt_set_gpio_out(GPIO_LED_EN,GPIO_OUT_ZERO)){PK_DBG(" set gpio failed!! \n");}
        #endif
        //2015/02/09-joubert.she
    }

    return 0;

}



int flashDisable_LM3648(void)
{
    int temp;

    PK_DBG("flashDisable_LM3648\n");


    if(isTorchFlag == 1)
    {
        writeReg(LM3648_REG_ENABLE, 0x08);//torch mode close
    }
    else
    {
        writeReg(LM3648_REG_ENABLE, 0x0C);//flash mode close
    }

    return 0;
}


int setDuty_LM3648(int duty)
{
    if(duty<0)
        duty=0;
    else if(duty>=TORCHDUTYNUM)
        duty=TORCHDUTYNUM-1;
    m_duty=duty;

    PK_DBG("setDuty_LM3648:m_duty = %d!\n", m_duty);

    if(isMovieMode[m_duty] == 1)
    {
        isTorchFlag = 1;
        writeReg(LM3648_REG_TORCH_BR, torchLED1Reg[m_duty] | (1 << 7));
    }
    else
    {
        isTorchFlag = 0;
         writeReg(LM3648_REG_FLASH_BR, flashLED1Reg[m_duty] | (2 << 6));
    }

    return 0;
}

//*******************************************************************LM3648 ED************************************************************************//
//*********************************************************************************************************************************************************//


int FL_Enable(void)
{
//<2015/01/08-samhuang, add driver for E1-2 HW


#if defined(E12_HW)
  if(g_duty==0)
  {
     sky81294_set_flashlight_control(255);
  }
  else
  {
      sky81294_set_flashlight_control(200);
  }
#else
    PK_DBG(" FL_Enable line=%d\n",__LINE__);
    //<2015/02/09-joubert.she-[V10][Feature][Common][BSP][][] add SKY81294 ic driver
    #if defined(SKY81294_IS_USED)

    flashEnable_SKY81294();
    #else
    flashEnable_LM3648();
    #endif
    //>2015/02/09-joubert.she


#endif
//>2015/01/08-samhuang
    return 0;
}



int FL_Disable(void)
{
//<2015/01/08-samhuang, add driver for E1-2 HW


#if defined(E12_HW)
    sky81294_set_flashlight_control(0);
#else
    PK_DBG(" FL_Disable line=%d\n",__LINE__);
    //<2015/02/09-joubert.she-[V10][Feature][Common][BSP][][] add SKY81294 ic driver
    #if defined(SKY81294_IS_USED)
    flashDisable_SKY81294();
    #else
        flashDisable_LM3648();
    #endif
    //>2015/02/09-joubert.she
#endif

//>2015/01/08-samhuang
    return 0;
}

int FL_dim_duty(kal_uint32 duty)
{
//<2015/01/08-samhuang, add driver for E1-2 HW
#if defined(E12_HW)
#else
    PK_DBG(" FL_dim_duty line=%d\n",__LINE__);
    //<2015/02/09-joubert.she-[V10][Feature][Common][BSP][][] add SKY81294 ic driver
    #if defined(SKY81294_IS_USED)
    //2015/02/09-joubert.she-[V10][Feature][Common][BSP][][]distinguish torch and flash mode
        g_duty = duty;
        PK_DBG(" g_duty=%d\n",g_duty);
    //2015/02/09-joubert.she
    #else
    setDuty_LM3648(duty);
    #endif
    //>2015/02/09-joubert.she
#endif
//>2015/01/08-samhuang
    return 0;
}




int FL_Init(void)
{
//<2015/01/08-samhuang, add driver for E1-2 HW
#if defined(E12_HW)
  char buf[2];
  buf[0]=0xa;
    buf[1]=0x0;
    iWriteRegI2C(buf , 2, STROBE_DEVICE_ID);

    buf[0]=0x8;
    buf[1]=0x47;
    iWriteRegI2C(buf , 2, STROBE_DEVICE_ID);

    buf[0]=9;
    buf[1]=0x35;
    iWriteRegI2C(buf , 2, STROBE_DEVICE_ID);


    if(mt_set_gpio_mode(FLASH_GPIO_ENT,GPIO_MODE_00)){PK_DBG("[constant_flashlight] set gpio mode failed!! \n");}
    if(mt_set_gpio_dir(FLASH_GPIO_ENT,GPIO_DIR_OUT)){PK_DBG("[constant_flashlight] set gpio dir failed!! \n");}
    if(mt_set_gpio_out(FLASH_GPIO_ENT,GPIO_OUT_ZERO)){PK_DBG("[constant_flashlight] set gpio failed!! \n");}

        if(mt_set_gpio_mode(FLASH_GPIO_ENF,GPIO_MODE_00)){PK_DBG("[constant_flashlight] set gpio mode failed!! \n");}
    if(mt_set_gpio_dir(FLASH_GPIO_ENF,GPIO_DIR_OUT)){PK_DBG("[constant_flashlight] set gpio dir failed!! \n");}
    if(mt_set_gpio_out(FLASH_GPIO_ENF,GPIO_OUT_ZERO)){PK_DBG("[constant_flashlight] set gpio failed!! \n");}

#else

//<2015/02/09-joubert.she-[V10][Feature][Common][BSP][][] add SKY81294 ic driver
    PK_DBG("LED_FL_Init!\n");
    #if defined(SKY81294_IS_USED)
    if(mt_set_gpio_mode(GPIO_LED_EN,GPIO_MODE_00)){PK_DBG(" set gpio mode failed!! \n");}
    if(mt_set_gpio_dir(GPIO_LED_EN,GPIO_DIR_OUT)){PK_DBG(" set gpio dir failed!! \n");}
    if(mt_set_gpio_out(GPIO_LED_EN,GPIO_OUT_ZERO)){PK_DBG(" set gpio failed!! \n");}

        if(mt_set_gpio_mode(GPIO_LED_TOR_EN,GPIO_MODE_00)){PK_DBG(" set gpio mode failed!! \n");}
    if(mt_set_gpio_dir(GPIO_LED_TOR_EN,GPIO_DIR_OUT)){PK_DBG(" set gpio dir failed!! \n");}
    if(mt_set_gpio_out(GPIO_LED_TOR_EN,GPIO_OUT_ZERO)){PK_DBG(" set gpio failed!! \n");}
        //INIT_WORK(&workTimeOut, work_timeOutFunc);
    #else
    if(mt_set_gpio_mode(GPIO_LED_EN,GPIO_MODE_00)){PK_DBG(" set gpio mode failed!! \n");}
    if(mt_set_gpio_dir(GPIO_LED_EN,GPIO_DIR_OUT)){PK_DBG(" set gpio dir failed!! \n");}
    if(mt_set_gpio_out(GPIO_LED_EN,GPIO_OUT_ONE)){PK_DBG(" set gpio failed!! \n");}

    if(mt_set_gpio_mode(GPIO_LED_TOR_EN,GPIO_MODE_00)){PK_DBG(" set gpio mode failed!! \n");}
    if(mt_set_gpio_dir(GPIO_LED_TOR_EN,GPIO_DIR_OUT)){PK_DBG(" set gpio dir failed!! \n");}
    if(mt_set_gpio_out(GPIO_LED_TOR_EN,GPIO_OUT_ONE)){PK_DBG(" set gpio failed!! \n");}

    INIT_WORK(&workTimeOut, work_timeOutFunc);
    #endif
//>2015/02/09-joubert.she
#endif
//>2015/01/08-samhuang
    PK_DBG(" FL_Init line=%d\n",__LINE__);
    return 0;
}


int FL_Uninit(void)
{
    PK_DBG("LED1_FL_Uninit!\n");
    FL_Disable();
    //<2015/01/08-samhuang, add driver for E1-2 HW
#if defined(E12_HW)
#else
//<2015/02/09-joubert.she-[V10][Feature][Common][BSP][][] add SKY81294 ic driver
#if defined(SKY81294_IS_USED)
//2015/02/09-joubert.she-[V10][Feature][Common][BSP][][]disable enable gpio of ic
    if(mt_set_gpio_out(GPIO_LED_EN,GPIO_OUT_ZERO)){PK_DBG(" set gpio failed!! \n");}
     if(mt_set_gpio_out(GPIO_LED_TOR_EN,GPIO_OUT_ZERO)){PK_DBG(" set gpio failed!! \n");}
#else
    if(mt_set_gpio_out(GPIO_LED_EN,GPIO_OUT_ZERO)){PK_DBG(" set gpio failed!! \n");}
     if(mt_set_gpio_out(GPIO_LED_TOR_EN,GPIO_OUT_ZERO)){PK_DBG(" set gpio failed!! \n");}
#endif
//2015/02/09-joubert.she
//>2015/02/09-joubert.she
#endif
    //>2015/01/08-samhuang
    return 0;
}

/*****************************************************************************
User interface
*****************************************************************************/

static void work_timeOutFunc(struct work_struct *data)
{
    FL_Disable();
    PK_DBG("LED1TimeOut_callback\n");
}



enum hrtimer_restart ledTimeOutCallback(struct hrtimer *timer)
{
    schedule_work(&workTimeOut);
    return HRTIMER_NORESTART;
}
static struct hrtimer g_timeOutTimer;
void timerInit(void)
{
      INIT_WORK(&workTimeOut, work_timeOutFunc);
    g_timeOutTimeMs=1000; //1s
    hrtimer_init( &g_timeOutTimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
    g_timeOutTimer.function=ledTimeOutCallback;

}



static int constant_flashlight_ioctl(MUINT32 cmd, MUINT32 arg)
{
    int i4RetValue = 0;
    int ior_shift;
    int iow_shift;
    int iowr_shift;
    ior_shift = cmd - (_IOR(FLASHLIGHT_MAGIC,0, int));
    iow_shift = cmd - (_IOW(FLASHLIGHT_MAGIC,0, int));
    iowr_shift = cmd - (_IOWR(FLASHLIGHT_MAGIC,0, int));
    PK_DBG("constant_flashlight_ioctl() line=%d ior_shift=%d, iow_shift=%d iowr_shift=%d arg=%d\n",__LINE__, ior_shift, iow_shift, iowr_shift, arg);
    switch(cmd)
    {

        case FLASH_IOC_SET_TIME_OUT_TIME_MS:
            PK_DBG("FLASH_IOC_SET_TIME_OUT_TIME_MS: %d\n",arg);
            g_timeOutTimeMs=arg;
        break;


        case FLASH_IOC_SET_DUTY :
            PK_DBG("FLASHLIGHT_DUTY: %d\n",arg);
//<2015/01/08-samhuang, add driver for E1-2 HW
#if defined(E12_HW)
#else
//<2015/02/09-joubert.she-[V10][Feature][Common][BSP][][] add SKY81294 ic driver
            #if defined(SKY81294_IS_USED)
            //2015/02/09-joubert.she-[V10][Feature][Common][BSP][][]distinguish torch and flash mode
                FL_dim_duty(arg);
            //2015/02/09-joubert.she
            #else
            setDuty_LM3648(arg);
            #endif
//<2015/02/09-joubert.she
#endif
//>2015/01/08-samhuang
            break;


        case FLASH_IOC_SET_STEP:
            PK_DBG("FLASH_IOC_SET_STEP: %d\n",arg);

            break;

        case FLASH_IOC_SET_ONOFF :
            PK_DBG("FLASHLIGHT_ONOFF: %d\n",arg);
            if(arg==1)
            {
                if(g_timeOutTimeMs!=0)
                {
                    ktime_t ktime;
                    ktime = ktime_set( 0, g_timeOutTimeMs*1000000 );
                    hrtimer_start( &g_timeOutTimer, ktime, HRTIMER_MODE_REL );
                }
                FL_Enable();
            }
            else
            {
                FL_Disable();
                hrtimer_cancel( &g_timeOutTimer );
            }
            break;
        case FLASH_IOC_SET_REG_ADR:
            break;
        case FLASH_IOC_SET_REG_VAL:
            break;
        case FLASH_IOC_SET_REG:
            break;
        case FLASH_IOC_GET_REG:
            break;



        default :
            PK_DBG(" No such command \n");
            i4RetValue = -EPERM;
            break;
    }
    return i4RetValue;
}




static int constant_flashlight_open(void *pArg)
{
    int i4RetValue = 0;
    PK_DBG("constant_flashlight_open line=%d\n", __LINE__);

    if (0 == strobe_Res)
    {
        FL_Init();
        timerInit();
    }
    PK_DBG("constant_flashlight_open line=%d\n", __LINE__);
    spin_lock_irq(&g_strobeSMPLock);


    if(strobe_Res)
    {
        PK_ERR(" busy!\n");
        i4RetValue = -EBUSY;
    }
    else
    {
        strobe_Res += 1;
    }


    spin_unlock_irq(&g_strobeSMPLock);
    PK_DBG("constant_flashlight_open line=%d\n", __LINE__);

    return i4RetValue;

}


static int constant_flashlight_release(void *pArg)
{
    PK_DBG(" constant_flashlight_release\n");

    if (strobe_Res)
    {
        spin_lock_irq(&g_strobeSMPLock);

        strobe_Res = 0;
        strobe_Timeus = 0;

        /* LED On Status */
        g_strobe_On = FALSE;

        spin_unlock_irq(&g_strobeSMPLock);

        FL_Uninit();
    }

    PK_DBG(" Done\n");

    return 0;

}


FLASHLIGHT_FUNCTION_STRUCT    constantFlashlightFunc=
{
    constant_flashlight_open,
    constant_flashlight_release,
    constant_flashlight_ioctl
};


MUINT32 constantFlashlightInit(PFLASHLIGHT_FUNCTION_STRUCT *pfFunc)
{
    if (pfFunc != NULL)
    {
        *pfFunc = &constantFlashlightFunc;
    }
    return 0;
}



/* LED flash control for high current capture mode*/
ssize_t strobe_VDIrq(void)
{

    return 0;
}

EXPORT_SYMBOL(strobe_VDIrq);


