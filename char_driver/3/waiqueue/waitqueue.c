#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/wait.h>

#define TEST_IOCTL_CMD 0
#define WAKEUP_COMPLETION_CMD 1
dev_t dev_no;

#define DYNAMIC_DEFINE
#ifdef DYNAMIC_DEFINE
static wait_queue_head_t test_wq_h;
#else
DECLARE_WAIT_QUEUE_HEAD(test_wq_h); //静态定义一个等待队列头
#endif

static int condition = 0;

static int test_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "[KERN] test open..\n");
    return 0;
}

static int test_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "[KERN] hellowworld realse...\n");
    return 0;
}

//static int test_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
static long test_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
    case TEST_IOCTL_CMD:
        printk(KERN_INFO "IOCTL: hello  world..\n");
        break;
    case WAKEUP_COMPLETION_CMD:
        condition = 1;
        wake_up(&test_wq_h);
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
};

static   struct cdev cdev;
struct task_struct *kthreadp = NULL;

int thread_fun(void *p)
{
    printk("[kern] %s run...\n", __func__);
    printk("[kern] waiting to exit.\n");
    wait_event_interruptible(test_wq_h, (condition==1)); //将当前线程加入等待队列，等待condition=1时醒来
    printk("[kern]  wake up And exit current thread...\n");
    kthreadp = NULL;
    return 0;
    
}

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

#ifdef DYNAMIC_DEFINE
    init_waitqueue_head(&test_wq_h); //动态定义初始化
#endif

   kthreadp = kthread_run(thread_fun, NULL, "thread_fun");//创建一个内核线程
   if(!kthreadp)
       printk("[kern] thread_fun create failed.\n");
   else
       printk("[kern] thread_fun create success.\n");

   printk(KERN_INFO "test init OK, devno-%d..\n", MAJOR(dev_no));
    return 0;
}

static void  __exit test_exit(void)
{
    printk(KERN_INFO "test exit OK..\n");
    
    if(kthreadp){
        condition = 1;
        kthread_stop(kthreadp);
    }
        
    
    unregister_chrdev_region(dev_no, 1);
    cdev_del(&cdev);
}

module_init(test_init);
module_exit(test_exit);

MODULE_AUTHOR("WANGXC");
MODULE_LICENSE("GPL");
