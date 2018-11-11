#define MY_REGISTER (*(volatile uint16_t*)0x12345678u)
#include <stdint.h>

void main(void){
	volatile int i;
	i = 0x40;
	while(1 == 1){
		int temp;
		i++;
		temp = (*(unsigned char*)0x4210);
		temp = temp & 0x00F0;
		*(unsigned char*)0x212C = 0x00;
		*(unsigned char*)0x2100 = 0x0F;
		*(unsigned char*)0x2121 = 0x00;
		*(unsigned char*)0x2122 = 0xFF;
		*(unsigned char*)0x2122 = 0x7F;
		
	}
}

void far IRQHandler(void){
	
}