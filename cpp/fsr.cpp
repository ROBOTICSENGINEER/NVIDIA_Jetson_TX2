#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <iostream> 
#include <fstream>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


uint8_t command = 0x04; // 0x04 = 0b 0000 0100

uint8_t default_rx[ARRAY_SIZE(&command)] = {0, };

static const char *device = "/dev/spidev3.0";
static uint32_t mode;
static uint8_t bits = 8;
static uint32_t speed = 100000;
static uint16_t delay;





static void hex_dump(unsigned char *src, size_t length, size_t line_size, char *prefix)
{
	int i = 0;
	unsigned char *address = src;
	unsigned char *line = address;
	unsigned char c;

	printf("%s | ", prefix);
	while (length-- > 0) {
		printf("%02X ", *address++);
		if (!(++i % line_size) || (length == 0 && i % line_size)) {
			if (length == 0) {
				while (i++ % line_size)
					printf("__ ");
			}
			printf(" | ");  /* right close */
			while (line < address) {
				c = *line++;
				printf("%c", (c < 33 || c == 255) ? 0x2E : c);
			}
			printf("\n");
			if (length > 0)
				printf("%s | ", prefix);
		}
	}
}



static void transfer(int fd, uint8_t  *tx, uint8_t *rx, size_t len)
{
	int ret;

	struct spi_ioc_transfer tr;
	tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = len;
	tr.delay_usecs = delay;
	tr.speed_hz = speed;
	tr.bits_per_word = bits;


	if (mode & SPI_TX_QUAD)
		tr.tx_nbits = 4;
	else if (mode & SPI_TX_DUAL)
		tr.tx_nbits = 2;
	if (mode & SPI_RX_QUAD)
		tr.rx_nbits = 4;
	else if (mode & SPI_RX_DUAL)
		tr.rx_nbits = 2;
	if (!(mode & SPI_LOOP)) {
		if (mode & (SPI_TX_QUAD | SPI_TX_DUAL))
			tr.rx_buf = 0;
		else if (mode & (SPI_RX_QUAD | SPI_RX_DUAL))
			tr.tx_buf = 0;
	}

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
{
	perror("can't send spi message");
	abort();
}

	/*if (verbose)
		hex_dump(tx, len, 32, "TX");

*/
	//hex_dump(rx, len, 32, "RX");
	std::cout << (int)rx[0] << "\n";

}






int main(int argc, char *argv[])
{
	int ret = 0;
	int fd;
	uint8_t *tx;
	uint8_t *rx;
	int size;
	
	char number[4]={4,4,4};

fd = open(device, O_RDWR);

if (fd < 0)
{
	perror("can't open device");
	abort();
}



	/*  bits per word */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
{
	perror("can't set bits per word");
	abort();
}
	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
{
	perror("can't get bits per word");
	abort();
}



	/* max speed hz */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
{
	perror("can't set max speed hz");
	abort();
}
	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
{
	perror("can't get max speed hz");
	abort();
}



        /* Display result */
	printf("spi mode: 0x%x\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);


	std::ofstream fsr_file;
	fsr_file.open ("force.txt");


	if (fsr_file.is_open())
	  {
	    std::cout << "File is opened!\n";
	  }
	  else
	  {
	    std::cout << "Error opening file\n";
	    return -11;
	  }
	
	transfer(fd, &command, default_rx, sizeof(command));
	usleep(100000);
	std::cout << "\n\nstart\n\n";
	/* transfer */
	for(int kk=0;kk<100;kk++)
	{
	std::cout << kk << "\t";
	transfer(fd, &command, default_rx, sizeof(command));
	sprintf (number, "%d", ((int)default_rx[0]));
	fsr_file << number << "\n";
	usleep(100000);
	}

	close(fd);
	fsr_file.close();

	return ret;
}



