#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <linux/device.h>

//#define BIT(n) (1<<(n))

#define LED1 5
#define LED2 6
#define LED3 7
#define LED4 8
#define LED_NUM 4

#define LED_OFF 0
#define LED_ON  1

#define LED_SET_STATE_IOCTL_CMD  1
#define LED_GET_STATE_IOCTL_CMD   2
#define LED_SET_ON_IOCTL_CMD 3
#define LED_SET_OFF_IOCTL_CMD 4

#define GPBCON_BASE_ADDR (0x56000010)

struct led_ctrl {
    int led_num;
    int led_state;
};

struct gpio_reg {
    int gpncon;
    int gpndat;
    int gpnup;
    int reserved;
};

struct led_obj {
    u32 led_state[LED_NUM];
    int led_num_max;
    struct cdev cdev;
    struct gpio_reg *reg;
    struct device *devicep;
};

static struct led_obj led_dev[LED_NUM];

static int led_set(struct led_obj *led, int led_num, int state)
{
    switch (state){
    case LED_ON:
        led->reg->gpndat &= ~BIT(led_num);//低电平点灯
        led->led_state[led_num - 5] = LED_ON;
        break;
    case LED_OFF:
        led->reg->gpndat |= BIT(led_num);//低电平点灯
        led->led_state[led_num - 5] = LED_OFF;
        break;
    default:
        printk("Invalid led state value.\n");
        return -1;
    }

    return 0;        
        
}
static int led_open(struct inode *inode, struct file *filp)
{
    int minor = MINOR(inode->i_rdev);//or iminor(inode);

    filp->private_data = &led_dev[minor]; //传递设备指针

    printk(KERN_INFO "[KERN] led open, minor [%d]..\n", minor);
    
    return 0;
}

static int led_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "[KERN] led realse...\n");
    filp->private_data = NULL;

    return 0;
}


static int led_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    int led_index;
    struct led_ctrl led_config;
    struct led_obj *led = filp->private_data; //获取设备指针

    switch (cmd) {
    case LED_SET_ON_IOCTL_CMD:
        led_set(led, LED1, LED_ON);
        break;
    case LED_SET_OFF_IOCTL_CMD:
        led_set(led, LED1, LED_OFF);
        break;
    case LED_SET_STATE_IOCTL_CMD:
        if(copy_from_user(&led_config, (void __user *)arg, sizeof(struct led_ctrl))){
            printk("copy_from_user Error.\n");
            return -EINVAL;
        }
        led_set(led, led_config.led_num, led_config.led_state);  
        break;
    case LED_GET_STATE_IOCTL_CMD:
        if(copy_from_user(&led_config, (void __user *)arg, sizeof(struct led_ctrl))){
            printk("copy_from_user Error.\n");
            return -EINVAL;
        }
        led_index = led_config.led_num - 5;
        if(led_index < LED_NUM){
            led_config.led_state = led->led_state[led_index];
            if(copy_to_user((void __user *)arg, &led_config, sizeof(struct led_ctrl))){
                printk("copy_to_user Error.\n");
                return -EINVAL;
            }
        }else {
            printk("Invalid LED num.\n");
            return -EINVAL;
        }
        break;
    default:
        return -EINVAL;
    }
    return 0;
}

static struct file_operations led_ops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_release,
    .ioctl = led_ioctl,
};

dev_t dev_no;

#define CREATE_DEV_INODE_AUTO
#ifdef CREATE_DEV_INODE_AUTO
static struct class *classp;
#endif

static int __init led_init(void)
{
    int ret;
    int i=0, j;
    struct cdev *cdev;
    struct gpio_reg *reg;

    reg = ioremap(GPBCON_BASE_ADDR, sizeof(struct gpio_reg));
    if(!reg){
        printk("ioremap failed.\n");
        return -ENOMEM;
    }
        
#ifdef CREATE_DEV_INODE_AUTO
    classp =  class_create(THIS_MODULE, "led");
    if(classp == NULL){
        printk("Create led class failed.\n");
        goto err_exit;
    }
#endif

    ret = alloc_chrdev_region(&dev_no, 0, LED_NUM, "led"); //申请四个次设备号
    if(ret < 0){
        printk(KERN_ERR "alloc devno failed..\n");
        iounmap(reg);
        return -1;
    }

    for(i = 0; i < LED_NUM; i++){

        cdev = &led_dev[i].cdev;
        cdev_init(cdev, &led_ops);

        cdev->owner = THIS_MODULE;
        cdev->ops = &led_ops;

        ret = cdev_add(cdev, MKDEV(MAJOR(dev_no), i), 1);//添加n个次设备
        if(ret < 0) {
            printk(KERN_ERR "cdev add failed..\n");
            cdev_del(cdev);
            goto err_exit;
        }

#ifdef CREATE_DEV_INODE_AUTO
        led_dev[i].devicep = device_create(classp, NULL, MKDEV(MAJOR(dev_no),i), NULL, "led%d", i);
        if(led_dev[i].devicep == NULL){
            printk("Create led device failed.\n");
            for(j= 0; j < i; j++)
                device_destroy(classp, MKDEV(MAJOR(dev_no),j));

            class_destroy(classp);
            cdev_del(cdev);
            goto err_exit;
        }
#endif

        led_dev[i].reg = reg;
        led_dev[i].led_num_max = LED_NUM;

        //配置gpio为输出
        led_dev[i].reg->gpncon |= BIT(LED1*2) |BIT(LED2*2) | BIT(LED3*2) | BIT(LED4*2);
        led_dev[i].reg->gpncon &= ~BIT(LED1*2 + 1) & ~BIT(LED2*2 + 1) & ~BIT(LED3*2 + 1) & ~BIT(LED4*2 + 1);
    }

    
    printk(KERN_INFO "led init OK, dev_no-%d..\n", MAJOR(dev_no));
    return 0;
 err_exit:
    iounmap(reg);
    unregister_chrdev_region(dev_no, LED_NUM);
    return -1;
}

static void  __exit led_exit(void)
{
    int i = 0;
    struct gpio_reg *reg = led_dev[0].reg;
    iounmap(reg);

    for(i = 0; i < LED_NUM; i++){
        cdev_del(&led_dev[i].cdev);
        led_dev[i].reg = NULL;
    }

#ifdef CREATE_DEV_INODE_AUTO
    for(i = 0; i < LED_NUM; i++){
        device_destroy(classp, MKDEV(MAJOR(dev_no),i));
    }
    class_destroy(classp);
#endif

    unregister_chrdev_region(dev_no, LED_NUM);
        
}

module_init(led_init);
module_exit(led_exit);

MODULE_AUTHOR("WANGXC");
MODULE_LICENSE("GPL");


