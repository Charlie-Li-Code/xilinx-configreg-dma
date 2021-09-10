#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "dma.h"
#include <stdlib.h>
#include <string.h>

#define MAX_PKT_LEN		4096
#define TEST_START_VALUE	0xC
#define NUMBER_OF_TRANSFERS	10
#define	MMAP_SIZE	65535		//64KB
#define DDR_BASEARDDR      XPAR_DDR_MEM_BASEADDR + 0x10000000
#define MEM_BASE_ADDR		DDR_BASEARDDR
#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)
#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00300000)
#define MAX_PKT_LEN_WORDS	8
#define DATA_BUFFER_SIZE    65536

unsigned int* inst_DMAIP;
unsigned int* inst_RX_BUFFER;
unsigned int* inst_TX_BUFFER;

static __inline unsigned int XAxiDmaReadReg(unsigned int* InstancePtr,int RegOffset)
{
	return InstancePtr[RegOffset>>2]; // >>2 == /4,because type int == 4 bytes;
}

static __inline void XAxiDmaWriteReg(unsigned int* InstancePtr,int  RegOffset,unsigned int  Data)
{
	InstancePtr[RegOffset>>2] = Data;
}

void XAxiDmaS2MMStatus(unsigned int*  InstancePtr){
	unsigned int status = XAxiDmaReadReg(InstancePtr, S2MM_DMASR);
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

unsigned int DmaWriteBusy(unsigned int*  InstancePtr)
{
	return ((XAxiDmaReadReg(InstancePtr,MM2S_DMASR) &
				XAXIDMA_IDLE_MASK) ? 0 : 1);
}

unsigned int DmaReadBusy(unsigned int*  InstancePtr)
{
	return ((XAxiDmaReadReg(InstancePtr,S2MM_DMASR) &
				XAXIDMA_IDLE_MASK) ? 0 : 1);
}
void XAxiDmaMM2SIntrDisable(unsigned int* InstancePtr, unsigned int Mask)	{
	XAxiDmaWriteReg(InstancePtr,MM2S_DMACR, \
			XAxiDmaReadReg(InstancePtr,MM2S_DMACR)\
			& ~(Mask & XAXIDMA_IRQ_ALL_MASK));
}

void XAxiDmaS2MMIntrDisable(unsigned int* InstancePtr, unsigned int Mask)	{
	XAxiDmaWriteReg(InstancePtr,S2MM_DMACR, \
			XAxiDmaReadReg(InstancePtr,S2MM_DMACR)\
			& ~(Mask & XAXIDMA_IRQ_ALL_MASK));
}

void XAxiDmaReset(unsigned int* InstancePtr)
{
	//Resetting DMA
	XAxiDmaWriteReg(InstancePtr, S2MM_DMACR, 4);
	XAxiDmaWriteReg(InstancePtr, MM2S_DMACR, 4);
	//Halting DMA
	XAxiDmaWriteReg(InstancePtr, S2MM_DMACR, 0);
	XAxiDmaWriteReg(InstancePtr, MM2S_DMACR, 0);
}

int XAxiDma_ResetIsDone(unsigned int*  InstancePtr)
{
	unsigned int RegisterValue;

	/* Check transmit channel
	 */
	RegisterValue = XAxiDmaReadReg(InstancePtr,
			MM2S_DMACR);
	/* Reset is done when the reset bit is low
	 */
	if(RegisterValue & XAXIDMA_CR_RESET_MASK) {

		return 0;
	}

	/* Check receive channel
	 */
	RegisterValue = XAxiDmaReadReg(InstancePtr,
			S2MM_DMACR);

	/* Reset is done when the reset bit is low
	 */
	if(RegisterValue & XAXIDMA_CR_RESET_MASK) {

		return 0;
	}

	return 1;
}

unsigned int WriteDataDDR2Device(unsigned int*  InstancePtr, unsigned int BuffAddr, unsigned int  Length){
	XAxiDmaWriteReg(InstancePtr,MM2S_SRCADDR, BuffAddr);
	XAxiDmaWriteReg(InstancePtr,MM2S_DMACR, \
			XAxiDmaReadReg(InstancePtr,MM2S_DMACR) | \
			XAXIDMA_CR_RUNSTOP_MASK);
	XAxiDmaWriteReg(InstancePtr,MM2S_LENGTH, Length);

	return 0;
}

unsigned int ReadDataDevice2DDR(unsigned int*  InstancePtr, unsigned int BuffAddr, unsigned int  Length){
	XAxiDmaWriteReg(InstancePtr,S2MM_DESTADDR, BuffAddr);
	XAxiDmaWriteReg(InstancePtr,S2MM_DMACR, \
			XAxiDmaReadReg(InstancePtr,S2MM_DMACR) | \
			XAXIDMA_CR_RUNSTOP_MASK);
	XAxiDmaWriteReg(InstancePtr,S2MM_LENGTH, Length);
	
	return 0;
}

unsigned int DmaReadLength(unsigned int*  InstancePtr)
{
	return  XAxiDmaReadReg(InstancePtr, S2MM_LENGTH);
}

int InitDMA(){
	int memfd = open("/dev/mem",O_RDWR | O_SYNC);
	if(-1 == memfd){
		printf("Can't open /dev/mem.\n");
		return -1;
	}

	inst_DMAIP = static_cast<unsigned int*>(mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, DMAIP_BASEADDR));

	inst_RX_BUFFER = static_cast<unsigned int*>(mmap(NULL, DATA_BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, RX_BUFFER_BASE));
    inst_TX_BUFFER = static_cast<unsigned int*>(mmap(NULL, DATA_BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, TX_BUFFER_BASE));

	
	close(memfd);

	XAxiDmaMM2SIntrDisable(inst_DMAIP,XAXIDMA_IRQ_ALL_MASK);
	XAxiDmaS2MMIntrDisable(inst_DMAIP,XAXIDMA_IRQ_ALL_MASK);

	XAxiDmaS2MMStatus(inst_DMAIP);

	return 0;
}

int DestroyDMA(){
	
	munmap(inst_DMAIP,MMAP_SIZE);	
	munmap(inst_RX_BUFFER,DATA_BUFFER_SIZE);	
	munmap(inst_TX_BUFFER,DATA_BUFFER_SIZE);	


	printf("UNMAP Finish\n");

    return 0;
}

int main()
{
	int Value = 0;
	int Index = 0;

	if(InitDMA()==-1)
	{
		perror("Init MMAP Error");
		exit(0);
	}

	for(Index = 0; Index < ( MAX_PKT_LEN/4); Index ++) {
		inst_TX_BUFFER[Index] = Value;
		Value = (Value + 1) & 0xFF;
	}

	printf("WRITE START \r\n");
	WriteDataDDR2Device(inst_DMAIP, TX_BUFFER_BASE, MAX_PKT_LEN);
	while (DmaWriteBusy(inst_DMAIP))
	{
		/* code */
	}
	printf("WRITE FINISH\r\n");

	printf("READ START\r\n");
	ReadDataDevice2DDR(inst_DMAIP, RX_BUFFER_BASE, MAX_PKT_LEN);
	while (DmaReadBusy(inst_DMAIP))
	{
		/* code */
	}
	printf("READ FINISH\r\n");
	
	int count=DmaReadLength(inst_DMAIP);
	printf("Read Length=%d\n",count);

	for(int i=count/4-5;i<count/4;i++)
	printf("Data %d: %x\r\n",i, inst_RX_BUFFER[i]);

	DestroyDMA();
	return 0;

}

