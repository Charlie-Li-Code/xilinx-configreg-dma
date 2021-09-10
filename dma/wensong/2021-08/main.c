
#include <xaxidma.h>
#include <xparameters.h>

#include "lite_xil_hw.h"

int main()
{
	u32 buf[1024];
	u32 len=0;
	u32 status;
	int i;
	for(i=0;i<1024;i++){
		buf[i] = 0;
	}
	InitLiteDMA();
	LITE_XAxiDma_Reset(inst_DMAIP);
	while(!LITE_XAxiDma_ResetIsDone(inst_DMAIP)){
						//WAIT
	}
	xil_printf("DMA Reset success\n");
	LITE_XAxiDma_S2MM_Status(inst_DMAIP);
	LITE_ReadData_Device2DDR(inst_DMAIP,MIMODATA_BUFFER_BASE,1024);
	len = LITE_Device2DDR_RealLength(inst_DMAIP);
	xil_printf("DMA Transfer success %d\n",len);
	xil_printf("%x,%x,%x,%x\n",inst_MIMODATA_BUFFER[0],inst_MIMODATA_BUFFER[1],inst_MIMODATA_BUFFER[2],inst_MIMODATA_BUFFER[3]);
	LITE_XAxiDma_Reset(inst_DMAIP);
	while(!LITE_XAxiDma_ResetIsDone(inst_DMAIP)){
						//WAIT
	}
	xil_printf("DMA Reset success\n");


	return 0;
}
