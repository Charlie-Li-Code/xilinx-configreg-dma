#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "dma.h"
#include <stdlib.h>

#include <string.h>

//  REG START

// REG END


// DMA START
int memfd;

#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00100000)
#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00300000)
#define MAX_PKT_LEN		4096
#define TEST_START_VALUE	0xC
#define NUMBER_OF_TRANSFERS	10


#define		MMAP_SIZE	65535		//64KB
#define		MIMO_BUFFER_SIZE		65536	//64KB  

#define XPAR_DDR_MEM_BASEADDR		0x00000000U
#define XPAR_DDR_MEM_HIGHADDR		0x3FFFFFFFU
#define MM2S_DMACR	 			0x00000000   /**< Channel control */
#define MM2S_DMASR	 			0x00000004   /**< Status */
#define MM2S_SRCADDR	 		0x00000018   /**< MM2S Source Address. Lower 32 bits of address.*/
#define MM2S_SRCADDR_MSB	 	0x0000001C   /**< MM2S Source Address. Upper 32 bits of address.*/
#define MM2S_LENGTH	 			0x00000028   /**< MM2S Transfer Length (Bytes).*/

#define S2MM_DMACR	 			0x00000030   /**< Channel control */
#define S2MM_DMASR	 			0x00000034   /**< Status */
#define S2MM_DESTADDR	 		0x00000048   /**< S2MM Destination Address. Lower 32 bits of address.*/
#define S2MM_DESTADDR_MSB	 	0x0000004C   /**< S2MM Destination Address. Upper 32 bits of address.*/
#define S2MM_LENGTH	 			0x00000058   /**< S2MM Transfer Length (Bytes).*/


#define XAXIDMA_HALTED_MASK		0x00000001  /**< DMA channel halted */
#define XAXIDMA_IDLE_MASK		0x00000002  /**< DMA channel idle */

#define XAXIDMA_IRQ_IOC_MASK		0x00001000 /**< Completion intr */
#define XAXIDMA_IRQ_DELAY_MASK		0x00002000 /**< Delay interrupt */
#define XAXIDMA_IRQ_ERROR_MASK		0x00004000 /**< Error interrupt */
#define XAXIDMA_IRQ_ALL_MASK		0x00007000 /**< All interrupts */

#define XAXIDMA_CR_RUNSTOP_MASK	0x00000001 /**< Start/stop DMA channel */
#define XAXIDMA_CR_RESET_MASK	0x00000004 /**< Reset DMA engine */
#define XAXIDMA_CR_KEYHOLE_MASK	0x00000008 /**< Keyhole feature */
#define XAXIDMA_CR_CYCLIC_MASK	0x00000010 /**< Cyclic Mode */

/* sample sci */
#define DDR_BASEARDDR      XPAR_DDR_MEM_BASEADDR + 0x10000000
#define MEM_BASE_ADDR		DDR_BASEARDDR
#define MIMODATA_BUFFER_BASE		MEM_BASE_ADDR+ 0x01000000

#define MAX_PKT_LEN_WORDS	8
//#define MAX_PKT_LEN			4194304	//2^23


#define	DMAIP_BASEADDR		0x40400000

unsigned int* inst_DMAIP;
unsigned int* inst_MIMODATA_BUFFER;
unsigned int* inst_RX_BUFFER;
unsigned int* inst_TX_BUFFER;



static inline void Xil_Out8(unsigned int*  InstancePtr, unsigned char Value)
{
	*((volatile unsigned char *)(InstancePtr)) = Value;
}
static inline unsigned char Xil_In8(unsigned int*  InstancePtr)
{
	return *((volatile unsigned char *)(InstancePtr)) ;
}


static inline void Xil_Out32(unsigned int*  InstancePtr, unsigned int Value)
{
	*((volatile unsigned int *)(InstancePtr)) = Value;
}

static inline unsigned int Xil_In32(unsigned int*  InstancePtr)
{
	return *((volatile unsigned int *)(InstancePtr));
}
static __inline unsigned int XAxiDma_ReadReg(unsigned int* InstancePtr,int RegOffset)
{
	return InstancePtr[RegOffset>>2];
}

static __inline void XAxiDma_WriteReg(unsigned int* InstancePtr,int  RegOffset,unsigned int  Data)
{
	InstancePtr[RegOffset>>2] = Data;
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

int PLInstancePtrMMAP(){
	memfd = open("/dev/mem",O_RDWR | O_SYNC);
	if(-1 == memfd){
		printf("Can't open /dev/mem.\n");
		return -1;
	}

	

	inst_DMAIP = static_cast<unsigned int*>(mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, DMAIP_BASEADDR));
	inst_MIMODATA_BUFFER = static_cast<unsigned int*>(mmap(NULL, MIMO_BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, MIMODATA_BUFFER_BASE));

	inst_RX_BUFFER = static_cast<unsigned int*>(mmap(NULL, MIMO_BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, RX_BUFFER_BASE));
    inst_TX_BUFFER = static_cast<unsigned int*>(mmap(NULL, MIMO_BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, TX_BUFFER_BASE));



	//ptrByteDDR = static_cast<unsigned char*>((void*)inst_MIMODATA_BUFFER);
	
	close(memfd);


	memset(inst_MIMODATA_BUFFER, 0, MIMO_BUFFER_SIZE);	 // Clear destination block

	XAxiDma_MM2S_IntrDisable(inst_DMAIP,XAXIDMA_IRQ_ALL_MASK);
	XAxiDma_S2MM_IntrDisable(inst_DMAIP,XAXIDMA_IRQ_ALL_MASK);

	XAxiDma_S2MM_Status(inst_DMAIP);

	return 0;
}

int PLInstanceUNMap(){
	
	munmap(inst_DMAIP,MMAP_SIZE);	
	munmap(inst_MIMODATA_BUFFER,MIMO_BUFFER_SIZE);	
	munmap(inst_RX_BUFFER,MIMO_BUFFER_SIZE);	
	munmap(inst_TX_BUFFER,MIMO_BUFFER_SIZE);	


	printf("UNMAP Finish\n");

    return 0;
}
// DMA END


int main()
{
	int Value = 0;
    int Index = 0;



   if( -1 == PLInstancePtrMMAP())
   {
       perror("Init MMAP Error");
       exit(0);
   }

	for(Index = 0; Index < ( MAX_PKT_LEN/4); Index ++) {
		inst_TX_BUFFER[Index] = Value;
		Value = (Value + 10) & 0xFF;
	}
   
    printf("WRITE START \r\n");
   WriteData_DDR2Device(inst_DMAIP, TX_BUFFER_BASE, MAX_PKT_LEN);
   while (XAxiDma_MM2S__Busy(inst_DMAIP))
   {
       /* code */
   }
   printf("WRITE FINISH\r\n");
   ReadData_Device2DDR(inst_DMAIP, RX_BUFFER_BASE, MAX_PKT_LEN);
	printf("READ START\r\n");
   while (XAxiDma_S2MM__Busy(inst_DMAIP))// || XAxiDma_MM2S__Busy(inst_DMAIP))
   {
       /* code */
   }
   printf("READ FINISH\r\n");
   	for(Index = 0; Index < (MAX_PKT_LEN/4); Index+=10) {
		printf("Data %d: %x\r\n",Index, inst_RX_BUFFER[Index]);
	}

    PLInstanceUNMap();
	return 0;

}

