#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/string.h>
//#include <linux/export.h>

static char *version = "version 1.0";
int virt_bus_param = 1;
EXPORT_SYMBOL(virt_bus_param);
  
//用于判断指定的驱动程序是否你呢个处理指定的设备
static int virt_match(struct device *dev,struct device_driver *driver)
{
        int ret;
        ret = strncmp(dev->init_name,driver->name,strlen(driver->name));
        return !ret;
}

static void  virt_bus_release(struct device *dev)
{
        printk("bus release...\n");
//        return 0;
}
    
static ssize_t show_bus_version(struct bus_type *bus,char *buf)
{
        int ret;
//        ret = sprintf_s(buf,PAGE_SIZE,"%s\n",version);
        ret = sprintf(buf,"%s\n",version);
        return ret;
}

static BUS_ATTR(version1,S_IRUGO,show_bus_version,NULL);

struct device virt_bus = {//定义总线设备
        .init_name = "virt_bus0",
        .release = virt_bus_release,
};
EXPORT_SYMBOL(virt_bus);

struct bus_type virt_bus_type = {//定义总线类型
        .name = "virt_bus",
        .match = virt_match,
};
EXPORT_SYMBOL(virt_bus_type);

static int __init virt_bus_init(void)
{
        int ret;
        printk("virt bus init...\n");
        
        ret = bus_register(&virt_bus_type);//注册总线
        if(ret)
                printk("bus_register failed!\n");

        ret = bus_create_file(&virt_bus_type,&bus_attr_version1);//创建总线属性
        if(ret)
                printk("Create bus failed !\n");

        /* ret = device_register(&virt_bus);//注册总线设备 */
        /* if(ret) */
        /*         printk("device_register failed!\n"); */

        return ret;
}

static void __exit virt_bus_exit(void)
{
    bus_remove_file(&virt_bus_type, &bus_attr_version1);
        bus_unregister(&virt_bus_type);//删除总线属性
        /* device_unregister(&virt_bus);//删除总线设备 */
        printk("virt bus exit...\n");
}

module_init (virt_bus_init);
module_exit(virt_bus_exit);
MODULE_AUTHOR("wxc");
MODULE_LICENSE("GPL");
