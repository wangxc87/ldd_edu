#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <linux/device.h>
#include <asm/ioctl.h>

#define LED1 5
#define LED2 6
#define LED3 7
#define LED4 8
#define LED_NUM 4

#define LED_OFF 0
#define LED_ON  1

struct led_ctrl {
    int led_num;
    int led_state;
};

//#define IOCTL_CMD_MAKE

#ifdef IOCTL_CMD_MAKE

#define LED_DEV_MAGIC 'w'
#define LED_SET_STATE_IOCTL_CMD  (_IOW(LED_DEV_MAGIC, 0x20,  struct led_ctrl))
#define LED_GET_STATE_IOCTL_CMD  (_IOR(LED_DEV_MAGIC, 0x21, struct led_ctrl))
#define LED_SET_ON_IOCTL_CMD     (_IO(LED_DEV_MAGIC, 0x22))
#define LED_SET_OFF_IOCTL_CMD    (_IO(LED_DEV_MAGIC, 0x23))

#else

#define LED_SET_STATE_IOCTL_CMD   1
#define LED_GET_STATE_IOCTL_CMD   2
#define LED_SET_ON_IOCTL_CMD      3
#define LED_SET_OFF_IOCTL_CMD     4
#endif


#define GPBCON_BASE_ADDR (0x56000010)

//定义gpio寄存器结构体
struct gpio_reg {
    int gpncon;
    int gpndat;
    int gpnup;
    int reserved;
};

//定义led操作对象结构体
struct led_obj {
    u32 led_state[LED_NUM];
    int led_num_max;
    struct cdev *cdev;
    struct gpio_reg *reg;
};

//定义一全局变量
static struct led_obj led_dev;

//设置led状态，设置寄存器
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

//打开函数
static int led_open(struct inode *inode, struct file *filp)
{

    printk(KERN_INFO "[KERN] led open..\n");
    filp->private_data = &led_dev;
    
    //    nonseekable_open(inode, filp);    
    return 0;
}

//关闭函数
static int led_release(struct inode *inode, struct file *filp)
{
   
    printk(KERN_INFO "[KERN] led realse...\n");
    filp->private_data = NULL;

    return 0;
}

//io访问函数            
static int led_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    //    int ret;
    int led_index;
    struct led_ctrl led_config;

    switch (cmd) {
    case LED_SET_ON_IOCTL_CMD:
        led_set(&led_dev, LED1, LED_ON);
        break;
    case LED_SET_OFF_IOCTL_CMD:
        led_set(&led_dev, LED1, LED_OFF);
        break;
    case LED_SET_STATE_IOCTL_CMD:
        if(copy_from_user(&led_config, (void __user *)arg, sizeof(struct led_ctrl))){
            printk("copy_from_user Error.\n");
            return -EINVAL;
        }
        led_set(&led_dev, led_config.led_num, led_config.led_state);  
        break;
    case LED_GET_STATE_IOCTL_CMD:
        if(copy_from_user(&led_config, (void __user *)arg, sizeof(struct led_ctrl))){
            printk("copy_from_user Error.\n");
            return -EINVAL;
        }
        led_index = led_config.led_num - 5;
        if(led_index < LED_NUM){
            led_config.led_state = led_dev.led_state[led_index];
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

        //设备操作结构体
static struct file_operations led_ops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .release = led_release,
    .ioctl = led_ioctl,
    //    .llseek = no_llseek,
};

//定义一全局设备号
dev_t dev_no;

#define CREATE_DEV_INODE_AUTO
#ifdef CREATE_DEV_INODE_AUTO
static struct class *classp;
static struct device *devicep;
#endif


//led驱动初始化函数
static int __init led_init(void)
{
    int ret;
    struct cdev *cdev;

    led_dev.reg = ioremap(GPBCON_BASE_ADDR, sizeof(struct gpio_reg));
    if(!led_dev.reg){
        printk("ioremap failed.\n");
        return -ENOMEM;
    }
        
    ret = alloc_chrdev_region(&dev_no, 0, 1, "led");
    if(ret < 0){
        printk(KERN_ERR "alloc devno failed..\n");
        iounmap(led_dev.reg);
        return -1;
    }

    cdev = cdev_alloc();
    if( cdev == NULL){
        printk("cdev_alloc failed.\n");
        unregister_chrdev_region(dev_no, 1);
        goto err_exit;
    }
    cdev->owner = THIS_MODULE;
    cdev->ops = &led_ops;

    ret = cdev_add(cdev, dev_no, 1);
    if(ret < 0) {
        printk(KERN_ERR "cdev add failed..\n");
        cdev_del(cdev);
        goto err_exit;
    }

#ifdef CREATE_DEV_INODE_AUTO
    classp =  class_create(THIS_MODULE, "led");
    if(classp == NULL){
        printk("Create led class failed.\n");
        cdev_del(cdev);
        goto err_exit;
    }

    devicep = device_create(classp, NULL, dev_no, NULL, "led");
    if(devicep == NULL){
        printk("Create led device failed.\n");
        class_destroy(classp);
        cdev_del(cdev);
        goto err_exit;
    }
#endif

    led_dev.led_num_max = LED_NUM;
    led_dev.cdev = cdev;

    //配置gpio为输出
    led_dev.reg->gpncon |= BIT(LED1*2) |BIT(LED2*2) | BIT(LED3*2) | BIT(LED4*2);
    led_dev.reg->gpncon &= ~BIT(LED1*2 + 1) & ~BIT(LED2*2 + 1) & ~BIT(LED3*2 + 1) & ~BIT(LED4*2 + 1);

    printk(KERN_INFO "led init OK, dev_no-%d..\n", MAJOR(dev_no));
    return 0;

 err_exit:
    iounmap(led_dev.reg);
    unregister_chrdev_region(dev_no, 1);
    return -1;
}

//模块退出函数
static void  __exit led_exit(void)
{
    iounmap(led_dev.reg);

#ifdef  CREATE_DEV_INODE_AUTO
    device_destroy(classp, dev_no);
    class_destroy(classp);
#endif

    cdev_del(led_dev.cdev);
    unregister_chrdev_region(dev_no, 1);
    printk("led module exit.\n");
}

module_init(led_init);
module_exit(led_exit);

MODULE_AUTHOR("WANGXC");
MODULE_LICENSE("GPL");




