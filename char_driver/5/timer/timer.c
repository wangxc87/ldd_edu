#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/timer.h>

#define TIMER_INTERVAL_S 1

static struct timer_list second_timer;
static unsigned int gCount = 0;

void second_timer_fun(unsigned long data)
{
    printk("timer timeout-%u.\n", *(unsigned int *)data);
    gCount ++;
#if  1
    mod_timer(&second_timer, TIMER_INTERVAL_S * HZ);
#else
    second_timer.expires = TIMER_INTERVAL_S * HZ;
    add_timer(&second_timer);
#endif
}

static int __init second_init(void)
{
	int ret = 0;

    init_timer(&second_timer);
    second_timer.function = second_timer_fun;
    second_timer.data = (unsigned long )&gCount;
    second_timer.expires = jiffies + TIMER_INTERVAL_S * HZ;
    add_timer(&second_timer);
    

	printk ("%s success.\n", __func__);

	return ret;
}

static void __exit second_exit(void)
{
    del_timer(&second_timer);
	printk ("%s success.\n", __func__);
}

module_init(second_init);
module_exit(second_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("WXC");
