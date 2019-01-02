#include <types.h>
#include <stdio.h>
#include <wait.h>
#include <sync.h>
#include <proc.h>
#include <sched.h>
#include <dev.h>
#include <vfs.h>
#include <iobuf.h>
#include <inode.h>
#include <unistd.h>
#include <error.h>
#include <assert.h>

#include <arch.h>

#define BLUETOOTH_BUFSIZE               4096

static char bluetooth_buffer[BLUETOOTH_BUFSIZE];
static off_t p_rpos, p_wpos;

void dev_bluetooth_write()
{
	
	while((*READ_IO(BT_UART_BASE + UART_lsr) & 0x00000001) == 0x00000001){
		unsigned int RecievedByte;
		RecievedByte = *READ_IO(BT_UART_BASE + UART_rbr);
		bluetooth_buffer[p_wpos % BLUETOOTH_BUFSIZE] = RecievedByte;
		if (p_wpos - p_rpos < BLUETOOTH_BUFSIZE) {
			p_wpos++;
		}		
		delay();
	}
}

static int dev_bluetooth_read(char *buf, size_t len)
{
	int ret = 0;
    while (1) {
        if (ret >= len)
            break;
        if (p_rpos < p_wpos) {
            char c = bluetooth_buffer[p_rpos % BLUETOOTH_BUFSIZE];
            //FIXME
            //cons_putc(c); commented by xiaohan.
            *buf++ = c;
            p_rpos++;
            ret++;
            if (p_rpos >= p_wpos)
                break;
        } 
        else 
            break;
    }
	return ret;
}

static int bluetooth_open(struct device *dev, uint32_t open_flags)
{
	if (open_flags != O_RDONLY) {
		return -E_INVAL;
	} 
	return 0;
}

static int bluetooth_close(struct device *dev)
{
	return 0;
}

static int bluetooth_io(struct device *dev, struct iobuf *iob, bool write)
{
	if (!write) {
		int ret;
		if ((ret = dev_bluetooth_read(iob->io_base, iob->io_resid)) > 0) {
			iob->io_resid -= ret;
		}
		return ret;
	}
	return -E_INVAL;
}

static int bluetooth_ioctl(struct device *dev, int op, void *data)
{
	return -E_INVAL;
}

static void bluetooth_device_init(struct device *dev)
{
	memset(dev, 0, sizeof(*dev));
	dev->d_blocks = 0;
	dev->d_blocksize = 1;
	dev->d_open = bluetooth_open;
	dev->d_close = bluetooth_close;
	dev->d_io = bluetooth_io;
	dev->d_ioctl = bluetooth_ioctl;

	p_rpos = p_wpos = 0; //read and write pointer
}

void dev_init_bluetooth(void)
{
	struct inode *node;
	if ((node = dev_create_inode()) == NULL) {
		panic("bluetooth: dev_create_node.\n\r");
	}
	bluetooth_device_init(vop_info(node, device));

	int ret;
	if ((ret = vfs_add_dev("bluetooth", node, 0)) != 0) {
		panic("bluetooth: vfs_add_dev: %e.\n\r", ret);
	}
}
