#include <linux/ftrace.h>
#include <linux/kprobes.h>
#include <linux/module.h>
#include <linux/kernel.h>

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

static asmlinkage long (*real_openat)(const struct pt_regs *);
static bool log_openat;
module_param(log_openat, bool, 0644);
MODULE_PARM_DESC(log_openat, "Enable openat hook logging");

static asmlinkage long fake_openat(const struct pt_regs *regs)
{
    if (log_openat)
        pr_info_ratelimited("openat() called\n");

    return real_openat(regs);
}

static struct ftrace_hook hooks[] = {
    {
        .name = "__x64_sys_openat",
        .function = fake_openat,
        .original = &real_openat,
    },
};

static int __init lkm_init(void)
{
    int err;

    err = install_hook(&hooks[0]);
    if (err) {
        printk(KERN_ERR "hook failed\n");
        return err;
    }

    printk(KERN_INFO "hook installed\n");
    return 0;
}

static void __exit lkm_exit(void)
{
    remove_hook(&hooks[0]);
    printk(KERN_INFO "hook removed\n");
}

module_init(lkm_init);
module_exit(lkm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antoine Rouquette");
MODULE_DESCRIPTION("A simple ftrace hook example");