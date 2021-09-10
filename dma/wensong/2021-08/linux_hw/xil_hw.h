/*
 * xil_hw.h
 *
 *  Created on: 2017年5月18日
 *      Author: wensong
 */

#ifndef SRC_PS_XIL_HW_H_
#define SRC_PS_XIL_HW_H_


/* Canonical definitions for DDR MEMORY */
#define XPAR_DDR_MEM_BASEADDR		0x00000000U
#define XPAR_DDR_MEM_HIGHADDR		0x3FFFFFFFU


/////////////////////   DMA   /////////////////////////////////////////
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
#define MAX_PKT_LEN			4194304	//2^23



#define	DMAIP_BASEADDR		0x80400000


#define CMD_READY_BASEADDR 		0x81220000
#define CMD_EN_BASEADDR 		0x81210000
#define CMD_DATA_BASEADDR 		0x81200000
#define CMD_RESULT_BASEADDR 	0x81230000


#define MM2S_FINISH_BASEADDR 	0x81240000
#define S2MM_FINISH_BASEADDR 	0x81250000


#define Rx_Need_BASEADDR 	0x81260000
#define Tx_Need_BASEADDR 	0x81280000

#define Rx_ACK_BASEADDR 	0x81270000
#define Tx_ACK_BASEADDR 	0x81290000


extern unsigned int* inst_CMD_READY;
extern unsigned int* inst_CMD_EN;
extern unsigned int* inst_CMD_DATA;
extern unsigned int* inst_CMD_RESULT;
extern unsigned int* inst_MM2S_FINISH;
extern unsigned int* inst_S2MM_FINISH;
extern unsigned int* inst_Rx_Need;
extern unsigned int* inst_Tx_Need;
extern unsigned int* inst_Rx_Ack;
extern unsigned int* inst_Tx_Ack;

extern unsigned int* inst_DMAIP;
extern unsigned int* inst_MIMODATA_BUFFER;
//extern unsigned char *ptrByteDDR;

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

extern int PLInstanceUNMap();
extern int PLInstancePtrMMAP();
extern unsigned int XAxiDma_MM2S__Busy(unsigned int*  InstancePtr);
extern unsigned int XAxiDma_S2MM__Busy(unsigned int*  InstancePtr);
extern void XAxiDma_MM2S_IntrDisable(unsigned int* InstancePtr, unsigned int Mask)	;
extern void XAxiDma_S2MM_IntrDisable(unsigned int* InstancePtr, unsigned int Mask);
extern void XAxiDma_Reset(unsigned int* InstancePtr);
extern unsigned int WriteData_DDR2Device(unsigned int*  InstancePtr, unsigned int BuffAddr, unsigned int  Length);
extern unsigned int ReadData_Device2DDR(unsigned int*  InstancePtr, unsigned int BuffAddr, unsigned int  Length);
extern void XAxiDma_S2MM_Status(unsigned int*  InstancePtr);
extern int XAxiDma_ResetIsDone(unsigned int* InstancePtr);
extern unsigned int Device2DDR_RealLength(unsigned int*  InstancePtr);

#endif /* SRC_PS_XIL_HW_H_ */

