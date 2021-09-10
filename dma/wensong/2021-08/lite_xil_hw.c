#include "lite_xil_hw.h"


unsigned int* inst_DMAIP = DMAIP_BASEADDR;


void InitLiteDMA(){

	LITE_XAxiDma_MM2S_IntrDisable(inst_DMAIP,LITE_XAXIDMA_IRQ_ALL_MASK);
	LITE_XAxiDma_S2MM_IntrDisable(inst_DMAIP,LITE_XAXIDMA_IRQ_ALL_MASK);
	LITE_XAxiDma_S2MM_Status(inst_DMAIP);
}



void LITE_XAxiDma_S2MM_Status(unsigned int*  InstancePtr){
	unsigned int status = LITE_XAxiDma_ReadReg(InstancePtr, LITE_S2MM_DMASR);
	xil_printf("Stream to memory-mapped status (0x%08x):",status);
	if (status & 0x00000001)
		xil_printf("Halted");
	else
		xil_printf("Running ");
	if (status & 0x00000002)	xil_printf("Idle");
	if (status & 0x00000008) 	xil_printf("SGIncld");
	if (status & 0x00000010)	xil_printf("DMAIntErr");
	if (status & 0x00000020)	xil_printf("DMASlvErr");
	if (status & 0x00000040)	xil_printf("DMADecErr");
	if (status & 0x00000100)	xil_printf("SGIntErr");
	if (status & 0x00000200)	xil_printf("SGSlvErr");
	if (status & 0x00000400)	xil_printf("SGDecErr");
	if (status & 0x00001000)	xil_printf("IOC_Irq");
	if (status & 0x00002000)	xil_printf("Dly_Irq");
	if (status & 0x00004000)	xil_printf("Err_Irq");
	xil_printf("\n");
}



unsigned int LITE_XAxiDma_MM2S__Busy(unsigned int*  InstancePtr)
{
	return ((LITE_XAxiDma_ReadReg(InstancePtr,LITE_MM2S_DMASR) &
			LITE_XAXIDMA_IDLE_MASK) ? 0 : 1);
}
unsigned int LITE_XAxiDma_S2MM__Busy(unsigned int*  InstancePtr)
{
	return ((LITE_XAxiDma_ReadReg(InstancePtr,LITE_S2MM_DMASR) &
			LITE_XAXIDMA_IDLE_MASK) ? 0 : 1);
}
void LITE_XAxiDma_MM2S_IntrDisable(unsigned int* InstancePtr, unsigned int Mask)	{
	LITE_XAxiDma_WriteReg(InstancePtr,LITE_MM2S_DMACR, \
			LITE_XAxiDma_ReadReg(InstancePtr,LITE_MM2S_DMACR)\
			& ~(Mask & LITE_XAXIDMA_IRQ_ALL_MASK));
}

void LITE_XAxiDma_S2MM_IntrDisable(unsigned int* InstancePtr, unsigned int Mask)	{
	LITE_XAxiDma_WriteReg(InstancePtr,LITE_S2MM_DMACR, \
			LITE_XAxiDma_ReadReg(InstancePtr,LITE_S2MM_DMACR)\
			& ~(Mask & LITE_XAXIDMA_IRQ_ALL_MASK));
}


void LITE_XAxiDma_Reset(unsigned int* InstancePtr)
{
	//Resetting DMA
	LITE_XAxiDma_WriteReg(InstancePtr, LITE_S2MM_DMACR, 4);
	LITE_XAxiDma_WriteReg(InstancePtr, LITE_MM2S_DMACR, 4);
	//Halting DMA
	LITE_XAxiDma_WriteReg(InstancePtr, LITE_S2MM_DMACR, 0);
	LITE_XAxiDma_WriteReg(InstancePtr, LITE_MM2S_DMACR, 0);
}

int LITE_XAxiDma_ResetIsDone(unsigned int*  InstancePtr)
{
	unsigned int RegisterValue;

	/* Check transmit channel
	 */
	RegisterValue = LITE_XAxiDma_ReadReg(InstancePtr,
			LITE_MM2S_DMACR);
	/* Reset is done when the reset bit is low
	 */
	if(RegisterValue & LITE_XAXIDMA_CR_RESET_MASK) {

		return 0;
	}

	/* Check receive channel
	 */
	RegisterValue = LITE_XAxiDma_ReadReg(InstancePtr,
			LITE_S2MM_DMACR);

	/* Reset is done when the reset bit is low
	 */
	if(RegisterValue & LITE_XAXIDMA_CR_RESET_MASK) {

		return 0;
	}

	return 1;
}

unsigned int LITE_WriteData_DDR2Device(unsigned int*  InstancePtr, unsigned int BuffAddr, unsigned int  Length){
	LITE_XAxiDma_WriteReg(InstancePtr,LITE_MM2S_SRCADDR, BuffAddr);
	LITE_XAxiDma_WriteReg(InstancePtr,LITE_MM2S_DMACR, \
			LITE_XAxiDma_ReadReg(InstancePtr,LITE_MM2S_DMACR) | \
			LITE_XAXIDMA_CR_RUNSTOP_MASK);
	LITE_XAxiDma_WriteReg(InstancePtr,LITE_MM2S_LENGTH, Length);

	return 0;
}
unsigned int LITE_ReadData_Device2DDR(unsigned int*  InstancePtr, unsigned int BuffAddr, unsigned int  Length){
	LITE_XAxiDma_WriteReg(InstancePtr,LITE_S2MM_DESTADDR, BuffAddr);
	LITE_XAxiDma_WriteReg(InstancePtr,LITE_S2MM_DMACR, \
			LITE_XAxiDma_ReadReg(InstancePtr,LITE_S2MM_DMACR) | \
			LITE_XAXIDMA_CR_RUNSTOP_MASK);
	LITE_XAxiDma_WriteReg(InstancePtr,LITE_S2MM_LENGTH, Length);

	return 0;
}

unsigned int LITE_Device2DDR_RealLength(unsigned int*  InstancePtr)
{
	return  LITE_XAxiDma_ReadReg(InstancePtr, LITE_S2MM_LENGTH);
}


