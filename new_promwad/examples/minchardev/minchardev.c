#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("John Doe");
MODULE_DESCRIPTION("Minimal char device");
MODULE_VERSION("0.1");

struct my_device {
  char data[100];
  struct semaphore sem;
} minchardevice;

struct cdev *mycdev;

int major;
int ret;

dev_t dev_num;

#define DEVICE_NAME "minchardevice"

int minchardev_open(struct inode *inode, struct file *filp){
  if(down_interruptible(&minchardevice.sem) !=0){
    printk(KERN_ALERT "Can not lock device " DEVICE_NAME);
    return -EPERM;
  }

  printk(KERN_INFO "Opened device " DEVICE_NAME);
  return 0;
}

ssize_t minchardev_read(struct file *filp, char *buf, size_t count, loff_t* offset){
  if(count<sizeof(minchardevice.data)){
    ret = copy_to_user(buf,minchardevice.data,count);
  }else{
    ret = copy_to_user(buf,minchardevice.data,sizeof(minchardevice.data));
  }
  return ret;
}

ssize_t minchardev_write(struct file *filp, const char *buf, size_t count,loff_t *offset){
  if(count<sizeof(minchardevice.data)){
    ret = copy_from_user(minchardevice.data,buf,count);
  }
  return ret;
}

int minchardev_close(struct inode *inode,struct file *filp){
  up(&minchardevice.sem);
  printk(KERN_INFO "Closing device: " DEVICE_NAME);
  return 0;
}

struct file_operations fops = {
  .owner = THIS_MODULE,
  .open = minchardev_open,
  .release = minchardev_close,
  .write = minchardev_write,
  .read = minchardev_read
};

int __init minchardev_init(void){
    ret = alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME);
    if(ret<0){
       printk(KERN_ALERT "can not allocate device " DEVICE_NAME);
       return ret; 
    }
    major = MAJOR(dev_num);

    mycdev = cdev_alloc();
    mycdev->ops = &fops;
    mycdev->owner = THIS_MODULE;
    ret = cdev_add(mycdev,dev_num,1);
    if(ret<0){
       printk(KERN_ALERT "Unable to add cdev to kernel: " DEVICE_NAME);
       return ret; 
    }
    sema_init(&minchardevice.sem,1);
    printk(KERN_INFO "Initialized device " DEVICE_NAME "major number is %d",major);
    return 0;
}

void __exit minchardev_exit(void){
  cdev_del(mycdev);
  unregister_chrdev_region(dev_num,1);
  printk(KERN_ALERT "Unregistering " DEVICE_NAME);
}


module_init(minchardev_init);
module_exit(minchardev_exit);


