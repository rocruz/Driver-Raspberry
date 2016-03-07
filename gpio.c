/* Crea un dispositivo para leer/escribir de gpio */

#include <linux/init.h>
#include <linux/kernel.h> /* printk() */
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/stat.h> 
#include <linux/slab.h> /* kmalloc() */
#include <linux/proc_fs.h>
#include <linux/mman.h> /* mmap() */
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/ioport.h> /* check_mem_region() */
#include <asm/uaccess.h> /* copy_from/to_user */


MODULE_LICENSE("Dual BSD/GPL");

#define GPIO_BASE 0x20200000
#define GPI_NUM   7
/* Declaration of gpio functions */

int gpio_open(struct inode *inode, struct file *filp);
int gpio_release(struct inode *inode, struct file *filp);
ssize_t gpio_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t gpio_write(struct file *filp, char *buf, size_t count, loff_t *f_pos);
void gpio_exit(void);
int gpio_init(void);

/* Structure that declares the usual file */
/* access functions */
struct file_operations gpio_fops = {
  .read= gpio_read,
  .write= gpio_write,
  .open= gpio_open,
  .release= gpio_release
};

/* Declaration of the init and exit functions */
module_init(gpio_init);
module_exit(gpio_exit);

/* Global variables of the driver */
/* Major number */
int gpio_major = 60;

/* I/O access */
volatile unsigned long gpio_addr;
volatile unsigned *gpio_ptr;
unsigned long gpio_size=2;

int mem_fd;
void *gpio_map;

int gpio_init() {
  int result;
  /* register char_major */
  result = register_chrdev(gpio_major, "gpio", &gpio_fops);
  if (result < 0) {
    printk(KERN_WARNING "Registering char device failed with %d\n", result);
    return result;
  }

  gpio_addr = (volatile unsigned long)GPIO_BASE;
  gpio_ptr = (volatile unsigned *)gpio_addr;
// int check_mem_region(unsigned long start, unsigned long len);
// int request_mem_region(unsigned long start, unsigned long len, char *name);
// int release_mem_region(unsigned long start, unsigned long len);
  result = check_mem_region(gpio_addr, gpio_size);
  if (result < 0) {
    printk(KERN_WARNING "GPIO memory not available\n");
    unregister_chrdev(gpio_major, "gpio");
    return result;
  }
  result = request_mem_region(gpio_addr, gpio_size, "gpio");
  if (result == NULL) {
    printk(KERN_WARNING "Cannot allocate GPIO memory:%d\n", result);
    unregister_chrdev(gpio_major, "gpio");
    return result;
  } 

  printk(KERN_INFO "Allocating GPIO memory at 0x%X\n", gpio_addr);

  return 0;
}

int gpio_release(struct inode *inode, struct file *filp) {
  return 0;
  printk(KERN_INFO "Releasing gpio module\n");
  unregister_chrdev(gpio_major, "gpio");
  release_mem_region(gpio_addr, gpio_size);
  gpio_ptr = (volatile unsigned *)NULL;
  return 0;
}


void gpio_exit() {
  printk(KERN_INFO "Releasing gpio module\n");
  unregister_chrdev(gpio_major, "gpio");
  release_mem_region(gpio_addr, gpio_size);
  gpio_ptr = (volatile unsigned *)NULL;
}


int gpio_open(struct inode *inode, struct file *filp) {
  return 0;
}

ssize_t gpio_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
  static unsigned int nread=0;
  printk(KERN_INFO "Read from device %d ntimes\n", ++nread);
  return 0;
}

ssize_t gpio_write(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
  return 0;
}

