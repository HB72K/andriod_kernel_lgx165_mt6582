/* 613Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   sensor.c
 *
 * Project:
 * --------
 *   DUMA
 *
 * Description:
 * ------------
 *   Source code of Sensor driver
 *
 *
 * Author:
 * -------
 *   PC Huang (MTK02204)
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 *
 * 10 12 2010 sean.cheng
 * [ALPS00021722] [Need Patch] [Volunteer Patch][Camera]MT6573 Camera related function
 * .rollback the lib3a for mt6573 camera related files
 *
 * 09 10 2010 jackie.su
 * [ALPS00002279] [Need Patch] [Volunteer Patch] ALPS.Wxx.xx Volunteer patch for
 * .alps dual sensor
 *
 * 09 02 2010 jackie.su
 * [ALPS00002279] [Need Patch] [Volunteer Patch] ALPS.Wxx.xx Volunteer patch for
 * .roll back dual sensor
 *
 * 07 27 2010 sean.cheng
 * [ALPS00003112] [Need Patch] [Volunteer Patch] ISP/Sensor driver modification for Customer support
 * .1. add master clock switcher 
 *  2. add master enable/disable 
 *  3. add dummy line/pixel for sensor 
 *  4. add sensor driving current setting
 *
 * 07 01 2010 sean.cheng
 * [ALPS00121215][Camera] Change color when switch low and high 
 * .Add video delay frame.
 *
 * 07 01 2010 sean.cheng
 * [ALPS00002805][Need Patch] [Volunteer Patch]10X Patch for DS269 Video Frame Rate 
 * .Change the sensor clock to let frame rate to be 30fps in vidoe mode
 *
 * 06 13 2010 sean.cheng
 * [ALPS00002514][Need Patch] [Volunteer Patch] ALPS.10X.W10.11 Volunteer patch for E1k Camera 
 * .
 * 1. Add set zoom factor and capdelay frame for YUV sensor 
 * 2. Modify e1k sensor setting
 *
 * 05 25 2010 sean.cheng
 * [ALPS00002250][Need Patch] [Volunteer Patch] ALPS.10X.W10.11 Volunteer patch for YUV video frame rate 
 * .
 * Add 15fps option for video mode
 *
 * 05 03 2010 sean.cheng
 * [ALPS00001357][Meta]CameraTool 
 * .
 * Fix OV2685 MIPI YUV sensor frame rate to 30fps in vidoe mode
 *
 * Mar 4 2010 mtk70508
 * [DUMA00154792] Sensor driver
 * 
 *
 * Mar 4 2010 mtk70508
 * [DUMA00154792] Sensor driver
 * 
 *
 * Mar 1 2010 mtk01118
 * [DUMA00025869] [Camera][YUV I/F & Query feature] check in camera code
 * 
 *
 * Feb 24 2010 mtk01118
 * [DUMA00025869] [Camera][YUV I/F & Query feature] check in camera code
 * 
 *
 * Nov 24 2009 mtk02204
 * [DUMA00015869] [Camera Driver] Modifiy camera related drivers for dual/backup sensor/lens drivers.
 * 
 *
 * Oct 29 2009 mtk02204
 * [DUMA00015869] [Camera Driver] Modifiy camera related drivers for dual/backup sensor/lens drivers.
 * 
 *
 * Oct 27 2009 mtk02204
 * [DUMA00015869] [Camera Driver] Modifiy camera related drivers for dual/backup sensor/lens drivers.
 * 
 *
 * Aug 13 2009 mtk01051
 * [DUMA00009217] [Camera Driver] CCAP First Check In
 * 
 *
 * Aug 5 2009 mtk01051
 * [DUMA00009217] [Camera Driver] CCAP First Check In
 * 
 *
 * Jul 17 2009 mtk01051
 * [DUMA00009217] [Camera Driver] CCAP First Check In
 * 
 *
 * Jul 7 2009 mtk01051
 * [DUMA00008051] [Camera Driver] Add drivers for camera high ISO binning mode.
 * Add ISO query info for Sensor
 *
 * May 18 2009 mtk01051
 * [DUMA00005921] [Camera] LED Flashlight first check in
 * 
 *
 * May 16 2009 mtk01051
 * [DUMA00005921] [Camera] LED Flashlight first check in
 * 
 *
 * May 16 2009 mtk01051
 * [DUMA00005921] [Camera] LED Flashlight first check in
 * 
 *
 * Apr 7 2009 mtk02204
 * [DUMA00004012] [Camera] Restructure and rename camera related custom folders and folder name of came
 * 
 *
 * Mar 27 2009 mtk02204
 * [DUMA00002977] [CCT] First check in of MT6516 CCT drivers
 *
 *
 * Mar 25 2009 mtk02204
 * [DUMA00111570] [Camera] The system crash after some operations
 *
 *
 * Mar 20 2009 mtk02204
 * [DUMA00002977] [CCT] First check in of MT6516 CCT drivers
 *
 *
 * Mar 2 2009 mtk02204
 * [DUMA00001084] First Check in of MT6516 multimedia drivers
 *
 *
 * Feb 24 2009 mtk02204
 * [DUMA00001084] First Check in of MT6516 multimedia drivers
 *
 *
 * Dec 27 2008 MTK01813
 * DUMA_MBJ CheckIn Files
 * created by clearfsimport
 *
 * Dec 10 2008 mtk02204
 * [DUMA00001084] First Check in of MT6516 multimedia drivers
 *
 *
 * Oct 27 2008 mtk01051
 * [DUMA00000851] Camera related drivers check in
 * Modify Copyright Header
 *
 * Oct 24 2008 mtk02204
 * [DUMA00000851] Camera related drivers check in
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
//#include <windows.h>
//#include <memory.h>
//#include <nkintr.h>
//#include <ceddk.h>
//#include <ceddk_exp.h>

//#include "kal_release.h"
//#include "i2c_exp.h"
//#include "gpio_exp.h"
//#include "msdk_exp.h"
//#include "msdk_sensor_exp.h"
//#include "msdk_isp_exp.h"
//#include "base_regs.h"
//#include "Sensor.h"
//#include "camera_sensor_para.h"
//#include "CameraCustomized.h"
/*
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>*/
//#include <mach/mt6516_pll.h>

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/xlog.h>
#include <asm/atomic.h>
#include <asm/system.h>

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"

/*
#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"*/
#include "kd_camera_feature.h"
#include "ov2685mipiyuv_Sensor.h"
#include "ov2685mipiyuv_Camera_Sensor_para.h"
#include "ov2685mipiyuv_CameraCustomized.h"

#define OV2685PFX "[OV2685]"

#define OV2685_DBG_FUNC(fmt, arg...)    xlog_printk(ANDROID_LOG_ERROR, OV2685PFX , fmt, ##arg)


#define OV2685MIPIYUV_DEBUG
#ifdef OV2685MIPIYUV_DEBUG
#define SENSORDB OV2685_DBG_FUNC //printk
#else
#define SENSORDB(x,...)
#endif

#define __SLT_DRV_OV3660_BURST_SHOTS__	//Debug: Brightless became lower and lower in burst shot mode
#ifdef __SLT_DRV_OV3660_BURST_SHOTS__	//wujinyou, 2011.11.21
static kal_uint8 preview_init_flag = 0;
#endif
//<2014/12/26-joubertshe, [Y20][BUG][Common][BSP][Internal][]camera sunset and night mode issue fix
//UINT16 wb_mode_value=0;
//>2014/12/26-joubertshe
static DEFINE_SPINLOCK(ov2685_drv_lock);

extern int iReadReg(u16 a_u2Addr , u8 * a_puBuff , u16 i2cId);
extern int iWriteReg(u16 a_u2Addr , u32 a_u4Data , u32 a_u4Bytes , u16 i2cId);
#define OV2685_MIPI_write_cmos_sensor(addr, para) iWriteReg((u16) addr , (u32) para ,1,OV2685_MIPI_WRITE_ID)
#define OV2685_MIPI_write_cmos_sensor_2(addr, para, bytes) iWriteReg((u16) addr , (u32) para ,bytes,OV2685_MIPI_WRITE_ID)
kal_uint16 OV2685_MIPI_read_cmos_sensor(kal_uint32 addr)
{
kal_uint16 get_byte=0;
iReadReg((u16) addr ,(u8*)&get_byte,OV2685_MIPI_WRITE_ID);
return get_byte;
}



/*******************************************************************************
* // Adapter for Winmo typedef 
********************************************************************************/
#define WINMO_USE 0

#define Sleep(ms) mdelay(ms)
#define RETAILMSG(x,...)
#define TEXT


/*******************************************************************************
* // End Adapter for Winmo typedef 
********************************************************************************/


#define	OV2685_MIPI_LIMIT_EXPOSURE_LINES				        (1253)
#define	OV2685_MIPI_VIDEO_NORMALMODE_30FRAME_RATE       (30)
#define	OV2685_MIPI_VIDEO_NORMALMODE_FRAME_RATE         (30)
#define	OV2685_MIPI_VIDEO_NIGHTMODE_FRAME_RATE          (9)
#define BANDING50_30HZ
/* Global Valuable */

static kal_uint32 zoom_factor = 0; 
static kal_uint8 OV2685_MIPI_exposure_line_h = 0,OV2685_MIPI_exposure_line_m = 0, OV2685_MIPI_exposure_line_l = 0 ,OV2685_MIPI_gain =0;

static kal_bool OV2685_MIPI_gPVmode = KAL_TRUE; //PV size or Full size
static kal_bool OV2685_MIPI_VEDIO_encode_mode = KAL_FALSE; //Picture(Jpeg) or Video(Mpeg4)
static kal_bool OV2685_MIPI_sensor_cap_state = KAL_FALSE; //Preview or Capture

static kal_uint16 OV2685_MIPI_dummy_pixels=0, OV2685_MIPI_dummy_lines=0;

static kal_uint16 OV2685_MIPI_exposure_lines=0, OV2685_MIPI_extra_exposure_lines = 0;


static kal_int8 OV2685_MIPI_DELAY_AFTER_PREVIEW = -1;

static kal_uint8 OV2685_MIPI_Banding_setting = AE_FLICKER_MODE_50HZ;  //Wonder add

/****** OVT 6-18******/
static kal_uint16 OV2685_MIPI_Capture_Max_Gain16= 6*16;
static kal_uint16 OV2685_MIPI_Capture_Gain16=0 ;    
static kal_uint16 OV2685_MIPI_Capture_Shutter=0;
static kal_uint16 OV2685_MIPI_Capture_Extra_Lines=0;

static kal_uint16  OV2685_MIPI_PV_Dummy_Pixels =0,OV2685_MIPI_Capture_Dummy_Pixels =0, OV2685_MIPI_Capture_Dummy_Lines =0;
static kal_uint16  OV2685_MIPI_PV_Gain16 = 0;
static kal_uint16  OV2685_MIPI_PV_Shutter = 0;
static kal_uint16  OV2685_MIPI_PV_Extra_Lines = 0;

/*
kal_uint16 OV2685_MIPI_sensor_gain_base=0,OV2685_MIPI_FAC_SENSOR_REG=0,OV2685_MIPI_iOV2685_MIPI_Mode=0,OV2685_MIPI_max_exposure_lines=0;
kal_uint32 OV2685_MIPI_capture_pclk_in_M=520,OV2685_MIPI_preview_pclk_in_M=390,OV2685_MIPI_PV_dummy_pixels=0,OV2685_MIPI_PV_dummy_lines=0,OV2685_MIPI_isp_master_clock=0;
*/
kal_uint16 OV2685_MIPI_iOV2685_MIPI_Mode=0;
kal_uint32 OV2685_MIPI_capture_pclk_in_M=660,OV2685_MIPI_preview_pclk_in_M=660,OV2685_MIPI_PV_dummy_pixels=0,OV2685_MIPI_PV_dummy_lines=0,OV2685_MIPI_isp_master_clock=0;
//520, 390
static kal_uint32  OV2685_MIPI_sensor_pclk=660; //390 pclk 20140107
static kal_bool OV2685_MIPI_AWB_ENABLE = KAL_TRUE; 
static kal_bool OV2685_MIPI_AE_ENABLE = KAL_TRUE; 

static kal_bool OV2685_NIGHT_ENABLE = KAL_FALSE;  //added by sarah

static kal_uint32 Capture_Shutter = 0; 
static kal_uint32 Capture_Gain = 0; 

#if WINMO_USE
kal_uint8 OV2685_MIPI_sensor_write_I2C_address = OV2685_MIPI_WRITE_ID;
kal_uint8 OV2685_MIPI_sensor_read_I2C_address = OV2685_MIPI_READ_ID;

UINT32 OV2685GPIOBaseAddr;
HANDLE OV2685hGPIO;
HANDLE OV2685hDrvI2C;
I2C_TRANSACTION OV2685I2CConfig;
#endif 
UINT8 OV2685_MIPI_PixelClockDivider=0;

//SENSOR_REG_STRUCT OV2685SensorCCT[FACTORY_END_ADDR]=CAMERA_SENSOR_CCT_DEFAULT_VALUE;
//SENSOR_REG_STRUCT OV2685SensorReg[ENGINEER_END]=CAMERA_SENSOR_REG_DEFAULT_VALUE;
//	camera_para.SENSOR.cct	SensorCCT	=> SensorCCT
//	camera_para.SENSOR.reg	SensorReg
MSDK_SENSOR_CONFIG_STRUCT OV2685SensorConfigData;

#if WINMO_USE
void OV2685_MIPI_write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{

	//SENSORDB("enter OV2685_MIPI_write_cmos_sensor \n"); //debug sarah
//debug();


	OV2685I2CConfig.operation=I2C_OP_WRITE;
	OV2685I2CConfig.slaveAddr=OV2685_MIPI_sensor_write_I2C_address>>1;
	OV2685I2CConfig.transfer_num=1;	/* TRANSAC_LEN */
	OV2685I2CConfig.transfer_len=3;
	OV2685I2CConfig.buffer[0]=(UINT8)(addr>>8);
	OV2685I2CConfig.buffer[1]=(UINT8)(addr&0xFF);
	OV2685I2CConfig.buffer[2]=(UINT8)para;
	DRV_I2CTransaction(OV2685hDrvI2C, &OV2685I2CConfig);

	//SENSORDB("exit OV2685_MIPI_write_cmos_sensor \n"); //debug sarah
//debug();

}	/* OV2685_MIPI_write_cmos_sensor() */

kal_uint32 OV2685_MIPI_read_cmos_sensor(kal_uint32 addr)
{
	kal_uint8 get_byte=0xFF;

//SENSORDB("enter OV2685_MIPI_read_cmos_sensor \n"); //debug sarah
//debug();

	OV2685I2CConfig.operation=I2C_OP_WRITE;
	OV2685I2CConfig.slaveAddr=OV2685_MIPI_sensor_write_I2C_address>>1;
	OV2685I2CConfig.transfer_num=1;	/* TRANSAC_LEN */
	OV2685I2CConfig.transfer_len=2;
	OV2685I2CConfig.buffer[0]=(UINT8)(addr>>8);
	OV2685I2CConfig.buffer[1]=(UINT8)(addr&0xFF);
	DRV_I2CTransaction(OV2685hDrvI2C, &OV2685I2CConfig);

	OV2685I2CConfig.operation=I2C_OP_READ;
	OV2685I2CConfig.slaveAddr=OV2685_MIPI_sensor_read_I2C_address>>1;
	OV2685I2CConfig.transfer_num=1;	/* TRANSAC_LEN */
	OV2685I2CConfig.transfer_len=1;
	DRV_I2CTransaction(OV2685hDrvI2C, &OV2685I2CConfig);
	get_byte=OV2685I2CConfig.buffer[0];

	return get_byte;

	//SENSORDB("exit OV2685_MIPI_read_cmos_sensor \n"); //debug sarah
//debug();
}	/* OV2685_MIPI_read_cmos_sensor() */
#endif 

void debug(void)
{/*
	SENSORDB("OV2685 reg:5300,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5300)); //debug sarah
	SENSORDB("OV2685 reg:5301,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5301)); //debug sarah
	SENSORDB("OV2685 reg:5302,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5302)); //debug sarah
	SENSORDB("OV2685 reg:5303,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5303)); //debug sarah
	SENSORDB("OV2685 reg:5304,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5304)); //debug sarah
	SENSORDB("OV2685 reg:5305,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5305)); //debug sarah
	SENSORDB("OV2685 reg:5306,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5306)); //debug sarah
	SENSORDB("OV2685 reg:5307,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5307)); //debug sarah
	SENSORDB("OV2685 reg:5004,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5004)); //debug sarah
	SENSORDB("OV2685 reg:5500,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5500)); //debug sarah
	SENSORDB("OV2685 reg:5501,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5501)); //debug sarah
	SENSORDB("OV2685 reg:5502,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5502)); //debug sarah
	SENSORDB("OV2685 reg:5503,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5503)); //debug sarah
	SENSORDB("OV2685 reg:5504,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5504)); //debug sarah
	SENSORDB("OV2685 reg:5505,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5505)); //debug sarah
	SENSORDB("OV2685 reg:5506,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5506)); //debug sarah
	SENSORDB("OV2685 reg:5004,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5004)); //debug sarah
	SENSORDB("OV2685 reg:5280,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5280)); //debug sarah
	SENSORDB("OV2685 reg:5281,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5281)); //debug sarah
	SENSORDB("OV2685 reg:5282,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5282)); //debug sarah
	SENSORDB("OV2685 reg:5283,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5283)); //debug sarah
	SENSORDB("OV2685 reg:5284,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5284)); //debug sarah
	SENSORDB("OV2685 reg:5285,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5285)); //debug sarah
	SENSORDB("OV2685 reg:5286,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5286)); //debug sarah
	SENSORDB("OV2685 reg:5287,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x5287)); //debug sarah
	SENSORDB("OV2685 reg:3501,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x3501)); //debug sarah
	SENSORDB("OV2685 reg:3502,read reg = 0x%02x \n",OV2685_MIPI_read_cmos_sensor(0x3502)); //debug sarah
*/	
}

void OV2685_MIPI_set_dummy(kal_uint16 pixels, kal_uint16 lines)
{

SENSORDB("enter OV2685_MIPI_set_dummy \n"); //debug sarah
//debug();

    if (OV2685_MIPI_gPVmode)
    {
    	  pixels = pixels+0x06ac; 
        OV2685_MIPI_write_cmos_sensor(0x380D,( pixels&0xFF));         
        OV2685_MIPI_write_cmos_sensor(0x380C,(( pixels&0xFF00)>>8)); 
      
        lines= lines+0x0284; 
        OV2685_MIPI_write_cmos_sensor(0x380F,(lines&0xFF));       
        OV2685_MIPI_write_cmos_sensor(0x380E,((lines&0xFF00)>>8));  
    	
    }
    else
    {
        pixels = pixels+0x06a4; 
        OV2685_MIPI_write_cmos_sensor(0x380D,( pixels&0xFF));         
        OV2685_MIPI_write_cmos_sensor(0x380C,(( pixels&0xFF00)>>8)); 
      
        lines= lines+0x050e; 
        OV2685_MIPI_write_cmos_sensor(0x380F,(lines&0xFF));       
        OV2685_MIPI_write_cmos_sensor(0x380E,((lines&0xFF00)>>8));    	
    }    

//debug();
	SENSORDB("exit OV2685_MIPI_set_dummy \n"); //debug sarah

}    /* OV2685_MIPI_set_dummy */
void OV2685_MIPI_globle_init(void)
{
    SENSORDB("OV2685_MIPI_global_init begin \n");

SENSORDB("enter OV2685_MIPI_global_init begin \n"); //debug sarah
//debug();
	
//OV2685_MIPI_write_cmos_sensor(0x0103,0x01); //><
OV2685_MIPI_write_cmos_sensor(0x0100,0x00); //><
OV2685_MIPI_write_cmos_sensor(0x3002,0x00);
OV2685_MIPI_write_cmos_sensor(0x3016,0x1c);
OV2685_MIPI_write_cmos_sensor(0x3018,0x44);
OV2685_MIPI_write_cmos_sensor(0x301d,0xf0);
OV2685_MIPI_write_cmos_sensor(0x3020,0x00);
OV2685_MIPI_write_cmos_sensor(0x3082,0x2c);
OV2685_MIPI_write_cmos_sensor(0x3083,0x03);
OV2685_MIPI_write_cmos_sensor(0x3084,0x0f);
OV2685_MIPI_write_cmos_sensor(0x3085,0x03);
OV2685_MIPI_write_cmos_sensor(0x3086,0x00);
OV2685_MIPI_write_cmos_sensor(0x3087,0x00);

OV2685_MIPI_write_cmos_sensor(0x3501,0x26);//4  // 4e
OV2685_MIPI_write_cmos_sensor(0x3502,0x40); //e0
OV2685_MIPI_write_cmos_sensor(0x3503,0x03);
OV2685_MIPI_write_cmos_sensor(0x350b,0x36);
OV2685_MIPI_write_cmos_sensor(0x3600,0xb4);
OV2685_MIPI_write_cmos_sensor(0x3603,0x35);
OV2685_MIPI_write_cmos_sensor(0x3604,0x24);
OV2685_MIPI_write_cmos_sensor(0x3605,0x00);
OV2685_MIPI_write_cmos_sensor(0x3620,0x24);
OV2685_MIPI_write_cmos_sensor(0x3621,0x34);
OV2685_MIPI_write_cmos_sensor(0x3622,0x03);
OV2685_MIPI_write_cmos_sensor(0x3628,0x10);
OV2685_MIPI_write_cmos_sensor(0x3705,0x3c);
OV2685_MIPI_write_cmos_sensor(0x370a,0x21);
OV2685_MIPI_write_cmos_sensor(0x370c,0x50);
OV2685_MIPI_write_cmos_sensor(0x370d,0xc0);
OV2685_MIPI_write_cmos_sensor(0x3717,0x58);
OV2685_MIPI_write_cmos_sensor(0x3718,0x80);
OV2685_MIPI_write_cmos_sensor(0x3720,0x00);
OV2685_MIPI_write_cmos_sensor(0x3721,0x09);
OV2685_MIPI_write_cmos_sensor(0x3722,0x06);
OV2685_MIPI_write_cmos_sensor(0x3723,0x59);
OV2685_MIPI_write_cmos_sensor(0x3738,0x99);
OV2685_MIPI_write_cmos_sensor(0x3781,0x80);
OV2685_MIPI_write_cmos_sensor(0x3784,0x0c);
OV2685_MIPI_write_cmos_sensor(0x3789,0x60);
OV2685_MIPI_write_cmos_sensor(0x3800,0x00);
OV2685_MIPI_write_cmos_sensor(0x3801,0x00);
OV2685_MIPI_write_cmos_sensor(0x3802,0x00);
OV2685_MIPI_write_cmos_sensor(0x3803,0x00);
OV2685_MIPI_write_cmos_sensor(0x3804,0x06);
OV2685_MIPI_write_cmos_sensor(0x3805,0x4f);
OV2685_MIPI_write_cmos_sensor(0x3806,0x04);
OV2685_MIPI_write_cmos_sensor(0x3807,0xbf);
OV2685_MIPI_write_cmos_sensor(0x3808,0x06);
OV2685_MIPI_write_cmos_sensor(0x3809,0x40);
OV2685_MIPI_write_cmos_sensor(0x380a,0x04);
OV2685_MIPI_write_cmos_sensor(0x380b,0xb0);
OV2685_MIPI_write_cmos_sensor(0x380c,0x06);
OV2685_MIPI_write_cmos_sensor(0x380d,0xa4);
OV2685_MIPI_write_cmos_sensor(0x380e,0x05);
OV2685_MIPI_write_cmos_sensor(0x380f,0x0e);
OV2685_MIPI_write_cmos_sensor(0x3810,0x00);
OV2685_MIPI_write_cmos_sensor(0x3811,0x08);
OV2685_MIPI_write_cmos_sensor(0x3812,0x00);
OV2685_MIPI_write_cmos_sensor(0x3813,0x08);
OV2685_MIPI_write_cmos_sensor(0x3814,0x11);
OV2685_MIPI_write_cmos_sensor(0x3815,0x11);
OV2685_MIPI_write_cmos_sensor(0x3819,0x04);
OV2685_MIPI_write_cmos_sensor(0x3820,0xc0);
OV2685_MIPI_write_cmos_sensor(0x3821,0x00);
OV2685_MIPI_write_cmos_sensor(0x3a06,0x00);
OV2685_MIPI_write_cmos_sensor(0x3a07,0xc2);
OV2685_MIPI_write_cmos_sensor(0x3a08,0x00);
OV2685_MIPI_write_cmos_sensor(0x3a09,0xa1);
OV2685_MIPI_write_cmos_sensor(0x3a0a,0x09); //24 //07 ^.<
OV2685_MIPI_write_cmos_sensor(0x3a0b,0xda); //60 //94 ^.<
OV2685_MIPI_write_cmos_sensor(0x3a0c,0x0a); //28 //08 ^.<
OV2685_MIPI_write_cmos_sensor(0x3a0d,0x1c); //40 //2d ^.<
OV2685_MIPI_write_cmos_sensor(0x3a0e,0x04);  //exposure //02
OV2685_MIPI_write_cmos_sensor(0x3a0f,0x8c);  //exposure //46
OV2685_MIPI_write_cmos_sensor(0x3a10,0x05);  //exposure //02
OV2685_MIPI_write_cmos_sensor(0x3a11,0x08);  //exposure //84

OV2685_MIPI_write_cmos_sensor(0x3a15,0x21);  //>< //22

OV2685_MIPI_write_cmos_sensor(0x4000,0x81);
OV2685_MIPI_write_cmos_sensor(0x4001,0x40);
OV2685_MIPI_write_cmos_sensor(0x4008,0x02);
OV2685_MIPI_write_cmos_sensor(0x4009,0x09);
OV2685_MIPI_write_cmos_sensor(0x4300,0x32);//30 sarah
OV2685_MIPI_write_cmos_sensor(0x430e,0x00);
OV2685_MIPI_write_cmos_sensor(0x4602,0x02);
OV2685_MIPI_write_cmos_sensor(0x481b,0x40);
OV2685_MIPI_write_cmos_sensor(0x481f,0x40);
OV2685_MIPI_write_cmos_sensor(0x4837,0x1e);
OV2685_MIPI_write_cmos_sensor(0x5000,0xff);
OV2685_MIPI_write_cmos_sensor(0x5001,0x05);
OV2685_MIPI_write_cmos_sensor(0x5002,0x32);
OV2685_MIPI_write_cmos_sensor(0x5003,0x04);
OV2685_MIPI_write_cmos_sensor(0x5004,0xff);
OV2685_MIPI_write_cmos_sensor(0x5005,0x12);
OV2685_MIPI_write_cmos_sensor(0x0100,0x01); //^.<
//OV2685_MIPI_write_cmos_sensor(0x0101,0x01); //^.<
//OV2685_MIPI_write_cmos_sensor(0x1000,0x01); //^.<
//OV2685_MIPI_write_cmos_sensor(0x0129,0x10); //^.<

//OV2685_MIPI_write_cmos_sensor(0x5180,0xfc);   //0107		//^^
OV2685_MIPI_write_cmos_sensor(0x5181,0x11);   //awb
OV2685_MIPI_write_cmos_sensor(0x5182,0x41);   
OV2685_MIPI_write_cmos_sensor(0x5183,0x42);   
OV2685_MIPI_write_cmos_sensor(0x5184,0x82); //tune sensory test awb 8f //78
OV2685_MIPI_write_cmos_sensor(0x5185,0x7a);   
OV2685_MIPI_write_cmos_sensor(0x5186,0x00); //0e  //tune sensory test awb 08
OV2685_MIPI_write_cmos_sensor(0x5187,0x10); //tune sensory test awb 0c //1c
OV2685_MIPI_write_cmos_sensor(0x5188,0x12);   
OV2685_MIPI_write_cmos_sensor(0x5189,0x0c);   
OV2685_MIPI_write_cmos_sensor(0x518a,0x0c); //tune sensory test awb 0c //10
OV2685_MIPI_write_cmos_sensor(0x518b,0x38);   
OV2685_MIPI_write_cmos_sensor(0x518c,0x40); //tune sensory test awb 24 //48
OV2685_MIPI_write_cmos_sensor(0x518d,0xf8);   
OV2685_MIPI_write_cmos_sensor(0x518e,0x04);   
OV2685_MIPI_write_cmos_sensor(0x518f,0x7f);   
OV2685_MIPI_write_cmos_sensor(0x5190,0x40);   
OV2685_MIPI_write_cmos_sensor(0x5191,0x5f);   
OV2685_MIPI_write_cmos_sensor(0x5192,0x40);   
OV2685_MIPI_write_cmos_sensor(0x5193,0xff);   
OV2685_MIPI_write_cmos_sensor(0x5194,0x40);   
OV2685_MIPI_write_cmos_sensor(0x5195,0x05); //tune sensory test awb 05 //04
OV2685_MIPI_write_cmos_sensor(0x5196,0x95); //f5 //c0 //tune sensory test awb c0 //44
OV2685_MIPI_write_cmos_sensor(0x5197,0x04);   
OV2685_MIPI_write_cmos_sensor(0x5198,0x00);   
OV2685_MIPI_write_cmos_sensor(0x5199,0x07); //tune sensory test awb 06  
OV2685_MIPI_write_cmos_sensor(0x519a,0xe2); //fe //9b //tune sensory test awb 9b //a0
OV2685_MIPI_write_cmos_sensor(0x519b,0x04);  //0107

OV2685_MIPI_write_cmos_sensor(0x5200,0x09); //line stretch registers
OV2685_MIPI_write_cmos_sensor(0x5201,0x00);
OV2685_MIPI_write_cmos_sensor(0x5202,0x06);
OV2685_MIPI_write_cmos_sensor(0x5203,0x20);
OV2685_MIPI_write_cmos_sensor(0x5204,0x41);
OV2685_MIPI_write_cmos_sensor(0x5205,0x16);
OV2685_MIPI_write_cmos_sensor(0x5206,0x00);
OV2685_MIPI_write_cmos_sensor(0x5207,0x05);
OV2685_MIPI_write_cmos_sensor(0x520b,0x30);
OV2685_MIPI_write_cmos_sensor(0x520c,0x75);
OV2685_MIPI_write_cmos_sensor(0x520d,0x00);
OV2685_MIPI_write_cmos_sensor(0x520e,0x30);
OV2685_MIPI_write_cmos_sensor(0x520f,0x75);
OV2685_MIPI_write_cmos_sensor(0x5210,0x00);

///* !

OV2685_MIPI_write_cmos_sensor(0x5280,0x15);//;bGDNSEnable=0;nNoiseYSlope=5; //raw denoise registers
OV2685_MIPI_write_cmos_sensor(0x5281,0x06);//;nNoiseList_0=2
OV2685_MIPI_write_cmos_sensor(0x5282,0x06);//;nNoiseList_1=2
OV2685_MIPI_write_cmos_sensor(0x5283,0x08);//;nNoiseList_2=4
OV2685_MIPI_write_cmos_sensor(0x5284,0x0c);//;nNoiseList_3=6
OV2685_MIPI_write_cmos_sensor(0x5285,0x10);//;nNoiseList_4=8
OV2685_MIPI_write_cmos_sensor(0x5286,0x20);//;nNoiseList_5=12
OV2685_MIPI_write_cmos_sensor(0x5287,0x10);//;nMaxEdgeThre=16 

//!*/


OV2685_MIPI_write_cmos_sensor(0x5300,0xc5);//;nNoiseYSlope=5;bAntiAliasing=1;nDetailSlope=0 //cip registers
OV2685_MIPI_write_cmos_sensor(0x5301,0xa0);//;bGbGrRemove=0;nGbGrShift=0;nSharpenSlope=5
OV2685_MIPI_write_cmos_sensor(0x5302,0x08);//;nNoiseList_0=6
OV2685_MIPI_write_cmos_sensor(0x5303,0x0c);//;nNoiseList_1=8
OV2685_MIPI_write_cmos_sensor(0x5304,0x18);//;nNoiseList_2=24
OV2685_MIPI_write_cmos_sensor(0x5305,0x30);//;nNoiseList_3=48
OV2685_MIPI_write_cmos_sensor(0x5306,0x60);//;nNoiseList_4=96
OV2685_MIPI_write_cmos_sensor(0x5307,0xc0);//;nNoiseList_5=192

OV2685_MIPI_write_cmos_sensor(0x5308,0x82); 
OV2685_MIPI_write_cmos_sensor(0x5309,0x00); 
OV2685_MIPI_write_cmos_sensor(0x530a,0x3f); // 26 //20 sarah //36 test max sharpen sarah 20150202
OV2685_MIPI_write_cmos_sensor(0x530b,0x01); //02 //04 sarah //02 sarah test  //02 20150128
OV2685_MIPI_write_cmos_sensor(0x530c,0x02);
OV2685_MIPI_write_cmos_sensor(0x530d,0x00);
OV2685_MIPI_write_cmos_sensor(0x530e,0x0c);
OV2685_MIPI_write_cmos_sensor(0x530f,0x14);
OV2685_MIPI_write_cmos_sensor(0x5310,0x1a);
OV2685_MIPI_write_cmos_sensor(0x5311,0x20);
OV2685_MIPI_write_cmos_sensor(0x5312,0x80);
OV2685_MIPI_write_cmos_sensor(0x5313,0x4b);
OV2685_MIPI_write_cmos_sensor(0x5380,0x01); //01
OV2685_MIPI_write_cmos_sensor(0x5381,0xda); //52
OV2685_MIPI_write_cmos_sensor(0x5382,0x00); //00
OV2685_MIPI_write_cmos_sensor(0x5383,0x5a); // 4a
OV2685_MIPI_write_cmos_sensor(0x5384,0x00); //00
OV2685_MIPI_write_cmos_sensor(0x5385,0x77); //b6
OV2685_MIPI_write_cmos_sensor(0x5386,0x00); //00
OV2685_MIPI_write_cmos_sensor(0x5387,0x80); //8d
OV2685_MIPI_write_cmos_sensor(0x5388,0x00); //00
OV2685_MIPI_write_cmos_sensor(0x5389,0x1d); // 3a
OV2685_MIPI_write_cmos_sensor(0x538a,0x00); //00
OV2685_MIPI_write_cmos_sensor(0x538b,0xd0); //a6
OV2685_MIPI_write_cmos_sensor(0x538c,0x00); //00

OV2685_MIPI_write_cmos_sensor(0x5400,0x04);//;0d //gamma
OV2685_MIPI_write_cmos_sensor(0x5401,0x0b);//;18
OV2685_MIPI_write_cmos_sensor(0x5402,0x1e);//;31
OV2685_MIPI_write_cmos_sensor(0x5403,0x52);//;5a
OV2685_MIPI_write_cmos_sensor(0x5404,0x63);//;65
OV2685_MIPI_write_cmos_sensor(0x5405,0x72);
OV2685_MIPI_write_cmos_sensor(0x5406,0x7c);
OV2685_MIPI_write_cmos_sensor(0x5407,0x86);	
OV2685_MIPI_write_cmos_sensor(0x5408,0x8e);
OV2685_MIPI_write_cmos_sensor(0x5409,0x95);
OV2685_MIPI_write_cmos_sensor(0x540a,0xa4);
OV2685_MIPI_write_cmos_sensor(0x540b,0xaf);
OV2685_MIPI_write_cmos_sensor(0x540c,0xc7);
OV2685_MIPI_write_cmos_sensor(0x540d,0xd7);
OV2685_MIPI_write_cmos_sensor(0x540e,0xe7);
OV2685_MIPI_write_cmos_sensor(0x540f,0xa0);//;nShadowHGain=160
OV2685_MIPI_write_cmos_sensor(0x5410,0x6e);//;nMidToneHGain=110
OV2685_MIPI_write_cmos_sensor(0x5411,0x06);//;nHighLightHGain=6

///*!
OV2685_MIPI_write_cmos_sensor(0x5480,0x19); //RGB de-noise registers
OV2685_MIPI_write_cmos_sensor(0x5481,0x00); 
OV2685_MIPI_write_cmos_sensor(0x5482,0x09);
OV2685_MIPI_write_cmos_sensor(0x5483,0x12);
OV2685_MIPI_write_cmos_sensor(0x5484,0x04);
OV2685_MIPI_write_cmos_sensor(0x5485,0x06);
OV2685_MIPI_write_cmos_sensor(0x5486,0x08);
OV2685_MIPI_write_cmos_sensor(0x5487,0x0c);
OV2685_MIPI_write_cmos_sensor(0x5488,0x10);
OV2685_MIPI_write_cmos_sensor(0x5489,0x18);

OV2685_MIPI_write_cmos_sensor(0x5500,0x04); //uv denoise registers
OV2685_MIPI_write_cmos_sensor(0x5501,0x06);
OV2685_MIPI_write_cmos_sensor(0x5502,0x08);
OV2685_MIPI_write_cmos_sensor(0x5503,0x0c);
OV2685_MIPI_write_cmos_sensor(0x5504,0x10);
OV2685_MIPI_write_cmos_sensor(0x5505,0x18);
OV2685_MIPI_write_cmos_sensor(0x5506,0x00);
//!*/

OV2685_MIPI_write_cmos_sensor(0x5600,0x02); //sde 
OV2685_MIPI_write_cmos_sensor(0x5603,0x34); //40
OV2685_MIPI_write_cmos_sensor(0x5604,0x28); //28 //sarah low light sat 30
OV2685_MIPI_write_cmos_sensor(0x5609,0x20);
OV2685_MIPI_write_cmos_sensor(0x560a,0x60);
OV2685_MIPI_write_cmos_sensor(0x5780,0x3e);
OV2685_MIPI_write_cmos_sensor(0x5781,0x0f);
OV2685_MIPI_write_cmos_sensor(0x5782,0x04);
OV2685_MIPI_write_cmos_sensor(0x5783,0x02);
OV2685_MIPI_write_cmos_sensor(0x5784,0x01);
OV2685_MIPI_write_cmos_sensor(0x5785,0x01);
OV2685_MIPI_write_cmos_sensor(0x5786,0x00);
OV2685_MIPI_write_cmos_sensor(0x5787,0x04);
OV2685_MIPI_write_cmos_sensor(0x5788,0x02);
OV2685_MIPI_write_cmos_sensor(0x5789,0x00);
OV2685_MIPI_write_cmos_sensor(0x578a,0x01);
OV2685_MIPI_write_cmos_sensor(0x578b,0x02);
OV2685_MIPI_write_cmos_sensor(0x578c,0x03);
OV2685_MIPI_write_cmos_sensor(0x578d,0x03);
OV2685_MIPI_write_cmos_sensor(0x578e,0x08);
OV2685_MIPI_write_cmos_sensor(0x578f,0x0c);
OV2685_MIPI_write_cmos_sensor(0x5790,0x08);
OV2685_MIPI_write_cmos_sensor(0x5791,0x04);
OV2685_MIPI_write_cmos_sensor(0x5792,0x00);
OV2685_MIPI_write_cmos_sensor(0x5793,0x00);
OV2685_MIPI_write_cmos_sensor(0x5794,0x03);

OV2685_MIPI_write_cmos_sensor(0x5800,0x03); //lens shading
OV2685_MIPI_write_cmos_sensor(0x5801,0x1c); //24
OV2685_MIPI_write_cmos_sensor(0x5802,0x02);
OV2685_MIPI_write_cmos_sensor(0x5803,0x20); //40
OV2685_MIPI_write_cmos_sensor(0x5804,0x1a); //34
OV2685_MIPI_write_cmos_sensor(0x5805,0x05);
OV2685_MIPI_write_cmos_sensor(0x5806,0x12);
OV2685_MIPI_write_cmos_sensor(0x5807,0x05);
OV2685_MIPI_write_cmos_sensor(0x5808,0x03);
OV2685_MIPI_write_cmos_sensor(0x5809,0x38); //3// 3c
OV2685_MIPI_write_cmos_sensor(0x580a,0x02);
OV2685_MIPI_write_cmos_sensor(0x580b,0x10); //40
OV2685_MIPI_write_cmos_sensor(0x580c,0x10); //26
OV2685_MIPI_write_cmos_sensor(0x580d,0x05);
OV2685_MIPI_write_cmos_sensor(0x580e,0x52);
OV2685_MIPI_write_cmos_sensor(0x580f,0x06);
OV2685_MIPI_write_cmos_sensor(0x5810,0x03);
OV2685_MIPI_write_cmos_sensor(0x5811,0x00); //28
OV2685_MIPI_write_cmos_sensor(0x5812,0x02);
OV2685_MIPI_write_cmos_sensor(0x5813,0x10); //40
OV2685_MIPI_write_cmos_sensor(0x5814,0x0e); //24
OV2685_MIPI_write_cmos_sensor(0x5815,0x05);
OV2685_MIPI_write_cmos_sensor(0x5816,0x42);
OV2685_MIPI_write_cmos_sensor(0x5817,0x06);
OV2685_MIPI_write_cmos_sensor(0x5818,0x0d);
OV2685_MIPI_write_cmos_sensor(0x5819,0x40);
OV2685_MIPI_write_cmos_sensor(0x581a,0x04);
OV2685_MIPI_write_cmos_sensor(0x581b,0x0c);
OV2685_MIPI_write_cmos_sensor(0x3a03,0x42); //4      // 4c  20140107 //42 ^.<
OV2685_MIPI_write_cmos_sensor(0x3a04,0x38);//40 20140107 //38 ^.<
OV2685_MIPI_write_cmos_sensor(0x3503,0x00); //00 >< try fe

// weighting table
OV2685_MIPI_write_cmos_sensor(0x5908,0x00);
OV2685_MIPI_write_cmos_sensor(0x5909,0x00);
OV2685_MIPI_write_cmos_sensor(0x590a,0x10);
OV2685_MIPI_write_cmos_sensor(0x590b,0x01);
OV2685_MIPI_write_cmos_sensor(0x590c,0x10);
OV2685_MIPI_write_cmos_sensor(0x590d,0x01);
OV2685_MIPI_write_cmos_sensor(0x590e,0x00);
OV2685_MIPI_write_cmos_sensor(0x590f,0x00);
//weighting table


OV2685_MIPI_write_cmos_sensor(0x5804,0x1a);//25; R-a1 
OV2685_MIPI_write_cmos_sensor(0x580c,0x10);// G-a1
OV2685_MIPI_write_cmos_sensor(0x5814,0x0e);//19; B-a1

OV2685_MIPI_write_cmos_sensor(0x5801,0x1c);// R_X[7:0]
OV2685_MIPI_write_cmos_sensor(0x5809,0x38);// G_X[7:0]
OV2685_MIPI_write_cmos_sensor(0x5811,0x00);// B_X[7:0]

OV2685_MIPI_write_cmos_sensor(0x5802,0x02);//; [2:0] R_Y[10:8]
OV2685_MIPI_write_cmos_sensor(0x5803,0x20);//; R_Y[7:0]
                                         
OV2685_MIPI_write_cmos_sensor(0x580a,0x02);//; [2:0] G_Y[10:8]
OV2685_MIPI_write_cmos_sensor(0x580b,0x10);//; G_Y[7:0]
                                         
OV2685_MIPI_write_cmos_sensor(0x5812,0x02);//; [2:0] B_Y[10:8]
OV2685_MIPI_write_cmos_sensor(0x5813,0x10);//; R_Y[7:0]

OV2685_MIPI_write_cmos_sensor(0x3a03,0x42);// ;80
OV2685_MIPI_write_cmos_sensor(0x3a04,0x38);// ;74

OV2685_MIPI_write_cmos_sensor(0x3a0e,0x04);//;04 ; VTS band50[194x(6-1)=970=0x3ca]   //03 20141229
OV2685_MIPI_write_cmos_sensor(0x3a0f,0x8c);//;8c							     //ca 20141229
OV2685_MIPI_write_cmos_sensor(0x3a10,0x05);//;05 ; VTS band60[161x(8-2)=966=0x3c6]   //03 20141229
OV2685_MIPI_write_cmos_sensor(0x3a11,0x08);//;08						            //c6 20141229

										 
//OV2685_MIPI_write_cmos_sensor(0x3a00,0x43);//;02 02 
OV2685_MIPI_write_cmos_sensor(0x382a,0x08);//; 08 ;vts_auto_en;
OV2685_MIPI_write_cmos_sensor(0x3a06,0x00);
OV2685_MIPI_write_cmos_sensor(0x3a07,0xc2);// ; 194
OV2685_MIPI_write_cmos_sensor(0x3a08,0x00); 
OV2685_MIPI_write_cmos_sensor(0x3a09,0xa1);// ; 161;
OV2685_MIPI_write_cmos_sensor(0x3a0a,0x09);// ;max exp of 50Hz;
OV2685_MIPI_write_cmos_sensor(0x3a0b,0xda);// ;max exp of 50Hz
OV2685_MIPI_write_cmos_sensor(0x3a0c,0x0a);// ;max exp of 60Hz 
OV2685_MIPI_write_cmos_sensor(0x3a0d,0x1c);// ;max exp of 60Hz
                                         

                                         
OV2685_MIPI_write_cmos_sensor(0x3a13,0xc8);//; max gain(13.5x)
OV2685_MIPI_write_cmos_sensor(0x5604,0x28);//; uv sat //30

/*                                         
//OV2685_MIPI_write_cmos_sensor(0x5180,0xfc);   			//^^
OV2685_MIPI_write_cmos_sensor(0x5181,0x11);   
OV2685_MIPI_write_cmos_sensor(0x5182,0x41);   
OV2685_MIPI_write_cmos_sensor(0x5183,0x42);   
OV2685_MIPI_write_cmos_sensor(0x5184,0x8f);   
OV2685_MIPI_write_cmos_sensor(0x5185,0x7a);   
OV2685_MIPI_write_cmos_sensor(0x5186,0x0e);//0e //08  
OV2685_MIPI_write_cmos_sensor(0x5187,0x0c);   
OV2685_MIPI_write_cmos_sensor(0x5188,0x12);   
OV2685_MIPI_write_cmos_sensor(0x5189,0x0c);   
OV2685_MIPI_write_cmos_sensor(0x518a,0x0c);   
OV2685_MIPI_write_cmos_sensor(0x518b,0x38);   
OV2685_MIPI_write_cmos_sensor(0x518c,0x24);   
OV2685_MIPI_write_cmos_sensor(0x518d,0xf8);   
OV2685_MIPI_write_cmos_sensor(0x518e,0x04);   
OV2685_MIPI_write_cmos_sensor(0x518f,0x7f);   
OV2685_MIPI_write_cmos_sensor(0x5190,0x40);   
OV2685_MIPI_write_cmos_sensor(0x5191,0x5f);   
OV2685_MIPI_write_cmos_sensor(0x5192,0x40);   
OV2685_MIPI_write_cmos_sensor(0x5193,0xff);   
OV2685_MIPI_write_cmos_sensor(0x5194,0x40);   
OV2685_MIPI_write_cmos_sensor(0x5195,0x05);   
OV2685_MIPI_write_cmos_sensor(0x5196,0xf5);//f5 //c0
OV2685_MIPI_write_cmos_sensor(0x5197,0x04);   
OV2685_MIPI_write_cmos_sensor(0x5198,0x00);   
OV2685_MIPI_write_cmos_sensor(0x5199,0x06);   
OV2685_MIPI_write_cmos_sensor(0x519a,0xfe);//fe //9b
OV2685_MIPI_write_cmos_sensor(0x519b,0x04);  
*/

										 
OV2685_MIPI_write_cmos_sensor(0x530a,0x3f);//;26;20; lower edge  //20 sarah //36 test max sharpen sarah 20150202
OV2685_MIPI_write_cmos_sensor(0x530b,0x01);//;02;04;0a; lower edge detail //04 sarah //02 sarah test //02 20150128

                                         
OV2685_MIPI_write_cmos_sensor(0x5603,0x34);//; lower sat for d50-1000lux //38 20141229
                                         
OV2685_MIPI_write_cmos_sensor(0x5400,0x04);//;0d
OV2685_MIPI_write_cmos_sensor(0x5401,0x0b);//;18
OV2685_MIPI_write_cmos_sensor(0x5402,0x1e);//;31
OV2685_MIPI_write_cmos_sensor(0x5403,0x52);//;5a
OV2685_MIPI_write_cmos_sensor(0x5404,0x63);//;65
OV2685_MIPI_write_cmos_sensor(0x5405,0x72);
OV2685_MIPI_write_cmos_sensor(0x5406,0x7c);
OV2685_MIPI_write_cmos_sensor(0x5407,0x86);	
OV2685_MIPI_write_cmos_sensor(0x5408,0x8e);
OV2685_MIPI_write_cmos_sensor(0x5409,0x95);
OV2685_MIPI_write_cmos_sensor(0x540a,0xa4);
OV2685_MIPI_write_cmos_sensor(0x540b,0xaf);
OV2685_MIPI_write_cmos_sensor(0x540c,0xc7);
OV2685_MIPI_write_cmos_sensor(0x540d,0xd7);
OV2685_MIPI_write_cmos_sensor(0x540e,0xe7);
OV2685_MIPI_write_cmos_sensor(0x540f,0xa0);//;nShadowHGain=160
OV2685_MIPI_write_cmos_sensor(0x5410,0x6e);//;nMidToneHGain=110
OV2685_MIPI_write_cmos_sensor(0x5411,0x06);//;nHighLightHGain=6
                                         
OV2685_MIPI_write_cmos_sensor(0x5300,0xc5);//;nNoiseYSlope=5;bAntiAliasing=1;nDetailSlope=0
OV2685_MIPI_write_cmos_sensor(0x5301,0xa0);//;bGbGrRemove=0;nGbGrShift=0;nSharpenSlope=5
OV2685_MIPI_write_cmos_sensor(0x5302,0x08);//;nNoiseList_0=6 //08 test normal light denoise sarah
OV2685_MIPI_write_cmos_sensor(0x5303,0x0c);//;nNoiseList_1=8
OV2685_MIPI_write_cmos_sensor(0x5304,0x18);//;nNoiseList_2=24
OV2685_MIPI_write_cmos_sensor(0x5305,0x30);//;nNoiseList_3=48
OV2685_MIPI_write_cmos_sensor(0x5306,0x60);//;nNoiseList_4=96
OV2685_MIPI_write_cmos_sensor(0x5307,0xc0);//;nNoiseList_5=192
                                         
OV2685_MIPI_write_cmos_sensor(0x5004,0xff);//;80 80
OV2685_MIPI_write_cmos_sensor(0x5500,0x04);
OV2685_MIPI_write_cmos_sensor(0x5501,0x06);
OV2685_MIPI_write_cmos_sensor(0x5502,0x08);
OV2685_MIPI_write_cmos_sensor(0x5503,0x0c);
OV2685_MIPI_write_cmos_sensor(0x5504,0x10);
OV2685_MIPI_write_cmos_sensor(0x5505,0x18);
OV2685_MIPI_write_cmos_sensor(0x5506,0x00);
                                         
OV2685_MIPI_write_cmos_sensor(0x5004,0xff);//;04 04
OV2685_MIPI_write_cmos_sensor(0x5280,0x15);//;bGDNSEnable=0;nNoiseYSlope=5;
OV2685_MIPI_write_cmos_sensor(0x5281,0x06);//;nNoiseList_0=2
OV2685_MIPI_write_cmos_sensor(0x5282,0x06);//;nNoiseList_1=2
OV2685_MIPI_write_cmos_sensor(0x5283,0x08);//;nNoiseList_2=4
OV2685_MIPI_write_cmos_sensor(0x5284,0x0c);//;nNoiseList_3=6
OV2685_MIPI_write_cmos_sensor(0x5285,0x10);//;nNoiseList_4=8
OV2685_MIPI_write_cmos_sensor(0x5286,0x20);//;nNoiseList_5=12
OV2685_MIPI_write_cmos_sensor(0x5287,0x10);//;nMaxEdgeThre=16 
OV2685_MIPI_write_cmos_sensor(0x3a00,0x43);//;02 02 
            OV2685_MIPI_write_cmos_sensor(0x5180,0xfe);   	//^^
            OV2685_MIPI_write_cmos_sensor(0x5195, 0x04);    			//^^
            OV2685_MIPI_write_cmos_sensor(0x5196, 0xfa); //dc			//^^
            OV2685_MIPI_write_cmos_sensor(0x5197, 0x04);  			//^^
            OV2685_MIPI_write_cmos_sensor(0x5198, 0x00); 			//^^
            OV2685_MIPI_write_cmos_sensor(0x5199, 0x07); //06			//^^
            OV2685_MIPI_write_cmos_sensor(0x519a, 0xca); //d3 		//^^

	debug();  
    SENSORDB("OV2685_MIPI_global_init end! \n");

	SENSORDB("exit OV2685_MIPI_global_init begin \n"); //debug sarah

}

kal_uint16 OV2685_MIPI_read_OV2685_MIPI_gain(void)
{
    kal_uint8  temp_reg;
    kal_uint16 sensor_gain;
	    SENSORDB("enter OV2685_MIPI_read_OV2685_MIPI_gain \n"); //debug sarah
//debug();

    temp_reg=OV2685_MIPI_read_cmos_sensor(0x350b);  


    sensor_gain=(16+(temp_reg&0x0F));
    if(temp_reg&0x10)
        sensor_gain<<=1;
    if(temp_reg&0x20)
        sensor_gain<<=1;

    if(temp_reg&0x40)
        sensor_gain<<=1;

    if(temp_reg&0x80)
        sensor_gain<<=1;

//debug();
	SENSORDB("exit OV2685_MIPI_read_OV2685_MIPI_gain \n"); //debug sarah
    return sensor_gain;

}  /* OV2685_MIPI_read_OV2685_MIPI_gain */
kal_uint16 OV2685_MIPI_read_shutter(void)
{
    kal_uint16 temp_reg1, temp_reg2, temp_reg3;
    kal_uint16 temp_reg;

          SENSORDB("enter OV2685_MIPI_read_shutter \n"); //debug sarah
//debug();

    temp_reg1 = OV2685_MIPI_read_cmos_sensor(0x3500);   
    temp_reg2 = OV2685_MIPI_read_cmos_sensor(0x3501);  
    temp_reg3 = OV2685_MIPI_read_cmos_sensor(0x3502);  
    
    temp_reg = (temp_reg1<<12)|(temp_reg2<<4)|(temp_reg3>>4);    

    spin_lock(&ov2685_drv_lock);
    OV2685_MIPI_PV_Shutter = temp_reg ;   
    spin_unlock(&ov2685_drv_lock);

//debug();
	    SENSORDB("exit OV2685_MIPI_read_shutter \n"); //debug sarah    
    return temp_reg;

}    /* OV2685_MIPI_read_shutter */

void OV2685_MIPI_write_OV2685_MIPI_gain(kal_uint16 gain)
{    

	kal_uint16 temp_reg;

	  //OV2685_MIPI_write_cmos_sensor(0x3500,OV2685_MIPI_read_cmos_sensor(0x3500)); //mark on by sarah
	  //OV2685_MIPI_write_cmos_sensor(0x3501,OV2685_MIPI_read_cmos_sensor(0x3501)); //mark on by sarah
	  //OV2685_MIPI_write_cmos_sensor(0x3502,OV2685_MIPI_read_cmos_sensor(0x3502)); //mark on by sarah
	  //OV2685_MIPI_write_cmos_sensor(0x3503,OV2685_MIPI_read_cmos_sensor(0x3503)); //mark on by sarah
	  //OV2685_MIPI_write_cmos_sensor(0x350a,OV2685_MIPI_read_cmos_sensor(0x350a)); //mark on by sarah
	  //OV2685_MIPI_write_cmos_sensor(0x350b,OV2685_MIPI_read_cmos_sensor(0x350b)); //mark on by sarah

	  SENSORDB("enter OV2685_MIPI_write_OV2685_MIPI_gain \n"); //debug sarah
//debug();
//return; //mark off below by sarah


    RETAILMSG(1, (TEXT("OV2685 write gain: %d\r\n"), gain));

    if(gain > 248)  return ;//ASSERT(0);

    temp_reg = 0;
    if (gain > 31)
    {
        temp_reg |= 0x10;
        gain = gain >> 1;
    }
    if (gain > 31)
    {
        temp_reg |= 0x20;
        gain = gain >> 1;
    }

    if (gain > 31)
    {
        temp_reg |= 0x40;
        gain = gain >> 1;
    }
    if (gain > 31)
    {
        temp_reg |= 0x80;
        gain = gain >> 1;
    }

    if (gain > 16)
    {
        temp_reg |= ((gain -16) & 0x0f);
    }   

 //  OV2685_MIPI_write_cmos_sensor(0x350b,temp_reg); 
//debug();
	  SENSORDB("exit OV2685_MIPI_write_OV2685_MIPI_gain \n"); //debug sarah

}  /* OV2685_MIPI_write_OV2685_MIPI_gain */

static void OV2685_MIPI_write_shutter(kal_uint16 shutter)
{
	  kal_uint32 OV2685_MIPI_extra_exposure_lines = 0;
	  
	  SENSORDB("enter OV2685_MIPI_write_shutter \n"); //debug sarah
//debug();
	
	  if (shutter < 1)
	  {
		    shutter = 1;
	  }
	
    if (OV2685_MIPI_gPVmode) 
    {
        if (shutter <= OV2685_MIPI_PV_EXPOSURE_LIMITATION) 
        {
            spin_lock(&ov2685_drv_lock);
            OV2685_MIPI_extra_exposure_lines = 0;
	          spin_unlock(&ov2685_drv_lock);
        }
        else 
        {
	          spin_lock(&ov2685_drv_lock);
            OV2685_MIPI_extra_exposure_lines=shutter - OV2685_MIPI_PV_EXPOSURE_LIMITATION;
	          spin_unlock(&ov2685_drv_lock);
        }

        if (shutter > OV2685_MIPI_PV_EXPOSURE_LIMITATION) 
        {
            shutter = OV2685_MIPI_PV_EXPOSURE_LIMITATION;
        }
    }
    else 
    {
        if (shutter <= OV2685_MIPI_FULL_EXPOSURE_LIMITATION) 
        {
	          spin_lock(&ov2685_drv_lock);
            OV2685_MIPI_extra_exposure_lines = 0;
	          spin_unlock(&ov2685_drv_lock);
    }
        else 
        {
	          spin_lock(&ov2685_drv_lock);
            OV2685_MIPI_extra_exposure_lines = shutter - OV2685_MIPI_FULL_EXPOSURE_LIMITATION;
	          spin_unlock(&ov2685_drv_lock);
        }

        if (shutter > OV2685_MIPI_FULL_EXPOSURE_LIMITATION) 
        {
            shutter = OV2685_MIPI_FULL_EXPOSURE_LIMITATION;
        }
    }
   
	  //AEC PK EXPOSURE
	  shutter*=16;
	  //OV2685_MIPI_write_cmos_sensor(0x3502, (shutter&0x00FF));         //mark off by sarah
	  //OV2685_MIPI_write_cmos_sensor(0x3501, ((shutter&0x0FF00)>>8));  //mark off by sarah
	  //OV2685_MIPI_write_cmos_sensor(0x3500, ((shutter&0xFF0000)>>16)); //mark off by sarah
	  
	  OV2685_MIPI_set_dummy(0, OV2685_MIPI_extra_exposure_lines);
//debug();
	  SENSORDB("exit OV2685_MIPI_write_shutter \n"); //debug sarah

	  
	  //OV2685_MIPI_write_cmos_sensor(0x350D, OV2685_MIPI_extra_exposure_lines&0xFF);        
	  //OV2685_MIPI_write_cmos_sensor(0x350C, (OV2685_MIPI_extra_exposure_lines&0xFF00)>> 8);     

}    /* OV2685_MIPI_write_shutter */
/*
void OV2685_MIPI_Computer_AEC(kal_uint16 preview_clk_in_M, kal_uint16 capture_clk_in_M)
{
    kal_uint16 PV_Line_Width;
    kal_uint16 Capture_Line_Width;
    kal_uint16 Capture_Maximum_Shutter;
    kal_uint16 Capture_Exposure;
    kal_uint16 Capture_Gain16;
    kal_uint16 Capture_Banding_Filter;
    kal_uint32 Gain_Exposure=0;

    PV_Line_Width = OV2685_MIPI_PV_PERIOD_PIXEL_NUMS + OV2685_MIPI_PV_Dummy_Pixels;   

    Capture_Line_Width = OV2685_MIPI_FULL_PERIOD_PIXEL_NUMS + OV2685_MIPI_Capture_Dummy_Pixels;
    Capture_Maximum_Shutter = OV2685_MIPI_FULL_EXPOSURE_LIMITATION + OV2685_MIPI_Capture_Dummy_Lines;
    Gain_Exposure = 1;
    ///////////////////////
    Gain_Exposure *=(OV2685_MIPI_PV_Shutter+OV2685_MIPI_PV_Extra_Lines);
    Gain_Exposure *=PV_Line_Width;  //970
    //   Gain_Exposure /=g_Preview_PCLK_Frequency;
    Gain_Exposure /=Capture_Line_Width;//1940
    Gain_Exposure = Gain_Exposure*capture_clk_in_M/preview_clk_in_M;// for clock   

    //OV2685_MIPI_Capture_Gain16 = Capture_Gain16;
    OV2685_MIPI_Capture_Extra_Lines = (Gain_Exposure > Capture_Maximum_Shutter)?
            (Gain_Exposure - Capture_Maximum_Shutter):0;     
    
    OV2685_MIPI_Capture_Shutter = Gain_Exposure - OV2685_MIPI_Capture_Extra_Lines;
}
*/

void OV2685_MIPI_Computer_AECAGC(kal_uint16 preview_clk_in_M, kal_uint16 capture_clk_in_M)
{
    kal_uint16 PV_Line_Width;
    kal_uint16 Capture_Line_Width;
    kal_uint16 Capture_Maximum_Shutter;
    kal_uint16 Capture_Exposure;
    kal_uint16 Capture_Gain16;
    kal_uint32 Capture_Banding_Filter;
    kal_uint32 Gain_Exposure=0;

	SENSORDB("enter OV2685_MIPI_Computer_AECAGC \n"); //debug sarah
//debug();

    PV_Line_Width = OV2685_MIPI_PV_PERIOD_PIXEL_NUMS + OV2685_MIPI_PV_Dummy_Pixels;   

    Capture_Line_Width = OV2685_MIPI_FULL_PERIOD_PIXEL_NUMS + OV2685_MIPI_Capture_Dummy_Pixels;
    Capture_Maximum_Shutter = OV2685_MIPI_FULL_EXPOSURE_LIMITATION + OV2685_MIPI_Capture_Dummy_Lines;

    if (OV2685_MIPI_Banding_setting == AE_FLICKER_MODE_50HZ)
#if WINMO_USE        
        Capture_Banding_Filter = (kal_uint32)(capture_clk_in_M*1000000/100/(2*Capture_Line_Width)+0.5);
#else 
        Capture_Banding_Filter = (kal_uint32)(capture_clk_in_M*100000/100/(2*Capture_Line_Width));
#endif 
    else
#if WINMO_USE
        Capture_Banding_Filter = (kal_uint16)(capture_clk_in_M*1000000/120/(2*Capture_Line_Width)+0.5);
#else 
        Capture_Banding_Filter = (kal_uint32)(capture_clk_in_M*100000/120/(2*Capture_Line_Width) );
#endif 

    /*   Gain_Exposure = OV2685_MIPI_PV_Gain16*(OV2685_MIPI_PV_Shutter+OV2685_MIPI_PV_Extra_Lines)*PV_Line_Width/g_Preview_PCLK_Frequency/Capture_Line_Width*g_Capture_PCLK_Frequency
    ;*/
spin_lock(&ov2685_drv_lock);
    //OV2685_MIPI_PV_Gain16 = OV2685_MIPI_read_OV2685_MIPI_gain();
spin_unlock(&ov2685_drv_lock);
    Gain_Exposure = 1 * OV2685_MIPI_PV_Gain16;  //For OV2685
    ///////////////////////
    Gain_Exposure *=(OV2685_MIPI_PV_Shutter+OV2685_MIPI_PV_Extra_Lines);
    Gain_Exposure *=PV_Line_Width;  //970
    //   Gain_Exposure /=g_Preview_PCLK_Frequency;
    Gain_Exposure /=Capture_Line_Width;//1940
    Gain_Exposure = Gain_Exposure*capture_clk_in_M/preview_clk_in_M;// for clock   

    //redistribute gain and exposure
    if (Gain_Exposure < (kal_uint32)(Capture_Banding_Filter * 16))     // Exposure < 1/100/120
    {
       if(Gain_Exposure<16){//exposure line smaller than 2 lines and gain smaller than 0x08 
            Gain_Exposure = Gain_Exposure*4;     
            Capture_Exposure = 1;
            Capture_Gain16 = (Gain_Exposure*2 + 1)/Capture_Exposure/2/4;
        }
        else
        {
            Capture_Exposure = Gain_Exposure /16;
            Capture_Gain16 = (Gain_Exposure*2 + 1)/Capture_Exposure/2;
        }
    }
    else 
    {
        if (Gain_Exposure >(kal_uint32)( Capture_Maximum_Shutter * 16)) // Exposure > Capture_Maximum_Shutter
        {
           
            Capture_Exposure = Capture_Maximum_Shutter/Capture_Banding_Filter*Capture_Banding_Filter;
            Capture_Gain16 = (Gain_Exposure*2 + 1)/Capture_Exposure/2;
            if (Capture_Gain16 > OV2685_MIPI_Capture_Max_Gain16) 
            {
                // gain reach maximum, insert extra line
                Capture_Exposure = (kal_uint16)(Gain_Exposure*11 /10 /OV2685_MIPI_Capture_Max_Gain16);
                
                // Exposure = n/100/120
                Capture_Exposure = Capture_Exposure/Capture_Banding_Filter * Capture_Banding_Filter;
                Capture_Gain16 = ((Gain_Exposure *4)/ Capture_Exposure+3)/4;
            }
        }
        else  // 1/100 < Exposure < Capture_Maximum_Shutter, Exposure = n/100/120
        {
            Capture_Exposure = Gain_Exposure/16/Capture_Banding_Filter;
            Capture_Exposure = Capture_Exposure * Capture_Banding_Filter;
            Capture_Gain16 = (Gain_Exposure*2 +1) / Capture_Exposure/2;
        }
    }

    	spin_lock(&ov2685_drv_lock);
    OV2685_MIPI_Capture_Gain16 = Capture_Gain16;
    OV2685_MIPI_Capture_Extra_Lines = (Capture_Exposure > Capture_Maximum_Shutter)?
            (Capture_Exposure - Capture_Maximum_Shutter/Capture_Banding_Filter*Capture_Banding_Filter):0;     
    

    OV2685_MIPI_Capture_Shutter = Capture_Exposure - OV2685_MIPI_Capture_Extra_Lines;
	  spin_unlock(&ov2685_drv_lock);

//debug();
	SENSORDB("exit OV2685_MIPI_Computer_AECAGC \n"); //debug sarah



}

//void OV2685_MIPI_set_isp_driving_current(kal_uint8 current)
//{
	
//}
static void OV2685_MIPI_set_AE_mode(kal_bool AE_enable)
{

	SENSORDB("enter OV2685_MIPI_set_AE_mode \n"); //debug sarah
//debug();


    kal_uint8 temp_AE_reg = 0;

    if (AE_enable == KAL_TRUE)
    {
        // turn on AEC/AGC
        temp_AE_reg = OV2685_MIPI_read_cmos_sensor(0x3503); 
        OV2685_MIPI_write_cmos_sensor(0x3503, temp_AE_reg&0x00);  //00 ><
    }
    else
    {
        // turn off AEC/AGC
        temp_AE_reg = OV2685_MIPI_read_cmos_sensor(0x3503);
        OV2685_MIPI_write_cmos_sensor(0x3503, temp_AE_reg|0x03);
    }
//debug();
	SENSORDB("exit OV2685_MIPI_set_AE_mode \n"); //debug sarah

	
}
static void OV2685_MIPI_set_AWB_mode(kal_bool AWB_enable)
{
    kal_uint8 temp_AWB_reg = 0;

	SENSORDB("enter OV2685_MIPI_set_AWB_mode \n"); //debug sarah
debug();

    //return ;

    if (AWB_enable == KAL_TRUE)
    {
        //enable Auto WB
        temp_AWB_reg = OV2685_MIPI_read_cmos_sensor(0x5180);
        OV2685_MIPI_write_cmos_sensor(0x5180, temp_AWB_reg&0xfc);        //fc sarah
    }
    else
    {
        //turn off AWB
        temp_AWB_reg = OV2685_MIPI_read_cmos_sensor(0x5180);
        OV2685_MIPI_write_cmos_sensor(0x5180, temp_AWB_reg|0xfe);        //02
    }

debug();
	SENSORDB("exit OV2685_MIPI_set_AWB_mode \n"); //debug sarah
	
}



void OV2685_get_AEAWB_lock(UINT32 *pAElockRet32, UINT32 *pAWBlockRet32)
{
	SENSORDB("[OV2685_get_AEAWB_lock]\n");	
	
	*pAElockRet32 = 1;
	*pAWBlockRet32 = 1;
}


/*************************************************************************
* FUNCTION
*	OV2685_MIPI_night_mode
*
* DESCRIPTION
*	This function night mode of OV2685.
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
BOOL OV2685_MIPI_set_param_banding(UINT16 para); 
void OV2685_MIPI_night_mode(kal_bool enable)
{

	SENSORDB("enter OV2685_MIPI_night_mode \n"); //debug sarah
debug();

	  kal_uint8 night = OV2685_MIPI_read_cmos_sensor(0x3a00); 
	
	if (OV2685_MIPI_sensor_cap_state == KAL_FALSE)  
		{
			if (enable) 
			{

                        spin_lock(&ov2685_drv_lock);
		          OV2685_NIGHT_ENABLE = KAL_TRUE; //added by sarah
	 		   spin_unlock(&ov2685_drv_lock); 
	  
			  if (OV2685_MIPI_VEDIO_encode_mode == KAL_TRUE) 
				{
	     OV2685_MIPI_write_cmos_sensor(0x3082, 0x2c);
            OV2685_MIPI_write_cmos_sensor(0x3083, 0x03);
            OV2685_MIPI_write_cmos_sensor(0x3084, 0x0f);
            OV2685_MIPI_write_cmos_sensor(0x3085, 0x03);
            OV2685_MIPI_write_cmos_sensor(0x3086, 0x00);
            OV2685_MIPI_write_cmos_sensor(0x3087, 0x00);
           
            OV2685_MIPI_write_cmos_sensor(0x380c, 0x06);
            OV2685_MIPI_write_cmos_sensor(0x380d, 0xac);
            OV2685_MIPI_write_cmos_sensor(0x380e, 0x02);
            OV2685_MIPI_write_cmos_sensor(0x380f, 0x84);                                        
                           
            OV2685_MIPI_write_cmos_sensor(0x3a06, 0x00);
            OV2685_MIPI_write_cmos_sensor(0x3a07, 0xc1);
            OV2685_MIPI_write_cmos_sensor(0x3a08, 0x00);
            OV2685_MIPI_write_cmos_sensor(0x3a09, 0xa1);
          
            OV2685_MIPI_write_cmos_sensor(0x3a0e, 0x04); 
            OV2685_MIPI_write_cmos_sensor(0x3a0f, 0x8c); 
            OV2685_MIPI_write_cmos_sensor(0x3a10, 0x05); 
            OV2685_MIPI_write_cmos_sensor(0x3a11, 0x08);  

	     OV2685_MIPI_write_cmos_sensor(0x382a,0x08);
			 
            OV2685_MIPI_write_cmos_sensor(0x3a00, night&0x43);                                     
            OV2685_MIPI_write_cmos_sensor(0x3a0a, 0x09);
	     OV2685_MIPI_write_cmos_sensor(0x3a0b, 0xda);
	     OV2685_MIPI_write_cmos_sensor(0x3a0c, 0x0a);
	     OV2685_MIPI_write_cmos_sensor(0x3a0d, 0x1c);           
	     OV2685_MIPI_write_cmos_sensor(0x3a12,0x00);//; max gain(13.5x) //added by sarah
            OV2685_MIPI_write_cmos_sensor(0x3a13,0xc8);//; max gain(13.5x) //added by sarah
                              }
				else 
				{
					  /* Camera mode only */	
	     OV2685_MIPI_write_cmos_sensor(0x382a,0x08);
            OV2685_MIPI_write_cmos_sensor(0x3a00, night|0x43);                                     
	     OV2685_MIPI_write_cmos_sensor(0x3a06, 0x00);  //add sarah
            OV2685_MIPI_write_cmos_sensor(0x3a07, 0xc1); //add sarah
            OV2685_MIPI_write_cmos_sensor(0x3a08, 0x00); //add sarah
            OV2685_MIPI_write_cmos_sensor(0x3a09, 0xa1); //add sarah
            OV2685_MIPI_write_cmos_sensor(0x3a0e, 0x03);  //02
            OV2685_MIPI_write_cmos_sensor(0x3a0f, 0x9e);  //43
            OV2685_MIPI_write_cmos_sensor(0x3a10, 0x04);  //02
            OV2685_MIPI_write_cmos_sensor(0x3a11, 0x06);  //84
            OV2685_MIPI_write_cmos_sensor(0x3a0a, 0x07); //04
	     OV2685_MIPI_write_cmos_sensor(0x3a0b, 0x94); //86
	     OV2685_MIPI_write_cmos_sensor(0x3a0c, 0x08); //05
	     OV2685_MIPI_write_cmos_sensor(0x3a0d, 0x16); //08
	     OV2685_MIPI_write_cmos_sensor(0x3a12,0x03);//; max gain(108x) //add by sarah //06
            OV2685_MIPI_write_cmos_sensor(0x3a13,0xe8);//; max gain(108x) //add by sarah //40
	     OV2685_MIPI_write_cmos_sensor(0x5309,0x00);//;02;04;0a; lower edge detail //04 sarah //02 sarah test //02 20150128	
	     OV2685_MIPI_write_cmos_sensor(0x530c,0x02);//;02;04;0a; lower edge detail //04 sarah //02 sarah test //02 20150128			
				}	   
			}
			else 
			{
			   spin_lock(&ov2685_drv_lock);
		          OV2685_NIGHT_ENABLE = KAL_FALSE; //added by sarah
	 		   spin_unlock(&ov2685_drv_lock); 
				/* when enter normal mode (disable night mode) without light, the AE vibrate */
				if (OV2685_MIPI_VEDIO_encode_mode == KAL_TRUE) 
				{
					  /* MJPEG or MPEG4 Apps */
	     OV2685_MIPI_write_cmos_sensor(0x3082, 0x2c);
            OV2685_MIPI_write_cmos_sensor(0x3083, 0x03);
            OV2685_MIPI_write_cmos_sensor(0x3084, 0x0f);
            OV2685_MIPI_write_cmos_sensor(0x3085, 0x03);
            OV2685_MIPI_write_cmos_sensor(0x3086, 0x00);
            OV2685_MIPI_write_cmos_sensor(0x3087, 0x00);
           
            OV2685_MIPI_write_cmos_sensor(0x380c, 0x06);
            OV2685_MIPI_write_cmos_sensor(0x380d, 0xac);
            OV2685_MIPI_write_cmos_sensor(0x380e, 0x02);
            OV2685_MIPI_write_cmos_sensor(0x380f, 0x84);                                        
                           
            OV2685_MIPI_write_cmos_sensor(0x3a06, 0x00);
            OV2685_MIPI_write_cmos_sensor(0x3a07, 0xc1); //c1
            OV2685_MIPI_write_cmos_sensor(0x3a08, 0x00);
            OV2685_MIPI_write_cmos_sensor(0x3a09, 0xa1);
          
            OV2685_MIPI_write_cmos_sensor(0x3a0e, 0x02);
            OV2685_MIPI_write_cmos_sensor(0x3a0f, 0x43);
            OV2685_MIPI_write_cmos_sensor(0x3a10, 0x02);
            OV2685_MIPI_write_cmos_sensor(0x3a11, 0x84);

            OV2685_MIPI_write_cmos_sensor(0x382a,0x08);
            
            OV2685_MIPI_write_cmos_sensor(0x3a00, night&0x43); //41                                    
            OV2685_MIPI_write_cmos_sensor(0x3a0a, 0x04); //24
	     OV2685_MIPI_write_cmos_sensor(0x3a0b, 0x86); //60
	     OV2685_MIPI_write_cmos_sensor(0x3a0c, 0x05); //28
	     OV2685_MIPI_write_cmos_sensor(0x3a0d, 0x08); //40
	     OV2685_MIPI_write_cmos_sensor(0x3a12,0x00);//; max gain(13.5x) //added by sarah
            OV2685_MIPI_write_cmos_sensor(0x3a13,0xc8);//; max gain(13.5x) //added by sarah
				 }
				else 
				{
					  /* Camera mode only */					                     
            OV2685_MIPI_write_cmos_sensor(0x382a,0x08);
	     OV2685_MIPI_write_cmos_sensor(0x3a00, night|0x43); //41      
	     OV2685_MIPI_write_cmos_sensor(0x3a06, 0x00);  //add sarah
            OV2685_MIPI_write_cmos_sensor(0x3a07, 0xc1); //add sarah
            OV2685_MIPI_write_cmos_sensor(0x3a08, 0x00); //add sarah
            OV2685_MIPI_write_cmos_sensor(0x3a09, 0xa1); //add sarah
            OV2685_MIPI_write_cmos_sensor(0x3a0e, 0x02); 
	     OV2685_MIPI_write_cmos_sensor(0x3a0f, 0x43); 
	     OV2685_MIPI_write_cmos_sensor(0x3a10, 0x02); 
	     OV2685_MIPI_write_cmos_sensor(0x3a11, 0x84);
            OV2685_MIPI_write_cmos_sensor(0x3a0a, 0x04); //24
	     OV2685_MIPI_write_cmos_sensor(0x3a0b, 0x86); //60
	     OV2685_MIPI_write_cmos_sensor(0x3a0c, 0x05); //28
	     OV2685_MIPI_write_cmos_sensor(0x3a0d, 0x08); //40
	     OV2685_MIPI_write_cmos_sensor(0x3a12,0x01);//; max gain(27x) //add by sarah
            OV2685_MIPI_write_cmos_sensor(0x3a13,0x90);//; max gain(27x) //add by sarah
				}					
		  }
	 }       
        OV2685_MIPI_set_param_banding(OV2685_MIPI_Banding_setting); 
debug();
	SENSORDB("exit OV2685_MIPI_night_mode \n"); //debug sarah


		
}	/* OV2685_MIPI_night_mode */

/*************************************************************************
* FUNCTION
*	OV2685_MIPI_GetSensorID
*
* DESCRIPTION
*	This function get the sensor ID
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
static kal_uint32 OV2685_MIPI_GetSensorID(kal_uint32 *sensorID)
{
	  volatile signed char i;
		kal_uint32 sensor_id=0;
		kal_uint8 temp_sccb_addr = 0;
		//s_move to here from CISModulePowerOn()

//	SENSORDB("enter OV2685_MIPI_GetSensorID \n"); //debug sarah
//debug();

		OV2685_MIPI_write_cmos_sensor(0x0103,0x01);// Reset sensor
		mDELAY(10);		
		
			//	Read sensor ID to adjust I2C is OK?
			for(i=0;i<3;i++)
			{
				sensor_id = (OV2685_MIPI_read_cmos_sensor(0x300A) << 8) | OV2685_MIPI_read_cmos_sensor(0x300B);
				if(sensor_id == 0x2685)
				{
					SENSORDB("++++OV2685_MIPI_GetSensorID,read id = 0x%x\n", sensor_id);
                    break;
                    
				}
			}
            if(0x2685 == sensor_id)
            {
				SENSORDB("++++OV2685_MIPI_GetSensorID,read id = 0x%x\n", sensor_id);
			    *sensorID = sensor_id;
            }
            else
            {
                *sensorID = 0xFFFFFFFF;
                SENSORDB("[OV2685_MIPI_GetSensorID] sensor_id :0x%02x is err !!!\n",sensor_id);
                return ERROR_SENSOR_CONNECT_FAIL;
            }

		RETAILMSG(1, (TEXT("OV2685 Sensor Read ID OK \r\n")));
//	debug();    
//		SENSORDB("exit OV2685_MIPI_GetSensorID \n"); //debug sarah	
    return ERROR_NONE;  


}   
/*****************************************************************************/
/* Windows Mobile Sensor Interface */
/*****************************************************************************/
/*************************************************************************
* FUNCTION
*	OV2685Open
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
UINT32 OV2685Open(void)
{
	volatile signed char i;
	kal_uint16 sensor_id=0;

// 	SENSORDB("enter OV2685Open \n"); //debug sarah
#if WINMO_USE	
	OV2685hDrvI2C=DRV_I2COpen(0);
	DRV_I2CSetParam(OV2685hDrvI2C, I2C_VAL_FS_SAMPLE_DIV, 3);
	DRV_I2CSetParam(OV2685hDrvI2C, I2C_VAL_FS_STEP_DIV, 8);
	DRV_I2CSetParam(OV2685hDrvI2C, I2C_VAL_DELAY_LEN, 2);
	DRV_I2CSetParam(OV2685hDrvI2C, I2C_VAL_CLK_EXT, I2C_CLKEXT_DISABLE);
	OV2685I2CConfig.trans_mode=I2C_TRANSFER_FAST_MODE;
	OV2685I2CConfig.slaveAddr=OV2685_MIPI_sensor_write_I2C_address>>1;
	OV2685I2CConfig.RS_ST=I2C_TRANSFER_STOP;	/* for fast mode */

	CISModulePowerOn(KAL_TRUE);      // Power On CIS Power
	Sleep(10);							// To wait for Stable Power
#endif 
	zoom_factor = 0; 
	OV2685_MIPI_write_cmos_sensor(0x0103,0x01);// Reset sensor
  Sleep(10);

	//  Read sensor ID to adjust I2C is OK?
	for(i=0;i<3;i++)
	{
        sensor_id = (OV2685_MIPI_read_cmos_sensor(0x300A) << 8) | OV2685_MIPI_read_cmos_sensor(0x300B);
        if(sensor_id == 0x2685)
        {
            SENSORDB("++++OV2685Open,read id = 0x%x\n", sensor_id);
            break;
        }
    }
    if(0x2685 != sensor_id)
    {
        SENSORDB("[OV2685Open] sensor_id :0x%02x is err !!!!\n",sensor_id);
        return ERROR_SENSOR_CONNECT_FAIL;
    }
	
RETAILMSG(1, (TEXT("OV2685 Sensor Read ID OK \r\n")));
	
//init MIPI
    OV2685_MIPI_globle_init();
// 	SENSORDB("exit OV2685Open \n"); //debug sarah
    return ERROR_NONE;
}	/* OV2685Open() */

/*************************************************************************
* FUNCTION
*	OV2685Close
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
UINT32 OV2685Close(void)
{
//	CISModulePowerOn(FALSE);

#if WINMO_USE
	#ifndef SOFTWARE_I2C_INTERFACE	/* software I2c MODE */
	DRV_I2CClose(OV2685hDrvI2C);
	#endif
#endif 	
	return ERROR_NONE;
}	/* OV2685Close() */

/*************************************************************************
* FUNCTION
*	OV2685Preview
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
UINT32 OV2685Preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	kal_uint8 iTemp, temp_AE_reg, temp_AWB_reg;
    kal_uint16 iDummyPixels = 0, iDummyLines = 0, iStartX = 0, iStartY = 0;
    
 	SENSORDB("enter OV2685Preview \n"); //debug sarah
	debug();
	spin_lock(&ov2685_drv_lock);
	OV2685_MIPI_sensor_cap_state = KAL_FALSE;
	spin_unlock(&ov2685_drv_lock);  
	
;
	
#ifdef __SLT_DRV_OV3660_BURST_SHOTS__	//wujinyou, 2011.11.21
	preview_init_flag = 1;
#endif

  //  OV2685_MIPI_set_AE_mode(KAL_FALSE);   

//spin_lock(&ov2685_drv_lock);  //^.<
//   OV2685_MIPI_write_cmos_sensor(0x3500, OV2685_MIPI_exposure_line_h);  //mark off by sarah  //^.<
//    OV2685_MIPI_write_cmos_sensor(0x3501, OV2685_MIPI_exposure_line_m);  //mark off by sarah /^.<
//    OV2685_MIPI_write_cmos_sensor(0x3502, OV2685_MIPI_exposure_line_l); //mark off by sarah //^.<
//    OV2685_MIPI_write_cmos_sensor(0x350b, OV2685_MIPI_gain); //mark off by sarah //^.<
//spin_unlock(&ov2685_drv_lock); //^.<

    //OV2685_MIPI_write_OV2685_MIPI_gain(OV2685_MIPI_PV_Gain16);    //mark off by sarah //==

	  spin_lock(&ov2685_drv_lock);
	       OV2685_MIPI_sensor_pclk=660; //390 pclk 20140107 
	  spin_unlock(&ov2685_drv_lock);

 //  OV2685_MIPI_write_cmos_sensor(0x3503,0x00); //added by sarah
 //   OV2685_MIPI_write_cmos_sensor(0x5180,0xfc); //added by sarah
    //YUV SVGA (800x600)      
//OV2685_MIPI_write_cmos_sensor(0x0100,0x00);   //mark off test green screen//^^
//OV2685_MIPI_write_cmos_sensor(0x3501,0x26);
//OV2685_MIPI_write_cmos_sensor(0x3502,0x40);
OV2685_MIPI_write_cmos_sensor(0x3620,0x26);
OV2685_MIPI_write_cmos_sensor(0x3621,0x37);
OV2685_MIPI_write_cmos_sensor(0x3622,0x04);
OV2685_MIPI_write_cmos_sensor(0x370a,0x23);
OV2685_MIPI_write_cmos_sensor(0x3718,0x88);
OV2685_MIPI_write_cmos_sensor(0x3721,0x00);
OV2685_MIPI_write_cmos_sensor(0x3722,0x00);
OV2685_MIPI_write_cmos_sensor(0x3723,0x00);
OV2685_MIPI_write_cmos_sensor(0x3738,0x00);
OV2685_MIPI_write_cmos_sensor(0x3808,0x03);
OV2685_MIPI_write_cmos_sensor(0x3809,0x20);
OV2685_MIPI_write_cmos_sensor(0x380a,0x02);
OV2685_MIPI_write_cmos_sensor(0x380b,0x58);
OV2685_MIPI_write_cmos_sensor(0x380c,0x06);
OV2685_MIPI_write_cmos_sensor(0x380d,0xac);
OV2685_MIPI_write_cmos_sensor(0x380e,0x02);
OV2685_MIPI_write_cmos_sensor(0x380f,0x84);
OV2685_MIPI_write_cmos_sensor(0x3810,0x00);
OV2685_MIPI_write_cmos_sensor(0x3811,0x04);
OV2685_MIPI_write_cmos_sensor(0x3812,0x00);
OV2685_MIPI_write_cmos_sensor(0x3813,0x04);
OV2685_MIPI_write_cmos_sensor(0x3814,0x31);
OV2685_MIPI_write_cmos_sensor(0x3815,0x31);
OV2685_MIPI_write_cmos_sensor(0x3819,0x04);
OV2685_MIPI_write_cmos_sensor(0x3820,0xc2);
OV2685_MIPI_write_cmos_sensor(0x3821,0x01);
//OV2685_MIPI_write_cmos_sensor(0x3a06,0x00);
//OV2685_MIPI_write_cmos_sensor(0x3a07,0xc1);
//OV2685_MIPI_write_cmos_sensor(0x3a08,0x00);
//OV2685_MIPI_write_cmos_sensor(0x3a09,0xa1);
//OV2685_MIPI_write_cmos_sensor(0x3a0a,0x12);
//OV2685_MIPI_write_cmos_sensor(0x3a0b,0x18);
//OV2685_MIPI_write_cmos_sensor(0x3a0c,0x14);
//OV2685_MIPI_write_cmos_sensor(0x3a0d,0x20);
//OV2685_MIPI_write_cmos_sensor(0x3a0e,0x02);
//OV2685_MIPI_write_cmos_sensor(0x3a0f,0x43); //43
//OV2685_MIPI_write_cmos_sensor(0x3a10,0x02);
//OV2685_MIPI_write_cmos_sensor(0x3a11,0x84);
OV2685_MIPI_write_cmos_sensor(0x4008,0x00);//00
OV2685_MIPI_write_cmos_sensor(0x4009,0x03);//03
OV2685_MIPI_write_cmos_sensor(0x3503,0x00); //added by sarah //== //00 ><
OV2685_MIPI_write_cmos_sensor(0x5180,0xfc); //added by sarah //==

//OV2685_MIPI_write_cmos_sensor(0x0100,0x01); //mark off test green screen//^^


	
	  spin_lock(&ov2685_drv_lock);
        OV2685_MIPI_gPVmode = KAL_TRUE;
	  spin_unlock(&ov2685_drv_lock);

    if(sensor_config_data->SensorOperationMode==MSDK_SENSOR_OPERATION_MODE_VIDEO)		// MPEG4 Encode Mode
    {
        RETAILMSG(1,(TEXT("Camera Video preview\r\n")));
	      spin_lock(&ov2685_drv_lock);
             OV2685_MIPI_VEDIO_encode_mode = KAL_TRUE;
	      spin_unlock(&ov2685_drv_lock);

        iDummyPixels = 0;
        iDummyLines = 0;

        /* to fix VSYNC, to fix frame rate */
   //     iTemp = OV2685_MIPI_read_cmos_sensor(0x3a00);    //mark off sarah
   //     OV2685_MIPI_write_cmos_sensor(0x3a00, iTemp&0x43); //Disable night mode //41 open sarah  //mark off sarah
   //     OV2685_MIPI_write_cmos_sensor(0x3a0a, 0x07); //12 //07   //mark off sarah
   //     OV2685_MIPI_write_cmos_sensor(0x3a0b, 0x94); //18 //94   //mark off sarah
   //     OV2685_MIPI_write_cmos_sensor(0x3a0c, 0x08); //14 //08   //mark off sarah
   //     OV2685_MIPI_write_cmos_sensor(0x3a0d, 0x2d); //20 //2d   //mark off sarah
        
        if (image_window->ImageTargetWidth <= OV2685_MIPI_VIDEO_QCIF_WIDTH)
        {
		        spin_lock(&ov2685_drv_lock);
            OV2685_MIPI_iOV2685_MIPI_Mode = OV2685_MIPI_MODE_QCIF_VIDEO;
		        spin_unlock(&ov2685_drv_lock);
        }
        else
        {
		        spin_lock(&ov2685_drv_lock);
            OV2685_MIPI_iOV2685_MIPI_Mode = OV2685_MIPI_MODE_QVGA_VIDEO;
		        spin_unlock(&ov2685_drv_lock);
        }
        //image_window->wait_stable_frame = 3;	
    }
    else
    {
        RETAILMSG(1,(TEXT("Camera preview\r\n")));
        //sensor_config_data->frame_rate == 30
        //ISP_PREVIEW_MODE
        //4  <2> if preview of capture PICTURE

        /* preview: 30 fps with 36M PCLK */
	      spin_lock(&ov2685_drv_lock);
             OV2685_MIPI_VEDIO_encode_mode = KAL_FALSE;
	      spin_unlock(&ov2685_drv_lock);

        iDummyPixels = 0; 
        iDummyLines = 0; 

	      spin_lock(&ov2685_drv_lock);
              OV2685_MIPI_iOV2685_MIPI_Mode = OV2685_MIPI_MODE_PREVIEW;
	      spin_unlock(&ov2685_drv_lock);        	
    }
        if(1)
        {
        	iStartX = 13;
          iStartY = 1;

 //<2014/12/09-Y46814 -Ivaan Pang.[Y20][Feature][Common][BSP][][] main camera rotate 180 degree. . 		  
          OV2685_MIPI_write_cmos_sensor(0x3820, 0xc2); //0xc6
          OV2685_MIPI_write_cmos_sensor(0x3821, 0x01);  //0x05  
 //>2014/12/09-Y46814 -Ivaan Pang.
        }
        else
        {
          iStartX = 1;
          iStartY = 1;          
          
          OV2685_MIPI_write_cmos_sensor(0x3820, 0xc6);
          OV2685_MIPI_write_cmos_sensor(0x3821, 0x05);     
        }

    //4 <6> set dummy
	  spin_lock(&ov2685_drv_lock);
         OV2685_MIPI_PV_Dummy_Pixels = iDummyPixels;
	  spin_unlock(&ov2685_drv_lock);
   // OV2685_MIPI_set_dummy(iDummyPixels, iDummyLines);
   
    temp_AE_reg = OV2685_MIPI_read_cmos_sensor(0x3503);
       	SENSORDB("after OV2685_MIPI_read_cmos_sensor(0x3503); \n"); //debug sarah	
debug();



 //   OV2685_MIPI_write_cmos_sensor(0x3503, temp_AE_reg&0x00);  //><
    
//    Sleep(200);  //><

    //4 <7> set shutter
    image_window->GrabStartX = iStartX;
    image_window->GrabStartY = iStartY;
    image_window->ExposureWindowWidth = OV2685_MIPI_IMAGE_SENSOR_PV_WIDTH - iStartX -2;
    image_window->ExposureWindowHeight = OV2685_MIPI_IMAGE_SENSOR_PV_HEIGHT- iStartY -2;
            
	  spin_lock(&ov2685_drv_lock);
         OV2685_MIPI_DELAY_AFTER_PREVIEW = 0;
	  spin_unlock(&ov2685_drv_lock);


debug();
   	SENSORDB("exit OV2685Preview \n"); //debug sarah	
  	memcpy(&OV2685SensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));

  	return ERROR_NONE;
}	/* OV2685Preview() */

UINT32 OV2685Capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	
 //if(OV2685_MIPI_sensor_cap_state = KAL_FALSE)
 {
    kal_uint32 shutter = 0;
    kal_uint32 temp_reg;
 	SENSORDB("enter OV2685Capture \n"); //debug sarah	
 debug();
	  spin_lock(&ov2685_drv_lock);
          OV2685_MIPI_sensor_cap_state = KAL_TRUE;
	  spin_unlock(&ov2685_drv_lock);  


 
//   OV2685_MIPI_set_AE_mode(KAL_FALSE);  //^.<
 //   temp_reg = OV2685_MIPI_read_cmos_sensor(0x3a00); //==

	
//    OV2685_MIPI_write_cmos_sensor(0x3a00, temp_reg&0x43); //Disable night mode  //41 open sarah      //==
  
	//  spin_lock(&ov2685_drv_lock);
//   OV2685_MIPI_exposure_line_h = OV2685_MIPI_read_cmos_sensor(0x3500);  //^.<
//  OV2685_MIPI_exposure_line_m = OV2685_MIPI_read_cmos_sensor(0x3501); //^.<
//  OV2685_MIPI_exposure_line_l = OV2685_MIPI_read_cmos_sensor(0x3502); //^.<
	 // spin_unlock(&ov2685_drv_lock);
    //OV2685_MIPI_PV_Gain16 = OV2685_MIPI_read_OV2685_MIPI_gain();
//    shutter = OV2685_MIPI_read_shutter();  //sarah 20150309

    if ((image_window->ImageTargetWidth<=OV2685_MIPI_IMAGE_SENSOR_PV_WIDTH)&&
        (image_window->ImageTargetHeight<=OV2685_MIPI_IMAGE_SENSOR_PV_HEIGHT))
    {    /* Less than PV Mode */

	      spin_lock(&ov2685_drv_lock);
        OV2685_MIPI_gPVmode=KAL_TRUE;
        OV2685_MIPI_dummy_pixels = 0;
        OV2685_MIPI_dummy_lines = 0;
	      spin_unlock(&ov2685_drv_lock);
        
      //  OV2685_MIPI_write_shutter(shutter);
        
        image_window->GrabStartX = 1;
        image_window->GrabStartY = 1;
        image_window->ExposureWindowWidth= OV2685_MIPI_IMAGE_SENSOR_PV_WIDTH - 3;
        image_window->ExposureWindowHeight = OV2685_MIPI_IMAGE_SENSOR_PV_HEIGHT - 3;
    }
    else 
    {    
    	  /* 2M FULL Mode */
        image_window->GrabStartX=1;
        image_window->GrabStartY=6;
        image_window->ExposureWindowWidth=OV2685_MIPI_IMAGE_SENSOR_FULL_WIDTH - image_window->GrabStartX - 2;
        image_window->ExposureWindowHeight=OV2685_MIPI_IMAGE_SENSOR_FULL_HEIGHT -image_window->GrabStartY - 2;    	 

        // 1600x1200      
OV2685_MIPI_write_cmos_sensor(0x5180,0xfc); //added by sarah  //== //lock capture awb //fe
//OV2685_MIPI_write_cmos_sensor(0x3503,0x03); //add by sarah  //==
//OV2685_MIPI_write_cmos_sensor(0x0100,0x00); //^^
//OV2685_MIPI_write_cmos_sensor(0x3501,0x4e);
//OV2685_MIPI_write_cmos_sensor(0x3502,0xe0);
OV2685_MIPI_write_cmos_sensor(0x3620,0x24);
OV2685_MIPI_write_cmos_sensor(0x3621,0x34);
OV2685_MIPI_write_cmos_sensor(0x3622,0x03);
OV2685_MIPI_write_cmos_sensor(0x370a,0x21);
OV2685_MIPI_write_cmos_sensor(0x3718,0x80);
OV2685_MIPI_write_cmos_sensor(0x3721,0x09);
OV2685_MIPI_write_cmos_sensor(0x3722,0x06);
OV2685_MIPI_write_cmos_sensor(0x3723,0x59);
OV2685_MIPI_write_cmos_sensor(0x3738,0x99);
OV2685_MIPI_write_cmos_sensor(0x3808,0x06);
OV2685_MIPI_write_cmos_sensor(0x3809,0x40);
OV2685_MIPI_write_cmos_sensor(0x380a,0x04);
OV2685_MIPI_write_cmos_sensor(0x380b,0xb0);
OV2685_MIPI_write_cmos_sensor(0x380c,0x06);
OV2685_MIPI_write_cmos_sensor(0x380d,0xa4);
OV2685_MIPI_write_cmos_sensor(0x380e,0x05);
OV2685_MIPI_write_cmos_sensor(0x380f,0x0e);
OV2685_MIPI_write_cmos_sensor(0x3810,0x00);
OV2685_MIPI_write_cmos_sensor(0x3811,0x08);
OV2685_MIPI_write_cmos_sensor(0x3812,0x00);
OV2685_MIPI_write_cmos_sensor(0x3813,0x08);
OV2685_MIPI_write_cmos_sensor(0x3814,0x11);
OV2685_MIPI_write_cmos_sensor(0x3815,0x11);
OV2685_MIPI_write_cmos_sensor(0x3819,0x04);
OV2685_MIPI_write_cmos_sensor(0x3820,0xc0);
OV2685_MIPI_write_cmos_sensor(0x3821,0x00);

		OV2685_MIPI_write_cmos_sensor(0x3a06,0x00);
		OV2685_MIPI_write_cmos_sensor(0x3a07,0xc2);
		OV2685_MIPI_write_cmos_sensor(0x3a08,0x00);
		OV2685_MIPI_write_cmos_sensor(0x3a09,0xa1);

//added by sarah//		
if (OV2685_NIGHT_ENABLE== KAL_FALSE) 
{
		OV2685_MIPI_write_cmos_sensor(0x3a0e,0x04);  //exposure //02
		OV2685_MIPI_write_cmos_sensor(0x3a0f,0x8c);  //exposure //46
		OV2685_MIPI_write_cmos_sensor(0x3a10,0x05);  //exposure //02
		OV2685_MIPI_write_cmos_sensor(0x3a11,0x08);  //exposure //84
            	OV2685_MIPI_write_cmos_sensor(0x3a0a,0x09);
	    	OV2685_MIPI_write_cmos_sensor(0x3a0b,0xda);
	     	OV2685_MIPI_write_cmos_sensor(0x3a0c,0x0a);
	     	OV2685_MIPI_write_cmos_sensor(0x3a0d,0x1c);
}
else
{
		OV2685_MIPI_write_cmos_sensor(0x3a0e,0x12);  //exposure //02
		OV2685_MIPI_write_cmos_sensor(0x3a0f,0x18);  //exposure //46
		OV2685_MIPI_write_cmos_sensor(0x3a10,0x14);  //exposure //02
		OV2685_MIPI_write_cmos_sensor(0x3a11,0x20);  //exposure //84
            	OV2685_MIPI_write_cmos_sensor(0x3a0a,0x24);
	    	OV2685_MIPI_write_cmos_sensor(0x3a0b,0x60);
	     	OV2685_MIPI_write_cmos_sensor(0x3a0c,0x28);
	     	OV2685_MIPI_write_cmos_sensor(0x3a0d,0x60);
}
//added by sarah//
OV2685_MIPI_write_cmos_sensor(0x3a12,0x00);//; max gain(13.5x)
OV2685_MIPI_write_cmos_sensor(0x3a13,0xc8);//; max gain(13.5x)

OV2685_MIPI_write_cmos_sensor(0x4008,0x02);
OV2685_MIPI_write_cmos_sensor(0x4009,0x09);

OV2685_MIPI_write_cmos_sensor(0x3503,0x00); //add by sarah  //== //^.<
OV2685_MIPI_write_cmos_sensor(0x5309,0x00);//;02;04;0a; lower edge detail //04 sarah //02 sarah test //02 20150128
OV2685_MIPI_write_cmos_sensor(0x530c,0x02);//;02;04;0a; lower edge detail //04 sarah //02 sarah test //02 20150128
//OV2685_MIPI_write_cmos_sensor(0x5180,0xfc); //added by sarah  //==  //^.<  


       
	      spin_lock(&ov2685_drv_lock);
          OV2685_MIPI_gPVmode = KAL_FALSE;
	      spin_unlock(&ov2685_drv_lock);
           
 /*       if ((image_window->ImageTargetWidth<=OV2685_MIPI_IMAGE_SENSOR_FULL_WIDTH)&&
        (image_window->ImageTargetHeight<=OV2685_MIPI_IMAGE_SENSOR_FULL_HEIGHT))
        {     
		   
	        if (zoom_factor  <  3) 
	        {  
			        shutter = shutter;          
	        }
	        else 
	        {
						  shutter = shutter;
	        }                  
        }
        else//Interpolate to 3M
        {
  	      if (image_window->ZoomFactor >= 340)
	        {  	        
					    shutter = shutter;       
			    }
	        else if (image_window->ZoomFactor >= 240) 
	        {          
					    shutter = shutter;
	        }
	        else 
	        {  
					    shutter = shutter;
			    }                  					
        }										*/ //sarah
//       	spin_lock(&ov2685_drv_lock);
//          OV2685_MIPI_Capture_Dummy_Pixels = OV2685_MIPI_dummy_pixels ;
//          OV2685_MIPI_Capture_Dummy_Lines = OV2685_MIPI_dummy_lines;
//	      spin_unlock(&ov2685_drv_lock);   
        
        //OV2685_MIPI_set_dummy(OV2685_MIPI_dummy_pixels, OV2685_MIPI_dummy_lines);
       
   //  OV2685_MIPI_write_shutter(shutter);		       
      }
 }
	  spin_lock(&ov2685_drv_lock);
        OV2685_MIPI_DELAY_AFTER_PREVIEW = 0;
	  spin_unlock(&ov2685_drv_lock);


	  
	  memcpy(&OV2685SensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
		   debug();
	  	   SENSORDB("exit OV2685Capture \n"); //debug sarah	  

	  return ERROR_NONE;


}	/* OV2685Capture() */

UINT32 OV2685GetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
	SENSORDB("enter OV2685GetResolution \n"); //debug sarah	
 debug();
	pSensorResolution->SensorFullWidth=OV2685_MIPI_IMAGE_SENSOR_FULL_WIDTH - 2 * IMAGE_SENSOR_START_GRAB_X - 8;  //modify by yanxu
	pSensorResolution->SensorFullHeight=OV2685_MIPI_IMAGE_SENSOR_FULL_HEIGHT - 2 * IMAGE_SENSOR_START_GRAB_Y -6;
	pSensorResolution->SensorPreviewWidth=OV2685_MIPI_IMAGE_SENSOR_PV_WIDTH - 2 * IMAGE_SENSOR_START_GRAB_X -8;
	pSensorResolution->SensorPreviewHeight=OV2685_MIPI_IMAGE_SENSOR_PV_HEIGHT - 2 * IMAGE_SENSOR_START_GRAB_Y -6;
	pSensorResolution->SensorVideoWidth=OV2685_MIPI_IMAGE_SENSOR_PV_WIDTH - 2 * IMAGE_SENSOR_START_GRAB_X -8;
	pSensorResolution->SensorVideoHeight=OV2685_MIPI_IMAGE_SENSOR_PV_HEIGHT - 2 * IMAGE_SENSOR_START_GRAB_Y -6;
 debug();
	SENSORDB("exit OV2685GetResolution \n"); //debug sarah	
	return ERROR_NONE;


}	/* OV2685GetResolution() */

UINT32 OV2685GetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
					  MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{

	SENSORDB("enter OV2685GetInfo \n"); //debug sarah	
//debug();
 
	pSensorInfo->SensorPreviewResolutionX=OV2685_MIPI_IMAGE_SENSOR_PV_WIDTH;
	pSensorInfo->SensorPreviewResolutionY=OV2685_MIPI_IMAGE_SENSOR_PV_HEIGHT;
	pSensorInfo->SensorFullResolutionX=OV2685_MIPI_IMAGE_SENSOR_FULL_WIDTH;
	pSensorInfo->SensorFullResolutionY=OV2685_MIPI_IMAGE_SENSOR_FULL_HEIGHT;

	pSensorInfo->SensorCameraPreviewFrameRate=30;
	pSensorInfo->SensorVideoFrameRate=30;
	pSensorInfo->SensorStillCaptureFrameRate=15; //10 sarah
	pSensorInfo->SensorWebCamCaptureFrameRate=15;
	pSensorInfo->SensorResetActiveHigh=FALSE;
	pSensorInfo->SensorResetDelayCount=1;
	pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_UYVY;
	pSensorInfo->SensorClockPolarity=SENSOR_CLOCK_POLARITY_LOW;	/*??? */
	pSensorInfo->SensorClockFallingPolarity=SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorInterruptDelayLines = 1;
	pSensorInfo->SensroInterfaceType=SENSOR_INTERFACE_TYPE_MIPI;    //SENSOR_INTERFACE_TYPE_PARALLEL
/*
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxWidth=CAM_SIZE_5M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxHeight=CAM_SIZE_5M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxWidth=CAM_SIZE_5M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxHeight=CAM_SIZE_5M_HEIGHT;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].ISOSupported=TRUE;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].BinningEnable=FALSE;

	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxWidth=CAM_SIZE_5M_WIDTH;
	pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxHeight=CAM_SIZE_5M_HEIGHT;
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
*/
	pSensorInfo->CaptureDelayFrame = 4; 
	pSensorInfo->PreviewDelayFrame = 4; 
	pSensorInfo->VideoDelayFrame = 2; 		
	pSensorInfo->SensorMasterClockSwitch = 0; 
  pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_8MA;  	

	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		//case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
			   pSensorInfo->SensorClockFreq=24;
			   pSensorInfo->SensorClockDividCount=	3;
			   pSensorInfo->SensorClockRisingCount= 0;
			   pSensorInfo->SensorClockFallingCount= 2;
			   pSensorInfo->SensorPixelClockCount= 3;
			   pSensorInfo->SensorDataLatchCount= 2;
         pSensorInfo->SensorGrabStartX = 4; 
         pSensorInfo->SensorGrabStartY = 2;        
			pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_1_LANE; 		
			pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
		 pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; 
		 pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
			pSensorInfo->SensorWidthSampling = 0;  // 0 is default 1x
			pSensorInfo->SensorHightSampling = 0;	// 0 is default 1x 
			pSensorInfo->SensorPacketECCOrder = 1;
		     break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_ZSD:
		//case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
			   pSensorInfo->SensorClockFreq=24;
			   pSensorInfo->SensorClockDividCount=	3;
			   pSensorInfo->SensorClockRisingCount= 0;
			   pSensorInfo->SensorClockFallingCount= 2;
			   pSensorInfo->SensorPixelClockCount= 3;
			   pSensorInfo->SensorDataLatchCount= 2;
         pSensorInfo->SensorGrabStartX = 4; 
         pSensorInfo->SensorGrabStartY = 2;   
		 
			pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_1_LANE; 		
			pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
		 pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; 
		 pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
			pSensorInfo->SensorWidthSampling = 0;  // 0 is default 1x
			pSensorInfo->SensorHightSampling = 0;	// 0 is default 1x 
			pSensorInfo->SensorPacketECCOrder = 1;
				 break;
		default:
			   pSensorInfo->SensorClockFreq=24;
			   pSensorInfo->SensorClockDividCount=3;
			   pSensorInfo->SensorClockRisingCount=0;
			   pSensorInfo->SensorClockFallingCount=2;
			   pSensorInfo->SensorPixelClockCount=3;
			   pSensorInfo->SensorDataLatchCount=2;
         pSensorInfo->SensorGrabStartX = 4; 
         pSensorInfo->SensorGrabStartY = 2;      
			pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_1_LANE; 		
			pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
		    pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; 
		    pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
			pSensorInfo->SensorWidthSampling = 0;  // 0 is default 1x
			pSensorInfo->SensorHightSampling = 0;	// 0 is default 1x 
			pSensorInfo->SensorPacketECCOrder = 1;
	       break;
	}
	spin_lock(&ov2685_drv_lock);
	OV2685_MIPI_PixelClockDivider=pSensorInfo->SensorPixelClockCount;
	spin_unlock(&ov2685_drv_lock);
	memcpy(pSensorConfigData, &OV2685SensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
//debug();
	SENSORDB("exit OV2685GetInfo \n"); //debug sarah		
	return ERROR_NONE;



}	/* OV2685GetInfo() */

UINT32 OV2685Control(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{

	SENSORDB("enter OV2685Control \n"); //debug sarah	
// debug();
 
	switch (ScenarioId)
	{
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
		//case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
			   OV2685Preview(pImageWindow, pSensorConfigData);
		     break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			case MSDK_SCENARIO_ID_CAMERA_ZSD:
		//case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
			   OV2685Capture(pImageWindow, pSensorConfigData);         
		     break;
		default:
		     break; 
	}
//debug();
	SENSORDB("exit OV2685Control \n"); //debug sarah		
	return TRUE;


}	/* OV2685Control() */

#if WINMO_USE
void OV2685Query(PMSDK_FEATURE_INFO_STRUCT pSensorFeatureInfo)
{

	SENSORDB("enter OV2685Query \n"); //debug sarah	
 //debug();

	MSDK_FEATURE_TYPE_RANGE_STRUCT *pFeatureRange;
	MSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT *pFeatureMultiSelection;
	switch (pSensorFeatureInfo->FeatureId)
	{
		case ISP_FEATURE_DSC_MODE:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_SCENE_MODE_MAX;
			pFeatureMultiSelection->DefaultSelection = CAM_AUTO_DSC_MODE;
			pFeatureMultiSelection->SupportedSelection = 
				(CAMERA_FEATURE_SUPPORT(CAM_AUTO_DSC_MODE)|
				CAMERA_FEATURE_SUPPORT(CAM_NIGHTSCENE_MODE));			
		break;
		case ISP_FEATURE_WHITEBALANCE:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_VIDEO_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_WB;
			pFeatureMultiSelection->DefaultSelection = CAM_WB_AUTO;
			pFeatureMultiSelection->SupportedSelection = 
				(CAMERA_FEATURE_SUPPORT(CAM_WB_AUTO)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_CLOUD)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_DAYLIGHT)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_INCANDESCENCE)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_TUNGSTEN)|
				CAMERA_FEATURE_SUPPORT(CAM_WB_FLUORESCENT));
		break;
		case ISP_FEATURE_IMAGE_EFFECT:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_VIDEO_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_EFFECT_ENC;
			pFeatureMultiSelection->DefaultSelection = CAM_EFFECT_ENC_NORMAL;
			pFeatureMultiSelection->SupportedSelection = 
				(CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_NORMAL)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_GRAYSCALE)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_COLORINV)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_SEPIAGREEN)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_SEPIABLUE)|
				CAMERA_FEATURE_SUPPORT(CAM_EFFECT_ENC_SEPIA));	
		break;
		case ISP_FEATURE_AE_METERING_MODE:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;
		break;
		case ISP_FEATURE_BRIGHTNESS:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_RANGE;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_VIDEO_SUPPORTED);
			pFeatureRange = (PMSDK_FEATURE_TYPE_RANGE_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureRange);
			pFeatureRange->MinValue = CAM_EV_NEG_4_3;
			pFeatureRange->MaxValue = CAM_EV_POS_4_3;
			pFeatureRange->StepValue = CAMERA_FEATURE_ID_EV_STEP;
			pFeatureRange->DefaultValue = CAM_EV_ZERO;
		break;
		case ISP_FEATURE_BANDING_FREQ:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_CAMERA_VIDEO_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_BANDING;
			pFeatureMultiSelection->DefaultSelection = CAM_BANDING_50HZ;
			pFeatureMultiSelection->SupportedSelection = 
				(CAMERA_FEATURE_SUPPORT(CAM_BANDING_50HZ)|
				CAMERA_FEATURE_SUPPORT(CAM_BANDING_60HZ));
		break;
		case ISP_FEATURE_AF_OPERATION:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;
		break;
		case ISP_FEATURE_AF_RANGE_CONTROL:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;
		break;
		case ISP_FEATURE_FLASH:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;			
		break;
		case ISP_FEATURE_VIDEO_SCENE_MODE:
			pSensorFeatureInfo->FeatureType = MSDK_FEATURE_TYPE_MULTI_SELECTION;
			pSensorFeatureInfo->FeatureSupported = (UINT8)(MSDK_SET_GET_FEATURE_SUPPORTED|MSDK_QUERY_VIDEO_SUPPORTED);
			pFeatureMultiSelection = (PMSDK_FEATURE_TYPE_MULTI_SELECTION_STRUCT)(&pSensorFeatureInfo->FeatureInformation.FeatureMultiSelection);
			pFeatureMultiSelection->TotalSelection = CAM_NO_OF_SCENE_MODE_MAX;
			pFeatureMultiSelection->DefaultSelection = CAM_VIDEO_AUTO_MODE;
			pFeatureMultiSelection->SupportedSelection = 
				(CAMERA_FEATURE_SUPPORT(CAM_VIDEO_AUTO_MODE)|
				CAMERA_FEATURE_SUPPORT(CAM_VIDEO_NIGHT_MODE));
		break;
		case ISP_FEATURE_ISO:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;			
		break;
		default:
			pSensorFeatureInfo->FeatureSupported = MSDK_FEATURE_NOT_SUPPORTED;			
		break;
	}
 //debug();	
		SENSORDB("exit OV2685Query \n"); //debug sarah	

}
#endif 
//<2014/12/26-joubertshe, [Y20][BUG][Common][BSP][Internal][]camera sunset and night mode issue fix

BOOL OV2685_MIPI_set_param_wb(UINT16 para)
{
    kal_uint8  temp_reg,temp_AE_reg;

	SENSORDB("enter OV2685_MIPI_set_param_wb \n"); //debug sarah
debug();

    temp_reg=OV2685_MIPI_read_cmos_sensor(0x5180);

    switch (para)
    {
        case AWB_MODE_OFF:
			//wb_mode_value=AWB_MODE_OFF;
            //OV2685_MIPI_AWB_ENABLE = KAL_FALSE; 
            //OV2685_MIPI_set_AWB_mode(OV2685_MIPI_AWB_ENABLE);            
            //break;                     
        case AWB_MODE_AUTO:
			//wb_mode_value=AWB_MODE_AUTO;
		        spin_lock(&ov2685_drv_lock);
                 OV2685_MIPI_AWB_ENABLE = KAL_TRUE;  
		        spin_unlock(&ov2685_drv_lock);
            OV2685_MIPI_set_AWB_mode(OV2685_MIPI_AWB_ENABLE);
            OV2685_MIPI_write_cmos_sensor(0x5180, temp_reg&0xfc);            //fc  //fc 20141229
            break;

        case AWB_MODE_CLOUDY_DAYLIGHT: //cloudy
       // wb_mode_value=AWB_MODE_CLOUDY_DAYLIGHT;
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5180, temp_reg|0xfe); //02
            OV2685_MIPI_write_cmos_sensor(0x5195, 0x06); //07
            OV2685_MIPI_write_cmos_sensor(0x5196, 0x09); //dc
            OV2685_MIPI_write_cmos_sensor(0x5197, 0x04);  
            OV2685_MIPI_write_cmos_sensor(0x5198, 0x00); 
            OV2685_MIPI_write_cmos_sensor(0x5199, 0x06); //05
            OV2685_MIPI_write_cmos_sensor(0x519a, 0x27); //d3
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case AWB_MODE_DAYLIGHT: //sunny
        //wb_mode_value=AWB_MODE_DAYLIGHT;
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5180, temp_reg|0xfe); //02
            OV2685_MIPI_write_cmos_sensor(0x5195, 0x05); //07
            OV2685_MIPI_write_cmos_sensor(0x5196, 0x70); //9c
            OV2685_MIPI_write_cmos_sensor(0x5197, 0x04);  
            OV2685_MIPI_write_cmos_sensor(0x5198, 0x00); 
            OV2685_MIPI_write_cmos_sensor(0x5199, 0x06); //05
            OV2685_MIPI_write_cmos_sensor(0x519a, 0xf2); //f3 
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case AWB_MODE_INCANDESCENT: //office
        //wb_mode_value=AWB_MODE_INCANDESCENT;
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5180, temp_reg|0xfe); //02
            OV2685_MIPI_write_cmos_sensor(0x5195, 0x04); //06
            OV2685_MIPI_write_cmos_sensor(0x5196, 0x2a); //b8
            OV2685_MIPI_write_cmos_sensor(0x5197, 0x04); //04 
            OV2685_MIPI_write_cmos_sensor(0x5198, 0x00); //00
            OV2685_MIPI_write_cmos_sensor(0x5199, 0x08); //06
            OV2685_MIPI_write_cmos_sensor(0x519a, 0x00); //5f
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case AWB_MODE_TUNGSTEN: //home
        // wb_mode_value=AWB_MODE_TUNGSTEN;
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5180, temp_reg|0xfe); //02
            OV2685_MIPI_write_cmos_sensor(0x5195, 0x04); 
            OV2685_MIPI_write_cmos_sensor(0x5196, 0x00); //90
            OV2685_MIPI_write_cmos_sensor(0x5197, 0x04);  
            OV2685_MIPI_write_cmos_sensor(0x5198, 0xb4); //00 
            OV2685_MIPI_write_cmos_sensor(0x5199, 0x09);
            OV2685_MIPI_write_cmos_sensor(0x519a, 0x82); //20 
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case AWB_MODE_FLUORESCENT:
		//	wb_mode_value=AWB_MODE_FLUORESCENT;
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5180, temp_reg|0xfe); //02
            OV2685_MIPI_write_cmos_sensor(0x5195, 0x04); 
            OV2685_MIPI_write_cmos_sensor(0x5196, 0xfa); //dc
            OV2685_MIPI_write_cmos_sensor(0x5197, 0x04);  
            OV2685_MIPI_write_cmos_sensor(0x5198, 0x00); 
            OV2685_MIPI_write_cmos_sensor(0x5199, 0x07); //06
            OV2685_MIPI_write_cmos_sensor(0x519a, 0xca); //d3 
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;
#if WINMO_USE
        case AWB_MODE_MANUAL:
            break;
#endif 
        default:
debug();
	SENSORDB("exit OV2685_MIPI_set_param_wb \n"); //debug sarah        	
            return FALSE;
    }
debug();
	SENSORDB("exit OV2685_MIPI_set_param_wb \n"); //debug sarah    
    return TRUE;



} /* OV2685_MIPI_set_param_wb */
//>2014/12/26-joubertshe

BOOL OV2685_MIPI_set_param_effect(UINT16 para)
{
    BOOL  ret = TRUE;   
       SENSORDB("enter OV2685_MIPI_set_param_effect \n"); //debug sarah
debug();

    switch (para)
    {
        case MEFFECT_OFF:
        	//OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);  //mark off by sarah
        	
            //OV2685_MIPI_write_cmos_sensor(0x5600, 0x06);  //mark off by sarah
            //OV2685_MIPI_write_cmos_sensor(0x5603, 0x48); //40  //mark off by sarah
            //OV2685_MIPI_write_cmos_sensor(0x5604, 0x30); //28  //mark off by sarah
            
            //OV2685_MIPI_write_cmos_sensor(0x3208, 0x10);  //mark off by sarah
            //OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);  //mark off by sarah
            break;

        case MEFFECT_SEPIA:
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5600, 0x1e); 
            OV2685_MIPI_write_cmos_sensor(0x5603, 0x40); 
            OV2685_MIPI_write_cmos_sensor(0x5604, 0xa0); 
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case MEFFECT_NEGATIVE:
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5600, 0x46); 
            OV2685_MIPI_write_cmos_sensor(0x5603, 0x40); 
            OV2685_MIPI_write_cmos_sensor(0x5604, 0x28); 
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case MEFFECT_SEPIAGREEN:
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5600, 0x1e); 
            OV2685_MIPI_write_cmos_sensor(0x5603, 0x60); 
            OV2685_MIPI_write_cmos_sensor(0x5604, 0x60); 
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case MEFFECT_SEPIABLUE:
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5600, 0x1e); 
            OV2685_MIPI_write_cmos_sensor(0x5603, 0xa0); 
            OV2685_MIPI_write_cmos_sensor(0x5604, 0x40); 
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;
		case MEFFECT_MONO: //B&W
			      OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
			
            OV2685_MIPI_write_cmos_sensor(0x5600, 0x1e); 
            OV2685_MIPI_write_cmos_sensor(0x5603, 0x80); 
            OV2685_MIPI_write_cmos_sensor(0x5604, 0x80); 
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
			break;
#if WINMO_USE
        case CAM_EFFECT_ENC_GRAYINV:
        case CAM_EFFECT_ENC_COPPERCARVING:
        case CAM_EFFECT_ENC_BLUECARVING:
        case CAM_EFFECT_ENC_EMBOSSMENT:
        case CAM_EFFECT_ENC_SKETCH:
        case CAM_EFFECT_ENC_BLACKBOARD:
        case CAM_EFFECT_ENC_WHITEBOARD:
        case CAM_EFFECT_ENC_JEAN:
        case CAM_EFFECT_ENC_OIL:
#endif 
        default:
            ret = FALSE;
    }
debug();        
SENSORDB("exit OV2685_MIPI_set_param_effect \n"); //debug sarah
   
return ret;


} /* OV2685_MIPI_set_param_effect */

BOOL OV2685_MIPI_set_param_banding(UINT16 para)
{
    kal_uint8 banding;
           SENSORDB("enter OV2685_MIPI_set_param_banding \n"); //debug sarah
debug();    
    banding = OV2685_MIPI_read_cmos_sensor(0x3a02);

    switch (para)
    {
    case AE_FLICKER_MODE_50HZ:
		                spin_lock(&ov2685_drv_lock);		
		                OV2685_MIPI_Banding_setting = AE_FLICKER_MODE_50HZ;		
		                spin_unlock(&ov2685_drv_lock);
		
		                OV2685_MIPI_write_cmos_sensor(0x3a02,banding|0x9f);    /* enable banding and 50 Hz */				
                    break;    
    case AE_FLICKER_MODE_60HZ:
		                spin_lock(&ov2685_drv_lock);
		                OV2685_MIPI_Banding_setting = AE_FLICKER_MODE_60HZ;	
		                spin_unlock(&ov2685_drv_lock);	
	 
                    OV2685_MIPI_write_cmos_sensor(0x3a02,(banding&0x1f)); /* enable banding and 60 Hz */            
                    break;
					

					
            default:
            	debug();  
							SENSORDB("exit OV2685_MIPI_set_param_banding \n"); //debug sarah
                  
					return FALSE;
    }
    debug();  
		SENSORDB("exit OV2685_MIPI_set_param_banding \n"); //debug sarah

    return TRUE;

        
} /* OV2685_MIPI_set_param_banding */

BOOL OV2685_MIPI_set_param_exposure(UINT16 para)
{
    kal_uint8  temp_reg;
	  SENSORDB("enter OV2685_MIPI_set_param_exposure \n"); //debug sarah
debug();
    temp_reg=OV2685_MIPI_read_cmos_sensor(0x5600);

	SENSORDB(" OV2685_MIPI_set_param_exposure para=%d \n",para);

    switch (para)
    {
        case AE_EV_COMP_20: //AE_EV_COMP_n13
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5600, temp_reg|0x4);
            OV2685_MIPI_write_cmos_sensor(0x5608, 0x00);
            OV2685_MIPI_write_cmos_sensor(0x5607, 0x40);
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case AE_EV_COMP_10:    
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5600, temp_reg|0x4);
            OV2685_MIPI_write_cmos_sensor(0x5608, 0x00);
            OV2685_MIPI_write_cmos_sensor(0x5607, 0x30);
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case AE_EV_COMP_07:
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5600, temp_reg|0x4);
            OV2685_MIPI_write_cmos_sensor(0x5608, 0x00);
            OV2685_MIPI_write_cmos_sensor(0x5607, 0x20);
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case AE_EV_COMP_03:
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5600, temp_reg|0x4);
            OV2685_MIPI_write_cmos_sensor(0x5608, 0x00);
            OV2685_MIPI_write_cmos_sensor(0x5607, 0x10);
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case AE_EV_COMP_00:
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5600, temp_reg&0xfb); //temp_reg|0x4
            OV2685_MIPI_write_cmos_sensor(0x5608, 0x00);
            OV2685_MIPI_write_cmos_sensor(0x5607, 0x00);
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case AE_EV_COMP_n03:
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5600, temp_reg|0x4);
            OV2685_MIPI_write_cmos_sensor(0x5608, 0x08);
            OV2685_MIPI_write_cmos_sensor(0x5607, 0x10);
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case AE_EV_COMP_n07:
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5600, temp_reg|0x4);
            OV2685_MIPI_write_cmos_sensor(0x5608, 0x08);
            OV2685_MIPI_write_cmos_sensor(0x5607, 0x20);
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case AE_EV_COMP_n10:
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5600, temp_reg|0x4);
            OV2685_MIPI_write_cmos_sensor(0x5608, 0x08);
            OV2685_MIPI_write_cmos_sensor(0x5607, 0x30);
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        case AE_EV_COMP_n20://AE_EV_COMP_13
        	  OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5600, temp_reg|0x4);
            OV2685_MIPI_write_cmos_sensor(0x5608, 0x08);
            OV2685_MIPI_write_cmos_sensor(0x5607, 0x40);
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
            break;

        default:
            return FALSE;
debug();
	  SENSORDB("exit OV2685_MIPI_set_param_exposure \n"); //debug sarah

    }

    return TRUE;
} /* OV2685_MIPI_set_param_exposure */

//<2014/12/26-joubertshe, [Y20][BUG][Common][BSP][Internal][]camera sunset and night mode issue fix
/*
void OV2685_set_scene_mode(UINT16 para)
{
	UINT16 para_scene;
	kal_uint8  temp_reg;
	SENSORDB("[OV2685_set_scene_mode]para=%d\n", para);

	
	temp_reg=OV2685_MIPI_read_cmos_sensor(0x5180);
	
    switch (para)
    { 
	case SCENE_MODE_NIGHTSCENE:
	 	OV2685_MIPI_set_AWB_mode(KAL_TRUE);
            	OV2685_MIPI_write_cmos_sensor(0x5180, temp_reg&0xfc);  
          	OV2685_MIPI_night_mode(1); 
			 break;
     //   case SCENE_MODE_PORTRAIT:
       // case SCENE_MODE_LANDSCAPE:

      //  case SCENE_MODE_SPORTS:	 
        //case SCENE_MODE_HDR:

	case SCENE_MODE_SUNSET:
		OV2685_MIPI_write_cmos_sensor(0x3208, 0x00);
        	
            OV2685_MIPI_write_cmos_sensor(0x5180, temp_reg|0xfe); //02
            OV2685_MIPI_write_cmos_sensor(0x5195, 0x04); //06
            OV2685_MIPI_write_cmos_sensor(0x5196, 0x2a); //b8
            OV2685_MIPI_write_cmos_sensor(0x5197, 0x04); //04 
            OV2685_MIPI_write_cmos_sensor(0x5198, 0x00); //00
            OV2685_MIPI_write_cmos_sensor(0x5199, 0x08); //06
            OV2685_MIPI_write_cmos_sensor(0x519a, 0x00); //5f
            
            OV2685_MIPI_write_cmos_sensor(0x3208, 0x10); 
            OV2685_MIPI_write_cmos_sensor(0x3208, 0xa0);
                  break;
        case SCENE_MODE_OFF:
		// para_scene=wb_mode_value;
		// OV2685_MIPI_set_param_wb( para_scene);
		 OV2685_MIPI_night_mode(0); 
	          break;
        default:
			return KAL_FALSE;
            break;
    }
}
	*/
	//>2014/12/26-joubertshe



UINT32 OV2685MIPIYUVSensorSetting(FEATURE_ID iCmd, UINT32 iPara)
{

	  SENSORDB("enter OV2685MIPIYUVSensorSetting \n"); //debug sarah
debug();


	switch (iCmd) {
	case FID_SCENE_MODE:	  
	                   if (iPara == SCENE_MODE_OFF)
	                   {
	                      OV2685_MIPI_night_mode(0); 
	                   }
	                   else if (iPara == SCENE_MODE_NIGHTSCENE)
	                   {
                        OV2685_MIPI_night_mode(1); 
	                   }	    
	                   break; 	    
	case FID_AWB_MODE:
	                   OV2685_MIPI_set_param_wb(iPara);
	                   break;
	case FID_COLOR_EFFECT:	  
                     OV2685_MIPI_set_param_effect(iPara);
	                   break;
	case FID_AE_EV:
                     #if WINMO_USE	    
	case ISP_FEATURE_EXPOSURE:
                     #endif 	    
	                   OV2685_MIPI_set_param_exposure(iPara);
	                   break;
	case FID_AE_FLICKER:  	    	    
                     OV2685_MIPI_set_param_banding(iPara);
	                   break;
  case FID_AE_SCENE_MODE: 
                     if (iPara == AE_MODE_OFF)
                     {
		                 spin_lock(&ov2685_drv_lock);
                     // OV2685_MIPI_AE_ENABLE = KAL_FALSE; 
                        OV2685_MIPI_AE_ENABLE = KAL_TRUE;
		                 spin_unlock(&ov2685_drv_lock);
                     }
                     else
                     {
		                 spin_lock(&ov2685_drv_lock);
                        OV2685_MIPI_AE_ENABLE = KAL_TRUE; 
		                 spin_unlock(&ov2685_drv_lock);
	                   }
                     OV2685_MIPI_set_AE_mode(OV2685_MIPI_AE_ENABLE);
                     break; 
	 case FID_ZOOM_FACTOR:
		                 spin_lock(&ov2685_drv_lock);
		                    zoom_factor = iPara; 
		                 spin_unlock(&ov2685_drv_lock);
                     break; 
      default:
	                   break;
	}
	debug();
SENSORDB("exit OV2685MIPIYUVSensorSetting \n"); //debug sarah
	
	return TRUE;

}   /* OV2685MIPIYUVSensorSetting */

UINT32 OV2685MIPIYUVSetVideoMode(UINT16 u2FrameRate)
{
    kal_uint8 iTemp;

	SENSORDB("enter OV2685MIPIYUVSetVideoMode \n"); //debug sarah
debug();
     
    iTemp = OV2685_MIPI_read_cmos_sensor(0x3a00);
    OV2685_MIPI_write_cmos_sensor(0x3a00, iTemp&0x41); //Disable night mode //41 open sarah

    if (u2FrameRate == 30)
    {
    	OV2685_MIPI_write_cmos_sensor(0x3082, 0x2c);
        OV2685_MIPI_write_cmos_sensor(0x3083, 0x03);
        OV2685_MIPI_write_cmos_sensor(0x3084, 0x0f);
        OV2685_MIPI_write_cmos_sensor(0x3085, 0x03);
        OV2685_MIPI_write_cmos_sensor(0x3086, 0x00);
        OV2685_MIPI_write_cmos_sensor(0x3087, 0x00);
        
        OV2685_MIPI_write_cmos_sensor(0x380c, 0x06);
        OV2685_MIPI_write_cmos_sensor(0x380d, 0xac);
        OV2685_MIPI_write_cmos_sensor(0x380e, 0x02);
        OV2685_MIPI_write_cmos_sensor(0x380f, 0x84);                                        
                       
        OV2685_MIPI_write_cmos_sensor(0x3a06, 0x00);
        OV2685_MIPI_write_cmos_sensor(0x3a07, 0xc2); //c1 sarah
        OV2685_MIPI_write_cmos_sensor(0x3a08, 0x00);
        OV2685_MIPI_write_cmos_sensor(0x3a09, 0xa1);
        
        OV2685_MIPI_write_cmos_sensor(0x3a0e, 0x02);
        OV2685_MIPI_write_cmos_sensor(0x3a0f, 0x43);
        OV2685_MIPI_write_cmos_sensor(0x3a10, 0x02);
        OV2685_MIPI_write_cmos_sensor(0x3a11, 0x84);
                                   
              OV2685_MIPI_write_cmos_sensor(0x3a0a, 0x04); //02 sarah
		OV2685_MIPI_write_cmos_sensor(0x3a0b, 0x86); //84 sarah
		OV2685_MIPI_write_cmos_sensor(0x3a0c, 0x05); //02 sarah
		OV2685_MIPI_write_cmos_sensor(0x3a0d, 0x08); //84 sarah

OV2685_MIPI_write_cmos_sensor(0x3a12,0x00);//; max gain(13.5x) //added by sarah
OV2685_MIPI_write_cmos_sensor(0x3a13,0xc8);//; max gain(13.5x) //added by sarah
		
    }
    else if (u2FrameRate == 15)       
    {
    	OV2685_MIPI_write_cmos_sensor(0x3082, 0x2c);
        OV2685_MIPI_write_cmos_sensor(0x3083, 0x03);
        OV2685_MIPI_write_cmos_sensor(0x3084, 0x0f);
        OV2685_MIPI_write_cmos_sensor(0x3085, 0x03);
        OV2685_MIPI_write_cmos_sensor(0x3086, 0x00);
        OV2685_MIPI_write_cmos_sensor(0x3087, 0x00);
        
        OV2685_MIPI_write_cmos_sensor(0x380c, 0x06);
        OV2685_MIPI_write_cmos_sensor(0x380d, 0xa4);
        OV2685_MIPI_write_cmos_sensor(0x380e, 0x05);
        OV2685_MIPI_write_cmos_sensor(0x380f, 0x0e);                                        
                       
        OV2685_MIPI_write_cmos_sensor(0x3a06, 0x00);
        OV2685_MIPI_write_cmos_sensor(0x3a07, 0xc2); //60 sarah
        OV2685_MIPI_write_cmos_sensor(0x3a08, 0x00);
        OV2685_MIPI_write_cmos_sensor(0x3a09, 0xa1); //50 sarah
        
        OV2685_MIPI_write_cmos_sensor(0x3a0e, 0x04);
        OV2685_MIPI_write_cmos_sensor(0x3a0f, 0x8c);
        OV2685_MIPI_write_cmos_sensor(0x3a10, 0x05);
        OV2685_MIPI_write_cmos_sensor(0x3a11, 0x08);
                                   
              OV2685_MIPI_write_cmos_sensor(0x3a0a, 0x09); //02 sarah
		OV2685_MIPI_write_cmos_sensor(0x3a0b, 0xda); //84 sarah
		OV2685_MIPI_write_cmos_sensor(0x3a0c, 0x0a); //02 sarah
		OV2685_MIPI_write_cmos_sensor(0x3a0d, 0x1c);  //84 sarah

OV2685_MIPI_write_cmos_sensor(0x3a12,0x00);//; max gain(13.5x) //added by sarah 
OV2685_MIPI_write_cmos_sensor(0x3a13,0xc8);//; max gain(13.5x) //added by sarah
		
    }
    else 
    {
        SENSORDB("Wrong frame rate setting \n");
    }

	spin_lock(&ov2685_drv_lock);
	OV2685_MIPI_VEDIO_encode_mode = KAL_TRUE;
	spin_unlock(&ov2685_drv_lock);
 debug();
	SENSORDB("exit OV2685MIPIYUVSetVideoMode \n"); //debug sarah       
    return TRUE;


}

UINT32 OV2685MIPIYUVSetSoftwarePWDNMode(kal_bool bEnable)
{

	SENSORDB("enter OV2685MIPIYUVSetSoftwarePWDNMode \n"); //debug sarah
debug();

    SENSORDB("[OV2685MIPIYUVSetSoftwarePWDNMode] Software Power down enable:%d\n", bEnable);
    if(bEnable) {   // enable software power down mode   
	     OV2685_MIPI_write_cmos_sensor(0x0100, 0x00);
    } else {
       OV2685_MIPI_write_cmos_sensor(0x0100, 0x01);  
    }
debug();
	SENSORDB("exit OV2685MIPIYUVSetSoftwarePWDNMode \n"); //debug sarah    
    return TRUE;


	
}
/*************************************************************************
* FUNCTION
*OV2685MIPIClose
*
* DESCRIPTION
* This HI257SetMaxFramerateByScenario is to turn off sensor module power.
*
* PARAMETERS
* None
*
* RETURNS
* None
*
* GLOBALS AFFECTED
*
*************************************************************************/
  UINT32 OV2685MIPISetMaxFramerateByScenario(MSDK_SCENARIO_ID_ENUM scenarioId, MUINT32 frameRate) {
	kal_uint32 pclk;
	kal_int16 dummyLine;
	kal_uint16 lineLength,frameHeight;

	SENSORDB("enter OV2685MIPISetMaxFramerateByScenario \n"); //debug sarah
debug();
	
		
	SENSORDB("HI257SetMaxFramerateByScenario: scenarioId = %d, frame rate = %d\n",scenarioId,frameRate);
	/*switch (scenarioId) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			pclk = 134200000;
			lineLength = IMX111MIPI_PV_LINE_LENGTH_PIXELS;
			frameHeight = (10 * pclk)/frameRate/lineLength;
			dummyLine = frameHeight - IMX111MIPI_PV_FRAME_LENGTH_LINES;
			break;
        case MSDK_SCENARIO_ID_CAMERA_3D_CAPTURE: //added   
			break;		
		default:
			break;
	}	*/
debug();
	SENSORDB("exit OV2685MIPISetMaxFramerateByScenario \n"); //debug sarah	
	return ERROR_NONE;


}
  /*************************************************************************
  * FUNCTION
  * HI257GetDefaultFramerateByScenario
  *
  * DESCRIPTION
  * This function is to turn off sensor module power.
  * RETURNS
  * None
  *
  * GLOBALS AFFECTED
  *
  *************************************************************************/
UINT32 OV2685MIPIGetDefaultFramerateByScenario(MSDK_SCENARIO_ID_ENUM scenarioId, MUINT32 *pframeRate) 
{

	SENSORDB("enter OV2685MIPIGetDefaultFramerateByScenario \n"); //debug sarah
debug();


	switch (scenarioId) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			 *pframeRate = 300;
			 break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_ZSD:
			 *pframeRate = 150; //220
			break;		//hhl 2-28
        case MSDK_SCENARIO_ID_CAMERA_3D_PREVIEW: //added
        case MSDK_SCENARIO_ID_CAMERA_3D_VIDEO:
        case MSDK_SCENARIO_ID_CAMERA_3D_CAPTURE: //added   
			 *pframeRate = 300;
			break;		
		default:
			break;
	}
debug();
		SENSORDB("exit OV2685MIPIGetDefaultFramerateByScenario \n"); //debug sarah


  	}

UINT32 OV2685FeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
							 UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{

		SENSORDB("enter OV2685FeatureControl \n"); //debug sarah
debug();
   //   OV2685_MIPI_write_cmos_sensor(0x5180,0xfe); //added by sarah

	UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
	UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
	UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
	UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
	MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
	MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;

#if WINMO_USE	
	PMSDK_FEATURE_INFO_STRUCT pSensorFeatureInfo=(PMSDK_FEATURE_INFO_STRUCT) pFeaturePara;
#endif 

	switch (FeatureId)
	{
		case SENSOR_FEATURE_GET_RESOLUTION:
			*pFeatureReturnPara16++=OV2685_MIPI_IMAGE_SENSOR_FULL_WIDTH;
			*pFeatureReturnPara16=OV2685_MIPI_IMAGE_SENSOR_FULL_HEIGHT;
			*pFeatureParaLen=4;
		break;
		case SENSOR_FEATURE_GET_PERIOD:
			*pFeatureReturnPara16++=OV2685_MIPI_PV_PERIOD_PIXEL_NUMS+OV2685_MIPI_PV_dummy_pixels;
			*pFeatureReturnPara16=OV2685_MIPI_PV_PERIOD_LINE_NUMS+OV2685_MIPI_PV_dummy_lines;
			*pFeatureParaLen=4;
		break;
		case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
			*pFeatureReturnPara32 = OV2685_MIPI_sensor_pclk/10;
			*pFeatureParaLen=4;
		break;
		case SENSOR_FEATURE_SET_ESHUTTER:
		break;
		case SENSOR_FEATURE_SET_NIGHTMODE:
			OV2685_MIPI_night_mode((BOOL) *pFeatureData16);
		break;
		case SENSOR_FEATURE_SET_GAIN:
		case SENSOR_FEATURE_SET_FLASHLIGHT:
		break;
		case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
			spin_lock(&ov2685_drv_lock);
			OV2685_MIPI_isp_master_clock=*pFeatureData32;
			spin_unlock(&ov2685_drv_lock);
		break;
		case SENSOR_FEATURE_SET_REGISTER:
			OV2685_MIPI_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
		break;
		case SENSOR_FEATURE_GET_REGISTER:
			pSensorRegData->RegData = OV2685_MIPI_read_cmos_sensor(pSensorRegData->RegAddr);
		break;
		case SENSOR_FEATURE_GET_CONFIG_PARA:
			memcpy(pSensorConfigData, &OV2685SensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
			*pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
		break;
		case SENSOR_FEATURE_SET_CCT_REGISTER:
		case SENSOR_FEATURE_GET_CCT_REGISTER:
		case SENSOR_FEATURE_SET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_ENG_REGISTER:
		case SENSOR_FEATURE_GET_REGISTER_DEFAULT:

		case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
		case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
		case SENSOR_FEATURE_GET_GROUP_INFO:
		case SENSOR_FEATURE_GET_ITEM_INFO:
		case SENSOR_FEATURE_SET_ITEM_INFO:
		case SENSOR_FEATURE_GET_ENG_INFO:
		break;


        case SENSOR_FEATURE_GET_AE_AWB_LOCK_INFO:
                OV2685_get_AEAWB_lock((UINT32*)(*pFeatureData32), (UINT32*)(*(pFeatureData32 + 1)));
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
		case SENSOR_FEATURE_CHECK_SENSOR_ID:
			 OV2685_MIPI_GetSensorID(pFeatureData32);
			 break;
		case SENSOR_FEATURE_SET_YUV_CMD:
//		       SENSORDB("OV2685 MIPI YUV sensor Setting:%d, %d \n", *pFeatureData32,  *(pFeatureData32+1));
			OV2685MIPIYUVSensorSetting((FEATURE_ID)*pFeatureData32, *(pFeatureData32+1));
		break;
#if WINMO_USE		    		
		case SENSOR_FEATURE_QUERY:
			OV2685Query(pSensorFeatureInfo);
			*pFeatureParaLen = sizeof(MSDK_FEATURE_INFO_STRUCT);
		break;		
		case SENSOR_FEATURE_SET_YUV_CAPTURE_RAW_SUPPORT:
			/* update yuv capture raw support flag by *pFeatureData16 */
		break;		
#endif 			
		case SENSOR_FEATURE_SET_VIDEO_MODE:
		       OV2685MIPIYUVSetVideoMode(*pFeatureData16);
		       break; 
	        case SENSOR_FEATURE_SET_SOFTWARE_PWDN:
	            OV2685MIPIYUVSetSoftwarePWDNMode((BOOL)*pFeatureData16);        	        	
	            break;
			   
		default:
			break;			
	}
	return ERROR_NONE;


debug();
		SENSORDB("exit OV2685FeatureControl \n"); //debug sarah


	
}	/* OV2685FeatureControl() */


SENSOR_FUNCTION_STRUCT	SensorFuncOV2685=
{
	OV2685Open,
	OV2685GetInfo,
	OV2685GetResolution,
	OV2685FeatureControl,
	OV2685Control,
	OV2685Close
};

UINT32 OV2685_MIPI_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{

//	SENSORDB("enter OV2685_MIPI_YUV_SensorInit \n"); //debug sarah
//debug();

	/* To Do : Check Sensor status here */
	if (pfFunc!=NULL)
		*pfFunc=&SensorFuncOV2685;

	return ERROR_NONE;

//	SENSORDB("exit OV2685_MIPI_YUV_SensorInit \n"); //debug sarah
//debug();

	
}	/* SensorInit() */



