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
    .name = "test_kset_kobj",
    .mode = S_IRWXUGO,
};

struct attribute my_attrs1 =      {
    .name = "test_kset_kobj0",
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

//kobject atrr
struct attribute kobj_attrs =      {
    .name = "test_kobj0",
    .mode = S_IRWXUGO,
};

struct attribute *kobj_def[] = {
    &kobj_attrs, 
    NULL,
};
struct kobj_type my_kobj_ktype = {
    /* .release = my_obj_release, */
    .sysfs_ops = &my_sysfs_ops,
    .default_attrs = kobj_def,
};
struct kobject my_kobj ;


static int kset_filter(struct kset *kset,struct kobject *kobj)
{
    printk("%s: Filter: kobj %s.\n",__func__, kobj->name);
    return 1;
}

static const char *kset_name(struct kset *kset,struct kobject *kobj)
{
    static char buf[20];
    printk("%s: Name kobj %s.\n",__func__,kobj->name);
    sprintf(buf,"%s","kset_name");
    return buf;
}

static int kset_uevent(struct kset *kset,struct kobject *kobj, struct kobj_uevent_env *env)

{
    int i = 0;
    printk("%s: uevent kobj %s.\n",__func__, kobj->name);

    while(i < env->envp_idx)
    {
        printk("%s: %s.\n",__func__, env->envp[i]);
        i ++;
    }

    return 0;
}

static struct kset_uevent_ops uevent_ops =   {
    .filter = kset_filter,
    .name = kset_name,
    .uevent = kset_uevent,
};

struct kset *kset_p;
struct kset kset_c;

static int __init kset_test_init(void)
{
    int ret = 0;

    printk("kset test init!\n");

    /* 动态创建并注册 kset_p */
    kset_p = kset_create_and_add("kset_p",&uevent_ops,NULL);

    /* 静态注册，添加 kset_c 到 kset_p */
    /* kset_init(&kset_c); */
    kobject_set_name(&kset_c.kobj,"kset_c");
    kset_c.kobj.kset = kset_p;

    /* 对于较新版本的内核，在注册 kset 之前，需要  
     * 填充 kset.kobj 的 ktype 成员，否则注册不会成功 */
    kset_c.kobj.ktype = &my_ktype;
    ret = kset_register(&kset_c);

    if(ret){
        kset_unregister(kset_p);
        return -1;
    }
        

    my_kobj.kset = kset_p;
    if(kobject_init_and_add(&my_kobj, &my_kobj_ktype, NULL, "test"))
        printk("add my_kobj error.\n");
    
    printk("%s OK.\n", __func__);
    
    return 0;
}

static void __exit kset_test_exit(void)
{
    printk("kset test exit!\n");
    kobject_del(&my_kobj);
    printk("%s: unregister kset_p.\n", __func__);
    kset_unregister(kset_p);
    printk("%s: unregister kset_c .\n", __func__);
    kset_unregister(&kset_c);

}

module_init(kset_test_init);
module_exit(kset_test_exit);

MODULE_AUTHOR("wxc");
MODULE_LICENSE("Dual BSD/GPL");
