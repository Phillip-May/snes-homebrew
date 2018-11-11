#define MY_REGISTER (*(volatile uint16_t*)0x12345678u)
#include <stdint.h>

void main(void){
	volatile int i;
	i = 0x40;
	while(1 == 1){
		unsigned char temp;
		i++;
		temp = (*(unsigned char*)0x4210);
		temp = (temp & 0x80);
		if (temp == 0x80){
			(*(unsigned char *)0x00212C) = 0x00;
			(*(unsigned char *)0x002100) = 0x0F;
			(*(unsigned char *)0x002121) = 0x00;
			(*(unsigned char *)0x002122) = 0xFF;
			(*(unsigned char *)0x002122) = 0x7F;
		} 
		
	}
}

void far IRQHandler(void){
	
}

#define PortA (*(unsigned char *)0xfff002) 