#ifndef SRC_LITE_XIL_HW_H_
#define SRC_LITE_XIL_HW_H_


/////////////////////   DMA   /////////////////////////////////////////
#define LITE_MM2S_DMACR	 			0x00000000   /**< Channel control */
#define LITE_MM2S_DMASR	 			0x00000004   /**< Status */
#define LITE_MM2S_SRCADDR	 		0x00000018   /**< MM2S Source Address. Lower 32 bits of address.*/
#define LITE_MM2S_SRCADDR_MSB	 	0x0000001C   /**< MM2S Source Address. Upper 32 bits of address.*/
#define LITE_MM2S_LENGTH	 			0x00000028   /**< MM2S Transfer Length (Bytes).*/

#define LITE_S2MM_DMACR	 			0x00000030   /**< Channel control */
#define LITE_S2MM_DMASR	 			0x00000034   /**< Status */
#define LITE_S2MM_DESTADDR	 		0x00000048   /**< S2MM Destination Address. Lower 32 bits of address.*/
#define LITE_S2MM_DESTADDR_MSB	 	0x0000004C   /**< S2MM Destination Address. Upper 32 bits of address.*/
#define LITE_S2MM_LENGTH	 			0x00000058   /**< S2MM Transfer Length (Bytes).*/


#define LITE_XAXIDMA_HALTED_MASK		0x00000001  /**< DMA channel halted */
#define LITE_XAXIDMA_IDLE_MASK		0x00000002  /**< DMA channel idle */

#define LITE_XAXIDMA_IRQ_IOC_MASK		0x00001000 /**< Completion intr */
#define LITE_XAXIDMA_IRQ_DELAY_MASK		0x00002000 /**< Delay interrupt */
#define LITE_XAXIDMA_IRQ_ERROR_MASK		0x00004000 /**< Error interrupt */
#define LITE_XAXIDMA_IRQ_ALL_MASK		0x00007000 /**< All interrupts */

#define LITE_XAXIDMA_CR_RUNSTOP_MASK	0x00000001 /**< Start/stop DMA channel */
#define LITE_XAXIDMA_CR_RESET_MASK	0x00000004 /**< Reset DMA engine */
#define LITE_XAXIDMA_CR_KEYHOLE_MASK	0x00000008 /**< Keyhole feature */
#define LITE_XAXIDMA_CR_CYCLIC_MASK	0x00000010 /**< Cyclic Mode */





/////////////////////   CUSTOMER DEFINE START   /////////////////////////////////////////

#define	DMAIP_BASEADDR		0x40400000
#define DDR_BASEARDDR      XPAR_DDR_MEM_BASEADDR + 0x10000000
#define MEM_BASE_ADDR		DDR_BASEARDDR
#define MIMODATA_BUFFER_BASE		MEM_BASE_ADDR+ 0x01000000
/////////////////////   CUSTOMER DEFINE END   /////////////////////////////////////////


extern unsigned int* inst_DMAIP;


static inline void LITE_Xil_Out8(unsigned int*  InstancePtr, unsigned char Value)
{
	*((volatile unsigned char *)(InstancePtr)) = Value;
}
static inline unsigned char LITE_Xil_In8(unsigned int*  InstancePtr)
{
	return *((volatile unsigned char *)(InstancePtr)) ;
}


static inline void LITE_Xil_Out32(unsigned int*  InstancePtr, unsigned int Value)
{
	*((volatile unsigned int *)(InstancePtr)) = Value;
}

static inline unsigned int LITE_Xil_In32(unsigned int*  InstancePtr)
{
	return *((volatile unsigned int *)(InstancePtr));
}
static __inline unsigned int LITE_XAxiDma_ReadReg(unsigned int* InstancePtr,int RegOffset)
{
	return InstancePtr[RegOffset>>2];
}

static __inline void LITE_XAxiDma_WriteReg(unsigned int* InstancePtr,int  RegOffset,unsigned int  Data)
{
	InstancePtr[RegOffset>>2] = Data;
}




extern void InitLiteDMA();
extern unsigned int LITE_XAxiDma_MM2S__Busy(unsigned int*  InstancePtr);
extern unsigned int LITE_XAxiDma_S2MM__Busy(unsigned int*  InstancePtr);
extern void LITE_XAxiDma_MM2S_IntrDisable(unsigned int* InstancePtr, unsigned int Mask)	;
extern void LITE_XAxiDma_S2MM_IntrDisable(unsigned int* InstancePtr, unsigned int Mask);
extern void LITE_XAxiDma_Reset(unsigned int* InstancePtr);
extern unsigned int LITE_WriteData_DDR2Device(unsigned int*  InstancePtr, unsigned int BuffAddr, unsigned int  Length);
extern unsigned int LITE_ReadData_Device2DDR(unsigned int*  InstancePtr, unsigned int BuffAddr, unsigned int  Length);
extern void LITE_XAxiDma_S2MM_Status(unsigned int*  InstancePtr);
extern int LITE_XAxiDma_ResetIsDone(unsigned int* InstancePtr);
extern unsigned int LITE_Device2DDR_RealLength(unsigned int*  InstancePtr);



#endif /* SRC_PS_XIL_HW_H_ */
