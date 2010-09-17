/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef _ANDROID_PMEM_H_
#define _ANDROID_PMEM_H_

#include <stdint.h>

#ifndef __user
#define __user
#endif

struct pmem_region {
 unsigned long offset;
 unsigned long len;
};

#define PMEM_IOCTL_MAGIC 'p'
#define PMEM_GET_PHYS _IOW(PMEM_IOCTL_MAGIC, 1, struct pmem_region *)
#define PMEM_MAP _IOW(PMEM_IOCTL_MAGIC, 2, struct pmem_region *)
#define PMEM_GET_SIZE _IOW(PMEM_IOCTL_MAGIC, 3, struct pmem_region *)
#define PMEM_UNMAP _IOW(PMEM_IOCTL_MAGIC, 4, struct pmem_region *)

#define PMEM_ALLOCATE _IOW(PMEM_IOCTL_MAGIC, 5, unsigned int)

#define PMEM_CONNECT _IOW(PMEM_IOCTL_MAGIC, 6, unsigned int)

#define PMEM_GET_TOTAL_SIZE _IOW(PMEM_IOCTL_MAGIC, 7, struct pmem_region *)

#define HW3D_REVOKE_GPU _IOW(PMEM_IOCTL_MAGIC, 8, unsigned int)
#define HW3D_GRANT_GPU _IOW(PMEM_IOCTL_MAGIC, 9, unsigned int)
#define HW3D_WAIT_IRQ _IOW(PMEM_IOCTL_MAGIC,10, unsigned int)

struct android_pmem_platform_data;
struct pmem_file_operations {
 int (*mmap) (struct file *, struct vm_area_struct *);
 int (*open) (struct inode *, struct file *);
 ssize_t (*read) (struct file *, char __user *, size_t, long long *);
 int (*release) (struct inode *, struct file *);
 long (*ioctl) (struct file *, unsigned int, unsigned long);
};

#endif

