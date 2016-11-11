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
  #if defined( LCM_DEBUG_ENABLE )
    #undef    LCM_DEBUG_ENABLE
  #endif
  #define   LCM_DEBUG_ENABLE

  #if defined( LCM_PRINT )
    #undef    LCM_PRINT
  #endif
  #if defined( BUILD_LK ) || defined( BUILD_UBOOT )
    #define   LCM_PRINT                 printf
  #else
    #define   LCM_PRINT                 printk
  #endif /** End.. (BUILD_LK) **/

  #if defined( LCM_DEBUG_ENABLE )
    #define   LCM_DBG(srt,arg...)       LCM_PRINT(srt,##arg)
  #else
    #define   LCM_DBG(srt,arg...)       {/*Do Nothing*/}
  #endif
  
#define FRAME_WIDTH  (480) 
#define FRAME_HEIGHT (854)

#define LCM_ID_HX8389B 0x89


#define   LCM_HSYNC_NUM          (31)  
#define   LCM_HBP_NUM               (80)  
#define   LCM_HFP_NUM               (80) 
    
#define   LCM_VSYNC_NUM          (3)  
#define   LCM_VBP_NUM               (10)  
#define   LCM_VFP_NUM               (12)   

#define   LCM_LINE_BYTE             ((FRAME_WIDTH+LCM_HSYNC_NUM+LCM_HBP_NUM+LCM_HFP_NUM)*3)

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
       {0x39,0xB9,3,{0xFF,0x83,0x79}},
       
	{0x39,0xB1,20,{
        0x44,0x18,0x18,0x31,0x31,
        0x90,0xD0,0xEE,0x94,0x80,
        0x38,0x38,0xF8,0x22,0x22,
        0x22,0x00,0x80,0x30,0x00}},   
        
 	{0x39,0xB2,9,{0x82,0xFE,0x0D,0x0A,0x00,0x50,0x11,0x42,0x1D}},
 	//{0x39,0xBA,4,{0x41,0x83,0xA8,0x5D}}, 
 	{0x39,0xB4,10,{0x01,0x78,0x01,0x78,0x01,0x78,0x22,0x7F,0x23,0x7F}},		
 	
	{0x15,0xCC,1,{0x02}},
	
	{0x15,0xD2,1,{0x11}},
	
 	{0x39,0xD3,29,{
        0x00,0x07,0x00,0x3C,0x24,0x0A,0x0A,0x32,0x10,
        0x02,0x00,0x02,0x03,0x70,0x03,0x70,0x00,0x08,
        0x00,0x08,0x37,0x33,0x06,0x06,0x37,0x06,0x06,
        0x37,0x0B,0x00,0x00,0x00,0x0A,0x00,0x11}},	 	
 	
 	{0x39,0xD5,32,{
        0x19,0x19,0x18,0x18,0x1A,0x1A,0x1B,0x1B,
        0x02,0x03,0x00,0x01,0x06,0x07,0x04,0x05,
        0x20,0x21,0x22,0x23,0x18,0x18,0x18,0x18,
        0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}}, 
   
       {0x39,0xD6,32,{
        0x18,0x18,0x19,0x19,0x1B,0x1B,0x1A,0x1A,
        0x03,0x02,0x05,0x04,0x07,0x06,0x01,0x00,
        0x23,0x22,0x21,0x20,0x18,0x18,0x18,0x18,
        0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}}, 	              	     
     	
 	{0x39,0xE0,42,{
        0x00,0x00,0x00,0x0C,0x0C,0x3F,0x21,0x31,0x07,0x0C,
        0x0F,0x19,0x11,0x16,0x18,0x16,0x16,0x08,0x12,0x13,
        0x18,0x00,0x00,0x00,0x0B,0x0D,0x3F,0x20,0x31,0x08,
        0x0D,0x0F,0x19,0x12,0x16,0x19,0x16,0x16,0x08,0x14,
        0x14,0x19}},
 
 	{0x39,0xB6,2,{0x4D,0x4D}},
	
	{0x05,0x11,0,{}},		
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 120, {}},
	{0x05,0x29,0,{}},
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 10, {}},
};


static LCM_setting_table_V3   lcm_sleep_out_setting[] = {
  /* SLPOUT: Exit sleep mode, sleep out */
	{0x05,0x11,0,{}},		
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 120, {}},
  /* DISPON: Set display on, display on */
	{0x05,0x29,0,{}},
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 10, {}},
};

static LCM_setting_table_V3   lcm_sleep_in_setting[] = {
  /* DISPOFF: Set display off, Display off sequence */
    {0x05,0x28,0,{}},
    {REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 10, {}},
  /* SLPIN: Enter sleep mode, Sleep Mode On */
   {0x05,0x10,0,{}},
   {REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 120, {}},
};


/******************************************************************************
** push_table
*******************************************************************************/
static void push_table(LCM_setting_table_V3 *table, unsigned int count, unsigned char force_update )
{
    dsi_set_cmdq_V3( table, count, force_update );
} 

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
    	params->dsi.mode   = SYNC_PULSE_VDO_MODE; 
      #endif
    	
		// DSI
		/* Command mode setting */
		//1 Three lane or Four lane
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Video mode setting		
    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		
    params->dsi.vertical_sync_active       = LCM_VSYNC_NUM;  
    params->dsi.vertical_backporch         = LCM_VBP_NUM;
    params->dsi.vertical_frontporch         = LCM_VFP_NUM;
    params->dsi.vertical_active_line	       = FRAME_HEIGHT; 

    params->dsi.horizontal_sync_active    = LCM_HSYNC_NUM;  /* PCLK */
    params->dsi.horizontal_backporch      = LCM_HBP_NUM;
    params->dsi.horizontal_frontporch      = LCM_HFP_NUM;
    params->dsi.horizontal_active_pixel	= FRAME_WIDTH;
    params->dsi.ssc_range = 4;
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

static void lcm_init(void)
{
       LCM_DBG( "[HX8379C_DSI] %s.\n", __func__ );
       LCM_DBG( "[HX8379C_DSI] %s  ivaan lcm resume problem..\n", __func__ );
        SET_RESET_PIN(1);
	MDELAY(5); 
	SET_RESET_PIN(0);
	MDELAY(5); 
	SET_RESET_PIN(1);
	MDELAY(120); 
	                unsigned int data_array[35]; 

        //------------------B9h----------------// 
                data_array[0] = 0x00043902;                                                   
                data_array[1] = 0x7983FFB9;                                 
                dsi_set_cmdq(&data_array, 2, 1); 

        //------------------BAh----------------// 

                data_array[0] = 0x00053902;                                                   
                data_array[1] = 0xA88341BA; 
                data_array[2] = 0x00000053;   
                dsi_set_cmdq(&data_array,3, 1); 
   

        //------------------B1h----------------// 
                 data_array[0] = 0x00153902;                           
                 data_array[1] = 0x161642B1;   
                 data_array[2] = 0xD0903131; //Adjust gate voltage 
                 data_array[3] = 0x388094EE; 
                 data_array[4] = 0x2222F838; 
                 data_array[5] = 0x30800022; 
                 data_array[6] = 0x00000000; 
                 dsi_set_cmdq(&data_array, 7, 1); 

        //------------------B2h----------------// 
                data_array[0] = 0x000A3902;                                           
                data_array[1] = 0x0DFE80B2; 
                data_array[2] = 0x1150000A;         
                data_array[3] = 0x00001D42;                         
                dsi_set_cmdq(&data_array, 4, 1); 


        
        //------------------B4h----------------// 
                 data_array[0] = 0x000B3902;                           
                 data_array[1] = 0x017801B4;   
                 data_array[2] = 0x22780178; 
                 data_array[3] = 0x007F237F; 
                 dsi_set_cmdq(&data_array, 4, 1); 


        //------------------D2h----------------// 
                 data_array[0] = 0x00023902;                   
                 data_array[1] = 0x000011D2;   
                 dsi_set_cmdq(&data_array,2, 1); 

        //------------------D3h----------------// 
                 data_array[0] = 0x00243902;                   
                 data_array[1] = 0x300700D3;   
                 data_array[2] = 0x0A0A243C; 
                 data_array[3] = 0x00021032; 
                 data_array[4] = 0x03700302; 
                 data_array[5] = 0x00080070; 
                 data_array[6] = 0x06333708; 
                 data_array[7] = 0x06063706; 
                 data_array[8] = 0x00000B37; 
                 data_array[9] = 0x11000A00; 
                 dsi_set_cmdq(&data_array,10, 1); 


        //------------------D5h----------------// 
                 data_array[0] = 0x00213902;                   
                 data_array[1] = 0x181919D5;   
                 data_array[2] = 0x1B1A1A18; 
                 data_array[3] = 0x0003021B; 
                 data_array[4] = 0x04070601; 
                 data_array[5] = 0x22212005; 
                 data_array[6] = 0x18181823; 
                 data_array[7] = 0x18181818; 
                 data_array[8] = 0x18181818; 
                 data_array[9] = 0x00000018; 
                 dsi_set_cmdq(&data_array,10, 1); 


        //------------------D6h----------------// 
                 data_array[0] = 0x00213902;                   
                 data_array[1] = 0x191818D6;   
                 data_array[2] = 0x1A1B1B19; 
                 data_array[3] = 0x0502031A; 
                 data_array[4] = 0x01060704; 
                 data_array[5] = 0x21222300; 
                 data_array[6] = 0x18181820; 
                 data_array[7] = 0x18181818; 
                 data_array[8] = 0x18181818; 
                 data_array[9] = 0x00000018; 
                 dsi_set_cmdq(&data_array,10, 1); 
        

         //------------------E0h----------------// 
                 data_array[0] = 0x002B3902;                   
                 data_array[1] = 0x000000E0;   
                 data_array[2] = 0x233F0D0D;//213F0C0C; 
                 data_array[3] = 0x0E0C0734;//0F0C0731; 
                 data_array[4] = 0x18140E18;//18161119; 
                 data_array[5] = 0x140B1715;//12081616; 
                 data_array[6] = 0x00001C18;//00001813; 
                 data_array[7] = 0x3F0E0C00;//3F0D0B00; 
                 data_array[8] = 0x0D083423;//0D083120; 
                 data_array[9] = 0x140F180E;//1612190F; 
                 data_array[10] = 0x0B171519;//08161619; 
                 data_array[11] = 0x001D1916;//00191414; 
                 dsi_set_cmdq(&data_array,12, 1); 

        //------------------B6h----------------// 
                 data_array[0] = 0x00033902;                   
                 data_array[1] = 0x004D4DB6;     
                 dsi_set_cmdq(&data_array,2, 1); 
        
        //------------------CCh----------------// 
                 data_array[0] = 0x00023902;                   
                 data_array[1] = 0x000002CC;   
                 dsi_set_cmdq(&data_array,2, 1); 
        
        //------------------11h----------------//                 
                data_array[0] = 0x00110500;                           
                dsi_set_cmdq(&data_array, 1, 1); 
                MDELAY(200);         
                  
         //------------------29h----------------//   
                data_array[0] = 0x00290500;                           
                dsi_set_cmdq(&data_array, 1, 1); 
                MDELAY(10); 
        //push_table(lcm_initialization_setting1,sizeof(lcm_initialization_setting1)/sizeof(lcm_initialization_setting1[0]),1);

}

static void lcm_suspend( void )
{

    LCM_DBG( "[HX8379C_DSI] %s.\n", __func__ );
    LCM_DBG( "[HX8379C_DSI] %s  ivaan lcm resume problem..\n", __func__ );
    push_table( lcm_sleep_in_setting, sizeof( lcm_sleep_in_setting ) / sizeof( lcm_sleep_in_setting[0] ), 1 );

    SET_RESET_PIN( 1 );
    MDELAY( 5 );
    SET_RESET_PIN( 0 );
    MDELAY( 5 );
    SET_RESET_PIN( 1 );
    MDELAY( 50 );
}

static void lcm_resume( void )
{
    LCM_DBG( "[HX8379C_DSI] %s.\n", __func__ );
    LCM_DBG( "[HX8379C_DSI] %s  ivaan lcm resume problem..\n", __func__ );
    lcm_init();

   // push_table( lcm_sleep_out_setting, sizeof( lcm_sleep_out_setting ) / sizeof( lcm_sleep_out_setting[0] ), 1 );
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

	LCM_DBG("%s, id = 0x%08x  id1=%x \n",__func__, id,id1);


	//return (LCM_ID_HX8389B == id)?1:0;
       return 1;
}



static unsigned int lcm_esd_check(void)
{
	unsigned int ret=FALSE;
	printk("lcm_esd check\n\r");
  #ifndef BUILD_LK
	char  buffer[6];
	int   array[4];


	if(lcm_esd_test)
	{
		lcm_esd_test = FALSE;
		return TRUE;
	}

	array[0] = 0x00083702;
	dsi_set_cmdq(array, 1, 1);
       //20141007 tracy modify  for open esd function start
       //<2015/02/10-Lever.ni [V10][Feature][Common][BSP][][]add lcm esd check
       #if 0
	read_reg_v2(0x0A, buffer, 5);
	printk(KERN_EMERG" esd buffer0 =%x,buffer1 =%x, esd buffer2 =%x,buffer3 =%x,buffer4=%x\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4]);
       #else
       read_reg_v2(0x09,buffer,2);
       printk("esd buffer[0]=%x, buffer[1]=%x \n",buffer[0], buffer[1]);
       #endif

	if(buffer[0]==0x80 && buffer[1]==0x73)
	{
		ret=FALSE;
	}
        //>2015/02/10-Lever.ni
	else
	{			 
		ret=TRUE;
	}	
#endif
 return ret;
}

static unsigned int lcm_esd_recover(void)
{
	lcm_init();
	
	#ifndef BUILD_LK
	printk("lcm_esd_recover  hx8379c \n");
	#endif
	return TRUE;
}




LCM_DRIVER hx8379c_fwvga_dsi_vdo_lcm_drv = 
{
    .name			= "hx8379c_fwvga_dsi_vdo",
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
