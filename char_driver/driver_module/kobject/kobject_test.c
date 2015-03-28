#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/kernel.h>
     
     
/* void my_obj_release(struct kobject *kobj) */
/* { */
/*     printk("release ok.\n"); */
/* } */
     
ssize_t my_sysfs_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
    printk("my_sysfs_show.\n");
    printk("attrname:%s.\n", attr->name);
    sprintf(buf, "%s", attr->name);
    return strlen(attr->name) + 1;
}
     
ssize_t my_sysfs_store(struct kobject *kobj, struct attribute *attr, const char *buf,
                       size_t count)
{
    printk("my_sysfs_store.\n");
    printk("write:%sn", buf);
     
    return count;
}
     
struct sysfs_ops my_sysfs_ops = {
    .show = my_sysfs_show,
    .store = my_sysfs_store,
};
     
struct attribute my_attrs = {
    .name = "test_kobj",
    .mode = S_IRWXUGO,
};

struct attribute my_attrs1 =      {
    .name = "test_kobj0",
    .mode = S_IRWXUGO,
};

struct attribute *my_attrs_def[] = {
    &my_attrs,
    &my_attrs1, 
    NULL,
};

struct kobj_type my_ktype = {
    /* .release = my_obj_release, */
    .sysfs_ops = &my_sysfs_ops,
    .default_attrs = my_attrs_def,
};
     
struct kobject my_kobj ;
     
int __init kobj_test_init(void)
{
    printk("kobj_test init.\n");

    return kobject_init_and_add(&my_kobj, &my_ktype, NULL, "test");
     
}
     
void __exit kobj_test_exit(void)
{
    printk("kobj_test exit.\n");
}
     
module_init(kobj_test_init);
module_exit(kobj_test_exit);

MODULE_AUTHOR("wxc");
MODULE_LICENSE("Dual BSD/GPL");
