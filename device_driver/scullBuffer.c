#include "scullBuffer.h"

#define SIZE 512

/* Parameters which can be set at load time */
int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_size = SCULL_SIZE;	/* number of scull Buffer items */

module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_size, int, S_IRUGO);

MODULE_AUTHOR("Piyush");
MODULE_LICENSE("Dual BSD/GPL");

struct scull_buffer scullBufferDevice;	/* instance of the scullBuffer structure */

/* file ops struct indicating which method is called for which io operation */
struct file_operations scullBuffer_fops = {
	.owner =    THIS_MODULE,
	.read =     scullBuffer_read,
	.write =    scullBuffer_write,
	.open =     scullBuffer_open,
	.release =  scullBuffer_release,
};

/*
 * Method used to allocate resources and set things up when the module
 * is being loaded. 
*/
int scull_init_module(void)
{
	int result = 0;
	dev_t dev = 0;
	
	scull_size = scull_size * SIZE;

	/* first check if someone has passed a major number */
	if (scull_major) {
		dev = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev, SCULL_NR_DEVS, "scullBuffer");
	} else {
		/* we need a dynamically allocated major number..*/
		result = alloc_chrdev_region(&dev, scull_minor, SCULL_NR_DEVS,
				"scullBuffer");
		scull_major = MAJOR(dev);
	}
	if (result < 0) {
		printk(KERN_WARNING "scullBuffer: can't get major %d\n", scull_major);
		return result;
	}

	/* alloc space for the buffer (scull_size bytes) */
	scullBufferDevice.bufferPtr = kmalloc( scull_size , GFP_KERNEL);
	if(scullBufferDevice.bufferPtr == NULL)
	{
		scull_cleanup_module();
		return -ENOMEM;
	}
	
	/* Init the count vars */
	scullBufferDevice.readerCnt = 0;
	scullBufferDevice.writerCnt = 0;
	scullBufferDevice.size = 0;

	/* Initialize the semaphore*/
	sema_init(&scullBufferDevice.sem, 1);
	sema_init(&scullBufferDevice.blocksFull, 0);
	sema_init(&scullBufferDevice.blocksEmpty, scull_size/SIZE);
	
	scullBufferDevice.rPtr = scullBufferDevice.wPtr = (char*)scullBufferDevice.bufferPtr;
	scullBufferDevice.endPtr = scullBufferDevice.bufferPtr + scull_size;

	/* Finally, set up the c dev. Now we can start accepting calls! */
	scull_setup_cdev(&scullBufferDevice);
	printk(KERN_DEBUG "scullBuffer: Done with init module ready for requests buffer size= %d\n",scull_size);
	return result; 
}

/*
 * Set up the char_dev structure for this device.
 * inputs: dev: Pointer to the device struct which holds the cdev
 */
static void scull_setup_cdev(struct scull_buffer *dev)
{
	int err, devno = MKDEV(scull_major, scull_minor);
    
	cdev_init(&dev->cdev, &scullBuffer_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scullBuffer_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scullBuffer\n", err);
}

/*
 * Method used to cleanup the resources when the module is being unloaded
 * or when there was an initialization error
 */
void scull_cleanup_module(void)
{
	dev_t devno = MKDEV(scull_major, scull_minor);

	/* if buffer was allocated, get rid of it */
	if(scullBufferDevice.bufferPtr != NULL) {
		kfree(scullBufferDevice.bufferPtr);
		scullBufferDevice.bufferPtr = NULL;
	}
		
	/* Get rid of our char dev entries */
	cdev_del(&scullBufferDevice.cdev);

	/* cleanup_module is never called if registering failed */
	unregister_chrdev_region(devno, SCULL_NR_DEVS);
	
	printk(KERN_DEBUG "scullBuffer: Done with cleanup module \n");
}

module_init(scull_init_module);
module_exit(scull_cleanup_module);

/*
 * Implementation of the open system call
*/
int scullBuffer_open(struct inode *inode, struct file *filp)
{
	struct scull_buffer *dev;

	/* get and store the container scull_buffer */
	dev = container_of(inode->i_cdev, struct scull_buffer, cdev);
	filp->private_data = dev;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	if(!dev->bufferPtr) {
		printk(KERN_DEBUG "Buffer is null in function open\n");
		dev->bufferPtr = kmalloc(scull_size, GFP_KERNEL);
                if (!dev->bufferPtr) {
                        up(&dev->sem);
                        return -ENOMEM;
                }
		dev->wPtr = scullBufferDevice.wPtr;
	}
	dev->endPtr = dev->bufferPtr + scull_size;

	if (filp->f_mode & FMODE_READ)
		dev->readerCnt++;
	if (filp->f_mode & FMODE_WRITE)
		dev->writerCnt++;
		
	up(&dev->sem);
	return 0;
}

/* 
 * Implementation of the close system call
*/
int scullBuffer_release(struct inode *inode, struct file *filp)
{
	struct scull_buffer *dev = (struct scull_buffer *)filp->private_data;
	if (down_interruptible(&dev->sem) )
		return -ERESTARTSYS;
		
	if (filp->f_mode & FMODE_READ)
		dev->readerCnt--;
	if (filp->f_mode & FMODE_WRITE)
		dev->writerCnt--;
/*
	if (dev->readerCnt == 0 && dev->writerCnt == 0) {
		printk(KERN_DEBUG "scullBuffer: no more processes so freeing buffer\n");
		kfree(dev->bufferPtr);
		dev->bufferPtr = NULL;
	}
*/
	up(&dev->sem);

	if (dev->readerCnt == 0) {
		up(&dev->blocksEmpty);
	}
	if (dev->writerCnt == 0) {
		up(&dev->blocksFull);
	}
		
	return 0;
}

/*
 * Implementation of the read system call
*/
ssize_t scullBuffer_read(struct file *filp, char __user *buf, size_t count,
	loff_t *f_pos)
{
	struct scull_buffer *dev = (struct scull_buffer *)filp->private_data;
	ssize_t countRead = 0;
	
	/* get exclusive access */
	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	printk(KERN_DEBUG "scullBuffer: read called count= %d\n", count);
	printk(KERN_DEBUG "scullBuffer: cur pos= %lld, size= %d \n", *f_pos, dev->size);

	int status = scullBuffer_getReadSpace(dev, filp);

	if (status < 0) {
		countRead = -1;
		goto out;
	} else if (status == 0) {
		countRead = 0;
		goto out;
	}

/*
	// have we crossed reading the device? 
	if( *f_pos >= dev->size)
		goto out;

	// read till the end of device
	if( (*f_pos + count) > dev->size)
		count = dev->size - *f_pos;
*/
	
	printk(KERN_DEBUG "scullBuffer: reading %d bytes\n", (int)count);
	
	/* copy data to user space buffer */
	if (copy_to_user(buf, dev->rPtr, count)) {
		countRead = -EFAULT;
		up(&dev->sem);
		return countRead;
	}

	dev->rPtr += count;
	*f_pos += count;
	countRead = count;
	if (dev->rPtr >= dev->endPtr){
		dev->rPtr = (char*)dev->bufferPtr;
		*f_pos = 0;	
	}

	printk(KERN_DEBUG "scullBuffer: new pos= %lld\n", *f_pos);
	dev->size -= count;

	/* now we're done release the semaphore */
	out: 
	up(&dev->sem);
	up(&dev->blocksEmpty);
	return countRead;
}

/*
 * Implementation of the write system call
*/
ssize_t scullBuffer_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
	int countWritten = 0;
	struct scull_buffer *dev = (struct scull_buffer *)filp->private_data;
	
	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;	
	
	/* have we crossed the size of the buffer? */
	printk(KERN_DEBUG "scullBuffer: write called count= %d\n", count);
	printk(KERN_DEBUG "scullBuffer: cur pos= %lld, size= %d \n", *f_pos, (int)dev->size);
	
	int status = scullBuffer_getWriteSpace(dev, filp);
	if (status < 0) {
                countWritten = -1;
                goto out;
        } else if (status == 0) {
                countWritten = 0;
                goto out;
        }

#if 0
	//if( *f_pos >= scull_size)
	//	goto out;

	/* write till the end of buffer */
	//if( *f_pos + count > scull_size)
	//	count = scull_size - *f_pos;
#endif
	
	printk(KERN_DEBUG "scullBuffer: writing %d bytes \n", (int)count);
	
	/* write data to the buffer */
	if (copy_from_user(dev->wPtr, buf, count)) {
		countWritten = -EFAULT;
		up(&dev->sem);
		return countWritten;
	}

	// updating the write pointer
	dev->wPtr += count;
	*f_pos += count;
	if (dev->wPtr >= dev->endPtr) {
		dev->wPtr = (char*)dev->bufferPtr;
		*f_pos = 0;
	}

#if 0
	if (copy_from_user(dev->bufferPtr + *f_pos, buf, count)) {
		countWritten = -EFAULT;
		goto out;
	}

	// updating the write pointer
	*f_pos += count;
	if (*f_pos >= dev->endPtr) {
		*f_pos = dev->bufferPtr;
	}
#endif
	countWritten = count;

	/* update the size of the device */
	dev->size += count;

	printk(KERN_DEBUG "scullBuffer: new pos= %lld, new size= %d \n", *f_pos, (int)dev->size);
	
	out:
	up(&dev->sem);
	/* finally, awake any reader */
	up(&dev->blocksFull);

	return countWritten;
}

/* Wait for space for writing; caller must hold device semaphore.  On
 * error the semaphore will be released before returning. */
int scullBuffer_getWriteSpace(struct scull_buffer *dev, struct file *filp)
{
	while(dev->size == scull_size)
	{
                if(dev->readerCnt == 0) {
                        printk("Buffer is full and no waiting reader\n");
                        return 0;
                }
		up(&dev->sem);

		if (down_interruptible(&dev->blocksEmpty))
			return -ERESTARTSYS;

                if (down_interruptible(&dev->sem)) {
                        up(&dev->blocksEmpty);
                        return -ERESTARTSYS;
                }
        }
        return 1;
}

int scullBuffer_getReadSpace(struct scull_buffer *dev, struct file *filp) {
	while(dev->size == 0)
        {
                if(dev->writerCnt == 0) {
                        printk("Buffer is empty and no waiting writer\n");
                        return 0;
                }
                up(&dev->sem);

                printk("Reader: waiting on blocks full\n");

                if (down_interruptible(&dev->blocksFull))
                        return -ERESTARTSYS;

                printk("Reader: waiting on semaphpore\n");

                if (down_interruptible(&dev->sem)) {
                        up(&dev->blocksFull);
                        return -ERESTARTSYS;
                }
        }
	return 1;
}
