#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/completion.h>


#define HELLOWORLD_IOCTL_CMD 0
#define WAKEUP_COMPLETION_CMD 1
dev_t dev_no;
struct completion exit_completion;

static int helloworld_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "[KERN] helloworld open..\n");
    return 0;
}

static int helloworld_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "[KERN] hellowworld realse...\n");
    return 0;
}

static int helloworld_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
    case HELLOWORLD_IOCTL_CMD:
        printk(KERN_INFO "IOCTL: hello  world..\n");
        break;
    case WAKEUP_COMPLETION_CMD:
        complete(&exit_completion);
        break;
    default:
        return -EINVAL;
    }
    return 0;
}

static struct file_operations helloworld_ops = {
    .owner = THIS_MODULE,
    .open = helloworld_open,
    .release = helloworld_release,
    .ioctl = helloworld_ioctl,
};

static   struct cdev cdev;
int thread_fun(void *p)
{
    printk("[kern] %s run...\n", __func__);
    printk("[kern] waiting for complete to exit.\n");
    wait_for_completion(&exit_completion);//等待完成量的唤醒
    printk("[kern] exit_completion wake up And exit current thread...\n");
    return 0;
    
}
static int __init helloworld_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev_no, 0, 1, "helloworld");
    if(ret < 0){
        printk(KERN_ERR "alloc devno failed..\n");
        return 0;
    }

    cdev_init(&cdev, &helloworld_ops);
    cdev.owner = THIS_MODULE;
    cdev.ops = &helloworld_ops;

    ret = cdev_add(&cdev, dev_no, 1);
    if(ret < 0) {
        printk(KERN_ERR "cdev add failed..\n");
        unregister_chrdev_region(dev_no, 1);
        return -1;
    }
    init_completion(&exit_completion);//初始化完成里
   kthread_run(thread_fun, NULL, "thread_fun");//创建一个内核线程
   printk(KERN_INFO "helloworld init OK, devno-%d..\n", MAJOR(dev_no));
    return 0;
}

static void  __exit helloworld_exit(void)
{
    printk(KERN_INFO "helloworld exit OK..\n");
    unregister_chrdev_region(dev_no, 1);
    cdev_del(&cdev);
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_AUTHOR("WANGXC");
MODULE_LICENSE("GPL");
