============================= main.c =============================
"And one c file to rule them all..."

Here the initialization and clean-up of the module happens.
Setting an environment for the other c files to initialize, by registering a character device and a sysfs class and calling the initialization functions of the other c files.
This c file is also responsible for calling the cleanup function of the other modules in the right order.
This is the c file that ties all other c files together.

static void cleanup(enum stage stg)
    Cleans the module.

	Parameters:
    - stg: A designated enum's member that represents the stage of initialization the module is at. The enum is stage, and it's defined at main.c.


static char *setmode(struct device *dev, umode_t *mode)
    This function sets the permissions for a /dev device that is created by device_create.

    It's necessary for setting the permissions of /dev/fw_log to be as expected.