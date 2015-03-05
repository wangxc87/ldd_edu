#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <linux/device.h>
#include <asm/ioctl.h>

#include "key_char.h"

/* #define GPBCON_BASE_ADDR (0x56000010) */
#define GPGCON_BASE_ADDR (0x56000060)

//定义gpio寄存器结构体
struct gpio_reg {
    int gpncon;
    int gpndat;
    int gpnup;
    int reserved;
};

//定义key操作对象结构体
struct key_obj {
    u32 key_state[KEY_NUM];
    int key_num_max;
    struct cdev *cdev;
    struct gpio_reg *reg;
};

//定义一全局变量
static struct key_obj key_dev;


//
static int key_get(struct key_obj *key, int key_num, int *state)
{
    int value;
    switch (key_num){
    case KEY1_OFFSET:
    case KEY2_OFFSET:
    case KEY3_OFFSET:
    case KEY4_OFFSET:
    case KEY5_OFFSET:
    case KEY6_OFFSET:
        value = key->reg->gpndat;
        if(value & BIT(key_num))//读取按键按下状态，按下为低
            *state = KEY_NOT_PRESSED;
        else
            *state = KEY_PRESSED;
        break;
    default:
        printk("Invalid key_num..\n");
        return -EINVAL;
    }
    return 0;        
        
}

//打开函数
static int key_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "[KERN] key open..\n");
    filp->private_data = &key_dev;
    nonseekable_open(inode, filp);//no llseek funtions    
    return 0;
}

//关闭函数
static int key_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "[KERN] key realse...\n");
    filp->private_data = NULL;

    return 0;
}

//io访问函数            
static int key_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct key_ctrl key_value;

    switch (cmd) {
    case KEY_GET_STATE_IOCTL_CMD:
        if(copy_from_user(&key_value, (void __user *)arg, sizeof(struct key_ctrl))){
            printk("copy_from_user Error.\n");
            return -EINVAL;
        }

        key_get(&key_dev, key_value.key_num, &key_value.key_state);
        if(copy_to_user((void __user *)arg, &key_value, sizeof(struct key_ctrl))){
            printk("copy_to_user Error.\n");
            return -EINVAL;
        }
        break;
    default:
        return -EINVAL;
    }
    return 0;
}

        //设备操作结构体
static struct file_operations key_ops = {
    .owner = THIS_MODULE,
    .open = key_open,
    .release = key_release,
    .ioctl = key_ioctl,
};

//定义一全局设备号
dev_t dev_no;

#define CREATE_DEV_INODE_AUTO
#ifdef CREATE_DEV_INODE_AUTO
static struct class *classp;
static struct device *devicep;
#endif


//key驱动初始化函数
static int __init key_init(void)
{
    int ret;
    struct cdev *cdev;

    key_dev.reg = ioremap(GPGCON_BASE_ADDR, sizeof(struct gpio_reg));
    if(!key_dev.reg){
        printk("ioremap faikey.\n");
        return -ENOMEM;
    }
        
    ret = alloc_chrdev_region(&dev_no, 0, 1, "key");
    if(ret < 0){
        printk(KERN_ERR "alloc devno faikey..\n");
        iounmap(key_dev.reg);
        return -1;
    }

    cdev = cdev_alloc();
    if( cdev == NULL){
        printk("cdev_alloc faikey.\n");
        unregister_chrdev_region(dev_no, 1);
        goto err_exit;
    }
    cdev->owner = THIS_MODULE;
    cdev->ops = &key_ops;

    ret = cdev_add(cdev, dev_no, 1);
    if(ret < 0) {
        printk(KERN_ERR "cdev add faikey..\n");
        cdev_del(cdev);
        goto err_exit;
    }

#ifdef CREATE_DEV_INODE_AUTO
    classp =  class_create(THIS_MODULE, "key");
    if(classp == NULL){
        printk("Create key class faikey.\n");
        cdev_del(cdev);
        goto err_exit;
    }

    devicep = device_create(classp, NULL, dev_no, NULL, "key");
    if(devicep == NULL){
        printk("Create key device faikey.\n");
        class_destroy(classp);
        cdev_del(cdev);
        goto err_exit;
    }
#endif

    key_dev.key_num_max = KEY_NUM;
    key_dev.cdev = cdev;

    //配置gpio为输入
    key_dev.reg->gpncon &= ~BIT(KEY1_OFFSET*2) & ~BIT(KEY2_OFFSET*2 ) & ~BIT(KEY3_OFFSET*2) & ~BIT(KEY4_OFFSET*2);
    key_dev.reg->gpncon &= ~BIT(KEY1_OFFSET*2 + 1) & ~BIT(KEY2_OFFSET*2 + 1) & ~BIT(KEY3_OFFSET*2 + 1) & ~BIT(KEY4_OFFSET*2 + 1);

    printk(KERN_INFO "key init OK, dev_no-%d..\n", MAJOR(dev_no));
    return 0;

 err_exit:
    iounmap(key_dev.reg);
    unregister_chrdev_region(dev_no, 1);
    return -1;
}

//模块退出函数
static void  __exit key_exit(void)
{
    iounmap(key_dev.reg);

#ifdef  CREATE_DEV_INODE_AUTO
    device_destroy(classp, dev_no);
    class_destroy(classp);
#endif

    cdev_del(key_dev.cdev);
    unregister_chrdev_region(dev_no, 1);
}

module_init(key_init);
module_exit(key_exit);

MODULE_AUTHOR("WANGXC");
MODULE_LICENSE("GPL");
