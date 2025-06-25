#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
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

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x2c635209, "module_layout" },
	{ 0xe180ce57, "usb_deregister" },
	{ 0x942410fb, "usb_register_driver" },
	{ 0x2a753652, "usb_deregister_dev" },
	{ 0xdcc00bff, "usb_put_dev" },
	{ 0xec2dcbc3, "_dev_info" },
	{ 0x632ff929, "usb_register_dev" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0x5e23b2f7, "usb_get_dev" },
	{ 0xaf88e69b, "kmem_cache_alloc_trace" },
	{ 0x30a93ed, "kmalloc_caches" },
	{ 0x296695f, "refcount_warn_saturate" },
	{ 0xf9a6f7e4, "usb_find_interface" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xa7b6ca7d, "usb_bulk_msg" },
	{ 0x37a0cba, "kfree" },
	{ 0x2f886acb, "usb_free_urb" },
	{ 0xb20c3797, "usb_submit_urb" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xb5571d08, "usb_alloc_coherent" },
	{ 0x437b7175, "usb_alloc_urb" },
	{ 0x92997ed8, "_printk" },
	{ 0xff5c4137, "__dynamic_dev_dbg" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x54af84da, "usb_free_coherent" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("usb:v1A86p7523d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "DE3852B0FA285D01A829FA5");
