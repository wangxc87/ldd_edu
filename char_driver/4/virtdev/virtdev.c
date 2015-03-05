#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/signal.h>
#include <linux/poll.h>

#define TEST_IOCTL_CMD 0
#define TEST_IOCTL_CMD2 1
dev_t dev_no;

static struct fasync_struct *async_queue;

static int test_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "[KERN] test open..\n");
    return 0;
}


static int test_fasync(int fd, struct file *filp, int mode)
{
    return fasync_helper(fd, filp, mode, &async_queue);
}


static int test_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "[KERN] test release...\n");
    test_fasync(-1, filp, 0);//删除fasync列表中的文件
    return 0;
}

//static int test_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
static long test_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
    case TEST_IOCTL_CMD:
        printk(KERN_INFO "IOCTL: hello  world..\n");
        break;
    case TEST_IOCTL_CMD2:
        kill_fasync(&async_queue, SIGIO, POLL_IN);
        break;
    default:
        return -EINVAL;
    }
    return 0;
}
static struct file_operations test_ops = {
    .owner = THIS_MODULE,
    .open = test_open,
    .release = test_release,
    //    .ioctl = test_ioctl,
    .unlocked_ioctl = test_ioctl,
    .fasync = test_fasync,
};

static   struct cdev cdev;

static int __init test_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev_no, 0, 1, "test");
    if(ret < 0){
        printk(KERN_ERR "alloc devno failed..\n");
        return 0;
    }

    cdev_init(&cdev, &test_ops);
    cdev.owner = THIS_MODULE;
    cdev.ops = &test_ops;

    ret = cdev_add(&cdev, dev_no, 1);
    if(ret < 0) {
        printk(KERN_ERR "cdev add failed..\n");
        unregister_chrdev_region(dev_no, 1);
        return -1;
    }

   printk(KERN_INFO "test init OK, devno-%d..\n", MAJOR(dev_no));
    return 0;
}

static void  __exit test_exit(void)
{
    printk(KERN_INFO "test exit OK..\n");
    
    unregister_chrdev_region(dev_no, 1);
    cdev_del(&cdev);
}

module_init(test_init);
module_exit(test_exit);

MODULE_AUTHOR("WANGXC");
MODULE_LICENSE("GPL");
