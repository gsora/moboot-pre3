/*
 * Copyright (c) 2008, Google Inc.
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

#include <reg.h>
#include <dev/fbcon.h>
//#include <dev/klog.h>
#include <kernel/thread.h>
#include <platform/debug.h>
#include <mmu.h>
#include <platform/iomap.h>
#include <platform/irqs.h>
#include <reg.h>
#include "gpio_hw.h"

void platform_init_interrupts(void);
void platform_init_timer();

#define MB (1024*1024)
#define ARRAY_SIZE(a) (sizeof(a)/(sizeof((a)[0])))

static struct msm_gpio mmc_gpio_table[] = {
	{GPIO_CFG(64, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_16MA), "MMC_SD_CLK"},
	{GPIO_CFG(65, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA),"MMC_SD_CMD"},

	{GPIO_CFG(66, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA),"MMC_SD_DATA3"},
	{GPIO_CFG(67, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA),"MMC_SD_DATA2"},
	{GPIO_CFG(68, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA),"MMC_SD_DATA1"},
	{GPIO_CFG(69, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),"MMC_SD_DATA0"},

	{GPIO_CFG(115, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA),"MMC_SD_DATA4"},
	{GPIO_CFG(114, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA),"MMC_SD_DATA5"},
	{GPIO_CFG(113, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_8MA),"MMC_SD_DATA6"},
	{GPIO_CFG(112, 1, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_8MA),"MMC_SD_DATA7"},
};

static uint32_t ticks_per_sec = 0;
/* Initialize DGT timer */
void platform_init_timer(void)
{
	uint32_t val = 0;

	/* Disable timer */
	writel(0, DGT_ENABLE);

	/* Check for the hardware revision */
	val = readl(HW_REVISION_NUMBER);
	val = (val >> 28) & 0x0F;
	if(val >= 1)
		writel(1, DGT_CLK_CTL);

#if _EMMC_BOOT
	ticks_per_sec = 19200000; /* Uses TCXO (19.2 MHz) */
#else
	ticks_per_sec = 6144000; /* Uses LPXO/4 (24.576 MHz / 4) */
#endif
}

/* Returns timer ticks per sec */
uint32_t platform_tick_rate(void)
{
	return ticks_per_sec;
}

void platform_early_init(void)
{
	platform_gpios_enable(mmc_gpio_table, ARRAY_SIZE(mmc_gpio_table));
	platform_init_interrupts();
	platform_init_timer();
}

void platform_init(void)
{

}

void platform_uninit(void)
{
	printf("platform_uninit\n");
	mask_interrupt(INT_USB_OTG_HS);
	mask_interrupt(INT_DEBUG_TIMER_EXP);
	platform_uninit_timer();
}

void display_init(void)
{
	struct fbcon_config *fb_cfg;

	mddi_pmdh_clock_init();
	fb_cfg = mddi_init();
	fbcon_setup(fb_cfg);
	platform_init_display();
	gfxconsole_start_on_display();
}
