/***************************************/
/* Author	: Naveenkumar N        */
/* Date		:  -Dec-2020           */
/* Filename	: HMC5883L.h           */
/* Description	: Header files         */
/***************************************/







#ifndef HMC5883L_H
#define HMC5883L_H


/*-----------INCLUDE HEADER FILES--------------*/
#include <linux/types.h>
#include <sys/ioctl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>


/* ---------------MACRO DEFINITIONS----------- */
#define I2C_SLAVE 	0x0703
#define I2C_SMBUS    	0x0720   

#define I2C_SMBUS_READ	1
#define I2C_SMBUS_WRITE	0

#define I2C_SMBUS_BYTE_DATA 	2
#define I2C_SMBUS_BLOCK_MAX	32    


/* ---------------FUNCTION PROTO-TYPES--------- */

/*
 * __u8;  unsigned byte (8 bits) 
 * __u16; unsigned word (16 bits) 
 * __u32; unsigned 32-bit value 
 * __u64; unsigned 64-bit value 
*/




struct i2c_smbus_ioctl_data {
    __u8 read_write;
    __u8 command;
    __u32 size;
    union i2c_smbus_data *data;
};


union i2c_smbus_data {
    __u8 byte;
    __u16 word;
    __u8 block[I2C_SMBUS_BLOCK_MAX + 2]; /* block[0] is used for length */
                                                /* and one more for PEC */
};



int fun_read(int reg);
static inline __s32 i2c_smbus_read_byte_data(int fd, __u8 command);
static inline __s32 i2c_smbus_access(int fd, char read_write, __u8 command, int size, union i2c_smbus_data *data);

#endif //HMC5883L_h
