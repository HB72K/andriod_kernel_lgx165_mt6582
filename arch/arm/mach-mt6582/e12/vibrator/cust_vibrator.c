#include <cust_vibrator.h>
#include <linux/types.h>

static struct vibrator_hw cust_vibrator_hw = {
	.vib_timer = 50,
  #ifdef CUST_VIBR_LIMIT
	.vib_limit = 9,
  #endif
  #ifdef CUST_VIBR_VOL
//<2014/12/30-samhuang, porting E1-2 driver change 2.8V(0x5) to 3.0V(0.6)
	.vib_vol = 0x6,//2.8V for vibr
//>2014/12/30-samhuang
  #endif
};

struct vibrator_hw *get_cust_vibrator_hw(void)
{
    return &cust_vibrator_hw;
}

