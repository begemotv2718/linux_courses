/*
 * mini2c.c - Driver for tutorial i2c device
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>

static int tutorial_i2c_probe(struct i2c_client *client,
			      const struct i2c_device_id *id)
{
	int ret;

	ret = i2c_smbus_read_byte_data(client, 0x06);

	if (ret < 0) {
		printk(KERN_ALERT "Smbus read returned bad status\n");
		return ret;
	}
	if (ret != 50) {
		printk(KERN_ALERT "Wrong device signature\n");
		return -EINVAL;
	}

	printk(KERN_INFO "Finishing device probe\n");
	ret = i2c_smbus_read_byte_data(client, 0x03);
	printk(KERN_INFO "Read resistor value %d", ret);
	return 0;

}

static const struct i2c_device_id tutorial_i2c_id[] = {
	{"tutorial-i2c", 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, tutorial_i2c_id);

static const struct of_device_id tutorial_i2c_dt_ids[] = {
	{.compatible = "promwad,tutorial-i2c"},
	{}
};

MODULE_DEVICE_TABLE(of, tutorial_i2c_dt_ids);

static struct i2c_driver tutorial_i2c_driver = {
	.driver = {
		   .name = "tutorial-i2c",
		   .of_match_table = of_match_ptr(tutorial_i2c_dt_ids),
		   },
	.probe = tutorial_i2c_probe,
	.id_table = tutorial_i2c_id,
};

module_i2c_driver(tutorial_i2c_driver);

MODULE_AUTHOR("Yury Adamov <yury.adamov@promwad.com>");
MODULE_DESCRIPTION("Tutorial i2c device driver");
MODULE_LICENSE("GPL v2");
