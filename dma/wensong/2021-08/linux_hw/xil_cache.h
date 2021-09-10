#ifndef SRC_PS_XIL_CACHE_H_
#define SRC_PS_XIL_CACHE_H_

#include "datatype.h"
#include "xil_hw.h"

#define XPS_L2CC_BASEADDR		0xF8F02000U


#define XPS_L2CC_CACHE_SYNC_OFFSET		0x0730U		/* Cache Sync */
#define XPS_L2CC_DUMMY_CACHE_SYNC_OFFSET	0x0740U		/* Dummy Register for Cache Sync */
#define XPS_L2CC_CACHE_INVLD_PA_OFFSET		0x0770U		/* Cache Invalid by PA */
#define XPS_L2CC_CACHE_INVLD_WAY_OFFSET		0x077CU		/* Cache Invalid by Way */
#define XPS_L2CC_CACHE_CLEAN_PA_OFFSET		0x07B0U		/* Cache Clean by PA */
#define XPS_L2CC_CACHE_CLEAN_INDX_OFFSET	0x07B8U		/* Cache Clean by Index */
#define XPS_L2CC_CACHE_CLEAN_WAY_OFFSET		0x07BCU		/* Cache Clean by Way */
#define XPS_L2CC_CACHE_INV_CLN_PA_OFFSET	0x07F0U		/* Cache Invalidate and Clean by PA */
#define XPS_L2CC_CACHE_INV_CLN_INDX_OFFSET	0x07F8U		/* Cache Invalidate and Clean by Index */
#define XPS_L2CC_CACHE_INV_CLN_WAY_OFFSET	0x07FCU		/* Cache Invalidate and Clean by Way */



/* pseudo assembler instructions */
#define mfcpsr()	({u32 rval; \
			  __asm__ __volatile__(\
			    "mrs	%0, cpsr\n"\
			    : "=r" (rval)\
			  );\
			  rval;\
			 })

#define mtcpsr(v)	__asm__ __volatile__(\
			  "msr	cpsr,%0\n"\
			  : : "r" (v)\
			)

#define mtcp(rn, v)	__asm__ __volatile__(\
			 "mcr " rn "\n"\
			 : : "r" (v)\
			);
			

/* Instruction Synchronization Barrier */
#define isb() __asm__ __volatile__ ("isb sy")

/* Data Synchronization Barrier */
#define dsb() __asm__ __volatile__("dsb sy")

/* Data Memory Barrier */
#define dmb() __asm__ __volatile__("dmb sy")

						
extern void Xil_DCacheInvalidateRange(unsigned int adr, u32 len);
extern void Xil_L1DCacheFlushLine(u32 adr);




















#endif 

