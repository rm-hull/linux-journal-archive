/*
 * Tiny I2C Chip Driver
 *
 * Copyright (C) 2003 Greg Kroah-Hartman (greg@kroah.com)
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, version 2 of the License.
 *
 * This driver shows how to create a minimal I2C bus and algorithm driver.
 *
 * Compile this driver with:

	echo "obj-m := tiny_i2c_chip.o" > Makefile
	make -C <path/to/kernel/src> SUBDIRS=$PWD modules
 */

#define DEBUG 1

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/i2c-sensor.h>

/* Addresses to scan */
static unsigned short normal_i2c[] = { I2C_CLIENT_END };
static unsigned short normal_i2c_range[] = { 0x00, 0xff, I2C_CLIENT_END };
static unsigned int normal_isa[] = { I2C_CLIENT_ISA_END };
static unsigned int normal_isa_range[] = { I2C_CLIENT_ISA_END };

/* Insmod parameters */
SENSORS_INSMOD_1(chip);

/* Each client has this additional data */
struct chip_data {
	struct semaphore update_lock;
	char valid;			/* !=0 if following fields are valid */
	unsigned long last_updated;	/* In jiffies */
	u16 temp_input;			/* Register values */
	u16 temp_max;
	u16 temp_hyst;
};

#define REG_TEMP_HYST	0x01
#define REG_TEMP_OS	0x02

static int chip_attach_adapter(struct i2c_adapter *adapter);
static int chip_detect(struct i2c_adapter *adapter, int address, int kind);
static int chip_detach_client(struct i2c_client *client);

/* This is the driver that will be inserted */
static struct i2c_driver chip_driver = {
	.owner		= THIS_MODULE,
	.name		= "tiny_chip",
	.flags		= I2C_DF_NOTIFY,
	.attach_adapter	= chip_attach_adapter,
	.detach_client	= chip_detach_client,
};


static void chip_update_client(struct i2c_client *client)
{
	struct chip_data *data = i2c_get_clientdata(client);

	down(&data->update_lock);
	dev_dbg(&client->dev, "%s\n", __FUNCTION__);
	++data->temp_input;
	++data->temp_max;
	++data->temp_hyst;
	data->last_updated = jiffies;
	data->valid = 1;
	up(&data->update_lock);
}

#define show(value)	\
static ssize_t show_##value(struct device *dev, char *buf)	\
{								\
	struct i2c_client *client = to_i2c_client(dev);		\
	struct chip_data *data = i2c_get_clientdata(client);	\
								\
	chip_update_client(client);				\
	return sprintf(buf, "%d\n", data->value);		\
}
show(temp_max);
show(temp_hyst);
show(temp_input);

#define set(value, reg)	\
static ssize_t set_##value(struct device *dev, const char *buf, size_t count)	\
{								\
	struct i2c_client *client = to_i2c_client(dev);		\
	struct chip_data *data = i2c_get_clientdata(client);	\
	int temp = simple_strtoul(buf, NULL, 10);		\
								\
	down(&data->update_lock);				\
	data->value = temp;					\
	up(&data->update_lock);					\
	return count;						\
}
set(temp_max, REG_TEMP_OS);
set(temp_hyst, REG_TEMP_HYST);

static DEVICE_ATTR(temp_max, S_IWUSR | S_IRUGO, show_temp_max, set_temp_max);
static DEVICE_ATTR(temp_min, S_IWUSR | S_IRUGO, show_temp_hyst, set_temp_hyst);
static DEVICE_ATTR(temp_input, S_IRUGO, show_temp_input, NULL);

static int chip_attach_adapter(struct i2c_adapter *adapter)
{
	return i2c_detect(adapter, &addr_data, chip_detect);
}

/* This function is called by i2c_detect */
static int chip_detect(struct i2c_adapter *adapter, int address, int kind)
{
	struct i2c_client *new_client = NULL;
	struct chip_data *data = NULL;
	int err = 0;

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		goto error;

	new_client = kmalloc(sizeof(*new_client), GFP_KERNEL);
	if (!new_client) {
		err = -ENOMEM;
		goto error;
	}
	data = kmalloc(sizeof(*data), GFP_KERNEL);
	if (!data) {
		err = -ENOMEM;
		goto error;
	}
			
	memset(new_client, 0x00, sizeof(*new_client));
	memset(data, 0x00, sizeof(*data));

	i2c_set_clientdata(new_client, data);
	new_client->addr = address;
	new_client->adapter = adapter;
	new_client->driver = &chip_driver;
	new_client->flags = 0;

	/* Fill in the remaining client fields */
	strncpy(new_client->name, "tiny_chip", I2C_NAME_SIZE);
	data->valid = 0;
	init_MUTEX(&data->update_lock);

	/* Tell the I2C layer a new client has arrived */
	err = i2c_attach_client(new_client);
	if (err)
		goto error;

	/* Register sysfs files */
	device_create_file(&new_client->dev, &dev_attr_temp_max);
	device_create_file(&new_client->dev, &dev_attr_temp_min);
	device_create_file(&new_client->dev, &dev_attr_temp_input);

	return 0;

error:
	kfree(new_client);
	kfree(data);
	return err;
}

static int chip_detach_client(struct i2c_client *client)
{
	struct chip_data *data = i2c_get_clientdata(client);
	int err;

	err = i2c_detach_client(client);
	if (err) {
		dev_err(&client->dev, "Client deregistration failed, client not detached.\n");
		return err;
	}

	kfree(client);
	kfree(data);
	return 0;
}

static int __init tiny_init(void)
{
	return i2c_add_driver(&chip_driver);
}

static void __exit tiny_exit(void)
{
	i2c_del_driver(&chip_driver);
}


MODULE_AUTHOR("Greg Kroah-Hartman <greg@kroah.com>");
MODULE_DESCRIPTION("Tiny i2c chip");
MODULE_LICENSE("GPL");

module_init(tiny_init);
module_exit(tiny_exit);
