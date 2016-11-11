#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
	#include <string.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

//20140814 tracy modify for 5" LCM start
#define FRAME_WIDTH  (540) //480
#define FRAME_HEIGHT (960)//854
//20140814 tracy modify for 5" LCM end
#define LCM_ID_HX8389B 0x89
#define LCM_Detect_Pin GPIO116  //20141126 tracy add for main/second source lcm detection

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

 unsigned int lcm_esd_test = FALSE;      ///only for ESD test

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util ;

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V3(para_tbl,size,force_update)        lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define   LCM_DSI_CMD_MODE							0
//20140826 tracy modify for fix pdp2 power reboot start

//20140814 tracy modify for 5" LCM start
static int LCM_Source;  //20141210 tracy modify for support main/second source lcm detection
static LCM_setting_table_V3 lcm_initialization_setting1[] = {
	
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/
	//20141210 tracy modify for support main/second source lcm detection start
	//TCL
    {0x39,0xB9,3,{0xFF,0x83,0x89}},
	  
	{0x39,0xB1,20,{0x7F,0x10,0x10,0x32,0x32,0x50,0x10,0xF2,0x58,0x80,0x20,0x20,0xF8,0xAA,
                 0xAA,0xA0,0x00,0x80,0x30,0x00}},
		
 	{0x39,0xB2,10,{0x80,0x50,0x05,0x07,0x40,0x38,0x11,0x64,0x5D,0x09}},
		            
 	{0x39,0xB4,11,{0x6E,0x6E,0x6E,0x6E,0x00,0x00,0x10,0x74,0x10,0x74,0xB0}},		
 	
 	{0x39,0xD3,35,{0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x32,0x10,0x00,
                   0x00,0x00,0x03,0xC6,0x03,0xC6,0x00,0x00,0x00,0x00,
                   0x35,0x33,0x04,0x04,0x37,0x00,0x00,0x00,0x05,0x08,
                   0x00,0x00,0x0A,0x00,0x01}},		 
 	
 	{0x39,0xD5,38,{0x18,0x18,0x18,0x18,0x19,0x19,0x18,0x18,0x20,0x21,
                   0x24,0x25,0x18,0x18,0x18,0x18,0x00,0x01,0x04,0x05,
                   0x02,0x03,0x06,0x07,0x18,0x18,0x18,0x18,0x18,0x18,
                   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}}, 
      
    {0x39,0xD6,38,{0x18,0x18,0x18,0x18,0x18,0x18,0x1B,0x1B,0x18,0x18,
                   0x21,0x20,0x18,0x18,0x18,0x18,0x07,0x06,0x03,0x02,
                   0x05,0x04,0x01,0x00,0x18,0x18,0x18,0x18,0x18,0x18,
                   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}},
                 
    {0x39,0xB6,3,{0x3D,0x3D,0xE0}},
	  
 	{0x39,0xE0,42,{0x07,0x0B,0x11,0x2E,0x27,0x3F,0x1F,0x41,0x07,0x0B,
                   0x0D,0x18,0x10,0x12,0x13,0x14,0x15,0x06,0x10,0x13,
                   0x18,0x00,0x0A,0x10,0x2E,0x27,0x3F,0x1F,0x42,0x07,
                   0x0B,0x0D,0x18,0x10,0x12,0x15,0x14,0x15,0x06,0x10,0x13,0x18}},

    {0x39,0xC7,3,{0x00,0x80,0x60}},
  
    {0x15,0xCC,1,{0x02}},
	
    {0x15,0xD2,1,{0x33}},
  
    {0x05,0x11,0,{}},		
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 150, {}},
	{0x05,0x29,0,{}},
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 10, {}},
	//20141210 tracy modify for support main/second source lcm detection end
};

//20140826 tracy modify for fix pdp2 power reboot end
static LCM_setting_table_V3 lcm_initialization_setting2[] = {
    {0x39,0xB9,3,{0xFF,0x83,0x89}},
	  
	{0x39,0xB1,20,{0x7F,0x10,0x10,0xf2,0x32,
                   0x90,0x10,0xEC,0x52,0x80,
                   0x20,0x20,0xF8,0xAA,0xAA,
                   0xA1,0x00,0x80,0x30,0x00}},
		
 	{0x39,0xB2,10,{0x80,0x50,0x05,0x07,0x40,
                   0x38,0x11,0x64,0x5D,0x09}},
		
 	{0x39,0xB4,11,{0x70,0x70,0x70,0x70,0x00,
                   0x00,0x10,0x56,0x10,0x56,0xB0}},		
	
 	{0x39,0xD3,35,{0x00,0x00,0x00,0x00,0x00,
                   0x08,0x00,0x32,0x10,0x00,
                   0x00,0x00,0x03,0xC6,0x03,
                   0xC6,0x00,0x00,0x00,0x00,
                   0x35,0x33,0x04,0x04,0x37,
                   0x00,0x00,0x00,0x05,0x08,
                   0x00,0x00,0x0A,0x00,0x01}},		
 	
 	
    {0x39,0xD5,38,{0x18,0x18,0x18,0x18,0x19,
	               0x19,0x18,0x18,0x20,0x21,
	               0x24,0x25,0x18,0x18,0x18,
	               0x18,0x00,0x01,0x04,0x05,
	               0x02,0x03,0x06,0x07,0x18,
	               0x18,0x18,0x18,0x18,0x18,
	               0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}}, 
   
    {0x39,0xD6,38,{0x18,0x18,0x18,0x18,0x18,
                   0x18,0x19,0x19,0x25,0x24,
                   0x21,0x20,0x18,0x18,0x18,
                   0x18,0x07,0x06,0x03,0x02,
                   0x05,0x04,0x01,0x00,0x18,
                   0x18,0x18,0x18,0x18,0x18,
                   0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}}, 	

    {0x39,0xBF,4,{0xB6,0x43,0x50,0xE0}},
  
 	{0x39,0xE0,42,{0x00,0x05,0x07,0x33,0x3A,
	               0x3F,0x1C,0x42,0x08,0x0B,
                   0x0E,0x18,0x0F,0x12,0x15,
	               0x13,0x14,0x06,0x11,0x13,
	               0x18,0x00,0x05,0x07,0x33,
	               0x3A,0x3F,0x1C,0x42,0x08,
	               0x0B,0x0E,0x18,0x0E,0x12,
	               0x14,0x12,0x13,0x06,0x11,0x13,0x18}},

    {0x15,0xD2,1,{0x33}},
        
    {0x15,0xCC,1,{0x02}},
	
    {0x39,0xC7,3,{0x00,0x80,0x60}},//20141121 tracy modify for protect surface noise

    {0x39,0xB6,2,{0x4C,0x4C}},
	{0x05,0x11,0,{}},		
	//{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 120, {}},
	{0x05,0x29,0,{}},
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 200, {}},

};
//20140814 tracy modify for 5" LCM end
//20141110 tracy add for increase close LCM speed start
static LCM_setting_table_V3 lcm_sleep_setting[] = {
    {0x05,0x28,0,{}},		
	{0x05,0x10,0,{}},
};
//20141110 tracy add for increase close LCM speed end
// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

        #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
        #else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE; //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE; 
        #endif
	
		// DSI
		/* Command mode setting */
		//1 Three lane or Four lane
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Video mode setting		
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		
		params->dsi.vertical_sync_active				= 4;// 3    2
		params->dsi.vertical_backporch					= 5;// 20   1
		params->dsi.vertical_frontporch					= 5; // 1  12
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 40;// 50  2
		params->dsi.horizontal_backporch				= 35;
		params->dsi.horizontal_frontporch				= 36;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		//20140925 tracy modify dsi clock for HW request start
        params->dsi.ssc_disable = 1;
		params->dsi.PLL_CLOCK=231;
		//20140925 tracy modify dsi clock for HW request end
		//params->dsi.pll_div1=0;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
		//params->dsi.pll_div2=0;		// div2=0,1,2,3;div1_real=1,2,4,4	
#if (LCM_DSI_CMD_MODE)
		//params->dsi.fbk_div =9;
#else
		//params->dsi.fbk_div =9;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	
#endif

}
//20140901 tracy modify for support pdp/pdp2 lcm start
static void lcm_init(void)
{
		//20140814 tracy modify for 5" LCM end
 #ifdef BUILD_LK
	printf("[LK]------hx8379c----%s------\n",__func__);
 #else
	printk(KERN_EMERG"[KERNEL]------hx8379c----%s------\n",__func__);
 #endif	
  SET_RESET_PIN(1);
  MDELAY(5); 
  SET_RESET_PIN(0);
  MDELAY(5); 
  SET_RESET_PIN(1);
  MDELAY(25); 

 
 #if BUILD_LK
	 MDELAY(10);
 //20141126 tracy modify for support main/second source lcm detection start
 #if 0    
   array[0]=0x00043902;
	 array[1]=0x8983FFB9;// page enable
	 dsi_set_cmdq(&array, 2, 1);
	 MDELAY(10);
   
	 array[0] = 0x00023700;// return byte number
	 dsi_set_cmdq(&array, 1, 1);
	 MDELAY(10);
   
	 read_reg_v2(0xDB, buffer, 2);	//0xF4
	 id1= buffer[0]; //0
 #endif
   mt_set_gpio_mode(LCM_Detect_Pin, GPIO_MODE_GPIO);
   mt_set_gpio_dir(LCM_Detect_Pin, GPIO_DIR_IN);
   LCM_Source=mt_get_gpio_in(LCM_Detect_Pin);
   printf("%s, LK hx8389b debug: id1 = %d\n", __func__, LCM_Source);

  if(LCM_Source==0)
    {    
	     printf("%s, LK hx8389b_truly\n");
	     dsi_set_cmdq_V3(lcm_initialization_setting2,sizeof(lcm_initialization_setting2)/sizeof(lcm_initialization_setting2[0]),1);
    }
  else
	{
	   printf("%s, LK hx8389b_TCL\n"); 
       dsi_set_cmdq_V3(lcm_initialization_setting1,sizeof(lcm_initialization_setting1)/sizeof(lcm_initialization_setting1[0]),1);
       LCM_Source=1;
	}
 #else
     mt_set_gpio_mode(LCM_Detect_Pin, GPIO_MODE_GPIO);
	 mt_set_gpio_dir(LCM_Detect_Pin, GPIO_DIR_IN);
	 LCM_Source=mt_get_gpio_in(LCM_Detect_Pin);
	 if(LCM_Source==0)
	   {
         dsi_set_cmdq_V3(lcm_initialization_setting2,sizeof(lcm_initialization_setting2)/sizeof(lcm_initialization_setting2[0]),1);
       }
	 else
	   {
         dsi_set_cmdq_V3(lcm_initialization_setting1,sizeof(lcm_initialization_setting1)/sizeof(lcm_initialization_setting1[0]),1);
       }
	
 #endif  
}
//20141126 tracy modify for support main/second source lcm detection end

static void lcm_suspend(void)
{

    //20141110 tracy add for increase close LCM speed start 
	#ifdef BUILD_LK
	  printf("[LK]------hx8379c----%s------\n",__func__);
    #else
	  printk("[KERNEL]------hx8379c----%s------\n",__func__);
    #endif	
	dsi_set_cmdq_V3(lcm_sleep_setting,sizeof(lcm_sleep_setting)/sizeof(lcm_sleep_setting[0]),1);
    //20141110 tracy add for increase close LCM speed end
}


static void lcm_resume(void)
{
	  lcm_init();

    #ifdef BUILD_LK
	  printf("[LK]------hx8379c----%s------\n",__func__);
    #else
	  printk("[KERNEL]------hx8379c----%s------\n",__func__);
    #endif	
}
         
#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);

}
#endif

static unsigned int lcm_compare_id(void)
{
	unsigned int id,id1=0;
	unsigned char buffer[2];
	unsigned int array[16];  
#if 0
	SET_RESET_PIN(0);
	MDELAY(20); 
	SET_RESET_PIN(1);
	MDELAY(20); 


	array[0]=0x00043902;
	array[1]=0x8983FFB9;// page enable
	dsi_set_cmdq(array, 2, 1);
	//MDELAY(10);
    //{0x39,0xBA,7,{0x41,0x93,0x00,0x16,0xA4,0x10,0x18}},	
	array[0]=0x00083902;
	array[1]=0x009341BA;// page enable
	array[2]=0x1810a416;
	dsi_set_cmdq(array, 3, 1);

	array[0] = 0x00043700;// return byte number
	dsi_set_cmdq(array, 1, 1);
	MDELAY(10);

	read_reg_v2(0xF4, buffer, 3);
	id  =  buffer[1]; 
	id1 =  buffer[0];
	#endif
#ifdef BUILD_LK
	printf("%s, id = 0x%08x id1=%x \n", __func__, id,id1);
#else
	printk("%s, id = 0x%08x  id1=%x \n",__func__, id,id1);
#endif

	//return (LCM_ID_HX8389B == id)?1:0;
       return 1;
}



static unsigned int lcm_esd_check(void)
{
	unsigned int ret=FALSE;
//20140915  tracy modify for LCM esd start
  #ifndef BUILD_LK
	char  buffer[6];
	int   array[4];


	if(lcm_esd_test)
	{
		lcm_esd_test = FALSE;
		return TRUE;
	}

	array[0] = 0x00083700;
	dsi_set_cmdq(array, 1, 1);
    //20141007 tracy modify  for open esd function start
	read_reg_v2(0x0A, buffer, 5);
	printk(KERN_EMERG" esd buffer0 =%x,buffer1 =%x, esd buffer2 =%x,buffer3 =%x,buffer4=%x\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4]);
	//read_reg_v2(0x09,buffer,2);
	//printk(KERN_EMERG" esd buffer4=%x \n",buffer[0]);

	if(buffer[0]==0x1C)
	{
		ret=FALSE;
	}
	else
	{			 
		ret=TRUE;
	}
	 //20141007 tracy modify  for open esd function end
#endif
 return ret;
//20140915  tracy modify for LCM esd end
}

static unsigned int lcm_esd_recover(void)
{
	lcm_init();
	
	#ifndef BUILD_LK
	printk("lcm_esd_recover  hx8379c \n");
	#endif
	return TRUE;
}

LCM_DRIVER hx8379c_dsi_vdo_lcm_drv = 
{
    .name			= "hx8379c_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
	.esd_check = lcm_esd_check,
	.esd_recover = lcm_esd_recover,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };
