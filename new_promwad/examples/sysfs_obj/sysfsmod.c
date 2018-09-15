#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yury Adamov");
MODULE_DESCRIPTION("Sysfs example");
MODULE_VERSION("0.1");

static unsigned int counter = 0;


static ssize_t count_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
    struct timespec ts;
    getnstimeofday(&ts);
    counter++;
    return sprintf(buf,"%d %lu.%lu",counter,ts.tv_sec,ts.tv_nsec);
}

static struct kobj_attribute my_attr = __ATTR(counter, 0444, count_show, NULL);


static struct attribute *my_attrs[] = {
  &my_attr.attr,
  NULL
};

static struct attribute_group attr_group = {
  .name = "sysfs_mod",
  .attrs = my_attrs
};

static struct kobject *sysfs_mod_kobj;

int __init sysfs_mod_init(void){
  int res=0;

  printk(KERN_INFO "Initializing sysfs obj module...\n");
  sysfs_mod_kobj = kobject_create_and_add("sysfs_mod", NULL); 
  if(!sysfs_mod_kobj){
      printk(KERN_ALERT "Sysfs_mod failed to  register in sysfs\n");
      return -ENOMEM;
  }
  
  res = sysfs_create_group(sysfs_mod_kobj, &attr_group);
  if(res){
      printk(KERN_ALERT "Sysfs_mod: failed to create sysfs group\n");
      kobject_put(sysfs_mod_kobj);
      return res;
  }
  
  return res;

}


void __exit sysfs_mod_exit(void){
  printk(KERN_INFO "Sysfs mod : exiting\n");
  kobject_put(sysfs_mod_kobj);
}

module_init(sysfs_mod_init);
module_exit(sysfs_mod_exit);
