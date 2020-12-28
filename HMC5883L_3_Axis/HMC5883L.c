/***************************************/
/* Author       : Naveenkumar N        */
/* Date         :  -Dec-2020           */
/* Filename     : HMC5883L.c           */
/* Description  : Source files         */
/***************************************/

#include "HMC5883L.h"



int main() {
    int data1 = 0,data2 = 0,data3 = 0,data4 =0,data5 =0,data6 =0;

    data1 = fun_read(0x03);/* Reading from data Output X MSB Register from HMC5883L */
    data2 = fun_read(0x04);/* Reading from data Output X LSB Register from HMC5883L */	
    data3 = fun_read(0x05);/* Reading from data Output Z MSB Register from HMC5883L */
    data4 = fun_read(0x06);/* Reading from data Output Z LSB Register from HMC5883L */
    data5 = fun_read(0x07);/* Reading from data Output Y MSB Register from HMC5883L */
    data6 = fun_read(0x08);/* Reading from data Output Y LSB Register from HMC5883L */



    data1 = (data1 << 8) |data2;/* Adding MSB and LSB for X asix */
    if(data1 >32768){
	    data1 = data1 - 65536;
    }
    data3 = (data3 <<8) |data4;/* Adding MSB and LSB for Z asix */
    if(data3 >32768){
	    data3 = data3 - 65536;
    }
    data5 = (data5 <<8) |data6;/* Adding MSB and LSB for Y asix */
    if(data5 >32768){
	   data5 = data5 - 65536;
    }

    printf("X axis data = %d\n",data1);
    printf("Y axis data = %d\n",data5);
    printf("Z axis data = %d\n",data3);
 
}


int fun_read(int reg){
	int data1;

	int  fd = open("/dev/i2c-1", O_RDWR);/* Device numbers are reserved for i2c /dev/i2c-n */
	perror("open");

	int addr = 0x1E; /* This slave device address */
	ioctl(fd, I2C_SLAVE, addr);/* Handling I/O operations of a device i2c /dev/i2c-n */
	perror("ioctl");

	int result = i2c_smbus_read_byte_data(fd, reg);
    if (result < 0) {
         printf("Error in i2c_smbus_read_byte_data()");
        exit (0);
    } else {
        printf("Register Address = 0X%02x  Data =  0x%02x\n", reg,result);
	data1 = result;
    }
    return data1;
}



static inline __s32 i2c_smbus_read_byte_data(int fd, __u8 command)
{
    union i2c_smbus_data data;
    if (i2c_smbus_access(fd,I2C_SMBUS_READ,command,I2C_SMBUS_BYTE_DATA,&data))
        return -1;
    else
        return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_access(int fd, char read_write, __u8 command, int size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data args;

    args.read_write = read_write;
    args.command = command;
    args.size = size;
    args.data = data;
    return ioctl(fd,I2C_SMBUS,&args);
}
