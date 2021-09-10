#ifndef DMA_H
#define DMA_H

#define MM2S_DMACR 0x00000000 /**< Channel control */
#define MM2S_DMASR 0x00000004 /**< Status */
#define MM2S_SRCADDR 0x00000018 /**< MM2S Source Address. Lower 32 bits of address.*/
#define MM2S_SRCADDR_MSB 0x0000001C /**< MM2S Source Address. Upper 32 bits of address.*/
#define MM2S_LENGTH 0x00000028 /**< MM2S Transfer Length (Bytes).*/

#define S2MM_DMACR 0x00000030 /**< Channel control */
#define S2MM_DMASR 0x00000034 /**< Status */
#define S2MM_DESTADDR 0x00000048 /**< S2MM Destination Address. Lower 32 bits of address.*/
#define S2MM_DESTADDR_MSB 0x0000004C /**< S2MM Destination Address. Upper 32 bits of address.*/
#define S2MM_LENGTH 0x00000058 /**< S2MM Transfer Length (Bytes).*/


#define XAXIDMA_HALTED_MASK 0x00000001 /**< DMA channel halted */
#define XAXIDMA_IDLE_MASK 0x00000002 /**< DMA channel idle */

#define XAXIDMA_IRQ_IOC_MASK 0x00001000 /**< Completion intr */
#define XAXIDMA_IRQ_DELAY_MASK 0x00002000 /**< Delay interrupt */
#define XAXIDMA_IRQ_ERROR_MASK 0x00004000 /**< Error interrupt */
#define XAXIDMA_IRQ_ALL_MASK 0x00007000 /**< All interrupts */

#define XAXIDMA_CR_RUNSTOP_MASK 0x00000001 /**< Start/stop DMA channel */
#define XAXIDMA_CR_RESET_MASK 0x00000004 /**< Reset DMA engine */
#define XAXIDMA_CR_KEYHOLE_MASK 0x00000008 /**< Keyhole feature */
#define XAXIDMA_CR_CYCLIC_MASK 0x00000010 /**< Cyclic Mode */

#define XPAR_DDR_MEM_BASEADDR		0x00000000U
#define XPAR_DDR_MEM_HIGHADDR		0x3FFFFFFFU

#define	DMAIP_BASEADDR		0x40400000

#endif
