
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <asm/io.h>

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"

#include "hm2056yuv_Sensor.h"
#include "hm2056yuv_Camera_Sensor_para.h"
#include "hm2056yuv_CameraCustomized.h"


#define HM2056YUV_DEBUG
#ifdef HM2056YUV_DEBUG
#define SENSORDB printk
#else
#define SENSORDB(x,...)
#endif

static void HM2056_Set_Video_Frame_Rate(kal_uint32 frame_rate);
extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);
void HM2056_write_cmos_sensor(kal_uint16 addr, kal_uint8 para)
{
    char puSendCmd[3] = {(char)((addr & 0xFF00)>>8), (char)(addr & 0xFF) , (char)(para & 0xFF)};
	
	iWriteRegI2C(puSendCmd , 3,HM2056_WRITE_ID);

}
kal_uint8 HM2056_read_cmos_sensor(kal_uint16 addr)
{
	kal_uint8 get_byte=0;
    char puSendCmd[2] = {(char)((addr & 0xFF00)>>8), (char)(addr & 0xFF)};
	iReadRegI2C(puSendCmd , 2, (u8*)&get_byte,1,HM2056_WRITE_ID);
	
    return get_byte;
}


#define	HM2056_LIMIT_EXPOSURE_LINES				(1253)
#define	HM2056_VIDEO_NORMALMODE_30FRAME_RATE       (30)
#define	HM2056_VIDEO_NORMALMODE_FRAME_RATE         (15)
#define	HM2056_VIDEO_NIGHTMODE_FRAME_RATE          (7.5)
#define BANDING50_30HZ
/* Global Valuable */

static kal_uint32 zoom_factor = 0; 
static kal_uint8 HM2056_exposure_line_h = 0, HM2056_exposure_line_l = 0,HM2056_extra_exposure_line_h = 0, HM2056_extra_exposure_line_l = 0;

static kal_bool HM2056_gPVmode = KAL_TRUE; //PV size or Full size
static kal_bool HM2056_VEDIO_encode_mode = KAL_FALSE; //Picture(Jpeg) or Video(Mpeg4)
static kal_bool HM2056_sensor_cap_state = KAL_FALSE; //Preview or Capture

static kal_uint16 HM2056_dummy_pixels=0, HM2056_dummy_lines=0;

static kal_uint16 HM2056_exposure_lines=0, HM2056_extra_exposure_lines = 0;


static kal_int8 HM2056_DELAY_AFTER_PREVIEW = -1;

static kal_uint8 HM2056_Banding_setting = AE_FLICKER_MODE_50HZ;  //Wonder add

/****** OVT 6-18******/
static kal_uint16 HM2056_Capture_Max_Gain16= 6*16;
static kal_uint16 HM2056_Capture_Gain16=0 ;    
static kal_uint16 HM2056_Capture_Shutter=0;
static kal_uint16 HM2056_Capture_Extra_Lines=0;

static kal_uint16  HM2056_PV_Dummy_Pixels =0, HM2056_Capture_Dummy_Pixels =0, HM2056_Capture_Dummy_Lines =0;
static kal_uint16  HM2056_PV_Gain16 = 0;
static kal_uint16  HM2056_PV_Shutter = 0;
static kal_uint16  HM2056_PV_Extra_Lines = 0;

kal_uint16 HM2056_sensor_gain_base=0,HM2056_FAC_SENSOR_REG=0,HM2056_iHM2056_Mode=0,HM2056_max_exposure_lines=0;
kal_uint32 HM2056_capture_pclk_in_M=480,HM2056_preview_pclk_in_M=480,HM2056_PV_dummy_pixels=0,HM2056_PV_dummy_lines=0,HM2056_isp_master_clock=0;

static kal_uint32  HM2056_sensor_pclk=480;	//390

kal_uint32 HM2056_pv_HM2056_exposure_lines = 0x0249f0,HM2056_cp_HM2056_exposure_lines=0;
kal_uint8 HM2056_HV_Mirror;
kal_uint8 HM2056_Sleep_Mode;

UINT8 HM2056_PixelClockDivider=0;

//SENSOR_REG_STRUCT HM2056SensorCCT[FACTORY_END_ADDR]=CAMERA_SENSOR_CCT_DEFAULT_VALUE;
//SENSOR_REG_STRUCT HM2056SensorReg[ENGINEER_END]=CAMERA_SENSOR_REG_DEFAULT_VALUE;
//	camera_para.SENSOR.cct	SensorCCT	=> SensorCCT
//	camera_para.SENSOR.reg	SensorReg
MSDK_SENSOR_CONFIG_STRUCT HM2056SensorConfigData;
 
static DEFINE_SPINLOCK(hm2056_drv_lock);
static struct
{
	kal_uint32  sceneMode;
	kal_uint16 	iso;
	kal_uint32  iShutter;
	kal_uint16 	iWB;
} HM2056Sensor; 


void HM2056_set_dummy(kal_uint16 pixels, kal_uint16 lines)
{
}    /* HM2056_set_dummy */

kal_uint16 HM2056_read_HM2056_gain(void)
{
	return 0;
}  /* HM2056_read_HM2056_gain */


void HM2056_write_HM2056_gain(kal_uint16 gain)
{
}  /* HM2056_write_HM2056_gain */

void HM2056_set_isp_driving_current(kal_uint8 current_value)
{
}
/**********************0915***********************/
void HM2056_get_exposure_gain()
{
  kal_uint32 again = 0, dgain = 0, exp = 0,iso;

  exp  = HM2056_read_cmos_sensor(0x0016); 
  exp += HM2056_read_cmos_sensor(0x0015) << 8 ; 

  spin_lock(&hm2056_drv_lock);
  HM2056Sensor.iShutter= exp * 1600 / 24 ;
  //HM2056Sensor.iShutter= exp * 5 / 2 ;
  spin_unlock(&hm2056_drv_lock);

  again = HM2056_read_cmos_sensor(0x0018); //A gain 
  //again = 1<< again;
  dgain = HM2056_read_cmos_sensor(0x001D); //D gain 
    
  //iso= (dgain * again) >> 6; 
  spin_lock(&hm2056_drv_lock);
  if (again==0 && dgain < 0x80) 
  {
     HM2056Sensor.iso = AE_ISO_100;
  }
  else if (again == 1 && dgain < 0x80) 
  {
     HM2056Sensor.iso = AE_ISO_200;
  }
  else if (again == 2 && dgain < 0x80)
  {
     HM2056Sensor.iso = AE_ISO_400;
  }
   
  else if (again == 3 && dgain <= 0x80)
  {
     HM2056Sensor.iso = AE_ISO_800;
  }
  else 
  {
     HM2056Sensor.iso = AE_ISO_1600;
  }
  
  spin_unlock(&hm2056_drv_lock);

}
/**********************0915***********************/
/*************************************************************************
* FUNCTION
*	HM2056_night_mode
*
* DESCRIPTION
*	This function night mode of HM2056.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void HM2056_night_mode(kal_bool enable)
{
	if (enable) 		/* Night Mode */
	{
		/* HM2056 night mode enable. */
		if (HM2056_VEDIO_encode_mode == KAL_TRUE)	/* Video */
		{
			HM2056_Set_Video_Frame_Rate(200); 
		}
		else 										/* Camera */
		{
			HM2056_Set_Video_Frame_Rate(200); 
		}			
#if 0    
		HM2056_write_cmos_sensor(0x02E0,0x00);	// 00 for Night Mode, By Brandon/20110208
		HM2056_write_cmos_sensor(0x0481,0x06);	// 06 for Night Mode, By Brandon/20110208
		HM2056_write_cmos_sensor(0x04B1,0x88);	// 88 for Night Mode, By Brandon/20110208
		HM2056_write_cmos_sensor(0x04B4,0x20);	// 20 for Night Mode, By Brandon/20110208
		
		HM2056_write_cmos_sensor(0x0000,0xFF);
		HM2056_write_cmos_sensor(0x0100,0xFF);  
#endif
	}
	else  				/* Normal Mode */
	{
		/* HM2056 night mode disable. */
		if (HM2056_VEDIO_encode_mode == KAL_TRUE)	/* Video */
		{
            HM2056_Set_Video_Frame_Rate(200); 
				
		}
		else										/* Camera */
		{
    		    HM2056_Set_Video_Frame_Rate(200); 
			
		}		
#if 0		
		HM2056_write_cmos_sensor(0x02E0,0x02);	//06->02, By Brandon/20110129
		HM2056_write_cmos_sensor(0x0481,0x08);	//06->08, By Brandon/20110129
		HM2056_write_cmos_sensor(0x04B1,0x00);
		HM2056_write_cmos_sensor(0x04B4,0x00);
		HM2056_write_cmos_sensor(0x0000,0xFF);
		HM2056_write_cmos_sensor(0x0100,0xFF);
#endif
	}

}	/* HM2056_night_mode */

/*************************************************************************
* FUNCTION
*	HM2056_set_mirror
*
* DESCRIPTION
*	This function mirror, flip or mirror & flip the sensor output image.
*
*	IMPORTANT NOTICE: For some sensor, it need re-set the output order Y1CbY2Cr after
*	mirror or flip.
*
* PARAMETERS
*	1. kal_uint16 : horizontal mirror or vertical flip direction.
*
* RETURNS
*	None
*
*************************************************************************/
static void HM2056_set_mirror(kal_uint16 image_mirror)
{
	
	switch (image_mirror)
	{
	case IMAGE_NORMAL:
		HM2056_write_cmos_sensor(0x0006, 0x00);     //0x02
		HM2056_write_cmos_sensor(0x0000, 0x01);   
		HM2056_write_cmos_sensor(0x0530,0x02);
		break;
	case IMAGE_H_MIRROR:
		HM2056_write_cmos_sensor(0x0006, 0x02);     
		HM2056_write_cmos_sensor(0x0000, 0x01);  
		HM2056_write_cmos_sensor(0x0530,0x03);
		break;
	case IMAGE_V_MIRROR:
		HM2056_write_cmos_sensor(0x0006, 0x01);     
		HM2056_write_cmos_sensor(0x0000, 0x01);  
		HM2056_write_cmos_sensor(0x0530,0x00);
		break;
	case IMAGE_HV_MIRROR:
		HM2056_write_cmos_sensor(0x0006, 0x03);    //0x01    
		HM2056_write_cmos_sensor(0x0000, 0x01); 
		HM2056_write_cmos_sensor(0x0530,0x01);
		break;
	default:
		ASSERT(0);
	}
}

/*************************************************************************
* FUNCTION
*	HM2056_awb_enable
*
* DESCRIPTION
*	This function enable or disable the awb (Auto White Balance).
*
* PARAMETERS
*	1. kal_bool : KAL_TRUE - enable awb, KAL_FALSE - disable awb.
*
* RETURNS
*	kal_bool : It means set awb right or not.
*
*************************************************************************/
static kal_bool HM2056_awb_enable(kal_bool enalbe)
{	 
	kal_uint16 temp_AWB_reg = 0;
	
	temp_AWB_reg = HM2056_read_cmos_sensor(0x0380);
	
	if (enalbe)
	{
		HM2056_write_cmos_sensor(0x0380, (temp_AWB_reg | 0x02));
	}
	else
	{
		HM2056_write_cmos_sensor(0x0380, (temp_AWB_reg& ~0x02));		
	}

	HM2056_write_cmos_sensor(0x0000,0xFF);	
	HM2056_write_cmos_sensor(0x0100,0xFF);
	HM2056_write_cmos_sensor(0x0101,0xFF);
	
	return KAL_TRUE;
}

/*************************************************************************
* FUNCTION
*	HM2056_ae_enable
*
* DESCRIPTION
*	This function enable or disable the ae (Auto Exposure).
*
* PARAMETERS
*	1. kal_bool : KAL_TRUE - enable ae, KAL_FALSE - disable awb.
*
* RETURNS
*	kal_bool : It means set awb right or not.
*
*************************************************************************/
static kal_bool HM2056_ae_enable(kal_bool enalbe)
{	 
	kal_uint16 temp_AE_reg = 0;


	temp_AE_reg = HM2056_read_cmos_sensor(0x0380);
	
	if (enalbe)	  
	{
		HM2056_write_cmos_sensor(0x0380, (temp_AE_reg| 0x01));	/* Turn ON AEC/AGC*/
	}
	else
	{
		HM2056_write_cmos_sensor(0x0380, (temp_AE_reg&(~0x01))); /* Turn OFF AEC/AGC*/
	}

	HM2056_write_cmos_sensor(0x0000,0xFF);	
	HM2056_write_cmos_sensor(0x0100,0xFF);
	HM2056_write_cmos_sensor(0x0101,0xFF);

	return KAL_TRUE;
}

/*************************************************************************
* FUNCTION
*	HM2056_YUV_sensor_initial_setting
*
* DESCRIPTION
*	This function apply all of the initial setting to sensor.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
*************************************************************************/
static void HM2056_YUV_sensor_initial_setting(void)
{
		/* HM2056 initial setting date: 2010.12.28. 
			 2010.12.30 updated by Troy
		*/
HM2056_write_cmos_sensor(0x0022,0x00);
HM2056_write_cmos_sensor(0x0020,0x00);
HM2056_write_cmos_sensor(0x0025,0x00); 
HM2056_write_cmos_sensor(0x0026,0x87); 
HM2056_write_cmos_sensor(0x0027,0x18);
HM2056_write_cmos_sensor(0x0028,0xC0); 
HM2056_write_cmos_sensor(0x002A,0x1f); 
HM2056_write_cmos_sensor(0x002C,0x0a); 
HM2056_write_cmos_sensor(0x0004,0x10);
HM2056_write_cmos_sensor(0x0006,0x00);
HM2056_write_cmos_sensor(0x000D,0x00);
HM2056_write_cmos_sensor(0x000E,0x00);
HM2056_write_cmos_sensor(0x000F,0x00);
HM2056_write_cmos_sensor(0x0011,0x02);
HM2056_write_cmos_sensor(0x0012,0x04);
HM2056_write_cmos_sensor(0x0013,0x00);
HM2056_write_cmos_sensor(0x0015,0x02);
HM2056_write_cmos_sensor(0x0016,0x80); 
HM2056_write_cmos_sensor(0x0018,0x00);  
HM2056_write_cmos_sensor(0x001D,0x40);  
HM2056_write_cmos_sensor(0x0040,0x20);
HM2056_write_cmos_sensor(0x0053,0x0A);
HM2056_write_cmos_sensor(0x0044,0x06);
HM2056_write_cmos_sensor(0x0046,0xD8);
HM2056_write_cmos_sensor(0x004A,0x0A);
HM2056_write_cmos_sensor(0x004B,0x72);
HM2056_write_cmos_sensor(0x0075,0x01);
HM2056_write_cmos_sensor(0x0070,0x5F);
HM2056_write_cmos_sensor(0x0071,0xAB);
HM2056_write_cmos_sensor(0x0072,0x55);
HM2056_write_cmos_sensor(0x0073,0x50);
HM2056_write_cmos_sensor(0x0077,0x04);
HM2056_write_cmos_sensor(0x0080,0xC8);
HM2056_write_cmos_sensor(0x0082,0xE2);
HM2056_write_cmos_sensor(0x0083,0xF0);
HM2056_write_cmos_sensor(0x0085,0x11);
HM2056_write_cmos_sensor(0x0086,0x02);
HM2056_write_cmos_sensor(0x0087,0x80);
HM2056_write_cmos_sensor(0x0088,0x6C);
HM2056_write_cmos_sensor(0x0089,0x2E);
HM2056_write_cmos_sensor(0x008A,0x6D);  
HM2056_write_cmos_sensor(0x008D,0x20);
HM2056_write_cmos_sensor(0x0090,0x00);
HM2056_write_cmos_sensor(0x0091,0x10);
HM2056_write_cmos_sensor(0x0092,0x11);
HM2056_write_cmos_sensor(0x0093,0x12);
HM2056_write_cmos_sensor(0x0094,0x16);
HM2056_write_cmos_sensor(0x0095,0x08);
HM2056_write_cmos_sensor(0x0096,0x00);
HM2056_write_cmos_sensor(0x0097,0x10);
HM2056_write_cmos_sensor(0x0098,0x11);
HM2056_write_cmos_sensor(0x0099,0x12);
HM2056_write_cmos_sensor(0x009A,0x16);
HM2056_write_cmos_sensor(0x009B,0x34);
HM2056_write_cmos_sensor(0x00A0,0x00);
HM2056_write_cmos_sensor(0x00A1,0x04);
HM2056_write_cmos_sensor(0x011F,0xFF);
HM2056_write_cmos_sensor(0x0120,0x37);
HM2056_write_cmos_sensor(0x0121,0x83);//Bit[3] enable AWB debug 83->8b
HM2056_write_cmos_sensor(0x0122,0x79);//140902 for Arima E1.2
HM2056_write_cmos_sensor(0x0123,0xC2);
HM2056_write_cmos_sensor(0x0124,0xDE);
HM2056_write_cmos_sensor(0x0125,0xDF);//0xD5_off,0xDF_ON
HM2056_write_cmos_sensor(0x0126,0x70);
HM2056_write_cmos_sensor(0x0128,0x1F);
HM2056_write_cmos_sensor(0x0132,0x13);	//140902 for Arima E1.2
HM2056_write_cmos_sensor(0x0136,0x0A);
HM2056_write_cmos_sensor(0x0131,0xBC);//0xB8_off,0xBC_ON
HM2056_write_cmos_sensor(0x0140,0x14);
HM2056_write_cmos_sensor(0x0141,0x0A);
HM2056_write_cmos_sensor(0x0142,0x14);
HM2056_write_cmos_sensor(0x0143,0x0A);
HM2056_write_cmos_sensor(0x0144,0x04);
HM2056_write_cmos_sensor(0x0145,0x00);
HM2056_write_cmos_sensor(0x0146,0x20);
HM2056_write_cmos_sensor(0x0147,0x0A);
HM2056_write_cmos_sensor(0x0148,0x10);
HM2056_write_cmos_sensor(0x0149,0x0C);
HM2056_write_cmos_sensor(0x014A,0x80);
HM2056_write_cmos_sensor(0x014B,0x80);
HM2056_write_cmos_sensor(0x014C,0x2E);
HM2056_write_cmos_sensor(0x014D,0x2E);
HM2056_write_cmos_sensor(0x014E,0x05);
HM2056_write_cmos_sensor(0x014F,0x05);
HM2056_write_cmos_sensor(0x0150,0x0D);
HM2056_write_cmos_sensor(0x0155,0x00);
HM2056_write_cmos_sensor(0x0156,0x10);
HM2056_write_cmos_sensor(0x0157,0x0A);
HM2056_write_cmos_sensor(0x0158,0x0A);
HM2056_write_cmos_sensor(0x0159,0x0A);
HM2056_write_cmos_sensor(0x015A,0x05);
HM2056_write_cmos_sensor(0x015B,0x05);
HM2056_write_cmos_sensor(0x015C,0x05);
HM2056_write_cmos_sensor(0x015D,0x05);
HM2056_write_cmos_sensor(0x015E,0x08);
HM2056_write_cmos_sensor(0x015F,0xFF);
HM2056_write_cmos_sensor(0x0160,0x50);
HM2056_write_cmos_sensor(0x0161,0x20);
HM2056_write_cmos_sensor(0x0162,0x14);
HM2056_write_cmos_sensor(0x0163,0x0A);
HM2056_write_cmos_sensor(0x0164,0x10);
HM2056_write_cmos_sensor(0x0165,0x08);
HM2056_write_cmos_sensor(0x0166,0x0A);
HM2056_write_cmos_sensor(0x018C,0x24);
HM2056_write_cmos_sensor(0x018D,0x04);
HM2056_write_cmos_sensor(0x018E,0x00);
HM2056_write_cmos_sensor(0x018F,0x11);
HM2056_write_cmos_sensor(0x0190,0x87);
HM2056_write_cmos_sensor(0x0191,0x47);
HM2056_write_cmos_sensor(0x0192,0x50);
HM2056_write_cmos_sensor(0x0193,0x64);
HM2056_write_cmos_sensor(0x0194,0x32);
HM2056_write_cmos_sensor(0x0195,0xc8);
HM2056_write_cmos_sensor(0x0196,0x96);
HM2056_write_cmos_sensor(0x0197,0x64);
HM2056_write_cmos_sensor(0x0198,0x32);
HM2056_write_cmos_sensor(0x0199,0x14);
HM2056_write_cmos_sensor(0x019A,0x20);
HM2056_write_cmos_sensor(0x019B,0x14);
HM2056_write_cmos_sensor(0x01BA,0x10);
HM2056_write_cmos_sensor(0x01BB,0x04);
HM2056_write_cmos_sensor(0x01D8,0x40);
HM2056_write_cmos_sensor(0x01DE,0x60);
HM2056_write_cmos_sensor(0x01E4,0x10);//
HM2056_write_cmos_sensor(0x01E5,0x10);//
HM2056_write_cmos_sensor(0x01F2,0x0C);
HM2056_write_cmos_sensor(0x01F3,0x14);
HM2056_write_cmos_sensor(0x01F8,0x04);
HM2056_write_cmos_sensor(0x01F9,0x0C);
HM2056_write_cmos_sensor(0x01FE,0x02);
HM2056_write_cmos_sensor(0x01FF,0x04);
HM2056_write_cmos_sensor(0x0220,0x00);//add start
HM2056_write_cmos_sensor(0x0221,0xA0);
HM2056_write_cmos_sensor(0x0222,0x00);
HM2056_write_cmos_sensor(0x0223,0x80);
HM2056_write_cmos_sensor(0x0224,0x8E);
HM2056_write_cmos_sensor(0x0225,0x00);
HM2056_write_cmos_sensor(0x0226,0x88);
HM2056_write_cmos_sensor(0x022A,0x88);
HM2056_write_cmos_sensor(0x022B,0x00);
HM2056_write_cmos_sensor(0x022C,0x88);
HM2056_write_cmos_sensor(0x022D,0x13);
HM2056_write_cmos_sensor(0x022E,0x0B);
HM2056_write_cmos_sensor(0x022F,0x13);
HM2056_write_cmos_sensor(0x0230,0x0B);
HM2056_write_cmos_sensor(0x0233,0x13);
HM2056_write_cmos_sensor(0x0234,0x0B);
HM2056_write_cmos_sensor(0x0235,0x28);
HM2056_write_cmos_sensor(0x0236,0x03);
HM2056_write_cmos_sensor(0x0237,0x28);
HM2056_write_cmos_sensor(0x0238,0x03);
HM2056_write_cmos_sensor(0x023B,0x28);
HM2056_write_cmos_sensor(0x023C,0x03);
HM2056_write_cmos_sensor(0x023D,0x5C);
HM2056_write_cmos_sensor(0x023E,0x02);
HM2056_write_cmos_sensor(0x023F,0x5C);
HM2056_write_cmos_sensor(0x0240,0x02);
HM2056_write_cmos_sensor(0x0243,0x5C);
HM2056_write_cmos_sensor(0x0244,0x02);
HM2056_write_cmos_sensor(0x0251,0x0E);//add end
HM2056_write_cmos_sensor(0x0252,0x0E);
HM2056_write_cmos_sensor(0x0280,0x0A);
HM2056_write_cmos_sensor(0x0282,0x14);
HM2056_write_cmos_sensor(0x0284,0x2A);
HM2056_write_cmos_sensor(0x0286,0x50);
HM2056_write_cmos_sensor(0x0288,0x60);
HM2056_write_cmos_sensor(0x028A,0x6D);
HM2056_write_cmos_sensor(0x028C,0x79);
HM2056_write_cmos_sensor(0x028E,0x82);
HM2056_write_cmos_sensor(0x0290,0x8A);
HM2056_write_cmos_sensor(0x0292,0x91);
HM2056_write_cmos_sensor(0x0294,0x9C);
HM2056_write_cmos_sensor(0x0296,0xA7);
HM2056_write_cmos_sensor(0x0298,0xBA);
HM2056_write_cmos_sensor(0x029A,0xCD);
HM2056_write_cmos_sensor(0x029C,0xE0);
HM2056_write_cmos_sensor(0x029E,0x2D);
HM2056_write_cmos_sensor(0x02A0,0x08);     
HM2056_write_cmos_sensor(0x02E0,0x04);//08->04 141201
HM2056_write_cmos_sensor(0x02C0,0x7E);//20141218_R1
HM2056_write_cmos_sensor(0x02C1,0x01);
HM2056_write_cmos_sensor(0x02C2,0x97);
HM2056_write_cmos_sensor(0x02C3,0x07);
HM2056_write_cmos_sensor(0x02C4,0xEC);
HM2056_write_cmos_sensor(0x02C5,0x07);
HM2056_write_cmos_sensor(0x02C6,0xC4);
HM2056_write_cmos_sensor(0x02C7,0x07);
HM2056_write_cmos_sensor(0x02C8,0x79);
HM2056_write_cmos_sensor(0x02C9,0x01);
HM2056_write_cmos_sensor(0x02CA,0xC4);
HM2056_write_cmos_sensor(0x02CB,0x07);
HM2056_write_cmos_sensor(0x02CC,0xF7);
HM2056_write_cmos_sensor(0x02CD,0x07);
HM2056_write_cmos_sensor(0x02CE,0x3B);
HM2056_write_cmos_sensor(0x02CF,0x07);
HM2056_write_cmos_sensor(0x02D0,0xCF);
HM2056_write_cmos_sensor(0x02D1,0x01);
HM2056_write_cmos_sensor(0x0302,0x00);
HM2056_write_cmos_sensor(0x0303,0x00);
HM2056_write_cmos_sensor(0x0304,0x00);
HM2056_write_cmos_sensor(0x02F0,0x2D);
HM2056_write_cmos_sensor(0x02F1,0x00);
HM2056_write_cmos_sensor(0x02F2,0x73);
HM2056_write_cmos_sensor(0x02F3,0x00);
HM2056_write_cmos_sensor(0x02F4,0xDE);
HM2056_write_cmos_sensor(0x02F5,0x07);
HM2056_write_cmos_sensor(0x02F6,0x84);
HM2056_write_cmos_sensor(0x02F7,0x07);
HM2056_write_cmos_sensor(0x02F8,0xB8);
HM2056_write_cmos_sensor(0x02F9,0x00);
HM2056_write_cmos_sensor(0x02FA,0xF7);
HM2056_write_cmos_sensor(0x02FB,0x07);
HM2056_write_cmos_sensor(0x02FC,0x1F);
HM2056_write_cmos_sensor(0x02FD,0x07);
HM2056_write_cmos_sensor(0x02FE,0x64);
HM2056_write_cmos_sensor(0x02FF,0x07);
HM2056_write_cmos_sensor(0x0300,0x7D);
HM2056_write_cmos_sensor(0x0301,0x01);
HM2056_write_cmos_sensor(0x0305,0x7A);
HM2056_write_cmos_sensor(0x0306,0x00);
HM2056_write_cmos_sensor(0x0307,0x05);//20141218_R1
HM2056_write_cmos_sensor(0x032D,0x00);
HM2056_write_cmos_sensor(0x032E,0x01);
HM2056_write_cmos_sensor(0x032F,0x00);
HM2056_write_cmos_sensor(0x0330,0x01);
HM2056_write_cmos_sensor(0x0331,0x00);
HM2056_write_cmos_sensor(0x0332,0x01);
HM2056_write_cmos_sensor(0x0333,0x04);
HM2056_write_cmos_sensor(0x0334,0x00);
HM2056_write_cmos_sensor(0x0335,0x84);
HM2056_write_cmos_sensor(0x0336,0x00);
HM2056_write_cmos_sensor(0x0337,0x01);
HM2056_write_cmos_sensor(0x0338,0x00);
HM2056_write_cmos_sensor(0x0339,0x01);
HM2056_write_cmos_sensor(0x033A,0x00);
HM2056_write_cmos_sensor(0x033B,0x01);
HM2056_write_cmos_sensor(0x0340,0x30);
HM2056_write_cmos_sensor(0x0341,0x44);
HM2056_write_cmos_sensor(0x0342,0x4A);
HM2056_write_cmos_sensor(0x0343,0x3A);//140904 for Arima E1.2 3A->3C
HM2056_write_cmos_sensor(0x0344,0x74);
HM2056_write_cmos_sensor(0x0345,0x4F);//4F->42 141203 for keep face color tone 
HM2056_write_cmos_sensor(0x0346,0x67);
HM2056_write_cmos_sensor(0x0347,0x5C);
HM2056_write_cmos_sensor(0x0348,0x59);
HM2056_write_cmos_sensor(0x0349,0x67);
HM2056_write_cmos_sensor(0x034A,0x4D);
HM2056_write_cmos_sensor(0x034B,0x6E);
HM2056_write_cmos_sensor(0x034C,0x44);
HM2056_write_cmos_sensor(0x0350,0x80);
HM2056_write_cmos_sensor(0x0351,0x80);
HM2056_write_cmos_sensor(0x0352,0x18);
HM2056_write_cmos_sensor(0x0353,0x18);
HM2056_write_cmos_sensor(0x0354,0x6E);
HM2056_write_cmos_sensor(0x0355,0x4A);
HM2056_write_cmos_sensor(0x0356,0x6c);// 86->6E->60->6c  x1.5
HM2056_write_cmos_sensor(0x0357,0xCA);
HM2056_write_cmos_sensor(0x0358,0x06);
HM2056_write_cmos_sensor(0x035A,0x06);
HM2056_write_cmos_sensor(0x035B,0xA0);
HM2056_write_cmos_sensor(0x035C,0x73);
HM2056_write_cmos_sensor(0x035D,0x5A);  
HM2056_write_cmos_sensor(0x035E,0xC6);
HM2056_write_cmos_sensor(0x035F,0xA0);
HM2056_write_cmos_sensor(0x0360,0x02);
HM2056_write_cmos_sensor(0x0361,0x18);
HM2056_write_cmos_sensor(0x0362,0x80);
HM2056_write_cmos_sensor(0x0363,0x6C);
HM2056_write_cmos_sensor(0x0364,0x00);
HM2056_write_cmos_sensor(0x0365,0xF0);
HM2056_write_cmos_sensor(0x0366,0x20);
HM2056_write_cmos_sensor(0x0367,0x0C);
HM2056_write_cmos_sensor(0x0369,0x00);
HM2056_write_cmos_sensor(0x036A,0x10);
HM2056_write_cmos_sensor(0x036B,0x10);
HM2056_write_cmos_sensor(0x036E,0x20);
HM2056_write_cmos_sensor(0x036F,0x00);
HM2056_write_cmos_sensor(0x0370,0x10);
HM2056_write_cmos_sensor(0x0371,0x18);
HM2056_write_cmos_sensor(0x0372,0x0C);
HM2056_write_cmos_sensor(0x0373,0x38);
HM2056_write_cmos_sensor(0x0374,0x3A);
HM2056_write_cmos_sensor(0x0375,0x13);
HM2056_write_cmos_sensor(0x0376,0x22);
HM2056_write_cmos_sensor(0x0380,0xFF);
HM2056_write_cmos_sensor(0x0381,0x46);  
HM2056_write_cmos_sensor(0x0382,0x32);
HM2056_write_cmos_sensor(0x038A,0x40);
HM2056_write_cmos_sensor(0x038B,0x08);
HM2056_write_cmos_sensor(0x038C,0xC1);
HM2056_write_cmos_sensor(0x038E,0x3C); 
HM2056_write_cmos_sensor(0x038F,0x09);
HM2056_write_cmos_sensor(0x0390,0xB0);
HM2056_write_cmos_sensor(0x0391,0x05);  
HM2056_write_cmos_sensor(0x0393,0x80);
HM2056_write_cmos_sensor(0x0395,0x21);
HM2056_write_cmos_sensor(0x0398,0x02);
HM2056_write_cmos_sensor(0x0399,0x74);
HM2056_write_cmos_sensor(0x039A,0x03);
HM2056_write_cmos_sensor(0x039B,0x11);
HM2056_write_cmos_sensor(0x039C,0x03);
HM2056_write_cmos_sensor(0x039D,0xAE);
HM2056_write_cmos_sensor(0x039E,0x04);
HM2056_write_cmos_sensor(0x039F,0xE8);
HM2056_write_cmos_sensor(0x03A0,0x06);
HM2056_write_cmos_sensor(0x03A1,0x22);
HM2056_write_cmos_sensor(0x03A2,0x07);
HM2056_write_cmos_sensor(0x03A3,0x5C);
HM2056_write_cmos_sensor(0x03A4,0x09);
HM2056_write_cmos_sensor(0x03A5,0xD0);
HM2056_write_cmos_sensor(0x03A6,0x0C);
HM2056_write_cmos_sensor(0x03A7,0x0E);
HM2056_write_cmos_sensor(0x03A8,0x10);
HM2056_write_cmos_sensor(0x03A9,0x18);
HM2056_write_cmos_sensor(0x03AA,0x20);
HM2056_write_cmos_sensor(0x03AB,0x28);
HM2056_write_cmos_sensor(0x03AC,0x1E);
HM2056_write_cmos_sensor(0x03AD,0x1A);
HM2056_write_cmos_sensor(0x03AE,0x13);
HM2056_write_cmos_sensor(0x03AF,0x0C);
HM2056_write_cmos_sensor(0x03B0,0x0B);
HM2056_write_cmos_sensor(0x03B1,0x09);
HM2056_write_cmos_sensor(0x03B3,0x10);
HM2056_write_cmos_sensor(0x03B4,0x00);
HM2056_write_cmos_sensor(0x03B5,0x10);
HM2056_write_cmos_sensor(0x03B6,0x00);
HM2056_write_cmos_sensor(0x03B7,0xEA);
HM2056_write_cmos_sensor(0x03B8,0x00);
HM2056_write_cmos_sensor(0x03B9,0x3A);
HM2056_write_cmos_sensor(0x03BA,0x01);
HM2056_write_cmos_sensor(0x03BB,0x9F);
HM2056_write_cmos_sensor(0x03BC,0xCF);
HM2056_write_cmos_sensor(0x03BD,0xE7);
HM2056_write_cmos_sensor(0x03BE,0xF3);
HM2056_write_cmos_sensor(0x03BF,0x01);
HM2056_write_cmos_sensor(0x03D0,0xF8);
HM2056_write_cmos_sensor(0x03E0,0x04);
HM2056_write_cmos_sensor(0x03E1,0x01);
HM2056_write_cmos_sensor(0x03E2,0x04);
HM2056_write_cmos_sensor(0x03E4,0x20);//10->20
HM2056_write_cmos_sensor(0x03E5,0x23);//12->23
HM2056_write_cmos_sensor(0x03E6,0x00);
HM2056_write_cmos_sensor(0x03E8,0x31);//21->31
HM2056_write_cmos_sensor(0x03E9,0x33);//23->33
HM2056_write_cmos_sensor(0x03EA,0x01);
HM2056_write_cmos_sensor(0x03EC,0x20);//21->20
HM2056_write_cmos_sensor(0x03ED,0x23);
HM2056_write_cmos_sensor(0x03EE,0x00);//01->00
HM2056_write_cmos_sensor(0x03F0,0x04);//20->04
HM2056_write_cmos_sensor(0x03F1,0x01);//22->01
HM2056_write_cmos_sensor(0x03F2,0x04);//00->04
HM2056_write_cmos_sensor(0x0420,0x84);
HM2056_write_cmos_sensor(0x0421,0x00);
HM2056_write_cmos_sensor(0x0422,0x00);
HM2056_write_cmos_sensor(0x0423,0x83);
HM2056_write_cmos_sensor(0x0430,0x08);
HM2056_write_cmos_sensor(0x0431,0x28);
HM2056_write_cmos_sensor(0x0432,0x10);
HM2056_write_cmos_sensor(0x0433,0x08);
HM2056_write_cmos_sensor(0x0435,0x0C);
HM2056_write_cmos_sensor(0x0450,0xFF);
HM2056_write_cmos_sensor(0x0451,0xE8);
HM2056_write_cmos_sensor(0x0452,0xC4);  
HM2056_write_cmos_sensor(0x0453,0x88);  
HM2056_write_cmos_sensor(0x0454,0x00);
HM2056_write_cmos_sensor(0x0458,0x98);
HM2056_write_cmos_sensor(0x0459,0x03);
HM2056_write_cmos_sensor(0x045A,0x00);
HM2056_write_cmos_sensor(0x045B,0x70);//28
HM2056_write_cmos_sensor(0x045C,0x00);
HM2056_write_cmos_sensor(0x045D,0xB0);//68
HM2056_write_cmos_sensor(0x0466,0x14);//AE&AWB speed [fast] 0~f [slow]
HM2056_write_cmos_sensor(0x047A,0x00);
HM2056_write_cmos_sensor(0x047B,0x00);
HM2056_write_cmos_sensor(0x0480,0x5a);//0x52_140924
HM2056_write_cmos_sensor(0x0481,0x08);//08->09->0D->08
HM2056_write_cmos_sensor(0x0482,0x0A);//0C->0A 141201
HM2056_write_cmos_sensor(0x04B0,0x50);
HM2056_write_cmos_sensor(0x04B6,0x30);
HM2056_write_cmos_sensor(0x04B9,0x10);
HM2056_write_cmos_sensor(0x04B3,0x10);
//HM2056_write_cmos_sensor(0x04B1,0x8E);
//HM2056_write_cmos_sensor(0x04B4,0x20);
HM2056_write_cmos_sensor(0x04B1,0x00);
HM2056_write_cmos_sensor(0x04B4,0x00);
HM2056_write_cmos_sensor(0x0540,0x00);
HM2056_write_cmos_sensor(0x0541,0x9B);
HM2056_write_cmos_sensor(0x0542,0x00);
HM2056_write_cmos_sensor(0x0543,0xBA);
HM2056_write_cmos_sensor(0x0580,0x01);
HM2056_write_cmos_sensor(0x0581,0x01);//F->07->01
HM2056_write_cmos_sensor(0x0582,0x04);
HM2056_write_cmos_sensor(0x0594,0x00);
HM2056_write_cmos_sensor(0x0595,0x20);	//140904 for Arima E1.2 0x20
HM2056_write_cmos_sensor(0x05A9,0x03);
HM2056_write_cmos_sensor(0x05AA,0x40);
HM2056_write_cmos_sensor(0x05AB,0x80);
HM2056_write_cmos_sensor(0x05AC,0x0A);
HM2056_write_cmos_sensor(0x05AD,0x10);
HM2056_write_cmos_sensor(0x05AE,0x0C);
HM2056_write_cmos_sensor(0x05AF,0x0C);
HM2056_write_cmos_sensor(0x05B0,0x03);
HM2056_write_cmos_sensor(0x05B1,0x03);
HM2056_write_cmos_sensor(0x05B2,0x1C);
HM2056_write_cmos_sensor(0x05B3,0x02);
HM2056_write_cmos_sensor(0x05B4,0x00);
HM2056_write_cmos_sensor(0x05B5,0x0C);
HM2056_write_cmos_sensor(0x05B8,0x80);
HM2056_write_cmos_sensor(0x05B9,0x32);
HM2056_write_cmos_sensor(0x05BA,0x00);
HM2056_write_cmos_sensor(0x05BB,0x80);
HM2056_write_cmos_sensor(0x05BC,0x03);
HM2056_write_cmos_sensor(0x05BD,0x00);
HM2056_write_cmos_sensor(0x05BF,0x05);
HM2056_write_cmos_sensor(0x05C0,0x10);
HM2056_write_cmos_sensor(0x05C3,0x00);
HM2056_write_cmos_sensor(0x05C4,0x0C);
HM2056_write_cmos_sensor(0x05C5,0x20);
HM2056_write_cmos_sensor(0x05C7,0x01);
HM2056_write_cmos_sensor(0x05C8,0x14);
HM2056_write_cmos_sensor(0x05C9,0x54);
HM2056_write_cmos_sensor(0x05CA,0x14);
HM2056_write_cmos_sensor(0x05CB,0xE0);
HM2056_write_cmos_sensor(0x05CC,0x20);
HM2056_write_cmos_sensor(0x05CD,0x00);
HM2056_write_cmos_sensor(0x05CE,0x08);
HM2056_write_cmos_sensor(0x05CF,0x60);
HM2056_write_cmos_sensor(0x05D0,0x10);
HM2056_write_cmos_sensor(0x05D1,0x05);
HM2056_write_cmos_sensor(0x05D2,0x03);
HM2056_write_cmos_sensor(0x05D4,0x00);
HM2056_write_cmos_sensor(0x05D5,0x05);
HM2056_write_cmos_sensor(0x05D6,0x05);
HM2056_write_cmos_sensor(0x05D7,0x05);
HM2056_write_cmos_sensor(0x05D8,0x08);
HM2056_write_cmos_sensor(0x05DC,0x0C);
HM2056_write_cmos_sensor(0x05D9,0x00);
HM2056_write_cmos_sensor(0x05DB,0x00);
HM2056_write_cmos_sensor(0x05DD,0x0F);
HM2056_write_cmos_sensor(0x05DE,0x00);
HM2056_write_cmos_sensor(0x05DF,0x0A);
HM2056_write_cmos_sensor(0x05E0,0xA0);  
HM2056_write_cmos_sensor(0x05E1,0x00);  
HM2056_write_cmos_sensor(0x05E2,0xA0);  
HM2056_write_cmos_sensor(0x05E3,0x00);
HM2056_write_cmos_sensor(0x05E4,0x0A);
HM2056_write_cmos_sensor(0x05E5,0x00);
HM2056_write_cmos_sensor(0x05E6,0x49);
HM2056_write_cmos_sensor(0x05E7,0x06);
HM2056_write_cmos_sensor(0x05E8,0x0A);
HM2056_write_cmos_sensor(0x05E9,0x00);
HM2056_write_cmos_sensor(0x05EA,0xB9);
HM2056_write_cmos_sensor(0x05EB,0x04);
HM2056_write_cmos_sensor(0x0660,0x04);
HM2056_write_cmos_sensor(0x0661,0x10);//add start
HM2056_write_cmos_sensor(0x0662,0x04);
HM2056_write_cmos_sensor(0x0663,0x3B);
HM2056_write_cmos_sensor(0x0664,0x04);
HM2056_write_cmos_sensor(0x0665,0x3A);
HM2056_write_cmos_sensor(0x0666,0x04);
HM2056_write_cmos_sensor(0x0667,0x2C);
HM2056_write_cmos_sensor(0x0668,0x04);
HM2056_write_cmos_sensor(0x0669,0x2D);
HM2056_write_cmos_sensor(0x066A,0x04);
HM2056_write_cmos_sensor(0x066B,0x34);
HM2056_write_cmos_sensor(0x066C,0x04);
HM2056_write_cmos_sensor(0x066D,0x17);
HM2056_write_cmos_sensor(0x066E,0x00);
HM2056_write_cmos_sensor(0x066F,0x90);
HM2056_write_cmos_sensor(0x0670,0x00);
HM2056_write_cmos_sensor(0x0671,0x4E);
HM2056_write_cmos_sensor(0x0672,0x04);
HM2056_write_cmos_sensor(0x0673,0x4B);
HM2056_write_cmos_sensor(0x0674,0x04);
HM2056_write_cmos_sensor(0x0675,0x29);
HM2056_write_cmos_sensor(0x0676,0x00);
HM2056_write_cmos_sensor(0x0677,0x7C);
HM2056_write_cmos_sensor(0x0678,0x01);
HM2056_write_cmos_sensor(0x0679,0xAA);
HM2056_write_cmos_sensor(0x067A,0x00);
HM2056_write_cmos_sensor(0x067B,0xF0);
HM2056_write_cmos_sensor(0x067C,0x04);
HM2056_write_cmos_sensor(0x067D,0x42);
HM2056_write_cmos_sensor(0x067E,0x04);
HM2056_write_cmos_sensor(0x067F,0x21);
HM2056_write_cmos_sensor(0x0680,0x00);
HM2056_write_cmos_sensor(0x0681,0x12);
HM2056_write_cmos_sensor(0x0682,0x00);
HM2056_write_cmos_sensor(0x0683,0xAC);
HM2056_write_cmos_sensor(0x0684,0x00);
HM2056_write_cmos_sensor(0x0685,0x60);
HM2056_write_cmos_sensor(0x0686,0x04);
HM2056_write_cmos_sensor(0x0687,0x42);
HM2056_write_cmos_sensor(0x0688,0x04);
HM2056_write_cmos_sensor(0x0689,0x0E);
HM2056_write_cmos_sensor(0x068A,0x04);
HM2056_write_cmos_sensor(0x068B,0x36);
HM2056_write_cmos_sensor(0x068C,0x04);
HM2056_write_cmos_sensor(0x068D,0x38);
HM2056_write_cmos_sensor(0x068E,0x04);
HM2056_write_cmos_sensor(0x068F,0x29);
HM2056_write_cmos_sensor(0x0690,0x04);
HM2056_write_cmos_sensor(0x0691,0x2B);//add end
HM2056_write_cmos_sensor(0x0698,0x20);//0x10_off,0x20_on
HM2056_write_cmos_sensor(0x0699,0x20);//0x10_off,0x20_on
HM2056_write_cmos_sensor(0x069A,0x01);
HM2056_write_cmos_sensor(0x069B,0xFF);
HM2056_write_cmos_sensor(0x069C,0x22);
HM2056_write_cmos_sensor(0x069D,0x10);
HM2056_write_cmos_sensor(0x069E,0x10);
HM2056_write_cmos_sensor(0x069F,0x08);
HM2056_write_cmos_sensor(0x0B20,0xBE);
HM2056_write_cmos_sensor(0x0078,0x00);   
HM2056_write_cmos_sensor(0x007C,0x05);
HM2056_write_cmos_sensor(0x0B02,0x04);
HM2056_write_cmos_sensor(0x0B07,0x25);
HM2056_write_cmos_sensor(0x0B0E,0x1D);
HM2056_write_cmos_sensor(0x0B0F,0x07);
HM2056_write_cmos_sensor(0x0B22,0x02);
HM2056_write_cmos_sensor(0x0B11,0x7F);
HM2056_write_cmos_sensor(0x0B12,0x7F);
HM2056_write_cmos_sensor(0x0B17,0xE0);
HM2056_write_cmos_sensor(0x0B30,0x0F);
HM2056_write_cmos_sensor(0x0B31,0x02);
HM2056_write_cmos_sensor(0x0B32,0x00);
HM2056_write_cmos_sensor(0x0B33,0x00);
HM2056_write_cmos_sensor(0x0B39,0x0F);  
HM2056_write_cmos_sensor(0x0B3B,0x12);
HM2056_write_cmos_sensor(0x0B3F,0x01);

HM2056_write_cmos_sensor(0x0B04,0x03); //Default:0x05
HM2056_write_cmos_sensor(0x0B3A,0x07); //Default:0x06  

HM2056_write_cmos_sensor(0x0340,0x38);//141003 30->38
HM2056_write_cmos_sensor(0x0344,0x74);//141003 74
HM2056_write_cmos_sensor(0x0356,0x90);//141003 86->90
HM2056_write_cmos_sensor(0x0357,0xD0);//141003 CA->D0
//HM2056_write_cmos_sensor(0x0024,0x40);
//HM2056_write_cmos_sensor(0x0000,0x01);
//HM2056_write_cmos_sensor(0x0100,0x01);
//HM2056_write_cmos_sensor(0x0101,0x01);

/* CMU */
HM2056_write_cmos_sensor(0x0000,0x01);
HM2056_write_cmos_sensor(0x0100,0x01);
HM2056_write_cmos_sensor(0x0101,0x01);
HM2056_write_cmos_sensor(0x0005,0x01);

} /* HM2056_YUV_sensor_initial_setting */

/*************************************************************************
* FUNCTION
*	HM2056_PV_setting
*
* DESCRIPTION
*	This function apply the preview mode setting, normal the preview size is 1/4 of full size.
*	Ex. 2M (1600 x 1200)
*	Preview: 800 x 600 (Use sub-sample or binning to acheive it)
*	Full Size: 1600 x 1200 (Output every effective pixels.)
*
* PARAMETERS
*	1. image_sensor_exposure_window_struct : Set the grab start x,y and width,height.
*	2. image_sensor_config_struct : Current operation mode.
*
* RETURNS
*	None
*
*************************************************************************/
static void HM2056_PV_setting()
{
	
		/* Preview mode parameters */
		
	HM2056_write_cmos_sensor(0x0005,0x00);
	HM2056_write_cmos_sensor(0x0024,0x00);
	//msleep(100);
	HM2056_write_cmos_sensor(0x0006,0x00);
	HM2056_write_cmos_sensor(0x000D,0x11);
	HM2056_write_cmos_sensor(0x000E,0x11);
	HM2056_write_cmos_sensor(0x0012,0x1C);
	HM2056_write_cmos_sensor(0x0013,0x01);
	HM2056_write_cmos_sensor(0x002A,0x1F);
	HM2056_write_cmos_sensor(0x0071,0xFF);
	HM2056_write_cmos_sensor(0x0082,0xA2);
	HM2056_write_cmos_sensor(0x011F,0xF7);
	HM2056_write_cmos_sensor(0x0125,0xDF);//FF->DF
	HM2056_write_cmos_sensor(0x0126,0x70);
	HM2056_write_cmos_sensor(0x0131,0xBD);
	HM2056_write_cmos_sensor(0x0144,0x06);
	HM2056_write_cmos_sensor(0x0190,0x80);
	HM2056_write_cmos_sensor(0x0192,0x48);
	//HM2056_write_cmos_sensor(0x03B7,0xEA);//AE Window 800x600
  //HM2056_write_cmos_sensor(0x03B8,0x00);
  //HM2056_write_cmos_sensor(0x03B9,0x3A);
  //HM2056_write_cmos_sensor(0x03BA,0x01);//AE Window 800x600
	HM2056_write_cmos_sensor(0x0541,0x9B);
	HM2056_write_cmos_sensor(0x0543,0xBA);
	//HM2056_write_cmos_sensor(0x05E0,0xA0);//800x600
	//HM2056_write_cmos_sensor(0x05E1,0x00);//800x600
	//HM2056_write_cmos_sensor(0x05E2,0xA0);//800x600
	//HM2056_write_cmos_sensor(0x05E3,0x00);//800x600
	HM2056_write_cmos_sensor(0x05E4,0x05);//04->05_preview_VGA to 800x600
	HM2056_write_cmos_sensor(0x05E5,0x00);//00->00
	HM2056_write_cmos_sensor(0x05E6,0x24);//83->24
	HM2056_write_cmos_sensor(0x05E7,0x03);//02->03
	HM2056_write_cmos_sensor(0x05E8,0x08);//06->08
	HM2056_write_cmos_sensor(0x05E9,0x00);//00->00
	HM2056_write_cmos_sensor(0x05EA,0x5F);//E5->5F
	HM2056_write_cmos_sensor(0x05EB,0x02);//01->02
//	HM2056_write_cmos_sensor(0x0123,0x21);
//	HM2056_write_cmos_sensor(0x0012,0x0D);
//	HM2056_write_cmos_sensor(0x0013,0x03);
//	HM2056_write_cmos_sensor(0x0B39,0x0F); //0x03?
//	HM2056_write_cmos_sensor(0x0B09,0x01);
//	HM2056_write_cmos_sensor(0x0B08,0xE0);
//	HM2056_write_cmos_sensor(0x0B0B,0x02);
//	HM2056_write_cmos_sensor(0x0B0A,0x80);
	HM2056_write_cmos_sensor(0x007c,0x17);//43->17
	HM2056_write_cmos_sensor(0x0024,0x40);
	//msleep(100);
//	HM2056_write_cmos_sensor(0x0000,0x01);
//	HM2056_write_cmos_sensor(0x0100,0x01);
//	HM2056_write_cmos_sensor(0x0101,0x01);	

	//Start
	HM2056_write_cmos_sensor(0x0000,0x01); 
	HM2056_write_cmos_sensor(0x0100,0x01); 
	HM2056_write_cmos_sensor(0x0101,0x01); 
	HM2056_write_cmos_sensor(0x0005,0x01); //Turn on rolling shutter
	
		
} /* HM2056_PV_setting */
/****************************/
static void HM2056_VIDEO_setting()
{
	HM2056_write_cmos_sensor(0x0005,0x00);
	HM2056_write_cmos_sensor(0x0024,0x00);
	msleep(100);
	HM2056_write_cmos_sensor(0x0006,0x08);	//HD720
	HM2056_write_cmos_sensor(0x000D,0x00);
	HM2056_write_cmos_sensor(0x000E,0x00);
	HM2056_write_cmos_sensor(0x0012,0x08);
	HM2056_write_cmos_sensor(0x0013,0x00);
	HM2056_write_cmos_sensor(0x0071,0x99);
	HM2056_write_cmos_sensor(0x0082,0xE2);
	HM2056_write_cmos_sensor(0x011F,0xFF);
	HM2056_write_cmos_sensor(0x0125,0xDF);//0xD5_off,0xDF_ON
	HM2056_write_cmos_sensor(0x0126,0x70);
	HM2056_write_cmos_sensor(0x0131,0xBC);//0xB8_off,0xBC_ON
	HM2056_write_cmos_sensor(0x0144,0x04);
	HM2056_write_cmos_sensor(0x0190,0x87);
	HM2056_write_cmos_sensor(0x0192,0x50);
	HM2056_write_cmos_sensor(0x0541,0xBC);
	HM2056_write_cmos_sensor(0x0543,0xE1);
	HM2056_write_cmos_sensor(0x03B7,0x8D);	// AE Window
	HM2056_write_cmos_sensor(0x03B8,0x00);
	HM2056_write_cmos_sensor(0x03B9,0xFF);
	HM2056_write_cmos_sensor(0x03BA,0x00);
	HM2056_write_cmos_sensor(0x05E4,0x08);
	HM2056_write_cmos_sensor(0x05E5,0x00);
	HM2056_write_cmos_sensor(0x05E6,0x07);
	HM2056_write_cmos_sensor(0x05E7,0x05);
	HM2056_write_cmos_sensor(0x05E8,0x06);
	HM2056_write_cmos_sensor(0x05E9,0x00);
	HM2056_write_cmos_sensor(0x05EA,0xD5);
	HM2056_write_cmos_sensor(0x05EB,0x02);

	HM2056_write_cmos_sensor(0x002A,0x2F);	// For HD720p 30 fps
	HM2056_write_cmos_sensor(0x007c,0x04);
	HM2056_write_cmos_sensor(0x0024,0x40);
	msleep(100);

								
	/* CMU */
			HM2056_write_cmos_sensor(0x0000,0x01);
			HM2056_write_cmos_sensor(0x0100,0x01);
			HM2056_write_cmos_sensor(0x0101,0x01);			
	HM2056_write_cmos_sensor(0x0005,0x01);
} /*HM2056_VIDEO_setting*/

/*************************************************************************
* FUNCTION
*	HM2056_Set_Video_Frame_Rate
*
* DESCRIPTION
*	This function set the sensor output frmae to target frame and fix the frame rate for 
*	video encode.
*
* PARAMETERS
*	1. kal_uint32 : Target frame rate to fixed.
*
* RETURNS
*	None
*
*************************************************************************/
static void HM2056_Set_Video_Frame_Rate(kal_uint32 frame_rate)
{
	switch (frame_rate)
	{
		case 50:	/* Auto*/
			HM2056_write_cmos_sensor(0x038F,0x0E);	
    			HM2056_write_cmos_sensor(0x0390,0xBA);   
			break;
		case 75:
			/* Fixed frame rate in 5 FPS */
			HM2056_write_cmos_sensor(0x038F,0x09);	//Max 7.5 FPS.
    		HM2056_write_cmos_sensor(0x0390,0xD1);           
			break;
		case 100:
			/* Fixed frame rate in 10 FPS */
			HM2056_write_cmos_sensor(0x038F,0x07);	//Max 10 FPS.
    		HM2056_write_cmos_sensor(0x0390,0x5D);      
			break;
		case 125:
			/* Fixed frame rate in 10 FPS */
			HM2056_write_cmos_sensor(0x038F,0x05);	//Max 10 FPS.
    		HM2056_write_cmos_sensor(0x0390,0xE4);      
			break;
		case 150:
				/* Fixed frame rate in 15 FPS */
			HM2056_write_cmos_sensor(0x038F,0x04);	//Max 15 FPS.
    		HM2056_write_cmos_sensor(0x0390,0xE8);           
			break;
		case 200:
				/* Fixed frame rate in 30 FPS */
			HM2056_write_cmos_sensor(0x038F,0x03);	//Max 20 FPS.
    		HM2056_write_cmos_sensor(0x0390,0xAE);    		  		          
			break;
		case 250:
				/* Fixed frame rate in 30 FPS */
			HM2056_write_cmos_sensor(0x038F,0x02);	//Max 20 FPS.
    		HM2056_write_cmos_sensor(0x0390,0xf2);    		  		          
			break;
		case 300:
			HM2056_write_cmos_sensor(0x038F,0x02);	//Max 30 FPS.//02
    		HM2056_write_cmos_sensor(0x0390,0x74);//70       //70   
			break;
		case 400:
			HM2056_write_cmos_sensor(0x038F,0x01);	//Max 30 FPS.//02
    		HM2056_write_cmos_sensor(0x0390,0xD7);//70       //70   
			break;	
		case 500:
			HM2056_write_cmos_sensor(0x038F,0x01);	//Max 30 FPS.//02
    		HM2056_write_cmos_sensor(0x0390,0x79);//70       //70   
			break;	
		case 600:
			HM2056_write_cmos_sensor(0x038F,0x01);	//Max 60 FPS.//02
    		HM2056_write_cmos_sensor(0x0390,0x3A);//70       //70   
			break;
		default:
				/* Do nothing */

			break;
	}
	
	HM2056_write_cmos_sensor(0x0000,0xFF);
	HM2056_write_cmos_sensor(0x0100,0xFF);
}

/*************************************************************************
* FUNCTION
*	HM2056_CAP_setting
*
* DESCRIPTION
*	This function apply the full size mode setting.
*	Ex. 2M (1600 x 1200)
*	Preview: 800 x 600 (Use sub-sample or binning to acheive it)
*	Full Size: 1600 x 1200 (Output every effective pixels.)
*
* PARAMETERS
*	1. image_sensor_exposure_window_struct : Set the grab start x,y and width,height.
*	2. image_sensor_config_struct : Current operation mode.
*
* RETURNS
*	None
*
*************************************************************************/
static void HM2056_CAP_setting()
{
	kal_uint8 temp;
	
  kal_uint16 exp = 0;
  kal_uint8 exp_l = 0;
  kal_uint8 exp_h = 0;
  kal_uint8 max_gain = 0;
  
  /*
	exp =(HM2056_read_cmos_sensor(0x15)<<8 | HM2056_read_cmos_sensor(0x16));//hm2056
	exp = (exp/2);
	exp_h = (exp&0xff00)>>8;
	exp_l =  exp&0x00ff;
	HM2056_write_cmos_sensor(0x0380,0xFE);
	HM2056_write_cmos_sensor(0x0015,exp_h);
	HM2056_write_cmos_sensor(0x0016,exp_l);
	*/
	//HM2056_write_cmos_sensor(0x0380,0xFE);//1006_test _ AE switch_off 
  
  max_gain = HM2056_read_cmos_sensor(0x0392);
  
  if(max_gain ==3 ) // bin -> full  X2  and 30fps => 15 fps X 2
  	{
  	max_gain = 1;
  	HM2056_Set_Video_Frame_Rate(100); // 1/12.5  X 2 
    }
     else if(max_gain ==2 )
    	{
    		max_gain = 0;
    		HM2056_Set_Video_Frame_Rate(100);
    		
    	}
    	else if (max_gain ==1 )
    		{
    		max_gain = 0;
    		HM2056_Set_Video_Frame_Rate(200);
    		
    	  }
    	  else if (max_gain ==0 )
    	  	{
    		   max_gain = 0;
    		   HM2056_Set_Video_Frame_Rate(400);
    		
    	    }
  HM2056_write_cmos_sensor(0x0392,max_gain);
   
	HM2056_write_cmos_sensor(0x0005,0x00);
	HM2056_write_cmos_sensor(0x0024,0x00);
	//msleep(100);
	HM2056_write_cmos_sensor(0x0006,0x00);
	HM2056_write_cmos_sensor(0x000D,0x00);
	HM2056_write_cmos_sensor(0x000E,0x00);
	HM2056_write_cmos_sensor(0x0012,0x0d); // 0x04
	HM2056_write_cmos_sensor(0x0013,0x00);
	//HM2056_write_cmos_sensor(0x002A,0x1F); // 0x2F
	HM2056_write_cmos_sensor(0x0071,0xAB);
	HM2056_write_cmos_sensor(0x0082,0xA2); // 0xE2
	HM2056_write_cmos_sensor(0x011F,0xFF);
	HM2056_write_cmos_sensor(0x0125,0xDF);//0xD5_off,0xDF_ON
	HM2056_write_cmos_sensor(0x0126,0x70);
	HM2056_write_cmos_sensor(0x0131,0xBC);//0xB8_off,0xBC_ON
	HM2056_write_cmos_sensor(0x0144,0x04);
	HM2056_write_cmos_sensor(0x0190,0x87);
	HM2056_write_cmos_sensor(0x0192,0x50);
//HM2056_write_cmos_sensor(0x03B7,0xEA);	// AE Window
//HM2056_write_cmos_sensor(0x03B8,0x00);
//HM2056_write_cmos_sensor(0x03B9,0x3A);
//HM2056_write_cmos_sensor(0x03BA,0x01);
	HM2056_write_cmos_sensor(0x0541,0x64);//9B
	HM2056_write_cmos_sensor(0x0543,0x78);//bA
	HM2056_write_cmos_sensor(0x05E4,0x0A);
	HM2056_write_cmos_sensor(0x05E5,0x00);
	HM2056_write_cmos_sensor(0x05E6,0x49);
	HM2056_write_cmos_sensor(0x05E7,0x06);
	HM2056_write_cmos_sensor(0x05E8,0x0A);
	HM2056_write_cmos_sensor(0x05E9,0x00);
	HM2056_write_cmos_sensor(0x05EA,0xB9);
	HM2056_write_cmos_sensor(0x05EB,0x04);

//	HM2056_write_cmos_sensor(0x002A,0x1F);
//	HM2056_write_cmos_sensor(0x0B39,0x0F);
//	HM2056_write_cmos_sensor(0x0B09,0x04);
//	HM2056_write_cmos_sensor(0x0B08,0xB0);
//	HM2056_write_cmos_sensor(0x0B0B,0x06);
//	HM2056_write_cmos_sensor(0x0B0A,0x40);
	HM2056_write_cmos_sensor(0x007c,0x14);
	HM2056_write_cmos_sensor(0x0024,0x40);
	//HM2056_write_cmos_sensor(0x0380,0xFF);//1006_test _ AE switch_on
	//msleep(100);

								
	/* CMU */
//	for (temp = 0; temp < 3; temp++)
//  {		/* Troy add this loop to let capture more stable. */   			
			HM2056_write_cmos_sensor(0x0000,0x01);
			HM2056_write_cmos_sensor(0x0100,0x01);
			HM2056_write_cmos_sensor(0x0101,0x01);			
			HM2056_write_cmos_sensor(0x0005,0x01);
//}

	HM2056_get_exposure_gain();
} /* HM2056_CAP_setting */

UINT32 HM2056GetSensorID(UINT32 *sensorID)
{
	volatile signed char i;
	kal_uint16 sensor_id=0;

	HM2056_write_cmos_sensor(0x0022, 0x01);
	mdelay(10);	

	sensor_id = (HM2056_read_cmos_sensor(0x0001) << 8) | HM2056_read_cmos_sensor(0x0002);


	if (HM2056_SENSOR_ID != sensor_id)
	{
		*sensorID = 0xffffffff;
		SENSORDB("HM2056 Sensor id read failed, ID = %x\n", sensor_id);
		return ERROR_SENSOR_CONNECT_FAIL;
	}

	*sensorID = sensor_id;

	printk("[HM2056] sensor id = 0x%x\n", sensor_id);

	return ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*	HM2056Open
*
* DESCRIPTION
*	This function initialize the registers of CMOS sensor
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 HM2056Open(void)
{
	volatile signed char i;
	kal_uint16 sensor_id=0;
	zoom_factor = 0; 
	HM2056_write_cmos_sensor(0x0022, 0x01);
	mdelay(50);	 //20121025	
	
	sensor_id = (HM2056_read_cmos_sensor(0x0001) << 8) | HM2056_read_cmos_sensor(0x0002);
	printk("[HM2056] sensor id = 0x%x\n", sensor_id);
	if (HM2056_SENSOR_ID != sensor_id)
		return ERROR_SENSOR_CONNECT_FAIL;
	
	HM2056_YUV_sensor_initial_setting();
	printk("HM2056 Initial setting \n");     
	mdelay(400);
	return ERROR_NONE;
}	/* HM2056Open() *///by Michael.Xie  2010/11/30

/*************************************************************************
* FUNCTION
*	HM2056Close
*
* DESCRIPTION
*	This function is to turn off sensor module power.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 HM2056Close(void)
{
	HM2056_write_cmos_sensor(0x04, 0x11);
	//HM2056_write_cmos_sensor(0x03, 0x00); //enter	page 0
	//HM2056_write_cmos_sensor(0x01, (HM2056_read_cmos_sensor(0x01)|0x01));
//	CISModulePowerOn(FALSE);
	return ERROR_NONE;
}	/* HM2056Close() */

/*************************************************************************
* FUNCTION
*	HM2056Preview
*
* DESCRIPTION
*	This function start the sensor preview.
*
* PARAMETERS
*	*image_window : address pointer of pixel numbers in one period of HSYNC
*  *sensor_config_data : address pointer of line numbers in one period of VSYNC
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/

UINT32 HM2056Preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	HM2056_sensor_cap_state = KAL_FALSE;
  HM2056_VEDIO_encode_mode = KAL_FALSE;  
  
  //4  <1> preview config sequence
	HM2056_sensor_pclk=480;
	
	SENSORDB("enter hm2056 preview\n");

	spin_lock(&hm2056_drv_lock);
	HM2056_iHM2056_Mode = HM2056_MODE_PREVIEW;
	//HM2056_Set_Video_Frame_Rate(200); 
	spin_unlock(&hm2056_drv_lock);	
 
	HM2056_PV_setting();
	mdelay(300);//500->300
	HM2056_Set_Video_Frame_Rate(200); 
	
	
	SENSORDB("leave preview\n");
	
	HM2056_ae_enable(KAL_TRUE);
	HM2056_awb_enable(KAL_TRUE);
	
    //4 <3> set mirror and flip
    sensor_config_data->SensorImageMirror = IMAGE_NORMAL;
    HM2056_set_mirror(sensor_config_data->SensorImageMirror);

    //4 <7> set shutter
    image_window->GrabStartX = IMAGE_SENSOR_START_GRAB_X;
    image_window->GrabStartY = IMAGE_SENSOR_START_GRAB_Y;
    image_window->ExposureWindowWidth = HM2056_IMAGE_SENSOR_PV_WIDTH;
    image_window->ExposureWindowHeight = HM2056_IMAGE_SENSOR_PV_HEIGHT;
    
   
    HM2056_DELAY_AFTER_PREVIEW = 1; 

	// copy sensor_config_data
	memcpy(&HM2056SensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
  	return ERROR_NONE;
}	/* HM2056Preview */

UINT32 HM2056Video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                                                MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	HM2056_sensor_cap_state = KAL_FALSE;
  HM2056_VEDIO_encode_mode = KAL_TRUE;  
  
  //4  <1> preview config sequence
	HM2056_sensor_pclk=480;
	
	SENSORDB("enter hm2056 video\n");
	
	spin_lock(&hm2056_drv_lock);
	HM2056_iHM2056_Mode = HM2056_MODE_VIDEO;
	//HM2056_Set_Video_Frame_Rate(200);
	spin_unlock(&hm2056_drv_lock);	
 
	HM2056_VIDEO_setting();
	mdelay(500);
	HM2056_Set_Video_Frame_Rate(300);
	

	SENSORDB("leave video\n");
	
	HM2056_ae_enable(KAL_TRUE);
	HM2056_awb_enable(KAL_TRUE);
	
    //4 <3> set mirror and flip
    //sensor_config_data->SensorImageMirror = IMAGE_NORMAL;
    //HM2056_set_mirror(sensor_config_data->SensorImageMirror);

    //4 <7> set shutter
    image_window->GrabStartX = IMAGE_SENSOR_START_GRAB_X;
    image_window->GrabStartY = IMAGE_SENSOR_START_GRAB_Y;
    image_window->ExposureWindowWidth = HM2056_IMAGE_SENSOR_VIDEO_WIDTH;
    image_window->ExposureWindowHeight = HM2056_IMAGE_SENSOR_VIDEO_HEIGHT;
    
   
    HM2056_DELAY_AFTER_PREVIEW = 1; 

	// copy sensor_config_data
	memcpy(&HM2056SensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
  	 return ERROR_NONE;
}	/* HM2056Video */

	
UINT32 HM2056Capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	kal_uint16 temp_reg;
	kal_uint16 msb;//0925
	kal_uint16 lsb;//0925
	kal_uint16 again;//0925
	kal_uint16 dgain;//0925
	kal_uint16 aetarget;//1006
	kal_uint16 imgcfg;//1023
	
	msb=HM2056_read_cmos_sensor(0x0015);//0925
	lsb=HM2056_read_cmos_sensor(0x0016);//0925
	again = HM2056_read_cmos_sensor(0x0018); //A gain 0925
  	//again = 1<< again;
  dgain = HM2056_read_cmos_sensor(0x001D); //D gain 0925
  aetarget = HM2056_read_cmos_sensor(0x0385);//ae_target 1006
	imgcfg = HM2056_read_cmos_sensor(0x000F);//1023
	
    HM2056_sensor_cap_state = KAL_TRUE;
    //HM2056_Set_Video_Frame_Rate(100);//1217 for test
    
    						
    if ((image_window->ImageTargetWidth<=HM2056_IMAGE_SENSOR_PV_WIDTH)&&
        (image_window->ImageTargetHeight<=HM2056_IMAGE_SENSOR_PV_HEIGHT))
    {
    	/* Less than PV Mode */
        HM2056_capture_pclk_in_M = HM2056_preview_pclk_in_M;   //Don't change the clk
  	SENSORDB("HM2056 Capture in VGA \n");
        image_window->GrabStartX = IMAGE_SENSOR_START_GRAB_X;
        image_window->GrabStartY = IMAGE_SENSOR_START_GRAB_X;
        image_window->ExposureWindowWidth= HM2056_IMAGE_SENSOR_PV_WIDTH;
        image_window->ExposureWindowHeight = HM2056_IMAGE_SENSOR_PV_HEIGHT;

    }
    else 
    {    
		/* Capture Size <= 2M */
		if ((image_window->ImageTargetWidth <= HM2056_IMAGE_SENSOR_FULL_WIDTH)
			&& (image_window->ImageTargetHeight <= HM2056_IMAGE_SENSOR_FULL_HEIGHT))
		{    
			if (image_window->ZoomFactor >= 400)
			{
#if 0	//Troy add, HM2056 don't need change dummy and shutter.
				HM2056_write_cmos_sensor(0x0012,0x01);   //fps = 6
				HM2056_write_cmos_sensor(0x0013,0x4A); 
				HM2056_write_cmos_sensor(0x0544,0x00);	//SS_fstep_60Hz_H.
				HM2056_write_cmos_sensor(0x0545,0x44);	//SS_fstep_60Hz_L.
				HM2056_write_cmos_sensor(0x0546,0x00);	//SS_fstep_50Hz_H.
				HM2056_write_cmos_sensor(0x0547,0x52);	//SS_fstep_50Hz_L.

#endif //Troy add				
			}
			else if (image_window->ZoomFactor >= 200)
			{
#if 0	//Troy add, HM2056 don't need change dummy and shutter.
				HM2056_write_cmos_sensor(0x0012,0x01);   //fps = 6
				HM2056_write_cmos_sensor(0x0013,0x4A); 
				HM2056_write_cmos_sensor(0x0544,0x00);	//SS_fstep_60Hz_H.
				HM2056_write_cmos_sensor(0x0545,0x44);	//SS_fstep_60Hz_L.
				HM2056_write_cmos_sensor(0x0546,0x00);	//SS_fstep_50Hz_H.
				HM2056_write_cmos_sensor(0x0547,0x52);	//SS_fstep_50Hz_L.

#endif //Troy add				
			}
			else
			{
#if 0	//Troy add, HM2056 don't need change dummy and shutter.				
				HM2056_write_cmos_sensor(0x0012,0x09);
				HM2056_write_cmos_sensor(0x0013,0x01);
				HM2056_write_cmos_sensor(0x0544,0x00);	//SS_fstep_60Hz_H.
				HM2056_write_cmos_sensor(0x0545,0x69);	//SS_fstep_60Hz_L.
				HM2056_write_cmos_sensor(0x0546,0x00);	//SS_fstep_50Hz_H.
				HM2056_write_cmos_sensor(0x0547,0x7E);	//SS_fstep_50Hz_L.

#endif //Troy add				
			}
		}
		else    		/* When Capture Size Interpolate to 3M */
		{
			if (image_window->ZoomFactor >= 400)
			{
#if 0	//Troy add, HM2056 don't need change dummy and shutter.								
				HM2056_write_cmos_sensor(0x0012,0x05);  //fps = 4
				HM2056_write_cmos_sensor(0x0013,0xC2);
				HM2056_write_cmos_sensor(0x0544,0x00);	//SS_fstep_60Hz_H.
				HM2056_write_cmos_sensor(0x0545,0x28);	//SS_fstep_60Hz_L.
				HM2056_write_cmos_sensor(0x0546,0x00);	//SS_fstep_50Hz_H.
				HM2056_write_cmos_sensor(0x0547,0x30);	//SS_fstep_50Hz_L.

#endif //Troy add				
			}
			else if (image_window->ZoomFactor >= 200)
			{
#if 0	//Troy add, HM2056 don't need change dummy and shutter.								
				HM2056_write_cmos_sensor(0x0012,0x05);  //fps = 4
				HM2056_write_cmos_sensor(0x0013,0xC2);
				HM2056_write_cmos_sensor(0x0544,0x00);	//SS_fstep_60Hz_H.
				HM2056_write_cmos_sensor(0x0545,0x28);	//SS_fstep_60Hz_L.
				HM2056_write_cmos_sensor(0x0546,0x00);	//SS_fstep_50Hz_H.
				HM2056_write_cmos_sensor(0x0547,0x30);	//SS_fstep_50Hz_L.

#endif //Troy add				
			}
			else
			{
#if 0	//Troy add, HM2056 don't need change dummy and shutter.								
				HM2056_write_cmos_sensor(0x0012,0x01);   //fps = 6
				HM2056_write_cmos_sensor(0x0013,0x4A); 
				HM2056_write_cmos_sensor(0x0544,0x00);	//SS_fstep_60Hz_H.
				HM2056_write_cmos_sensor(0x0545,0x44);	//SS_fstep_60Hz_L.
				HM2056_write_cmos_sensor(0x0546,0x00);	//SS_fstep_50Hz_H.
				HM2056_write_cmos_sensor(0x0547,0x52);	//SS_fstep_50Hz_L.

#endif //Troy add				
			}
		}	
       HM2056_CAP_setting();  //modify	
	
	mdelay(50); // 500 -> 200 
		SENSORDB("PV_HM2056 read register, 0x0015 = %x\n", msb);//0925
		SENSORDB("HM2056 read register, 0x0016 = %x\n", lsb);//0925
		SENSORDB("HM2056 read register, 0x0018 = %x\n", again);//0925
		SENSORDB("HM2056 read register, 0x001d = %x\n", dgain);//0925
		SENSORDB("HM2056 read register, 0x0385 = %x\n", aetarget);//1006
		SENSORDB("HM2056 read register, 0x000F = %x\n", imgcfg);//1023	
	 printk("HM2056 Capture setting is done \n");  
              
        if ((image_window->ImageTargetWidth<=HM2056_IMAGE_SENSOR_FULL_WIDTH)&&
        (image_window->ImageTargetHeight<=HM2056_IMAGE_SENSOR_FULL_HEIGHT))
        {
		SENSORDB("HM2056 Capture in 2M \n");
		HM2056_capture_pclk_in_M = 480;
        	HM2056_sensor_pclk = 480;                 
        }
        else//Interpolate to 3M
        {
        	HM2056_capture_pclk_in_M = 480;
        	HM2056_sensor_pclk = 480;                
        }   
        
        image_window->GrabStartX=IMAGE_SENSOR_START_GRAB_X;
        image_window->GrabStartY=IMAGE_SENSOR_START_GRAB_X;
        image_window->ExposureWindowWidth=HM2056_IMAGE_SENSOR_FULL_WIDTH;
        image_window->ExposureWindowHeight=HM2056_IMAGE_SENSOR_FULL_HEIGHT;
    }

    // AEC/AGC/AWB will be enable in preview and param_wb function
    /* total delay 4 frame for AE stable */
	HM2056_gPVmode=KAL_TRUE;

	HM2056_DELAY_AFTER_PREVIEW = 2;

	// copy sensor_config_data
	memcpy(&HM2056SensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	return ERROR_NONE;
}	/* HM2056Capture() */

UINT32 HM2056GetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
	pSensorResolution->SensorFullWidth=HM2056_IMAGE_SENSOR_FULL_WIDTH;  //modify by yanxu
	pSensorResolution->SensorFullHeight=HM2056_IMAGE_SENSOR_FULL_HEIGHT;
	pSensorResolution->SensorPreviewWidth=HM2056_IMAGE_SENSOR_PV_WIDTH;
	pSensorResolution->SensorPreviewHeight=HM2056_IMAGE_SENSOR_PV_HEIGHT;
	pSensorResolution->SensorVideoWidth=HM2056_IMAGE_SENSOR_VIDEO_WIDTH;
	pSensorResolution->SensorVideoHeight=HM2056_IMAGE_SENSOR_VIDEO_HEIGHT;

	//pSensorResolution->SensorVideoWidth=HM2056_IMAGE_SENSOR_PV_WIDTH - 2*IMAGE_SENSOR_START_GRAB_X;
	//pSensorResolution->SensorVideoHeight=HM2056_IMAGE_SENSOR_PV_HEIGHT - 2*IMAGE_SENSOR_START_GRAB_Y;

	return ERROR_NONE;
}	/* HM2056GetResolution() */

UINT32 HM2056GetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
					  MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
	pSensorInfo->SensorPreviewResolutionX=HM2056_IMAGE_SENSOR_PV_WIDTH;
	pSensorInfo->SensorPreviewResolutionY=HM2056_IMAGE_SENSOR_PV_HEIGHT;
	pSensorInfo->SensorFullResolutionX=HM2056_IMAGE_SENSOR_FULL_WIDTH;
	pSensorInfo->SensorFullResolutionY=HM2056_IMAGE_SENSOR_FULL_HEIGHT;

	pSensorInfo->SensorCameraPreviewFrameRate=30;
	pSensorInfo->SensorVideoFrameRate=30;
	pSensorInfo->SensorStillCaptureFrameRate=10;
	pSensorInfo->SensorWebCamCaptureFrameRate=15;
	pSensorInfo->SensorResetActiveHigh=FALSE;
	pSensorInfo->SensorResetDelayCount=5;		// 1
	pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_UYVY;	
	//pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_YUYV;
	pSensorInfo->SensorClockPolarity=SENSOR_CLOCK_POLARITY_LOW;	/*??? */
	pSensorInfo->SensorClockFallingPolarity=SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorInterruptDelayLines = 1;
	pSensorInfo->CaptureDelayFrame = 3; 
	pSensorInfo->PreviewDelayFrame = 2; 
	pSensorInfo->VideoDelayFrame = 2; 
	pSensorInfo->SensorMasterClockSwitch = 0; 
	pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_8MA;
	pSensorInfo->SensroInterfaceType=SENSOR_INTERFACE_TYPE_MIPI;	//20121022 BY Zeroy
/*
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxWidth=CAM_SIZE_2M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxHeight=CAM_SIZE_2M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxWidth=CAM_SIZE_2M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxHeight=CAM_SIZE_2M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxWidth=CAM_SIZE_2M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxHeight=CAM_SIZE_2M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].MaxWidth=CAM_SIZE_1M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].MaxHeight=CAM_SIZE_1M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].BinningEnable=TRUE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].MaxWidth=CAM_SIZE_1M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].MaxHeight=CAM_SIZE_1M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].BinningEnable=TRUE;
	/*pSensorInfo->CaptureDelayFrame = 1; 
	pSensorInfo->PreviewDelayFrame = 0; 
	pSensorInfo->VideoDelayFrame = 4; 		
	pSensorInfo->SensorMasterClockSwitch = 0; 
    pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_8MA;   		
*/
   	pSensorInfo->YUVAwbDelayFrame = 2 ;
  	pSensorInfo->YUVEffectDelayFrame = 2 ;

	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			pSensorInfo->SensorClockFreq=24;//24;
			pSensorInfo->SensorClockDividCount=	3;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 2;
			pSensorInfo->SensorPixelClockCount= 3;	//4// 4
			pSensorInfo->SensorDataLatchCount= 2;
			pSensorInfo->SensorGrabStartX = IMAGE_SENSOR_START_GRAB_X; 
			pSensorInfo->SensorGrabStartY = IMAGE_SENSOR_START_GRAB_Y;
			pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_1_LANE;			
      pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
	    pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; 
	    pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;             
			pSensorInfo->SensorWidthSampling = 0;  // 0 is default 1x
			pSensorInfo->SensorHightSampling = 0;   // 0 is default 1x 
			pSensorInfo->SensorPacketECCOrder = 1;
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		//case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
			pSensorInfo->SensorClockFreq=24;//24;
			pSensorInfo->SensorClockDividCount=	3;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 2;
			pSensorInfo->SensorPixelClockCount= 3;	//4// 4
			pSensorInfo->SensorDataLatchCount= 2;
			pSensorInfo->SensorGrabStartX = IMAGE_SENSOR_START_GRAB_X; 
			pSensorInfo->SensorGrabStartY = IMAGE_SENSOR_START_GRAB_Y;
			pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_1_LANE;			
      pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
	    pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; 
	    pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;             
			pSensorInfo->SensorWidthSampling = 0;  // 0 is default 1x
			pSensorInfo->SensorHightSampling = 0;   // 0 is default 1x 
			pSensorInfo->SensorPacketECCOrder = 1;

		break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		//case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
			pSensorInfo->SensorClockFreq=24;//24;
			pSensorInfo->SensorClockDividCount=	3;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 2;
			pSensorInfo->SensorPixelClockCount= 3;	// 4
			pSensorInfo->SensorDataLatchCount= 2;
			pSensorInfo->SensorGrabStartX = IMAGE_SENSOR_START_GRAB_X; 
			pSensorInfo->SensorGrabStartY = IMAGE_SENSOR_START_GRAB_Y;
			pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_1_LANE;			
      pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
	    pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; 
	    pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;             
			pSensorInfo->SensorWidthSampling = 0;  // 0 is default 1x
			pSensorInfo->SensorHightSampling = 0;   // 0 is default 1x 
			pSensorInfo->SensorPacketECCOrder = 1;
		break;
		default:
			pSensorInfo->SensorClockFreq=24;//24;
			pSensorInfo->SensorClockDividCount=	3;
			pSensorInfo->SensorClockRisingCount= 0;
			pSensorInfo->SensorClockFallingCount= 2;
			pSensorInfo->SensorPixelClockCount= 4;
			pSensorInfo->SensorDataLatchCount= 2;
			pSensorInfo->SensorGrabStartX = IMAGE_SENSOR_START_GRAB_X; 
			pSensorInfo->SensorGrabStartY = IMAGE_SENSOR_START_GRAB_X;             
		break;
	}
	HM2056_PixelClockDivider=pSensorInfo->SensorPixelClockCount;
	memcpy(pSensorConfigData, &HM2056SensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	return ERROR_NONE;
}	/* HM2056GetInfo() */


UINT32 HM2056Control(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			HM2056Preview(pImageWindow, pSensorConfigData);
		break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		//case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
			HM2056Video(pImageWindow, pSensorConfigData);
		break;
		
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		//case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
			HM2056Capture(pImageWindow, pSensorConfigData);
		break;
		default:
		    break; 
	}
	return TRUE;
}	/* HM2056Control() */ 

BOOL HM2056_set_param_wb(UINT16 para)
{
	  SENSORDB("HM2056_set_param_wb para=%d\n",para);
	  
    spin_lock(&hm2056_drv_lock);
    HM2056Sensor.iWB = para;
    spin_unlock(&hm2056_drv_lock);   
    
    switch (para)
    {
    case AWB_MODE_AUTO:
	HM2056_write_cmos_sensor(0x0380,(HM2056_read_cmos_sensor(0x0380)|0x02));// select Auto WB
	//HM2056_write_cmos_sensor(0x0380, 0xFF);   // select Auto WB
	HM2056_write_cmos_sensor(0x0101, 0xFF);  
        break;
    case AWB_MODE_CLOUDY_DAYLIGHT: //cloudy
    HM2056_write_cmos_sensor(0x0005, 0x00);
	HM2056_write_cmos_sensor(0x0380,(HM2056_read_cmos_sensor(0x0380)&0xFD));//Disable AWB
	//HM2056_write_cmos_sensor(0x0380, 0xFD);  	//Disable AWB
	HM2056_write_cmos_sensor(0x032D, 0x70);
	HM2056_write_cmos_sensor(0x032E, 0x01); 	//Red
	HM2056_write_cmos_sensor(0x032F, 0x00);
	HM2056_write_cmos_sensor(0x0330, 0x01);		//Green
	HM2056_write_cmos_sensor(0x0331, 0x08);
	HM2056_write_cmos_sensor(0x0332, 0x01);		//Blue
	
	HM2056_write_cmos_sensor(0x0000, 0xFF);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
	HM2056_write_cmos_sensor(0x0005, 0x01);
        break;
    case AWB_MODE_DAYLIGHT: //sunny
     HM2056_write_cmos_sensor(0x0005, 0x00);
	HM2056_write_cmos_sensor(0x0380,(HM2056_read_cmos_sensor(0x0380)&0xFD));//Disable AWB
	//HM2056_write_cmos_sensor(0x0380, 0xFD);  	//Disable AWB
	HM2056_write_cmos_sensor(0x032D, 0x60);
	HM2056_write_cmos_sensor(0x032E, 0x01); 	//Red
	HM2056_write_cmos_sensor(0x032F, 0x00);
	HM2056_write_cmos_sensor(0x0330, 0x01);		//Green
	HM2056_write_cmos_sensor(0x0331, 0x20);
	HM2056_write_cmos_sensor(0x0332, 0x01);		//Blue
	HM2056_write_cmos_sensor(0x0000, 0xFF);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
	HM2056_write_cmos_sensor(0x0005, 0x01);
        break;
    case AWB_MODE_INCANDESCENT: //office
     HM2056_write_cmos_sensor(0x0005, 0x00);
	HM2056_write_cmos_sensor(0x0380,(HM2056_read_cmos_sensor(0x0380)&0xFD));//Disable AWB
	//HM2056_write_cmos_sensor(0x0380, 0xFD);  	//Disable AWB
	HM2056_write_cmos_sensor(0x032D, 0x00);
	HM2056_write_cmos_sensor(0x032E, 0x01); 	//Red
	HM2056_write_cmos_sensor(0x032F, 0x14);
	HM2056_write_cmos_sensor(0x0330, 0x01);		//Green
	HM2056_write_cmos_sensor(0x0331, 0xD6);
	HM2056_write_cmos_sensor(0x0332, 0x01);		//Blue
	HM2056_write_cmos_sensor(0x0000, 0xFF);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
	HM2056_write_cmos_sensor(0x0005, 0x01);
        break;
    case AWB_MODE_TUNGSTEN: //home
     HM2056_write_cmos_sensor(0x0005, 0x00);
	HM2056_write_cmos_sensor(0x0380,(HM2056_read_cmos_sensor(0x0380)&0xFD));//Disable AWB
	//HM2056_write_cmos_sensor(0x0380, 0xFD);  	//Disable AWB
	HM2056_write_cmos_sensor(0x032D, 0x10);
	HM2056_write_cmos_sensor(0x032E, 0x01); 	//Red
	HM2056_write_cmos_sensor(0x032F, 0x00);
	HM2056_write_cmos_sensor(0x0330, 0x01);		//Green
	HM2056_write_cmos_sensor(0x0331, 0xA0);
	HM2056_write_cmos_sensor(0x0332, 0x01);		//Blue
	HM2056_write_cmos_sensor(0x0000, 0xFF);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
	HM2056_write_cmos_sensor(0x0005, 0x01);
        break;
    case AWB_MODE_FLUORESCENT:
	HM2056_write_cmos_sensor(0x0005, 0x00);
	HM2056_write_cmos_sensor(0x0380,(HM2056_read_cmos_sensor(0x0380)&0xFD));//Disable AWB
	//HM2056_write_cmos_sensor(0x0380, 0xFD);  	//Disable AWB
	HM2056_write_cmos_sensor(0x032D, 0x34);
	HM2056_write_cmos_sensor(0x032E, 0x01); 	//Red
	HM2056_write_cmos_sensor(0x032F, 0x00);
	HM2056_write_cmos_sensor(0x0330, 0x01);		//Green
	HM2056_write_cmos_sensor(0x0331, 0x92);
	HM2056_write_cmos_sensor(0x0332, 0x01);		//Blue
	HM2056_write_cmos_sensor(0x0000, 0xFF);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
	HM2056_write_cmos_sensor(0x0005, 0x01);
        break; 
    default:
        return FALSE;
    }

    return TRUE;
} /* HM2056_set_param_wb */

BOOL HM2056_set_param_effect(UINT16 para)
{
   BOOL  ret = TRUE;
   //UINT8  temp_reg;
   //temp_reg=HM2056_read_cmos_sensor(0x3391);
    switch (para)
    {
    case MEFFECT_OFF:
	HM2056_write_cmos_sensor(0x0005, 0x00);
	HM2056_write_cmos_sensor(0x0000, 0x01);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
	HM2056_write_cmos_sensor(0x0488, 0x10); //[0]:Image scense, [1]:Image Xor
	HM2056_write_cmos_sensor(0x0486, 0x00); //Hue, sin                       
	HM2056_write_cmos_sensor(0x0487, 0xFF); //Hue, cos
	HM2056_write_cmos_sensor(0x0120,(HM2056_read_cmos_sensor(0x0120)|0x10));
	//HM2056_write_cmos_sensor(0x0120, 0x37);
	HM2056_write_cmos_sensor(0x0005, 0x01);
	HM2056_write_cmos_sensor(0x0000, 0x01);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
        break;
    case MEFFECT_SEPIA:
	HM2056_write_cmos_sensor(0x0005, 0x00);
	HM2056_write_cmos_sensor(0x0000, 0x01);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
	HM2056_write_cmos_sensor(0x0486, 0x40); //Hue, sin						 
	HM2056_write_cmos_sensor(0x0487, 0xA0); //Hue, cos
	HM2056_write_cmos_sensor(0x0488, 0x11); //[0]:Image scense, [1]:Image Xor
	HM2056_write_cmos_sensor(0x0120,(HM2056_read_cmos_sensor(0x0120)&0xEF));
	//HM2056_write_cmos_sensor(0x0120, 0x27);
	HM2056_write_cmos_sensor(0x0005, 0x01);
	HM2056_write_cmos_sensor(0x0000, 0x01);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
        break;
    case MEFFECT_NEGATIVE:
	HM2056_write_cmos_sensor(0x0005, 0x00);
	HM2056_write_cmos_sensor(0x0000, 0x01);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
	HM2056_write_cmos_sensor(0x0488, 0x12); //[0]:Image scense, [1]:Image Xor
	HM2056_write_cmos_sensor(0x0486, 0x00); //Hue, sin						 
	HM2056_write_cmos_sensor(0x0487, 0xFF); //Hue, cos
	HM2056_write_cmos_sensor(0x0120,(HM2056_read_cmos_sensor(0x0120)|0x10));
	//HM2056_write_cmos_sensor(0x0120, 0x37);
	HM2056_write_cmos_sensor(0x0005, 0x01);
	HM2056_write_cmos_sensor(0x0000, 0x01);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
        break;
    case MEFFECT_SEPIAGREEN:
	HM2056_write_cmos_sensor(0x0005, 0x00);
	HM2056_write_cmos_sensor(0x0000, 0x01);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
	HM2056_write_cmos_sensor(0x0486, 0x60); //Hue, sin						 
	HM2056_write_cmos_sensor(0x0487, 0x60); //Hue, cos
	HM2056_write_cmos_sensor(0x0488, 0x11); //[0]:Image scense, [1]:Image Xor
	HM2056_write_cmos_sensor(0x0120,(HM2056_read_cmos_sensor(0x0120)&0xEF));
	//HM2056_write_cmos_sensor(0x0120, 0x27);
	HM2056_write_cmos_sensor(0x0005, 0x01);
	HM2056_write_cmos_sensor(0x0000, 0x01);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
        break;
    case MEFFECT_SEPIABLUE:
	HM2056_write_cmos_sensor(0x0005, 0x00);
	HM2056_write_cmos_sensor(0x0000, 0x01);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
	HM2056_write_cmos_sensor(0x0486, 0xB0); //Hue, sin						 
	HM2056_write_cmos_sensor(0x0487, 0x80); //Hue, cos
	HM2056_write_cmos_sensor(0x0488, 0x11); //[0]:Image scense, [1]:Image Xor
	HM2056_write_cmos_sensor(0x0120,(HM2056_read_cmos_sensor(0x0120)&0xEF));
	//HM2056_write_cmos_sensor(0x0120, 0x27);
	HM2056_write_cmos_sensor(0x0005, 0x01);
	HM2056_write_cmos_sensor(0x0000, 0x01);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
        break;
	case MEFFECT_MONO: //B&W
	HM2056_write_cmos_sensor(0x0005, 0x00);
	HM2056_write_cmos_sensor(0x0000, 0x01);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
	HM2056_write_cmos_sensor(0x0486, 0x80); //Hue, sin						 
	HM2056_write_cmos_sensor(0x0487, 0x80); //Hue, cos
	HM2056_write_cmos_sensor(0x0488, 0x11); //[0]:Image scense, [1]:Image Xor
	HM2056_write_cmos_sensor(0x0120,(HM2056_read_cmos_sensor(0x0120)&0xEF));
	//M2056_write_cmos_sensor(0x0120, 0x27);
	HM2056_write_cmos_sensor(0x0005, 0x01);
	HM2056_write_cmos_sensor(0x0000, 0x01);
	HM2056_write_cmos_sensor(0x0100, 0xFF);
	HM2056_write_cmos_sensor(0x0101, 0xFF);
		break; 
    default:
        ret = FALSE;
    }

    return ret;

} /* HM2056_set_param_effect */

BOOL HM2056_set_param_banding(UINT16 para)
{

    kal_uint8 banding;

    //banding = HM2056_read_cmos_sensor(0x3014);
    switch (para)
    {
	case AE_FLICKER_MODE_50HZ:
		HM2056_Banding_setting = AE_FLICKER_MODE_50HZ;
		HM2056_write_cmos_sensor(0x0120,(HM2056_read_cmos_sensor(0x0120)&0xFE)); //[0]:0=>50Hz
		HM2056_write_cmos_sensor(0x0015,(HM2056_read_cmos_sensor(0x0015)|0x01));
		HM2056_write_cmos_sensor(0x0016,(HM2056_read_cmos_sensor(0x0016)|0x1D));
		//HM2056_write_cmos_sensor(0x0120, 0x36);	//[0]:0=>50Hz
		HM2056_write_cmos_sensor(0x0000, 0xFF);	//AE CMU   
		HM2056_write_cmos_sensor(0x0100, 0xFF);	//AE CMU    
		HM2056_write_cmos_sensor(0x0101, 0xFF);	//AE CMU    
		break;
    case AE_FLICKER_MODE_60HZ:			
        HM2056_Banding_setting = AE_FLICKER_MODE_60HZ;
		HM2056_write_cmos_sensor(0x0120,(HM2056_read_cmos_sensor(0x0120)|0x01)); //[0]:1=>60Hz
		HM2056_write_cmos_sensor(0x0015,(HM2056_read_cmos_sensor(0x0015)|0x01));
		HM2056_write_cmos_sensor(0x0016,(HM2056_read_cmos_sensor(0x0016)|0x1D));
		//HM2056_write_cmos_sensor(0x0120, 0x37);	//[0]:1=>60Hz
		HM2056_write_cmos_sensor(0x0000, 0xFF);	//AE CMU   
		HM2056_write_cmos_sensor(0x0100, 0xFF);	//AE CMU    
		HM2056_write_cmos_sensor(0x0101, 0xFF);	//AE CMU 		 
        break;
      default:
          return FALSE;
    }

    return TRUE;
} /* HM2056_set_param_banding */

BOOL HM2056_set_param_exposure(UINT16 para)
{
		
		HM2056_write_cmos_sensor(0x125,(HM2056_read_cmos_sensor(0x125)|0x40));//make sure the Y Brightness control is opened.

    switch (para)
    {
    case AE_EV_COMP_n13:
		HM2056_write_cmos_sensor(0x04C0,0xC0);
		HM2056_write_cmos_sensor(0x038E,0x1A);	// 20-28-30
		HM2056_write_cmos_sensor(0x0381,0x24);
		HM2056_write_cmos_sensor(0x0382,0x10);
		HM2056_write_cmos_sensor(0x0100,0xFF);
        break;
    case AE_EV_COMP_n10:
		HM2056_write_cmos_sensor(0x04C0,0xB0);
		HM2056_write_cmos_sensor(0x038E,0x22);	// 28-30-38
		HM2056_write_cmos_sensor(0x0381,0x2C);
		HM2056_write_cmos_sensor(0x0382,0x18);
		HM2056_write_cmos_sensor(0x0100,0xFF);
        break;
    case AE_EV_COMP_n07:
		HM2056_write_cmos_sensor(0x04C0,0xA0);
		HM2056_write_cmos_sensor(0x038E,0x2A);  // 30-38-40
		HM2056_write_cmos_sensor(0x0381,0x34);
		HM2056_write_cmos_sensor(0x0382,0x20);
		HM2056_write_cmos_sensor(0x0100,0xFF);
        break;
    case AE_EV_COMP_n03:
		HM2056_write_cmos_sensor(0x04C0,0x90);
		HM2056_write_cmos_sensor(0x038E,0x32);	// 38-40-48
		HM2056_write_cmos_sensor(0x0381,0x3C);
		HM2056_write_cmos_sensor(0x0382,0x28);
		HM2056_write_cmos_sensor(0x0100,0xFF); 
        break;
    case AE_EV_COMP_00:
		HM2056_write_cmos_sensor(0x04C0,0x00);
		HM2056_write_cmos_sensor(0x038E,0x3A);	// 40-48-50
		HM2056_write_cmos_sensor(0x0381,0x44);
		HM2056_write_cmos_sensor(0x0382,0x30);
		HM2056_write_cmos_sensor(0x0100,0xFF);
        break;
    case AE_EV_COMP_03:
		HM2056_write_cmos_sensor(0x04C0,0x10);	
		HM2056_write_cmos_sensor(0x038E,0x42);	// 48-50-58
		HM2056_write_cmos_sensor(0x0381,0x4C);
		HM2056_write_cmos_sensor(0x0382,0x38);
		HM2056_write_cmos_sensor(0x0100,0xFF); 
        break;
    case AE_EV_COMP_07:
		HM2056_write_cmos_sensor(0x04C0,0x20);	
		HM2056_write_cmos_sensor(0x038E,0x4A);	// 50-58-60
		HM2056_write_cmos_sensor(0x0381,0x54);
		HM2056_write_cmos_sensor(0x0382,0x40);
		HM2056_write_cmos_sensor(0x0100,0xFF); 
        break;
    case AE_EV_COMP_10:
		HM2056_write_cmos_sensor(0x04C0,0x30);	
		HM2056_write_cmos_sensor(0x038E,0x52);	// 58-60-68
		HM2056_write_cmos_sensor(0x0381,0x5C);
		HM2056_write_cmos_sensor(0x0382,0x48);
		HM2056_write_cmos_sensor(0x0100,0xFF);
        break;
    case AE_EV_COMP_13:
		HM2056_write_cmos_sensor(0x04C0,0x40);
		HM2056_write_cmos_sensor(0x038E,0x5A);	// 60-68-70
		HM2056_write_cmos_sensor(0x0381,0x54);
		HM2056_write_cmos_sensor(0x0382,0x40);
		HM2056_write_cmos_sensor(0x0100,0xFF);
        break;
    default:
        return FALSE;
    }

    return TRUE;
} /* HM2056_set_param_exposure */

BOOL HM2056_set_param_iso(UINT16 para)
{
	  kal_uint32 again = 0, dgain = 0;
	  
	  SENSORDB("HM2056_param_set_iso para=%d\n",para);
	  again = HM2056_read_cmos_sensor(0x0018); //A gain 
    dgain = HM2056_read_cmos_sensor(0x001D); //D gain 
    
    switch (para)
    {
			case AE_ISO_AUTO:
				HM2056_write_cmos_sensor(0x0392,0x03);
				HM2056_write_cmos_sensor(0x0393,0xff);
				//HM2056_write_cmos_sensor(0x0393,0x80);
				SENSORDB("HM2056 read register_isoauto, 0x0018 = %x\n", again);
				SENSORDB("HM2056 read register_isoauto, 0x001d = %x\n", dgain);
				HM2056_write_cmos_sensor(0x0000,0x01); 
				HM2056_write_cmos_sensor(0x0100,0x01); 
				HM2056_write_cmos_sensor(0x0101,0x01); 
				break;
			case AE_ISO_100:
				HM2056_write_cmos_sensor(0x0392,0x00);
				HM2056_write_cmos_sensor(0x0393,0x80);
				SENSORDB("HM2056 read register_iso100, 0x0018 = %x\n", again);
				SENSORDB("HM2056 read register_iso100, 0x001d = %x\n", dgain);
				HM2056_write_cmos_sensor(0x0000,0x01); 
				HM2056_write_cmos_sensor(0x0100,0x01); 
				HM2056_write_cmos_sensor(0x0101,0x01); 
				break;
			case AE_ISO_200:
				HM2056_write_cmos_sensor(0x0392,0x01);
				HM2056_write_cmos_sensor(0x0393,0x80);
				SENSORDB("HM2056 read register_iso200, 0x0018 = %x\n", again);
				SENSORDB("HM2056 read register_iso200, 0x001d = %x\n", dgain);
				HM2056_write_cmos_sensor(0x0000,0x01); 
				HM2056_write_cmos_sensor(0x0100,0x01); 
				HM2056_write_cmos_sensor(0x0101,0x01); 
				break;
			case AE_ISO_400:
				HM2056_write_cmos_sensor(0x0392,0x02);
				
				HM2056_write_cmos_sensor(0x0393,0x80);
				SENSORDB("HM2056 read register_iso400, 0x0018 = %x\n", again);
				SENSORDB("HM2056 read register_iso400, 0x001d = %x\n", dgain);
				HM2056_write_cmos_sensor(0x0000,0x01);
				HM2056_write_cmos_sensor(0x0100,0x01);
				HM2056_write_cmos_sensor(0x0101,0x01);
				break;
			case AE_ISO_800:                    
				HM2056_write_cmos_sensor(0x0392,0x03);
				HM2056_write_cmos_sensor(0x0393,0x80);
				SENSORDB("HM2056 read register_iso800, 0x0018 = %x\n", again);
				SENSORDB("HM2056 read register_iso800, 0x001d = %x\n", dgain);
				HM2056_write_cmos_sensor(0x0000,0x01);
				HM2056_write_cmos_sensor(0x0100,0x01);
				HM2056_write_cmos_sensor(0x0101,0x01);
				break;
			case AE_ISO_1600:
				HM2056_write_cmos_sensor(0x0392,0x03);
				HM2056_write_cmos_sensor(0x0393,0xFF);
				SENSORDB("HM2056 read register_iso1600, 0x0018 = %x\n", again);
				SENSORDB("HM2056 read register_iso1600, 0x001d = %x\n", dgain);
				HM2056_write_cmos_sensor(0x0000,0x01);
				HM2056_write_cmos_sensor(0x0100,0x01);
				HM2056_write_cmos_sensor(0x0101,0x01);			
				break;
    default:
        break;
    }
    return;
}

UINT32 HM2056YUVSensorSetting(FEATURE_ID iCmd, UINT32 iPara)
{
//   if( HM2056_sensor_cap_state == KAL_TRUE)
//	   return TRUE;
	
	switch (iCmd) {
	case FID_SCENE_MODE:	    
//	    printk("Set Scene Mode:%d\n", iPara); 
	    if (iPara == SCENE_MODE_OFF)
	    {
	        HM2056_night_mode(0); 
	    }
	    else if (iPara == SCENE_MODE_NIGHTSCENE)
	    {
               HM2056_night_mode(1); 
	    }	    
	    break; 	    
	case FID_AWB_MODE:
//	    printk("Set AWB Mode:%d\n", iPara); 	    
           HM2056_set_param_wb(iPara);
	break;
	case FID_COLOR_EFFECT:
//	    printk("Set Color Effect:%d\n", iPara); 	    	    
           HM2056_set_param_effect(iPara);
	break;
	case FID_AE_EV: 	    
//           printk("Set EV:%d\n", iPara); 	    	    
           HM2056_set_param_exposure(iPara);
	break;
	case FID_AE_FLICKER:
//           printk("Set Flicker:%d\n", iPara); 	    	    	    
           HM2056_set_param_banding(iPara);
	break;
	case FID_ZOOM_FACTOR:
	    zoom_factor = iPara; 
	break;
	case FID_AE_ISO:
            HM2056_set_param_iso(iPara);
  break;
	default:
	break;
	}
	return TRUE;
}   /* MT9P012YUVSensorSetting */

UINT32 HM2056YUVSetVideoMode(UINT16 u2FrameRate)
{
    kal_uint8 iTemp;
    /* to fix VSYNC, to fix frame rate */
    //printk("Set YUV Video Mode \n");     
   // iTemp = HM2056_read_cmos_sensor(0x3014);
    //HM2056_write_cmos_sensor(0x3014, iTemp & 0xf7); //Disable night mode

    if (u2FrameRate == 30)
    {
        //HM2056_write_cmos_sensor(0x302d, 0x00);
        //HM2056_write_cmos_sensor(0x302e, 0x00);
    }
    else if (u2FrameRate == 15)       
    {
        //HM2056_write_cmos_sensor(0x300e, 0x34);
        //HM2056_write_cmos_sensor(0x302A, HM2056_VIDEO_15FPS_FRAME_LENGTH>>8);  /*  15fps*/
        //HM2056_write_cmos_sensor(0x302B, HM2056_VIDEO_15FPS_FRAME_LENGTH&0xFF);
                
        // clear extra exposure line
   // HM2056_write_cmos_sensor(0x302d, 0x00);
    //HM2056_write_cmos_sensor(0x302e, 0x00);
    }
    else 
    {
        printk("Wrong frame rate setting \n");
    }
    HM2056_VEDIO_encode_mode = KAL_TRUE; 
        
    return TRUE;
}
/*********************************0915*****************************************/
void HM2056GetExifInfo(UINT32 exifAddr)
{
	kal_uint16 msb;//0925
	kal_uint16 lsb;//0925
	kal_uint16 again;//0925
	kal_uint16 dgain;//0925
	kal_uint16 aetarget;//1006
	kal_uint16 imgcfg;//1023
	
		msb=HM2056_read_cmos_sensor(0x0015);//0925
		lsb=HM2056_read_cmos_sensor(0x0016);//0925
		again = HM2056_read_cmos_sensor(0x0018); //A gain 0925
  	//again = 1<< again;
  	dgain = HM2056_read_cmos_sensor(0x001D); //D gain 0925
  	aetarget = HM2056_read_cmos_sensor(0x0385);//ae_target 1006
  	imgcfg = HM2056_read_cmos_sensor(0x000F);//1023
  	
	SENSORDB("HM2056GetExifInfo\n");
				SENSORDB("CAP_HM2056 read register, 0x0015 = %x\n", msb);//0925
				SENSORDB("HM2056 read register, 0x0016 = %x\n", lsb);//0925
				SENSORDB("HM2056 read register, 0x0018 = %x\n", again);//0925
				SENSORDB("HM2056 read register, 0x001d = %x\n", dgain);//0925
				SENSORDB("HM2056 read register, 0x0385 = %x\n", aetarget);//1006
				SENSORDB("HM2056 read register, 0x000F = %x\n", imgcfg);//1023	
   HM2056_get_exposure_gain();
				
	SENSOR_EXIF_INFO_STRUCT* pExifInfo = (SENSOR_EXIF_INFO_STRUCT*)exifAddr;
  pExifInfo->FNumber = 28;
  pExifInfo->AEISOSpeed = HM2056Sensor.iso;
  pExifInfo->FlashLightTimeus = 0;
  pExifInfo->RealISOValue = HM2056Sensor.iso;
  pExifInfo->CapExposureTime = HM2056Sensor.iShutter;
	pExifInfo->AWBMode = HM2056Sensor.iWB;
}
/*********************************0915*****************************************/
UINT32 HM2056FeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
							 UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{
	UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
	UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
	UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
	UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
	MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
	MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara; 
	
	switch (FeatureId)
	{
		case SENSOR_FEATURE_GET_RESOLUTION:
			*pFeatureReturnPara16++=HM2056_IMAGE_SENSOR_FULL_WIDTH;
			*pFeatureReturnPara16=HM2056_IMAGE_SENSOR_FULL_HEIGHT;
			*pFeatureParaLen=4;
		break;
		case SENSOR_FEATURE_GET_PERIOD:
			*pFeatureReturnPara16++=HM2056_PV_PERIOD_PIXEL_NUMS+HM2056_PV_dummy_pixels;
			*pFeatureReturnPara16=HM2056_PV_PERIOD_LINE_NUMS+HM2056_PV_dummy_lines;
			*pFeatureParaLen=4;
		break;
		case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
			*pFeatureReturnPara32 = HM2056_sensor_pclk/10;
			*pFeatureParaLen=4;
		break;
		case SENSOR_FEATURE_SET_ESHUTTER:
		break;
		case SENSOR_FEATURE_SET_NIGHTMODE:
			HM2056_night_mode((BOOL) *pFeatureData16);
		break;
		case SENSOR_FEATURE_SET_GAIN:
		case SENSOR_FEATURE_SET_FLASHLIGHT:
		break;
		case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
			HM2056_isp_master_clock=*pFeatureData32;
		break;
		case SENSOR_FEATURE_SET_REGISTER:
			HM2056_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
		break;
		case SENSOR_FEATURE_GET_REGISTER:
			pSensorRegData->RegData = HM2056_read_cmos_sensor(pSensorRegData->RegAddr);
		break;
		case SENSOR_FEATURE_GET_CONFIG_PARA:
			memcpy(pSensorConfigData, &HM2056SensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
			*pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
		break;
		case SENSOR_FEATURE_SET_CCT_REGISTER:
		case SENSOR_FEATURE_GET_CCT_REGISTER:
		case SENSOR_FEATURE_SET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_REGISTER_DEFAULT:

		case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
		case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
		//case SENSOR_FEATURE_GET_GROUP_COUNT:
		case SENSOR_FEATURE_GET_GROUP_INFO:
		case SENSOR_FEATURE_GET_ITEM_INFO:
		case SENSOR_FEATURE_SET_ITEM_INFO:
		case SENSOR_FEATURE_GET_ENG_INFO:
		break;
		case SENSOR_FEATURE_GET_GROUP_COUNT:
                        *pFeatureReturnPara32++=0;
                        *pFeatureParaLen=4;	    
		    break; 
		case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
			// get the lens driver ID from EEPROM or just return LENS_DRIVER_ID_DO_NOT_CARE
			// if EEPROM does not exist in camera module.
			*pFeatureReturnPara32=LENS_DRIVER_ID_DO_NOT_CARE;
			*pFeatureParaLen=4;
		break;
		case SENSOR_FEATURE_SET_YUV_CMD:
//		       printk("HM2056 YUV sensor Setting:%d, %d \n", *pFeatureData32,  *(pFeatureData32+1));
			HM2056YUVSensorSetting((FEATURE_ID)*pFeatureData32, *(pFeatureData32+1));
		break; 	
		case SENSOR_FEATURE_CHECK_SENSOR_ID:
			HM2056GetSensorID(pFeatureData32);
		break;
		case SENSOR_FEATURE_SET_VIDEO_MODE:
		       HM2056YUVSetVideoMode(*pFeatureData16);
		       break; 
		case SENSOR_FEATURE_GET_EXIF_INFO:
            HM2056GetExifInfo(*pFeatureData32);       
            break;		       
		default:
			break;			
	}
	return ERROR_NONE;
}	/* HM2056FeatureControl() *///by Michael.Xie  2010/11/30
SENSOR_FUNCTION_STRUCT	SensorFuncHM2056=
{
	HM2056Open,
	HM2056GetInfo,
	HM2056GetResolution,
	HM2056FeatureControl,
	HM2056Control,
	HM2056Close
};

UINT32 HM2056_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
	/* To Do : Check Sensor status here */
	if (pfFunc!=NULL)
		*pfFunc=&SensorFuncHM2056;

	return ERROR_NONE;
}	/* SensorInit() */

