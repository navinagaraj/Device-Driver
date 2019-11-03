/**********************************************/
/* Author	    : Naveenkumar N           */
/* Date		    : 30-10-2019              */
/* Filename	    : hello_world.c           */
/* Description	    : hello_world source file */
/**********************************************/

#include <linux/init.h>
#include <linux/module.h>


MODULE_LICENSE("GPL");


static int hello_init(void)
{
	printk(KERN_ALERT "Hello, world\n");
	return 0;
}


static void hello_exit(void)
{
	printk(KERN_ALERT "Goodbye, cruel world\n");
}


module_init(hello_init);
module_exit(hello_exit);
