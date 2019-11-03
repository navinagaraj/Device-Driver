/**********************************************/
/* Author           : Naveenkumar N           */
/* Date             : 30-10-2019              */
/* Filename         : hlo_world.c             */
/* Description      : hlo_world file          */
/**********************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>		//Allocating memory in the kernel using kmalloc headerfile
#include <linux/uaccess.h>	//file transfer and reciver header file copy-from-user  copy-to-user

MODULE_LICENSE("GPL");


#define BUFFER_LEN 4096





//Function prototype//
static int hello_init(void);
static void hello_exit(void);
int hlo_open(struct inode *, struct file *);
ssize_t hlo_read (struct file *, char __user *, size_t, loff_t *);
ssize_t hlo_write (struct file *, const char __user *, size_t, loff_t *);
int hlo_release (struct inode *, struct file *);

dev_t hlo_dev;//device number with major num and minor num



struct cdev *hlo_cdev;
char *buffer;


struct file_operations hlo_fop={
	.open = hlo_open,
	.read = hlo_read,
	.write = hlo_write,
	.release = hlo_release,	
	.owner = THIS_MODULE,
};

//Open File operations//
int hlo_open (struct inode *hlo_inode, struct file *hlo_file)
{
	printk(KERN_ALERT"OPEN is called\n");
	return 0;
}


//Read File operations//
ssize_t hlo_read (struct file *hlo_file, char __user *hlo_user, size_t hlo_size, loff_t *hlo_loff)
{
	ssize_t test = 0 ;
	unsigned long int ret_size = 0;
	


	printk(KERN_ALERT"READ is called\n");
	printk(KERN_ALERT"init hlo_size =  %d \n",hlo_size);
	printk(KERN_ALERT"init hlo_loff =  %lld \n",*hlo_loff);
	if(BUFFER_LEN >= *hlo_loff)
	{
		if(*hlo_loff + hlo_size <= BUFFER_LEN)
		{
			ret_size = copy_to_user(hlo_user,&buffer[hlo_size - 1], hlo_size);
	
		printk(KERN_ALERT"1. ret_size %lu \n",ret_size);
		}
		else
		{

			 test = ( *hlo_loff+hlo_size - BUFFER_LEN );
			 test = ( ((hlo_size)-(test)) );
			 ret_size = copy_to_user(hlo_user,&buffer[hlo_size - 1],test);
		printk(KERN_ALERT"2. ret_size %lu\n ",ret_size);
		}
	}
	else
	{
		printk(KERN_ALERT"hlo_loff =  %lld \n",*hlo_loff);
		printk(KERN_ALERT"Buffer offset is beyond the limit \n");
		return 0;
	}

	*hlo_loff = ( (*hlo_loff + hlo_size) - ret_size);

	return (hlo_size - ret_size);
}


//Write File operations//
ssize_t hlo_write (struct file *hlo_file, const char __user *hlo_user, size_t hlo_size, loff_t *hlo_loff)
{
	ssize_t test = 0 ;
	unsigned long int ret_size = 0;

	printk(KERN_ALERT"WRITE is called\n");
	if(BUFFER_LEN >= *hlo_loff)
	{
		if(*hlo_loff + hlo_size <= BUFFER_LEN)
		{
			ret_size = copy_from_user(&buffer[hlo_size - 1] ,hlo_user, hlo_size);
		}
		else
		{
			 test = ( *hlo_loff+hlo_size - BUFFER_LEN );
			 test = ( ((hlo_size)-(test)) );
			 ret_size = copy_from_user(&buffer[hlo_size - 1], hlo_user, test);
		}
	}
	else
	{
		printk(KERN_ALERT"Buffer offset is beyond the limit \n");
		return 0;
	}
	return  (hlo_size - ret_size);
}
//( Close or Release ) File operations//
int hlo_release (struct inode *hlo_inode, struct file *hlo_file)
{
	printk(KERN_ALERT"Release is called\n");
	return 0;
}





static int hello_init(void)
{
	int check_state;
	printk(KERN_ALERT"hello_init called\n");

	hlo_dev = MKDEV(50,0);
	check_state = register_chrdev_region(hlo_dev,1,"SOL");
	if(check_state)
	{
		printk(KERN_ALERT"ERROR OCCURED : register_chrdev_region\n");
		return -1;
	}
	else
	{

		printk(KERN_ALERT"register_chrdev_region: register succesfully\n");
	}

	hlo_cdev = cdev_alloc();
	printk(KERN_ALERT"cdev_alloc: dynamic devic structure allocated  succesfully\n");


        hlo_cdev->owner = THIS_MODULE;
	hlo_cdev->ops   = &hlo_fop;
	hlo_cdev->dev   = hlo_dev;
	hlo_cdev->count = 1;
	


	cdev_add(hlo_cdev,hlo_dev,1);

	printk(KERN_ALERT"cdev_add: device added  succesfully\n");

	
	
	buffer = kmalloc(BUFFER_LEN,GFP_KERNEL); //allocating memory to buffer


	return 0;
}

static void hello_exit(void)
{

	kfree(buffer);//would free that block and return the memory to the kernel
	
	cdev_del(hlo_cdev);

	printk(KERN_ALERT"cdev_del:removed hlo_cdev succesfully\n");
	unregister_chrdev_region(hlo_dev,1);
	printk(KERN_ALERT"unregister_chrdev_region :unregister succesfully\n");

	printk(KERN_ALERT"Goodbye,Cruel world\n");

}





module_init(hello_init);
module_exit(hello_exit);
