#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pindef.h"


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
	read_value_h_gpio=mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_READVALUE_H_BASEADDR));
	if(read_value_h_gpio==NULL){
		printf("mmap read_value_h_gpio failed! \n");
		return -1;
	}		

	read_value_l_gpio=mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_READVALUE_L_BASEADDR));
	if(read_value_l_gpio==NULL){
		printf("mmap read_value_l_gpio failed! \n");
		return -1;
	}

	read_ready_gpio=mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_READREADY_BASEADDR));
	if(read_ready_gpio==NULL){
		printf("mmap read_ready_gpio failed! \n");
		return -1;
	}

	read_valid_gpio=mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_READVALID_BASEADDR));
	if(read_valid_gpio==NULL){
		printf("mmap read_valid_gpio failed! \n");
		return -1;
	}

	write_ready_gpio=mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_WRITEREADY_BASEADDR));
	if(write_ready_gpio==NULL){
		printf("mmap write_ready_gpio failed! \n");
		return -1;
	}

	write_valid_gpio=mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_WRITEVALID_BASEADDR));
	if(write_valid_gpio==NULL){
		printf("mmap write_valid_gpio failed! \n");
		return -1;
	}

	write_value_h_gpio=mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_WRITEVALUE_H_BASEADDR));
	if(write_value_h_gpio==NULL){
		printf("mmap write_value_h_gpio failed! \n");
		return -1;
	}

	write_value_l_gpio=mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_WRITEVALUE_L_BASEADDR));
	if(write_value_l_gpio==NULL){
		printf("mmap write_value_l_gpio failed! \n");
		return -1;
	}

	address_gpio=mmap(0,REG_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,map_fd,(AXI_GPIO_ADDRESS_BASEADDR));
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


int main()
{
	int map_fd=open("/dev/mem",O_RDWR);
	if(map_fd<0){
		printf("cannot open /dev/mem\n");
		return -1;
	}
	if(InitReg(map_fd)!=0){
		printf("InitReg failed\n");
		return -1;
	}
	close(map_fd);
	
	WriteToReg(0x00001002,0x01,0x03);
	ReadFromReg(0x00001002f0000000);

	WriteToReg(0x00001003,0x45,0x2e);
	ReadFromReg(0x00001003f0000000);


	WriteToReg(0x00001004,0x3d,0x0f);
	ReadFromReg(0x00001004f0000000);

	WriteToReg(0x00001005,0x2a345678,0x2e45673d);
	ReadFromReg(0x00001005f0000000);

	WriteToReg(0x00001005,0x2a345678,0x2e45673d);
	ReadFromReg(0x00001005f0010000);

	if(DestroyReg()!=0){
		printf("DestroyReg failed\n");
		return -1;
	}

	
	return 0;
}
