

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define  LED_ALL_OFF  0x00
#define  LED1_ON      0x11
#define  LED1_OFF     0x10
#define  LED2_ON      0x21
#define  LED2_OFF     0x20


void  led_write(unsigned char status);

void  led1_on()
{
	led_write(LED1_ON);
}

void  led1_off()
{
	led_write(LED1_OFF);
}

void  led2_on()
{
	led_write(LED2_ON);
}

void  led2_off()
{
	led_write(LED2_OFF);
}

/*
led_write(1)
led_write(0)
*/
void  led_write(unsigned char status)
{
	int fd;
	int val = 1;
	
	fd = open("/dev/xyz", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}

	if (status == LED1_ON){
		val  = 0x11;
	}
	else if(status == LED1_OFF)	{
		val = 0x10;
	}
	else if(status == LED2_ON)	{
		val = 0x21;
	}
	else if(status == LED2_OFF)	{
		val = 0x20;
	}
	else{}
	
	write(fd, &val, 4);
	close(fd);
	//return 0;
}


int main(int argc, char **argv)
{
	while(1)
	{
		printf("open led1\n");
		led_write(LED1_ON);   sleep(1);

		printf("open led2\n");		
		led_write(LED2_ON);   sleep(1);	
	
		printf("close led1 led2\n");	
		led_write(LED1_OFF);
		led_write(LED2_OFF);  sleep(1);
	}

	return 0;
}




