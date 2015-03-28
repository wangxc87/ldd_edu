#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/string.h> 

extern struct device virt_bus;
extern struct bus_type virt_bus_type;

//char description = "

void  virt_device_release(struct device *dev)
{
        printk("virt device release...\n");
}

struct device virt_dev = {//创建设备
        .init_name = "virt_dev",
        /* .bus = &virt_bus_type,//定义总线类型 */
        /* .parent = &virt_bus,//定义virt_dev的父设备 */
        .release = virt_device_release,
};

static ssize_t virt_dev_show(struct device *dev,struct device_attribute *attr, char *buf)
{
        int ret;
        ret = sprintf(buf,"%s: %s\n",attr->attr.name, "This is virt device!\n");
        return  ret;
}
static  DEVICE_ATTR(description,S_IRUGO,virt_dev_show,NULL);

static int __init virt_device_init(void)
{
        int ret;
        ret = device_register(&virt_dev);//注册设备
        
        if(ret){
                printk("virt device register failed...\n");
                return ret;            
        }

        ret = device_create_file(&virt_dev,&dev_attr_description);{//创建设备文件
            printk("virt device create_file failed..\n");
            return ret;
        }
        printk("%s Ok.\n", __func__);
        return ret;
}

static void __exit virt_device_exit(void)
{
    device_remove_file(&virt_dev, &dev_attr_description);
    device_unregister(&virt_dev);//卸载设备
        
    printk("virt device exit...\n");
}
module_init(virt_device_init);
module_exit(virt_device_exit);
MODULE_AUTHOR("wxc");
MODULE_LICENSE("GPL");
