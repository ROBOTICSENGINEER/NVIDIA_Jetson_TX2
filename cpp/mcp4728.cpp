#include <stdint.h>
#include <cmath>        // std::pow

#include <iostream>
#include <cstdint>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#include <linux/types.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#define PI 3.14159265

int file;
char filename[20] = "/dev/i2c-0";
int addr = 0x62; /* The I2C address */
int opening_slave;



int main(int argc, char **argv)
{
  
  
  file = open(filename, O_RDWR);
  if (file < 0) {
    std::cout<<"error: cannot open filename\n";
    return -1;
  }

  opening_slave=ioctl(file, I2C_SLAVE_FORCE, addr);
  if (opening_slave < 0) {
    std::cout<<opening_slave;
    std::cout<<"\nerror: cannot open address\n";
    return -2;
  }



  int N=1000;
  for(int i=0; i<N; i++){
    int q = floor(4095*(1-cos(2*i*PI/N))/2);
    uint8_t H = (uint8_t)floor(q/16);
    uint8_t L = (uint8_t)((q-16*H)*16);
    uint8_t buf[4];
    buf[0] = 96;
    buf[1] = H;
    buf[2] = L;
    write(file, buf, 3);
    //i2c_smbus_write_word_data(file, 96, q);
    std::cout<<"H: "<<(int)H<<" L: "<<(int)L<<" Vout "<<((double)H*16+(double)L)*3.3/4095<<std::endl;
    usleep(10000);
  }


  std::cout<<"\n";
  return 0;
}
