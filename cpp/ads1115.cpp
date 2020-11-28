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


int file;
char filename[20] = "/dev/i2c-1";
int addr = 0x48; /* The I2C address */
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


  //i2c_smbus_write_byte_data(file, 0, 0x10); // sleep



  //fwrite(i2c_obj, [1 244 131]); % write config =1 & chanel 3 + FSE 2v + continoues & 128hz + b00011
  uint8_t buf[3];
  buf[0]=1;
  buf[1]=244;
  buf[2]=131;
  write(file, buf, 3);
  usleep(10000);
  //fwrite(i2c_obj, 0); % change the pointer to measurment buffer
  //i2c_smbus_write_byte_data(file, 0, 0);
  uint8_t zero =0;

  write(file, &zero, 1);
  usleep(10000);

  uint8_t raw_bipolar[2] ={0,0};
  for(int i=0; i<100; i++){
    //i2c_smbus_read_word_data(file, raw_bipolar);
    read(file, raw_bipolar, 2);//16 bit binary split into high/low

    int raw = (256*raw_bipolar[0]) + raw_bipolar[1];//16 bit binary number
    
    
    if(raw_bipolar[0]>127){
       raw = raw&&32767;
       raw = 1-raw;
    }
    std::cout<<"newNum "<<raw<<std::endl;

    
    //std::cout<<"raw "<<(int)raw_bipolar[0]<<" "<<(int)raw_bipolar[1]<<std::endl;
    //pause(0.5)
    usleep(1000000);
  }



/*
//////////////////////////////////////////////////
  int channels[5] = {11,12,9,10,8};
  float num=0;
  /*
  set_pwm(11, 204+(num*205));
  usleep(1000000);
  set_pwm(12, 204+(num*205));
  usleep(1000000);
  set_pwm(9, 204+(num*205));
  usleep(1000000);
  set_pwm(10, 204+(num*205));
  usleep(1000000);
  set_pwm(8, 204+(num*205));
  usleep(1000000);
  //
  for(int i=0; i<6; i++){
    num +=.2*i;
    for(int j=0; j<5; j++){
      usleep(100000);
      set_pwm(channels[j], 204+(num*205));
      std::cout<<channels[j]<<" "<<num<<"\n";
    }
    usleep(1000000);
  }

  for(int i=0; i<6; i++){
    num -=.2*i;
    for(int j=0; j<5; j++){
      usleep(100000);
      set_pwm(channels[j], 204+(num*205));
      std::cout<<channels[j]<<" "<<num<<"\n";
    }
    usleep(1000000);
  }
  //num = 204+(num*205);
  //if(num>409) num=409;
  //set_pwm(channel, num);
/////////////////////////////////////////////////
*/


  usleep(1000000);

  std::cout<<"\n";
  return 0;
}
