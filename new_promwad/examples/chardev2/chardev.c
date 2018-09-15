#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DEFAULT_LEN PAGE_SIZE

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("John Doe");
MODULE_DESCRIPTION("Minimal character device");
MODULE_VERSION("0.1");

static struct chardev_device {
	char *data;
	struct mutex lock;
	struct cdev my_cdev;
} my_device;

static int major;

static int chardev_open(struct inode *inode, struct file *filep)
{
//#message typeof( (struct chardev_device *) 0->my_cdev))
//#warn "Test"
	const struct cdev *cdevp = inode->i_cdev;
	struct chardev_device *devp =
	    container_of(cdevp, struct chardev_device, my_cdev);
	filep->private_data = devp;
	return 0;
}

static ssize_t chardev_read(struct file *file, char __user * buf, size_t count,
			    loff_t * ppos)
{
	struct chardev_device *devp =
	    (struct chardev_device *)file->private_data;
	char *mydata = devp->data;
	unsigned long remaining;
	ssize_t sz;
	if (*ppos < 0 || *ppos > DEFAULT_LEN)
		return -EFAULT;
	mutex_lock(&devp->lock);
	sz = min((int)count, (int)(DEFAULT_LEN - *ppos));
	remaining = copy_to_user(buf, mydata + (*ppos), sz);
	mutex_unlock(&devp->lock);
	if (remaining) {
		return -EFAULT;
	}
	*ppos += sz;
	return sz;
}

static ssize_t chardev_write(struct file *file, const char __user * buf,
			     size_t count, loff_t * ppos)
{
	struct chardev_device *devp =
	    (struct chardev_device *)file->private_data;
	char *mydata = devp->data;
	unsigned long remaining;
	ssize_t sz;
	if (*ppos < 0 || *ppos > DEFAULT_LEN)
		return -EFAULT;
	mutex_lock(&devp->lock);
	sz = min((ssize_t) count, (ssize_t) (DEFAULT_LEN - *ppos));
	remaining = copy_from_user(mydata + (*ppos), buf, sz);
	mutex_unlock(&devp->lock);
	if (remaining)
		return -EFAULT;
	return sz;
}

static struct file_operations chardev_ops = {
	.owner = THIS_MODULE,
	.read = chardev_read,
	.write = chardev_write,
	.open = chardev_open,
	.llseek = no_llseek,
};

static int __init chardev_init(void)
{
	dev_t chardev_dev;
	int ret = -ENOMEM;
	ret = alloc_chrdev_region(&chardev_dev, 0, 1, "chardev");
	if (ret)
		goto cleanup_region;

	major = MAJOR(chardev_dev);

	my_device.data = kzalloc(DEFAULT_LEN, GFP_KERNEL);
	if (!my_device.data)
		goto cleanup_region;

	cdev_init(&my_device.my_cdev, &chardev_ops);
	mutex_init(&my_device.lock);
	ret = cdev_add(&my_device.my_cdev, chardev_dev, 1);
	if (ret)
		goto cleanup_add;
	printk(KERN_INFO "Initialized device chardev, major = %d\n", major);

	return 0;
cleanup_add:
	kfree(my_device.data);
	cdev_del(&my_device.my_cdev);
cleanup_region:
	unregister_chrdev_region(chardev_dev, 1);
	return ret;
}

/**
 * Exit function
 */
static void __exit chardev_exit(void)
{
	printk(KERN_DEBUG "Goodbye, exiting\n");
	cdev_del(&my_device.my_cdev);
	kfree(my_device.data);
	unregister_chrdev_region(MKDEV(major, 0), 1);
}

module_init(chardev_init);
module_exit(chardev_exit);
