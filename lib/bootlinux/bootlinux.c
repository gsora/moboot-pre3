/*
 * Copyright (c) 2011, James Sullins
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY1;11M72;12M2;12m OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <kernel/thread.h>
#include <debug.h>
#include <lib/bootlinux.h>
#include <lib/uimage.h>
#include <lib/atags.h>

#define RAMDISK_ADDR 0x0A000000

char root_dev_ram[] = "/dev/ram0";
char root_dev_noram[] = "/dev/mmcblk0p14";
char tty_dev_fbcon[] = "tty1";
char tty_dev_nofbcon[] = "ttyS2,115200n8";
char *root_dev;
char *tty_dev;

extern unsigned *passed_atags;

void bootlinux_direct(void *kernel, unsigned machtype, unsigned *tags)
{
	void (*entry)(unsigned,unsigned,unsigned*) = kernel;
	dprintf(INFO, "tags: 0x%08x\n", tags);	
	dprintf(INFO, "entering critical section\n");
	enter_critical_section();

	/*
	 * As per ARM requirements, before jumping to the kernel remember to:
	 *  - go in SVC mode
	 *  - disable the data cache
	 *  - disable the MMU
	 *
	 *  Then, call the kernel.
	 *
	 *  The first parameter of entry() is 0 - by definition - then there's the machine type,
	 *  and then the address containing all the ATAGS.
	 *
	 *  More infos:
	 *  http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html#d0e612
	 */
	platform_uninit();
	arch_disable_cache(UCACHE);
	arch_disable_mmu();
	entry(0, machtype, tags);
}

void bootlinux_atags(void *kernel, unsigned *tags,
		const char *cmdline, unsigned machtype,
		void *ramdisk, unsigned ramdisk_size)
{
	unsigned *ptr = tags;
	int cmdline_len = 0;
	int have_cmdline = 0;

	/*
	 * ATAG_CORE
	 *
	 * Size: 5
	 * Content: nothing, even if per reference, if the ATAG_CORE size is 5, it
	 * 	should at least contain meaningful data, not zeroes.
	 */
	*ptr++ = 5;		// size
	*ptr++ = 0x54410001;	// ATAG_CORE tag
	*ptr++ = 0; 		//
	*ptr++ = 0; 		// -> zeroes
	*ptr++ = 0; 		//

	/*
	 * ATAG_INITRD2
	 *
	 * Size: 4
	 * Content: physical start address of the ramdisk image, and its
	 * 	size in bytes.
	 */
	if (ramdisk_size) {
		*ptr++ = 4; 			// size
		*ptr++ = 0x54420005; 		// ATAG_INITRD2 tag
		*ptr++ = (unsigned)ramdisk; 	// ramdisk address
		*ptr++ = ramdisk_size;		// ramdisk image size
	}

	// Set up ATAG_MEM tags, see function declaration for docs.
	ptr = target_atag_mem(ptr);

	/*
	 * ATAG_REVISION
	 * 
	 * Size: 3
	 * Content: integer representing the board revision.
	 */
	*ptr++ = 3; 		// size
	*ptr++ = 0x54410007;	// ATAG_REVISION tag
	*ptr++ = 0;		// board revision

	// If there's a cmdline, append it using ATAG_CMDLINE
	if (cmdline && cmdline[0]) {
		cmdline_len = strlen(cmdline);
		have_cmdline = 1;
	}
	
	if (cmdline_len > 0) {
		const char *src;
		char *dst;
		unsigned n;

		/*
		 * Black magic has been used here.
		 *
		 * As per doc, n = 2 + ((cmdline_len + 3) / 4)
		 */
		n = (cmdline_len + 4) & (~3);
		*ptr++ = (n / 4) + 2;

		*ptr++ = 0x54410009; // ATAG_CMDLINE tag

		// Copy the cmdline to the next n bytes.
		dst = (char *)ptr;
		if (have_cmdline) {
			src = cmdline;
			while ((*dst++ = *src++));
		}
		ptr += (n / 4);
	}

	/*
	 * ATAG_NONE
	 *
	 * We're done here!
	 *
	 * The ATAGs have been set-up, Linux should boot fine :-)
	 */
	*ptr++ = 0;
	*ptr++ = 0;
	check_atags(tags);
	dprintf(INFO, "calling bootlinux_direct...\n");
	bootlinux_direct(kernel, machtype, (unsigned *)tags);
}

unsigned bootlinux_uimage_mem(void *data, unsigned len, void (*callback)(),
		unsigned flags)
{

	unsigned kernel_addr, ramdisk_addr;
	unsigned kernel_size, ramdisk_size;
	unsigned kernel_load, kernel_ep;

	char cmdline_buff[1024];

	char *cmdline = cmdline_buff;

	char *nduid;

	unsigned rc;

	printf("Checking uImage... ");
	if (uimage_check(data, len)) {
		return 1;
	} else {
		printf("OK\n");
	}

	printf("Parsing uImage... ");
	if (uimage_parse((struct image_header *)data, 
			&kernel_load, &kernel_ep, &kernel_size,
			&kernel_addr, &ramdisk_addr, &ramdisk_size)) {
		return 1;
	} else {
		printf("OK!\n");
		dprintf(INFO, "kernel_load 0x%08x\nkernel_addr 0x%08x\nkernel_size %d\nkernel_ep 0x%08x\n", kernel_load, kernel_addr, kernel_size, kernel_ep);
	}

	if (!kernel_size) {
		printf("No kernel!\n");
		return 1;
	}

	memmove(kernel_load, kernel_addr, kernel_size);

	if (ramdisk_size) {
		memmove(RAMDISK_ADDR, ramdisk_addr, ramdisk_size);
		root_dev = root_dev_ram;
	} else {
		root_dev = root_dev_noram;
	}

	printf("\nkernel @ 0x%08x (%u bytes)\n", kernel_load, kernel_size);

	if (ramdisk_size) {
		printf("ramdisk @ 0x%08x (%u bytes)\n", RAMDISK_ADDR, ramdisk_size);
	}


	if (flags & BOOTLINUX_VERBOSE) {
		sprintf(cmdline 
			, "root=%s rootwait rw logo.nologo console=tty1 %s%s%s%s%s%s"
			, root_dev
			, atags_get_cmdline_arg(passed_atags, "fb")
			, atags_get_cmdline_arg(passed_atags, "nduid")
			, atags_get_cmdline_arg(passed_atags, "klog")
			, atags_get_cmdline_arg(passed_atags, "klog_len")
			, atags_get_cmdline_arg(passed_atags, "boardtype")
			, atags_get_cmdline_arg(passed_atags, "lastboot")
			);
	} else {
		sprintf(cmdline 
			,"root=%s rootwait ro fbcon=disable console=ttyS2,115200n8 %s%s%s%s%s lcd_enable_vsync %s"
			, root_dev
			, atags_get_cmdline_arg(passed_atags, "fb")
			, atags_get_cmdline_arg(passed_atags, "nduid")
			, atags_get_cmdline_arg(passed_atags, "klog")
			, atags_get_cmdline_arg(passed_atags, "klog_len")
			, atags_get_cmdline_arg(passed_atags, "boardtype")
			, atags_get_cmdline_arg(passed_atags, "lastboot")
			);
	}

	printf("cmdline='%s'\n", cmdline);

	printf("\nBooting...\n");

	if (callback) callback();
	

	bootlinux_atags(kernel_ep, (void *)TAGS_ADDR, cmdline, MACH_TYPE,
			RAMDISK_ADDR, ramdisk_size);

}

