/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
 * Copyright (c) 2012 xndcn
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
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <debug.h>
#include "smem.h"
#include <platform/iomap.h>
#include <platform/irqs.h>

extern unsigned int mmc_boot_main(unsigned char slot, unsigned int base);
static unsigned mmc_sdc_base[] = { MSM_SDC1_BASE, MSM_SDC2_BASE, MSM_SDC3_BASE, MSM_SDC4_BASE};

void target_early_init()
{
	//close irq gpt0, which is used by bootie
	mask_interrupt(INT_GPT0_TIMER_EXP);
}

void target_init()
{
	unsigned base_addr;
	unsigned char slot;	
	struct smem_ram_ptable ram_ptable;
	
	display_init();
	printf("scratch: 0x%x\n", target_get_scratch_address());
	printf("display init done\n");
	slot = 2;
	base_addr = mmc_sdc_base[slot-1];
	mmc_boot_main(slot, base_addr);
	printf("done loading from mmc\n");
}

void reboot_device(unsigned reboot_reason)
{       
    reboot(reboot_reason);
}
