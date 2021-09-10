/*
 * xil_hw.c
 *
 *  Created on: 2017年5月18日
 *      Author: wensong
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "xil_hw.h"

#define		MMAP_SIZE	65535		//64KB
#define		MIMO_BUFFER_SIZE		65536	//64KB   



int memfd;


unsigned int* inst_CMD_READY;
unsigned int* inst_CMD_EN;
unsigned int* inst_CMD_DATA;
unsigned int* inst_CMD_RESULT;
unsigned int* inst_MM2S_FINISH;
unsigned int* inst_S2MM_FINISH;
unsigned int* inst_Rx_Need;
unsigned int* inst_Tx_Need;
unsigned int* inst_Rx_Ack;
unsigned int* inst_Tx_Ack;

unsigned int* inst_DMAIP;
unsigned int* inst_MIMODATA_BUFFER;
//unsigned char *ptrByteDDR;


int PLInstanceUNMap(){
	munmap(inst_CMD_READY,MMAP_SIZE);
	munmap(inst_CMD_EN,MMAP_SIZE);
	munmap(inst_CMD_DATA,MMAP_SIZE);
	munmap(inst_CMD_RESULT,MMAP_SIZE);
	munmap(inst_MM2S_FINISH,MMAP_SIZE);	
	munmap(inst_S2MM_FINISH,MMAP_SIZE);	
	munmap(inst_Rx_Need,MMAP_SIZE);		
	munmap(inst_Tx_Need,MMAP_SIZE);	
	munmap(inst_Rx_Ack,MMAP_SIZE);	
	munmap(inst_Tx_Ack,MMAP_SIZE);	
	munmap(inst_DMAIP,MMAP_SIZE);	
	munmap(inst_MIMODATA_BUFFER,MIMO_BUFFER_SIZE);	
	printf("UNMAP Finish\n");
							
}
int PLInstancePtrMMAP(){
	memfd = open("/dev/mem",O_RDWR | O_SYNC);
	if(-1 == memfd){
		printf("Can't open /dev/mem.\n");
		return -1;
	}

	inst_CMD_READY = static_cast<unsigned int*>(mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, CMD_READY_BASEADDR));
	inst_CMD_EN = static_cast<unsigned int*>(mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, CMD_EN_BASEADDR));
	inst_CMD_DATA = static_cast<unsigned int*>(mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, CMD_DATA_BASEADDR));
	inst_CMD_RESULT = static_cast<unsigned int*>(mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, CMD_RESULT_BASEADDR));
	inst_MM2S_FINISH = static_cast<unsigned int*>(mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, MM2S_FINISH_BASEADDR));
	inst_S2MM_FINISH = static_cast<unsigned int*>(mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, S2MM_FINISH_BASEADDR));
	inst_Rx_Need = static_cast<unsigned int*>(mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, Rx_Need_BASEADDR));
	inst_Tx_Need = static_cast<unsigned int*>(mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, Tx_Need_BASEADDR));
	inst_Rx_Ack = static_cast<unsigned int*>(mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, Rx_ACK_BASEADDR));
	inst_Tx_Ack = static_cast<unsigned int*>(mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, Tx_ACK_BASEADDR));

	inst_DMAIP = static_cast<unsigned int*>(mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, DMAIP_BASEADDR));
	inst_MIMODATA_BUFFER = static_cast<unsigned int*>(mmap(NULL, MIMO_BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, MIMODATA_BUFFER_BASE));
	
	//ptrByteDDR = static_cast<unsigned char*>((void*)inst_MIMODATA_BUFFER);
	
	close(memfd);
	
	Xil_Out8(inst_MM2S_FINISH,0x00);
	Xil_Out8(inst_S2MM_FINISH,0x00);

	Xil_Out8(inst_Rx_Ack,0x00);
	Xil_Out8(inst_Tx_Ack,0x00);

	memset(inst_MIMODATA_BUFFER, 0, MIMO_BUFFER_SIZE);	 // Clear destination block

	XAxiDma_MM2S_IntrDisable(inst_DMAIP,XAXIDMA_IRQ_ALL_MASK);
	XAxiDma_S2MM_IntrDisable(inst_DMAIP,XAXIDMA_IRQ_ALL_MASK);

	XAxiDma_S2MM_Status(inst_DMAIP);
	/*
	Xil_Out32(inst_CMD_DATA,0x22000000);
	XAxiDma_Reset(inst_DMAIP);
    while(!XAxiDma_ResetIsDone(inst_DMAIP)){
    			//WAIT
    }
    */
	//XAxiDma_S2MM_Status(inst_DMAIP);
	return 0;
}

void XAxiDma_S2MM_Status(unsigned int*  InstancePtr){
	unsigned int status = XAxiDma_ReadReg(InstancePtr, S2MM_DMASR);
	printf("Stream to memory-mapped status (0x%08x):",status);
	if (status & 0x00000001)
		printf("Halted");
	else
		printf("Running ");
	if (status & 0x00000002)	printf("Idle");
	if (status & 0x00000008) 	printf("SGIncld");
	if (status & 0x00000010)	printf("DMAIntErr");
	if (status & 0x00000020)	printf("DMASlvErr");
	if (status & 0x00000040)	printf("DMADecErr");
	if (status & 0x00000100)	printf("SGIntErr");
	if (status & 0x00000200)	printf("SGSlvErr");
	if (status & 0x00000400)	printf("SGDecErr");
	if (status & 0x00001000)	printf("IOC_Irq");
	if (status & 0x00002000)	printf("Dly_Irq");
	if (status & 0x00004000)	printf("Err_Irq");
	printf("\n");
}

unsigned int XAxiDma_MM2S__Busy(unsigned int*  InstancePtr)
{
	return ((XAxiDma_ReadReg(InstancePtr,MM2S_DMASR) &
				XAXIDMA_IDLE_MASK) ? 0 : 1);
}

unsigned int XAxiDma_S2MM__Busy(unsigned int*  InstancePtr)
{
	return ((XAxiDma_ReadReg(InstancePtr,S2MM_DMASR) &
				XAXIDMA_IDLE_MASK) ? 0 : 1);
}
void XAxiDma_MM2S_IntrDisable(unsigned int* InstancePtr, unsigned int Mask)	{
	XAxiDma_WriteReg(InstancePtr,MM2S_DMACR, \
			XAxiDma_ReadReg(InstancePtr,MM2S_DMACR)\
			& ~(Mask & XAXIDMA_IRQ_ALL_MASK));
}

void XAxiDma_S2MM_IntrDisable(unsigned int* InstancePtr, unsigned int Mask)	{
	XAxiDma_WriteReg(InstancePtr,S2MM_DMACR, \
			XAxiDma_ReadReg(InstancePtr,S2MM_DMACR)\
			& ~(Mask & XAXIDMA_IRQ_ALL_MASK));
}

void XAxiDma_Reset(unsigned int* InstancePtr)
{
	//Resetting DMA
	XAxiDma_WriteReg(InstancePtr, S2MM_DMACR, 4);
	XAxiDma_WriteReg(InstancePtr, MM2S_DMACR, 4);
	//Halting DMA
	XAxiDma_WriteReg(InstancePtr, S2MM_DMACR, 0);
	XAxiDma_WriteReg(InstancePtr, MM2S_DMACR, 0);
}

int XAxiDma_ResetIsDone(unsigned int*  InstancePtr)
{
	unsigned int RegisterValue;

	/* Check transmit channel
	 */
	RegisterValue = XAxiDma_ReadReg(InstancePtr,
			MM2S_DMACR);
	/* Reset is done when the reset bit is low
	 */
	if(RegisterValue & XAXIDMA_CR_RESET_MASK) {

		return 0;
	}

	/* Check receive channel
	 */
	RegisterValue = XAxiDma_ReadReg(InstancePtr,
			S2MM_DMACR);

	/* Reset is done when the reset bit is low
	 */
	if(RegisterValue & XAXIDMA_CR_RESET_MASK) {

		return 0;
	}

	return 1;
}
unsigned int WriteData_DDR2Device(unsigned int*  InstancePtr, unsigned int BuffAddr, unsigned int  Length){
	XAxiDma_WriteReg(InstancePtr,MM2S_SRCADDR, BuffAddr);
	XAxiDma_WriteReg(InstancePtr,MM2S_DMACR, \
			XAxiDma_ReadReg(InstancePtr,MM2S_DMACR) | \
			XAXIDMA_CR_RUNSTOP_MASK);
	XAxiDma_WriteReg(InstancePtr,MM2S_LENGTH, Length);

	return 0;
}
unsigned int ReadData_Device2DDR(unsigned int*  InstancePtr, unsigned int BuffAddr, unsigned int  Length){
	XAxiDma_WriteReg(InstancePtr,S2MM_DESTADDR, BuffAddr);
	XAxiDma_WriteReg(InstancePtr,S2MM_DMACR, \
			XAxiDma_ReadReg(InstancePtr,S2MM_DMACR) | \
			XAXIDMA_CR_RUNSTOP_MASK);
	XAxiDma_WriteReg(InstancePtr,S2MM_LENGTH, Length);
	
	return 0;
}

unsigned int Device2DDR_RealLength(unsigned int*  InstancePtr)
{
	return  XAxiDma_ReadReg(InstancePtr, S2MM_LENGTH);
}

