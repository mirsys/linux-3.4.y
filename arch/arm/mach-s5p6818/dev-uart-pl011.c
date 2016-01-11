#include <mach/serial.h>
#include <mach/s5p6818.h>



#ifndef CFG_UART_CLKGEN_CLOCK_HZ
#define CFG_UART_CLKGEN_CLOCK_HZ                50000000 
#endif

#ifndef CFG_UART0_CLKGEN_CLOCK_HZ
#define CFG_UART0_CLKGEN_CLOCK_HZ CFG_UART_CLKGEN_CLOCK_HZ
#endif

#ifndef CFG_UART1_CLKGEN_CLOCK_HZ
#define CFG_UART1_CLKGEN_CLOCK_HZ CFG_UART_CLKGEN_CLOCK_HZ
#endif

#ifndef CFG_UART2_CLKGEN_CLOCK_HZ
#define CFG_UART2_CLKGEN_CLOCK_HZ CFG_UART_CLKGEN_CLOCK_HZ
#endif

#ifndef CFG_UART3_CLKGEN_CLOCK_HZ
#define CFG_UART3_CLKGEN_CLOCK_HZ CFG_UART_CLKGEN_CLOCK_HZ
#endif

#ifndef CFG_UART4_CLKGEN_CLOCK_HZ
#define CFG_UART4_CLKGEN_CLOCK_HZ CFG_UART_CLKGEN_CLOCK_HZ
#endif

#ifndef CFG_UART5_CLKGEN_CLOCK_HZ
#define CFG_UART5_CLKGEN_CLOCK_HZ CFG_UART_CLKGEN_CLOCK_HZ
#endif


#define	UART_CHARNNEL_INIT(ch)	{ \
	struct clk *clk;									\
	char name[16];										\
		sprintf(name, "nxp-uart.%d", ch);					\
		clk = clk_get(NULL, name);							\
		if (!nxp_soc_peri_reset_status(RESET_ID_UART## ch)) {		\
			NX_TIEOFF_Set(TIEOFF_UART## ch ##_USERSMC , 0);	\
			NX_TIEOFF_Set(TIEOFF_UART## ch ##_SMCTXENB, 0);	\
			NX_TIEOFF_Set(TIEOFF_UART## ch ##_SMCRXENB, 0);	\
			nxp_soc_peri_reset_set(RESET_ID_UART## ch);			\
		}													\
		clk_set_rate(clk, CFG_UART## ch ##_CLKGEN_CLOCK_HZ);		\
		clk_enable(clk);								\
	} while (0)



#define	uart_device_register(ch) 		{	\
			UART_CHARNNEL_INIT(ch); \
			platform_device_register(&uart## ch ##_device); \
		}
	static const unsigned char uart_port[6][5] =
	{
		{ PAD_GPIO_D, 14,PAD_GPIO_D, 18, NX_GPIO_PADFUNC_1},
		{ PAD_GPIO_D, 15,PAD_GPIO_D, 19, NX_GPIO_PADFUNC_1},
		{ PAD_GPIO_D, 16,PAD_GPIO_D, 20, NX_GPIO_PADFUNC_1},
		{ PAD_GPIO_D, 17,PAD_GPIO_D, 21, NX_GPIO_PADFUNC_1},
		{ PAD_GPIO_B, 28,PAD_GPIO_B, 29, NX_GPIO_PADFUNC_3},
		{ PAD_GPIO_B, 30,PAD_GPIO_B, 31, NX_GPIO_PADFUNC_3},
	};
	static void uart_device_init(int hwport)
	{
	
		switch(hwport)
		{
			case 0: UART_CHARNNEL_INIT(0);break;
			case 1: UART_CHARNNEL_INIT(1);break;
			case 2: UART_CHARNNEL_INIT(2);break;
			case 3: UART_CHARNNEL_INIT(3);break;
			case 4: UART_CHARNNEL_INIT(4);break;
			case 5: UART_CHARNNEL_INIT(5);break;
		}		
		NX_GPIO_SetPadFunction (PAD_GET_GROUP(uart_port[hwport][0]), uart_port[hwport][1], uart_port[hwport][4] );	// RX
		NX_GPIO_SetPadFunction (PAD_GET_GROUP(uart_port[hwport][2]), uart_port[hwport][3], uart_port[hwport][4] );	// TX
		NX_GPIO_SetOutputEnable(PAD_GET_GROUP(uart_port[hwport][0]), uart_port[hwport][1], CFALSE);
		NX_GPIO_SetOutputEnable(PAD_GET_GROUP(uart_port[hwport][2]), uart_port[hwport][3], CTRUE);
	}

/*------------------------------------------------------------------------------
 * Serial platform device
 */

#if defined(CONFIG_SERIAL_NXP_UART0)
static void __pl011_uart0_prepare(void)
{
	UART_CHARNNEL_INIT(0);
	NX_GPIO_SetPadFunction (PAD_GET_GROUP(PAD_GPIO_D), 14, NX_GPIO_PADFUNC_1);	// RX
	NX_GPIO_SetPadFunction (PAD_GET_GROUP(PAD_GPIO_D), 18, NX_GPIO_PADFUNC_1);	// TX
	NX_GPIO_SetOutputEnable(PAD_GET_GROUP(PAD_GPIO_D), 14, CFALSE);
	NX_GPIO_SetOutputEnable(PAD_GET_GROUP(PAD_GPIO_D), 18, CTRUE);
}

static void __pl011_uart0_wake_peer(void *uport) { }
static void __pl011_uart0_exit(void) { }

void pl011_uart0_prepare(void)			__attribute__((weak, alias("__pl011_uart0_prepare")));
void pl011_uart0_wake_peer(void *uport)	__attribute__((weak, alias("__pl011_uart0_wake_peer")));
void pl011_uart0_exit(void)				__attribute__((weak, alias("__pl011_uart0_exit")));

static struct amba_pl011_data  pl011_data_ch0 = {
#if defined(CONFIG_SERIAL_NXP_UART0_DMA)
	.dma_filter = pl08x_filter_id,
	.dma_rx_param = (void *) DMA_PERIPHERAL_NAME_UART0_RX,
	.dma_tx_param = (void *) DMA_PERIPHERAL_NAME_UART0_TX,
#endif
	.init = pl011_uart0_prepare,
	.exit = pl011_uart0_exit,
	.wake_peer = pl011_uart0_wake_peer,
};
static AMBA_AHB_DEVICE(uart0, "uart-pl011.0", 0, PHY_BASEADDR_UART0, {IRQ_PHY_UART0}, &pl011_data_ch0);
#endif

#if defined(CONFIG_SERIAL_NXP_UART1)
static void __pl011_uart1_prepare(void)
{
	UART_CHARNNEL_INIT(1);
	NX_GPIO_SetPadFunction (PAD_GET_GROUP(PAD_GPIO_D), 15, NX_GPIO_PADFUNC_1);	// RX
	NX_GPIO_SetPadFunction (PAD_GET_GROUP(PAD_GPIO_D), 19, NX_GPIO_PADFUNC_1);	// TX
	NX_GPIO_SetOutputEnable(PAD_GET_GROUP(PAD_GPIO_D), 15, CFALSE);
	NX_GPIO_SetOutputEnable(PAD_GET_GROUP(PAD_GPIO_D), 19, CTRUE);
}

static void __pl011_uart1_wake_peer(void *uport) { }
static void __pl011_uart1_exit(void) { }

void pl011_uart1_prepare(void)			__attribute__((weak, alias("__pl011_uart1_prepare")));
void pl011_uart1_wake_peer(void *uport)	__attribute__((weak, alias("__pl011_uart1_wake_peer")));
void pl011_uart1_exit(void)				__attribute__((weak, alias("__pl011_uart1_exit")));

static struct amba_pl011_data  pl011_data_ch1 = {
#if defined(CONFIG_SERIAL_NXP_UART1_DMA)
	.dma_filter = pl08x_filter_id,
	.dma_rx_param = (void *) DMA_PERIPHERAL_NAME_UART1_RX,
	.dma_tx_param = (void *) DMA_PERIPHERAL_NAME_UART1_TX,
#endif
	.init = pl011_uart1_prepare,
	.exit = pl011_uart1_exit,
	.wake_peer = pl011_uart1_wake_peer,
};
static AMBA_AHB_DEVICE(uart1, "uart-pl011.1", 0, PHY_BASEADDR_UART1, {IRQ_PHY_UART1}, &pl011_data_ch1);
#endif

#if defined(CONFIG_SERIAL_NXP_UART2)
static void __pl011_uart2_prepare(void)
{
	UART_CHARNNEL_INIT(2);
	NX_GPIO_SetPadFunction (PAD_GET_GROUP(PAD_GPIO_D), 16, NX_GPIO_PADFUNC_1);	// RX
	NX_GPIO_SetPadFunction (PAD_GET_GROUP(PAD_GPIO_D), 20, NX_GPIO_PADFUNC_1);	// TX
	NX_GPIO_SetOutputEnable(PAD_GET_GROUP(PAD_GPIO_D), 16, CFALSE);
	NX_GPIO_SetOutputEnable(PAD_GET_GROUP(PAD_GPIO_D), 20, CTRUE);
}

static void __pl011_uart2_wake_peer(void *uport) { }
static void __pl011_uart2_exit(void) { }

void pl011_uart2_prepare(void)			__attribute__((weak, alias("__pl011_uart2_prepare")));
void pl011_uart2_wake_peer(void *uport)	__attribute__((weak, alias("__pl011_uart2_wake_peer")));
void pl011_uart2_exit(void)				__attribute__((weak, alias("__pl011_uart2_exit")));

static struct amba_pl011_data  pl011_data_ch2 = {
#if defined(CONFIG_SERIAL_NXP_UART2_DMA)
	.dma_filter = pl08x_filter_id,
	.dma_rx_param = (void *) DMA_PERIPHERAL_NAME_UART2_RX,
	.dma_tx_param = (void *) DMA_PERIPHERAL_NAME_UART2_TX,
#endif
	.init = pl011_uart2_prepare,
	.exit = pl011_uart2_exit,
	.wake_peer = pl011_uart2_wake_peer,
};
static AMBA_AHB_DEVICE(uart2, "uart-pl011.2", 0, PHY_BASEADDR_UART2, {IRQ_PHY_UART2}, &pl011_data_ch2);
#endif

#if defined(CONFIG_SERIAL_NXP_UART3)
static void __pl011_uart3_prepare(void)
{
	UART_CHARNNEL_INIT(3);
	NX_GPIO_SetPadFunction (PAD_GET_GROUP(PAD_GPIO_D), 17, NX_GPIO_PADFUNC_1);	// RX
	NX_GPIO_SetPadFunction (PAD_GET_GROUP(PAD_GPIO_D), 21, NX_GPIO_PADFUNC_1);	// TX
	NX_GPIO_SetOutputEnable(PAD_GET_GROUP(PAD_GPIO_D), 17, CFALSE);
	NX_GPIO_SetOutputEnable(PAD_GET_GROUP(PAD_GPIO_D), 21, CTRUE);
}

static void __pl011_uart3_wake_peer(void *uport) { }
static void __pl011_uart3_exit(void) { }

void pl011_uart3_prepare(void)			__attribute__((weak, alias("__pl011_uart3_prepare")));
void pl011_uart3_wake_peer(void *uport)	__attribute__((weak, alias("__pl011_uart3_wake_peer")));
void pl011_uart3_exit(void)				__attribute__((weak, alias("__pl011_uart3_exit")));

static struct amba_pl011_data  pl011_data_ch3 = {
	.init = pl011_uart3_prepare,
	.exit = pl011_uart3_exit,
	.wake_peer = pl011_uart3_wake_peer,
};

static AMBA_AHB_DEVICE(uart3, "uart-pl011.3", 0, PHY_BASEADDR_UART3, {IRQ_PHY_UART3}, &pl011_data_ch3);
#endif

#if defined(CONFIG_SERIAL_NXP_UART4)
static void __pl011_uart4_prepare(void)
{
	UART_CHARNNEL_INIT(4);
	NX_GPIO_SetPadFunction (PAD_GET_GROUP(PAD_GPIO_B), 28, NX_GPIO_PADFUNC_3);	// RX
	NX_GPIO_SetPadFunction (PAD_GET_GROUP(PAD_GPIO_B), 29, NX_GPIO_PADFUNC_3);	// TX
	NX_GPIO_SetOutputEnable(PAD_GET_GROUP(PAD_GPIO_B), 28, CFALSE);
	NX_GPIO_SetOutputEnable(PAD_GET_GROUP(PAD_GPIO_B), 29, CTRUE);
}

static void __pl011_uart4_wake_peer(void *uport) { }
static void __pl011_uart4_exit(void) { }

void pl011_uart4_prepare(void)			__attribute__((weak, alias("__pl011_uart4_prepare")));
void pl011_uart4_wake_peer(void *uport)	__attribute__((weak, alias("__pl011_uart4_wake_peer")));
void pl011_uart4_exit(void)				__attribute__((weak, alias("__pl011_uart4_exit")));

static struct amba_pl011_data  pl011_data_ch4 = {
	.init = pl011_uart4_prepare,
	.exit = pl011_uart4_exit,
	.wake_peer = pl011_uart4_wake_peer,
};

static AMBA_AHB_DEVICE(uart4, "uart-pl011.4", 0, PHY_BASEADDR_UART4, {IRQ_PHY_UART4}, &pl011_data_ch4);
#endif

#if defined(CONFIG_SERIAL_NXP_UART5)
static void __pl011_uart5_prepare(void)
{
	UART_CHARNNEL_INIT(5);
	NX_GPIO_SetPadFunction (PAD_GET_GROUP(PAD_GPIO_B), 30, NX_GPIO_PADFUNC_3);	// RX
	NX_GPIO_SetPadFunction (PAD_GET_GROUP(PAD_GPIO_B), 31, NX_GPIO_PADFUNC_3);	// TX
	NX_GPIO_SetOutputEnable(PAD_GET_GROUP(PAD_GPIO_B), 30, CFALSE);
	NX_GPIO_SetOutputEnable(PAD_GET_GROUP(PAD_GPIO_B), 31, CTRUE);
}

static void __pl011_uart5_wake_peer(void *uport) { }
static void __pl011_uart5_exit(void) { }

void pl011_uart5_prepare(void)			__attribute__((weak, alias("__pl011_uart5_prepare")));
void pl011_uart5_wake_peer(void *uport)	__attribute__((weak, alias("__pl011_uart5_wake_peer")));
void pl011_uart5_exit(void)				__attribute__((weak, alias("__pl011_uart5_exit")));

static struct amba_pl011_data  pl011_data_ch5 = {
	.init = pl011_uart5_prepare,
	.exit = pl011_uart5_exit,
	.wake_peer = pl011_uart5_wake_peer,
};

static AMBA_AHB_DEVICE(uart5, "uart-pl011.5", 0, PHY_BASEADDR_UART5, {IRQ_PHY_UART5}, &pl011_data_ch5);
#endif

