#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>

#define DEVICE_NAME "smart_sensor"

static dev_t dev_number;
static struct cdev sensor_cdev;
static struct class *sensor_class;

static char sensor_data[256];

static ssize_t sensor_read(struct file *file,
                           char __user *buffer,
                           size_t length,
                           loff_t *offset)
{
    int temperature = 20 + (jiffies % 13);
    int humidity = 40 + (jiffies % 41);
    int occupied = jiffies % 2;
    int aqi = 20 + (jiffies % 101);

    const char *occupancy_status;
    const char *aqi_status;

    if (occupied)
        occupancy_status = "ACTIVE";
    else
        occupancy_status = "INACTIVE";

    if (aqi <= 50)
        aqi_status = "GOOD";
    else if (aqi <= 100)
        aqi_status = "MODERATE";
    else
        aqi_status = "POOR";

    scnprintf(sensor_data, sizeof(sensor_data),
              "Smart Sensor Driver\n"
              "Occupancy: %s\n"
              "Temperature: %d C\n"
              "Humidity: %d percent\n"
              "AQI: %d (%s)\n",
              occupancy_status,
              temperature,
              humidity,
              aqi,
              aqi_status);

    int data_length = strlen(sensor_data);

    if (*offset >= data_length)
        return 0;

    if (copy_to_user(buffer, sensor_data, data_length))
        return -EFAULT;

    *offset = data_length;

    return data_length;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = sensor_read,
};

static int __init sensor_driver_init(void)
{
    int result;

    printk(KERN_INFO "Smart Sensor Driver: Initializing\n");

    result = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);

    if (result < 0) {
        printk(KERN_ALERT "Failed to allocate device number\n");
        return result;
    }

    cdev_init(&sensor_cdev, &fops);

    result = cdev_add(&sensor_cdev, dev_number, 1);

    if (result < 0) {
        unregister_chrdev_region(dev_number, 1);
        return result;
    }

    sensor_class = class_create(DEVICE_NAME);

    if (IS_ERR(sensor_class)) {
        cdev_del(&sensor_cdev);
        unregister_chrdev_region(dev_number, 1);
        return PTR_ERR(sensor_class);
    }

    device_create(sensor_class, NULL, dev_number, NULL, DEVICE_NAME);

    printk(KERN_INFO "Smart Sensor Driver: Loaded successfully\n");
    printk(KERN_INFO "Device: /dev/%s\n", DEVICE_NAME);

    return 0;
}

static void __exit sensor_driver_exit(void)
{
    device_destroy(sensor_class, dev_number);
    class_destroy(sensor_class);

    cdev_del(&sensor_cdev);
    unregister_chrdev_region(dev_number, 1);

    printk(KERN_INFO "Smart Sensor Driver: Unloaded\n");
}

module_init(sensor_driver_init);
module_exit(sensor_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bikash");
MODULE_DESCRIPTION("Smart Home Sensor Character Device Driver");
MODULE_VERSION("1.0");
