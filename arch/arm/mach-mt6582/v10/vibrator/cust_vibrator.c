#include <cust_vibrator.h>
#include <linux/types.h>

//<2015/2/9-davidda, [V10][Feature][Common][BSP][]need vibrator more stronger.
static struct vibrator_hw cust_vibrator_hw = {
	.vib_timer = 50,
  #ifdef CUST_VIBR_LIMIT
	.vib_limit = 9,
  #endif
  #ifdef CUST_VIBR_VOL
	.vib_vol = 0x07 //0x6,//david modify to 3V //2.8V for vibr
  #endif
//>2015/2/9-davidda
};

struct vibrator_hw *get_cust_vibrator_hw(void)
{
    return &cust_vibrator_hw;
}

