#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "dma.h"

//  REG START
/* Definitions for peripheral AXI_GPIO_ADDRESS */
#define  AXI_GPIO_ADDRESS_BASEADDR 0x41200000
#define  AXI_GPIO_ADDRESS_HIGHADDR 0x4120FFFF


/* Definitions for peripheral AXI_GPIO_READREADY */
#define  AXI_GPIO_READREADY_BASEADDR 0x41210000
#define  AXI_GPIO_READREADY_HIGHADDR 0x4121FFFF


/* Definitions for peripheral AXI_GPIO_READVALID */
#define  AXI_GPIO_READVALID_BASEADDR 0x41220000
#define  AXI_GPIO_READVALID_HIGHADDR 0x4122FFFF


/* Definitions for peripheral AXI_GPIO_READVALUE_H */
#define  AXI_GPIO_READVALUE_H_BASEADDR 0x41230000
#define  AXI_GPIO_READVALUE_H_HIGHADDR 0x4123FFFF


/* Definitions for peripheral AXI_GPIO_READVALUE_L */
#define  AXI_GPIO_READVALUE_L_BASEADDR 0x41240000
#define  AXI_GPIO_READVALUE_L_HIGHADDR 0x4124FFFF


/* Definitions for peripheral AXI_GPIO_WRITEREADY */
#define  AXI_GPIO_WRITEREADY_BASEADDR 0x41250000
#define  AXI_GPIO_WRITEREADY_HIGHADDR 0x4125FFFF


/* Definitions for peripheral AXI_GPIO_WRITEVALID */
#define  AXI_GPIO_WRITEVALID_BASEADDR 0x41260000
#define  AXI_GPIO_WRITEVALID_HIGHADDR 0x4126FFFF


/* Definitions for peripheral AXI_GPIO_WRITEVALUE_H */
#define  AXI_GPIO_WRITEVALUE_H_BASEADDR 0x41270000
#define  AXI_GPIO_WRITEVALUE_H_HIGHADDR 0x4127FFFF

/* Definitions for peripheral AXI_GPIO_WRITEVALUE_L */
#define  AXI_GPIO_WRITEVALUE_L_BASEADDR 0x41280000
#define  AXI_GPIO_WRITEVALUE_L_HIGHADDR 0x4128FFFF
#define REG int
#define REG_SIZE 0x04

REG *read_ready_gpio,*read_valid_gpio,*read_value_h_gpio,*read_value_l_gpio,
		*write_ready_gpio,*write_valid_gpio,*write_value_h_gpio,*write_value_l_gpio,*address_gpio;

#define 	WRITE_READY 	(*(int*)write_ready_gpio)
#define 	WRITE_VALID 	(*(int*)write_valid_gpio)
#define 	WRITE_VALUE_H 	(*(int*)write_value_h_gpio)
#define 	WRITE_VALUE_L 	(*(int*)write_value_l_gpio)
#define 	ADDRESS 		(*(int*)address_gpio)
#define 	READ_READY 		(*(int*)read_ready_gpio)
#define 	READ_VALID 		(*(int*)read_valid_gpio)
#define 	READ_VALUE_H 	(*(int*)read_value_h_gpio)
#define 	READ_VALUE_L 	(*(int*)read_value_l_gpio)

#define 	HIGH_LEVEL      0x01	
#define 	LOW_LEVEL       0x00

#define     DEBUG_REG		0xF000

int InitReg(int map_fd){
	// read read_value register
	read_value_h_gpio=static_cast<int*>(mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_READVALUE_H_BASEADDR)));
	if(read_value_h_gpio==NULL){
		printf("mmap read_value_h_gpio failed! \n");
		return -1;
	}		

	read_value_l_gpio=static_cast<int*>(mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_READVALUE_L_BASEADDR)));
	if(read_value_l_gpio==NULL){
		printf("mmap read_value_l_gpio failed! \n");
		return -1;
	}

	read_ready_gpio=static_cast<int*>(mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_READREADY_BASEADDR)));
	if(read_ready_gpio==NULL){
		printf("mmap read_ready_gpio failed! \n");
		return -1;
	}

	read_valid_gpio=static_cast<int*>(mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_READVALID_BASEADDR)));
	if(read_valid_gpio==NULL){
		printf("mmap read_valid_gpio failed! \n");
		return -1;
	}

	write_ready_gpio=static_cast<int*>(mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_WRITEREADY_BASEADDR)));
	if(write_ready_gpio==NULL){
		printf("mmap write_ready_gpio failed! \n");
		return -1;
	}

	write_valid_gpio=static_cast<int*>(mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_WRITEVALID_BASEADDR)));
	if(write_valid_gpio==NULL){
		printf("mmap write_valid_gpio failed! \n");
		return -1;
	}

	write_value_h_gpio=static_cast<int*>(mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_WRITEVALUE_H_BASEADDR)));
	if(write_value_h_gpio==NULL){
		printf("mmap write_value_h_gpio failed! \n");
		return -1;
	}

	write_value_l_gpio=static_cast<int*>(mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_WRITEVALUE_L_BASEADDR)));
	if(write_value_l_gpio==NULL){
		printf("mmap write_value_l_gpio failed! \n");
		return -1;
	}

	address_gpio=static_cast<int*>(mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_ADDRESS_BASEADDR)));
	if(address_gpio==NULL){
		printf("mmap address_gpio failed! \n");
		return -1;
	}
	
	return 0;

}

int DestroyReg(){
	if(munmap(read_value_h_gpio,REG_SIZE)==-1){
		printf("munmap read_value_h_gpio error!\n");
		return -1;
	}
	if(munmap(read_value_l_gpio,REG_SIZE)==-1){
		printf("munmap read_value_l_gpio error!\n");
		return -1;
	}
	if(munmap(read_ready_gpio,REG_SIZE)==-1){
		printf("munmap read_ready_gpio error!\n");
		return -1;
	}
	if(munmap(read_valid_gpio,REG_SIZE)==-1){
		printf("munmap read_valid_gpio error!\n");
		return -1;
	}
	if(munmap(write_ready_gpio,REG_SIZE)==-1){
		printf("munmap write_ready_gpio error!\n");
		return -1;
	}
	if(munmap(write_valid_gpio,REG_SIZE)==-1){
		printf("munmap write_valid_gpio error!\n");
		return -1;
	}
	if(munmap(write_value_h_gpio,REG_SIZE)==-1){
		printf("munmap write_value_h_gpio error!\n");
		return -1;
	}
	if(munmap(write_value_l_gpio,REG_SIZE)==-1){
		printf("munmap write_value_l_gpio error!\n");
		return -1;
	}
	return 0;
}

void delay(){
	usleep(100);
}

int WriteToReg(int base_addr,int data_h,int data_l){
	WRITE_VALID=LOW_LEVEL;
	printf("wait WRITE_READY high...\n");
	while(WRITE_READY!=HIGH_LEVEL){
		delay();
	}
	printf("wait WRITE_READY high done\n");
	ADDRESS=base_addr;
	WRITE_VALUE_H=data_h;
	WRITE_VALUE_L=data_l;
	if(WRITE_READY==HIGH_LEVEL){
		WRITE_VALID=HIGH_LEVEL;
		//delay();
		WRITE_VALID=LOW_LEVEL;
	}
	return 0;
}


int ReadFromReg(int64_t base_addr){
	int addr_h,addr_l;
	int data_h,data_l;
	int* p=(int*)(&base_addr);
	addr_h=*(p+1);
	addr_l=*p;	
	printf("addr_h=%x\naddr_l=%x\n",addr_h,addr_l);

	READ_READY=LOW_LEVEL;
	WriteToReg(DEBUG_REG,addr_h,addr_l);
	READ_READY=HIGH_LEVEL;
	printf("wait READ_VALID high...\n");
	while(READ_VALID!=HIGH_LEVEL){
		delay();
	}
	printf("wait READ_VALID high done\n");
	if(READ_VALID==HIGH_LEVEL){
		data_h=READ_VALUE_H;
		data_l=READ_VALUE_L;
	}
	READ_READY=LOW_LEVEL;	
	printf("READ_VALUE_H=%x\n",data_h);
	printf("READ_VALUE_L=%x\n",data_l);

	return 0;
}
// REG END


#define AXI_DMA_DATA_BASEADDR 0x40400000
#define DMA_REG_SIZE 0X8000

unsigned int XAxiDma_WriteReg(unsigned int* InstancePtr, unsigned int offset,unsigned int BuffAddr){
    InstancePtr[offset>>2]=BuffAddr;
    return 0;
}

unsigned int XAxiDma_ReadReg(unsigned int* InstancePtr, unsigned int offset){
    return InstancePtr[offset>>2];    
}


unsigned int WriteData_DDR2Device(unsigned int* InstancePtr, unsigned int BuffAddr, unsigned int Length){
    XAxiDma_WriteReg(InstancePtr,MM2S_SRCADDR, BuffAddr);
    XAxiDma_WriteReg(InstancePtr,MM2S_DMACR, \
        XAxiDma_ReadReg(InstancePtr,MM2S_DMACR) | \
        XAXIDMA_CR_RUNSTOP_MASK);
    XAxiDma_WriteReg(InstancePtr,MM2S_LENGTH, Length);

    return 0;
}
unsigned int ReadData_Device2DDR(unsigned int* InstancePtr, unsigned int BuffAddr, unsigned int Length){
    XAxiDma_WriteReg(InstancePtr,S2MM_DESTADDR, BuffAddr);
    XAxiDma_WriteReg(InstancePtr,S2MM_DMACR, XAxiDma_ReadReg(InstancePtr,S2MM_DMACR) | XAXIDMA_CR_RUNSTOP_MASK);
    XAxiDma_WriteReg(InstancePtr,S2MM_LENGTH, Length);
    return 0;
}

int main()
{
	int map_fd=open("/dev/mem",O_RDWR);
	if(map_fd<0){
		printf("cannot open /dev/mem\n");
		return -1;
	}
	
    // if(InitReg(map_fd)!=0){
	// 	printf("InitReg failed\n");
	// 	return -1;
	// }

    // void *dma_addr_void=mmap(0,DMA_REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_DMA_DATA_BASEADDR));
	// if(dma_addr_void==NULL){
	// 	printf("mmap dma_addr failed! \n");
	// 	return -1;
	// }

    // unsigned int* dma_addr=(unsigned int*)dma_addr_void;

	// WriteToReg(0x00001002,0x3d,0x46);
	// ReadFromReg(0x00001002f0000000);

    // int64_t total_cnt = 8192;
    // int64_t per_cnt = 1024;
    // int64_t dma_mode = 0x03;

    // int64_t sa_reg_val =  (dma_mode << 55 ) | (per_cnt << 23) | total_cnt;
    // char recv_data[4096]={0};
    // WriteToReg(0x00001009,(sa_reg_val&0xFFFFFFFF00000000) >> 32,(sa_reg_val&0xFFFFFFFF));
    // ReadData_Device2DDR(dma_addr,(unsigned int)recv_data,1024);
    // for(int i=0;i<10;i++){
    //     printf("recv_data %d=%d\n",i,recv_data[i]);
    // }
	// if(munmap(dma_addr,REG_SIZE)==-1){
	// 	printf("munmap read_value_h_gpio error!\n");
	// 	return -1;
	// }
	// if(DestroyReg()!=0){
	// 	printf("DestroyReg failed\n");
	// 	return -1;
	// }

	// close(map_fd);

    void *dma_addr_void=mmap(0,DMA_REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_DMA_DATA_BASEADDR));
	if(dma_addr_void==NULL){
		printf("mmap dma_addr failed! \n");
		return -1;
	}

    unsigned int* dma_addr=(unsigned int*)dma_addr_void;
    char send_data[10]={1,2,3,4,5,6,7,8,9,0};
    char recv_data[10]={0};

    for(int i=0;i<10;i++){
        printf("send_data %d=%d\n",i,send_data[i]);
    }
    WriteData_DDR2Device(dma_addr,(unsigned int)send_data,10);
    ReadData_Device2DDR(dma_addr,(unsigned int)recv_data,10);
    
    for(int i=0;i<10;i++){
        printf("recv_data %d=%d\n",i,recv_data[i]);
    }
	if(munmap(dma_addr,REG_SIZE)==-1){
		printf("munmap read_value_h_gpio error!\n");
		return -1;
	}
    close(map_fd);
	return 0;

}

