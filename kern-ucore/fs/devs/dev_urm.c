/*
 * =====================================================================================
 *
 *       Filename:  dev_urm.c
 *
 *    Description:  ultrasonic ranging module
 *         Author:  larry (hualong), 709603507@qq.com
 *   Organization:  HUST
 *
 * =====================================================================================
 */

#include <types.h>
#include <dev.h>
#include <vfs.h>
#include <iobuf.h>
#include <inode.h>
#include <error.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <arch.h>

/* For open() */
static int urm_open(struct device *dev, uint32_t open_flags)
{
	return 0;
}

/* For close() */
static int urm_close(struct device *dev)
{
	return 0;
}

/* For dop_io() */
static int urm_io(struct device *dev, struct iobuf *iob, bool write)
{
	int ret;
	if(!write){
		ret = *READ_IO(GPIO_CORE_0);
		ret  = ret >> 16;
		return ret;
	}
	return -E_INVAL;
}

/* For ioctl() */
static int urm_ioctl(struct device *dev, int op, void *data)
{
	return -E_INVAL;
}

static void urm_device_init(struct device *dev)
{
	memset(dev, 0, sizeof(*dev));
	dev->d_blocks = 0;
	dev->d_blocksize = 1;
	dev->d_open = urm_open;
	dev->d_close = urm_close;
	dev->d_io = urm_io;
	dev->d_ioctl = urm_ioctl;
}

/*
 * Function to create and attach urm:
 */
void dev_init_urm(void)
{
	struct inode *node;
	if ((node = dev_create_inode()) == NULL) {
		panic("urm: dev_create_node.\n");
	}
	urm_device_init(vop_info(node, device));

	int ret;
	if ((ret = vfs_add_dev("urm", node, 0)) != 0) {
		panic("urm: vfs_add_dev: %e.\n", ret);
	}
}
