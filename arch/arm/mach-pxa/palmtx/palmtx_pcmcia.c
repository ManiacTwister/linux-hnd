/*

  Driver for Palm T|X PCMCIA

  Copyright (C) 2007 Marek Vasut <marek.vasut@gmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
  Boston, MA 02110-1301, USA.

*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/irq.h>

#include <asm/mach-types.h>
#include <asm/hardware.h>
#include <../drivers/pcmcia/soc_common.h>
#include <asm/arch/pxa-regs.h>
#include <asm/arch/palmtx-gpio.h>
#include <asm/irq.h>

#define DRV_NAME "palmtx_pcmcia"

/* Debuging macro */
#define PALMTX_PCMCIA_DEBUG

#ifdef PALMTX_PCMCIA_DEBUG
#define palmtx_pcmcia_dbg(format, args...) \
        printk(KERN_INFO DRV_NAME": " format, ## args)
#else
#define palmtx_pcmcia_dbg(format, args...) do {} while (0)
#endif

static struct pcmcia_irqs palmtx_socket_state_irqs[] = {
};

static int palmtx_pcmcia_hw_init (struct soc_pcmcia_socket *skt)
{
        GPSR(GPIO48_nPOE_MD) =	GPIO_bit(GPIO48_nPOE_MD) |
				GPIO_bit(GPIO49_nPWE_MD) |
				GPIO_bit(GPIO50_nPIOR_MD) |
				GPIO_bit(GPIO51_nPIOW_MD) |
				GPIO_bit(GPIO85_nPCE_1_MD) |
				GPIO_bit(GPIO53_nPCE_2_MD) |
				GPIO_bit(GPIO54_pSKTSEL_MD) |
				GPIO_bit(GPIO55_nPREG_MD) |
				GPIO_bit(GPIO56_nPWAIT_MD) |
				GPIO_bit(GPIO57_nIOIS16_MD);

        pxa_gpio_mode(GPIO48_nPOE_MD);
        pxa_gpio_mode(GPIO49_nPWE_MD);
        pxa_gpio_mode(GPIO50_nPIOR_MD);
        pxa_gpio_mode(GPIO51_nPIOW_MD);
        pxa_gpio_mode(GPIO85_nPCE_1_MD);
        pxa_gpio_mode(GPIO53_nPCE_2_MD);
        pxa_gpio_mode(GPIO54_pSKTSEL_MD);
        pxa_gpio_mode(GPIO55_nPREG_MD);
        pxa_gpio_mode(GPIO56_nPWAIT_MD);
        pxa_gpio_mode(GPIO57_nIOIS16_MD);

	skt->irq = IRQ_GPIO(GPIO_NR_PALMTX_PCMCIA_READY);

	palmtx_pcmcia_dbg("%s:%i, Socket:%d\n", __FUNCTION__, __LINE__, skt->nr);
        return soc_pcmcia_request_irqs(skt, palmtx_socket_state_irqs,
				       ARRAY_SIZE(palmtx_socket_state_irqs));
}

static void palmtx_pcmcia_hw_shutdown (struct soc_pcmcia_socket *skt)
{
	palmtx_pcmcia_dbg("%s:%i\n", __FUNCTION__, __LINE__);
}


static void
palmtx_pcmcia_socket_state (struct soc_pcmcia_socket *skt, struct pcmcia_state *state)
{
	state->detect = 1; /* always inserted */
	state->ready  = GET_PALMTX_GPIO(PCMCIA_READY) ? 1 : 0;
	state->bvd1   = 1;
	state->bvd2   = 1;
	state->wrprot = 0;
	state->vs_3v  = 1;
	state->vs_Xv  = 0;
}

static int
palmtx_pcmcia_configure_socket (struct soc_pcmcia_socket *skt, const socket_state_t *state)
{
	palmtx_pcmcia_dbg("%s:%i Reset:%d Vcc:%d\n", __FUNCTION__, __LINE__,
			  (state->flags & SS_RESET) ? 1 : 0, state->Vcc);
	
	SET_PALMTX_GPIO(PCMCIA_POWER1, 1);
	SET_PALMTX_GPIO(PCMCIA_POWER2, 1);
	SET_PALMTX_GPIO(PCMCIA_RESET, (state->flags & SS_RESET) ? 1 : 0);
	
	return 0;
}

static void palmtx_pcmcia_socket_init(struct soc_pcmcia_socket *skt)
{
	palmtx_pcmcia_dbg("%s:%i\n", __FUNCTION__, __LINE__);
}

static void palmtx_pcmcia_socket_suspend (struct soc_pcmcia_socket *skt)
{ 
	palmtx_pcmcia_dbg("%s:%i\n", __FUNCTION__, __LINE__);
}

static struct pcmcia_low_level palmtx_pcmcia_ops = {
	.owner			= THIS_MODULE,

	.first			= 0,
	.nr			= 1,

	.hw_init		= palmtx_pcmcia_hw_init,
	.hw_shutdown		= palmtx_pcmcia_hw_shutdown,

	.socket_state		= palmtx_pcmcia_socket_state,
	.configure_socket	= palmtx_pcmcia_configure_socket,

	.socket_init		= palmtx_pcmcia_socket_init,
	.socket_suspend		= palmtx_pcmcia_socket_suspend,
};


static void palmtx_pcmcia_release (struct device * dev)
{
	palmtx_pcmcia_dbg("%s:%i\n", __FUNCTION__, __LINE__);
}


static struct platform_device palmtx_pcmcia_device = {
	.name           = "pxa2xx-pcmcia",
	.id             = 0,
	.dev            = {
		.platform_data  = &palmtx_pcmcia_ops,
		.release	= palmtx_pcmcia_release
	}
};

static int __init palmtx_pcmcia_init(void)
{
	palmtx_pcmcia_dbg("%s:%i\n", __FUNCTION__, __LINE__);

	if(!machine_is_xscale_palmtx())
	    return -ENODEV;

	return platform_device_register (&palmtx_pcmcia_device);
}

static void __exit palmtx_pcmcia_exit(void)
{
	palmtx_pcmcia_dbg("%s:%i\n", __FUNCTION__, __LINE__);

	platform_device_unregister (&palmtx_pcmcia_device);
}

module_init(palmtx_pcmcia_init);
module_exit(palmtx_pcmcia_exit);

MODULE_AUTHOR ("Marek Vasut <marek.vasut@gmail.com>");
MODULE_DESCRIPTION ("PCMCIA support for Palm T|X");
MODULE_LICENSE ("GPL");
