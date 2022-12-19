#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>
#include <linux/blk-mq.h>
#include <linux/hdreg.h>
#include <linux/debugfs.h>

#ifndef SECTOR_SIZE
#define SECTOR_SIZE 512
#endif
#define PAGE_SIZE 4096


static int dev_major = 0;
static struct block_dev *block_device = NULL;


struct block_dev {
	sector_t capacity;
	u8 *data;
	struct blk_mq_tag_set tag_set;
	struct request_queue *queue; 
	struct gendisk *gdisk;
};

static struct block_device_operations blockdev_ops = {
	.owner 		= THIS_MODULE,
	.open  		= blockdev_open,
	.release 	= blockdev_release,
};

static struct blk_mq_ops my_queue_ops = {
	.queue_rq = queue_rq,
};


static int blockdev_open(struct block_device *dev, fmode_t mode)
{
	printk(" Blockdev_open Function called\n");
	return 0;
}


static void blockdev_release(struct gendisk *gdisk, fmode_t mode)
{
	printk("Blockdev_release Function called\n");
}



static int do_request(struct request *rq, unsigned int *nr_bytes)
{
	int ret = 0;
	struct bio_vec bvec;
	struct req_iterator iter;
	struct block_dev *dev 	= rq->q->queuedata;
	loff_t pos 		= blk_rq_pos(rq) << SECTOR_SHIFT;
	loff_t dev_size 	= (loff_t)(dev->capacity << SECTOR_SHIFT);

	rq_for_each_segment(bvec, rq, iter)
	{
		unsigned long b_len = bvec.bv_len;
		void* b_buf = page_address(bvec.bv_page) + bvec.bv_offset;
		if ((pos + b_len) > dev_size)
		{
			b_len = (unsigned long)(dev_size - pos);
		}

		if (rq_data_dir(rq) == WRITE)
		{
			memcpy(dev->data + pos, b_buf, b_len);
		}
		else
		{
			memcpy(b_buf, dev->data + pos, b_len);
		}
		pos += b_len;
		*nr_bytes += b_len;
	}
return ret;
}


static blk_status_t queue_rq(struct blk_mq_hw_ctx *hctx, const struct blk_mq_queue_data* bd)
{
	unsigned int nr_bytes 	= 0;
	blk_status_t status 	= BLK_STS_OK;
	struct request *rq 	= bd->rq;

	blk_mq_start_request(rq);

	if (do_request (rq, &nr_bytes) != 0)
	{
		status = BLK_STS_IOERR;
	}
	if (blk_update_request (rq, status, nr_bytes))
	{
		BUG ();
	}
	__blk_mq_end_request (rq, status);
return status;
}



static int __init myblock_driver_init(void)
{
	dev_major = register_blkdev(dev_major, "testblk");
	block_device = kmalloc(sizeof (struct block_dev), GFP_KERNEL);
	if (block_device == NULL)
	{
		printk("Failed to allocate struct block_dev\n");
		unregister_blkdev(dev_major, "testblk");
		return -ENOMEM;
	}

	struct blk_mq_tag_set *set = kmalloc(sizeof(struct blk_mq_tag_set), GFP_KERNEL);
	if (set == NULL)
	{
		printk("Failed to allocate device set");
		kfree(block_device);
		unregister_blkdev(dev_major, "testblk");
		return -ENOMEM;
	}
	block_device->capacity = (500000 * 512) >> SECTOR_SHIFT; 
	block_device->data = vmalloc(block_device->capacity << SECTOR_SHIFT);
	if (block_device->data == NULL) {
		printk("Failed to allocate device IO buffer\n");
		kfree(block_device);
		unregister_blkdev(dev_major, "testblk");
		return -ENOMEM;
	}
	memset(set, 0, sizeof(*set));
	set->ops 		= &my_queue_ops;
	set->nr_hw_queues 	= 1;
	set->cmd_size 		= 0;
	set->queue_depth 	= 128;
	set->numa_node 		= NUMA_NO_NODE;
	set->flags 		= BLK_MQ_F_SHOULD_MERGE;
	if( blk_mq_alloc_tag_set(set))
	{
		printk("????????????? Set allocation failed ?????????????????????\n");
		return -ENOMEM;
	}
	printk("Initializing queue\n");
	block_device->queue = blk_mq_init_queue (set);
	if (block_device->queue == NULL)
	{
		printk("Failed to allocate the device queue\n");
		vfree(block_device->data);
		kfree(set);
		kfree(block_device);
		unregister_blkdev(dev_major, "testblk");
		return -ENOMEM;
	}
	block_device->queue->queuedata = block_device;
	block_device->gdisk = blk_alloc_disk(1);
	block_device->gdisk->flags 		= GENHD_FL_NO_PART_SCAN;
	block_device->gdisk->major 		= dev_major;
	block_device->gdisk->first_minor 	= 0;
	block_device->gdisk->minors 		= 1;
	block_device->gdisk->fops 		= &blockdev_ops;
	block_device->gdisk->queue 		= block_device->queue;
	block_device->gdisk->private_data 	= block_device;
	strncpy(block_device->gdisk->disk_name, "Test_block\0", sizeof("Test_block\0"));
	printk("Adding the disk %s\n", block_device->gdisk->disk_name);
	set_capacity(block_device->gdisk, block_device->capacity);
	add_disk(block_device->gdisk);
return 0;
}

static void __exit myblock_driver_exit(void)
{
	if (block_device->gdisk)
	{
		del_gendisk(block_device->gdisk);
	}
	vfree(block_device->data);
	kfree(block_device);
	unregister_blkdev(dev_major, "testblk");
}



module_init(myblock_driver_init);
module_exit(myblock_driver_exit);
MODULE_LICENSE("GPL");
