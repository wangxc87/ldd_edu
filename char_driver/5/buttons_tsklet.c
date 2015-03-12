#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <mach/regs-gpio.h>
#include <linux/miscdevice.h>

#define DEVICE_NAME     "buttons"

struct button_irq_desc {
    int irq;
    int pin;
    int pin_setting;
    int number;
    char *name;	
};

static struct button_irq_desc button_irqs [] = {
    {IRQ_EINT8 , S3C2410_GPG(0) ,  S3C2410_GPG0_EINT8  , 0, "KEY0"},
    {IRQ_EINT11, S3C2410_GPG(3) ,  S3C2410_GPG3_EINT11 , 1, "KEY1"},
    {IRQ_EINT13, S3C2410_GPG(5) ,  S3C2410_GPG5_EINT13 , 2, "KEY2"},
    {IRQ_EINT14, S3C2410_GPG(6) ,  S3C2410_GPG6_EINT14 , 3, "KEY3"},
    {IRQ_EINT15, S3C2410_GPG(7) ,  S3C2410_GPG7_EINT15 , 4, "KEY4"},
    {IRQ_EINT19, S3C2410_GPG(11),  S3C2410_GPG11_EINT19, 5, "KEY5"},
};
static volatile char key_values [] = {'0', '0', '0', '0', '0', '0'};

//定义实际的底半部调用函数
void buttons_do_tasklet(unsigned long arg);
DECLARE_TASKLET(buttons_tasklet, buttons_do_tasklet, 0);

static irqreturn_t buttons_interrupt(int irq, void *dev_id)
{
    struct button_irq_desc *button_irqs = (struct button_irq_desc *)dev_id;

    tasklet_schedule(&buttons_tasklet);//调度底半部函数
    
    return IRQ_RETVAL(IRQ_HANDLED);
}

void buttons_do_tasklet(unsigned long arg)
{
    printk("%s enter..\n",__func__);
}

static int s3c24xx_buttons_open(struct inode *inode, struct file *file)
{
    int i;
    int err = 0;
    
    for (i = 0; i < sizeof(button_irqs)/sizeof(button_irqs[0]); i++) {
        if (button_irqs[i].irq < 0) {
            continue;
        }
        err = request_irq(button_irqs[i].irq, buttons_interrupt, IRQF_TRIGGER_FALLING,  //注册中断服务程序
                          button_irqs[i].name, (void *)&button_irqs[i]);
        if (err)
            break;
    }

    if (err) {
        i--;
        for (; i >= 0; i--) {
            if (button_irqs[i].irq < 0) {
                continue;
            }
            /* disable_irq(button_irqs[i].irq); */
            free_irq(button_irqs[i].irq, (void *)&button_irqs[i]); //释放注册的中断
        }
        return -EBUSY;
    }

    
    return 0;
}


static int s3c24xx_buttons_close(struct inode *inode, struct file *file)
{
    int i;
    
    for (i = 0; i < sizeof(button_irqs)/sizeof(button_irqs[0]); i++) {
        if (button_irqs[i].irq < 0) {
            continue;
        }
        free_irq(button_irqs[i].irq, (void *)&button_irqs[i]);
    }

    return 0;
}

static struct file_operations dev_fops = {
    .owner   =   THIS_MODULE,
    .open    =   s3c24xx_buttons_open,
    .release =   s3c24xx_buttons_close, 
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dev_fops,
};

static int __init dev_init(void)
{
	int ret;

	ret = misc_register(&misc);

	printk (DEVICE_NAME"\tinitialized\n");

	return ret;
}

static void __exit dev_exit(void)
{
	misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("FriendlyARM Inc.");
