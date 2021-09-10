#include "xil_cache.h"


#define IRQ_FIQ_MASK 0xC0U	/* Mask IRQ and FIQ interrupts in cpsr */

#define XREG_CP15_CACHE_SIZE_SEL		"cp15:2:c0:c0:0"
#define XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC	"cp15:0:c7:c14:1"


//////////////////////////////////////////////////
#ifdef __GNUC__
static inline void Xil_L2CacheSync(void)
#else
static void Xil_L2CacheSync(void)
#endif
{
#ifdef CONFIG_PL310_ERRATA_753970
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_DUMMY_CACHE_SYNC_OFFSET, 0x0U);
#else
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_SYNC_OFFSET, 0x0U);
#endif
}
#endif

/////////////////////////////////////////////////////////
#ifdef __GNUC__
static inline void Xil_L2WriteDebugCtrl(u32 Value)
#else
static void Xil_L2WriteDebugCtrl(u32 Value)
#endif
{
#if defined(CONFIG_PL310_ERRATA_588369) || defined(CONFIG_PL310_ERRATA_727915)
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_DEBUG_CTRL_OFFSET, Value);
#else
	(void)(Value);
#endif
}

/////////////////////////////////////////////
void Xil_L2CacheFlushLine(u32 adr)
{
#ifdef CONFIG_PL310_ERRATA_588369
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_CLEAN_PA_OFFSET, adr);
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INVLD_PA_OFFSET, adr);
#else
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INV_CLN_PA_OFFSET, adr);
#endif
	/* synchronize the processor */
	dsb();
}

//////////////////////////////////////////////
void Xil_DCacheInvalidateRange(unsigned int adr, u32 len)
{
	const u32 cacheline = 32U;
	u32 end;
	u32 tempadr = adr;
	u32 tempend;
	u32 currmask;
	
	volatile u32 *L2CCOffset = (volatile u32 *)(XPS_L2CC_BASEADDR +
				    XPS_L2CC_CACHE_INVLD_PA_OFFSET);
				    
	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	if (len != 0U) {
		end = tempadr + len;
		tempend = end;
		/* Select L1 Data cache in CSSR */
		mtcp(XREG_CP15_CACHE_SIZE_SEL, 0U);

		if ((tempadr & (cacheline-1U)) != 0U) {
			tempadr &= (~(cacheline - 1U));

			Xil_L1DCacheFlushLine(tempadr);
#ifndef USE_AMP
			/* Disable Write-back and line fills */
			Xil_L2WriteDebugCtrl(0x3U);
			Xil_L2CacheFlushLine(tempadr);
			/* Enable Write-back and line fills */
			Xil_L2WriteDebugCtrl(0x0U);
			Xil_L2CacheSync();
#endif
			tempadr += cacheline;
		}
		if ((tempend & (cacheline-1U)) != 0U) {
			tempend &= (~(cacheline - 1U));

			Xil_L1DCacheFlushLine(tempend);
#ifndef USE_AMP
			/* Disable Write-back and line fills */
			Xil_L2WriteDebugCtrl(0x3U);
			Xil_L2CacheFlushLine(tempend);
			/* Enable Write-back and line fills */
			Xil_L2WriteDebugCtrl(0x0U);
			Xil_L2CacheSync();
#endif
		}

		while (tempadr < tempend) {
#ifndef USE_AMP
			/* Invalidate L2 cache line */
			*L2CCOffset = tempadr;
			Xil_L2CacheSync();
#endif

	/* Invalidate L1 Data cache line */
#if defined (__GNUC__) || defined (__ICCARM__)
			asm_cp15_inval_dc_line_mva_poc(tempadr);
#else
			{ volatile register u32 Reg
				__asm(XREG_CP15_INVAL_DC_LINE_MVA_POC);
			  Reg = tempadr; }
#endif
			tempadr += cacheline;
		}
	}

	dsb();
	mtcpsr(currmask);
	
}

void Xil_L1DCacheFlushLine(u32 adr)
{
	mtcp(XREG_CP15_CACHE_SIZE_SEL, 0U);
	mtcp(XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC, (adr & (~0x1FU)));

	/* Wait for L1 flush to complete */
	dsb();
}

