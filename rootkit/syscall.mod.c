#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xe4de56b4, "__ubsan_handle_load_invalid_value" },
	{ 0xa29982fb, "unregister_ftrace_function" },
	{ 0x726d9448, "ftrace_set_filter_ip" },
	{ 0xbd03ed67, "__ref_stack_chk_guard" },
	{ 0x1c489eb6, "register_kprobe" },
	{ 0x7a8e92c6, "unregister_kprobe" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0xa29982fb, "register_ftrace_function" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0x1c9578f7, "param_ops_bool" },
	{ 0xd272d446, "__fentry__" },
	{ 0x5a844b26, "__x86_indirect_thunk_rax" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xff7fbdd1, "___ratelimit" },
	{ 0xe8213e80, "_printk" },
	{ 0x984622ae, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xe4de56b4,
	0xa29982fb,
	0x726d9448,
	0xbd03ed67,
	0x1c489eb6,
	0x7a8e92c6,
	0xd272d446,
	0xa29982fb,
	0x90a48d82,
	0x1c9578f7,
	0xd272d446,
	0x5a844b26,
	0xd272d446,
	0xff7fbdd1,
	0xe8213e80,
	0x984622ae,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__ubsan_handle_load_invalid_value\0"
	"unregister_ftrace_function\0"
	"ftrace_set_filter_ip\0"
	"__ref_stack_chk_guard\0"
	"register_kprobe\0"
	"unregister_kprobe\0"
	"__stack_chk_fail\0"
	"register_ftrace_function\0"
	"__ubsan_handle_out_of_bounds\0"
	"param_ops_bool\0"
	"__fentry__\0"
	"__x86_indirect_thunk_rax\0"
	"__x86_return_thunk\0"
	"___ratelimit\0"
	"_printk\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "44F6F0D8422479CE7540540");
