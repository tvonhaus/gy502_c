#include <stdio.h>
#include <i2c/smbus.h>
#include <stdlib.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <fcntl.h>

typedef __u8 u8;
typedef __u16 u16;
typedef __s16 s16;
typedef __s32 s32;

int open_bus(int bus_number,char* filename){
	snprintf(filename,19,"/dev/i2c-%d",bus_number);
	int file = open(filename,O_RDWR);
	if (file < 0){
		printf("Could not open bus %d\n",bus_number);
	}
	return file;
}

void reset_device(int bus_file){
	__u8 pwr_management1 = 0x6B;
	i2c_smbus_write_byte_data(bus_file,pwr_management1,0x80);
}

void remove_from_sleep(int bus_file){
	__u8 pwr_management1 = 0x6B;
	i2c_smbus_write_byte_data(bus_file,0x6B,0x00);
}

int is_sleep(int bus_file){
	__u8 pwr_management1 = 0x6B;
	if(i2c_smbus_read_byte_data(bus_file,0x6B) == 40){
		return 0;
	}
	return 1;
}

__s32 who_am_i(int bus_file,__u8 registerNum){
	__s32 result = i2c_smbus_read_byte_data(bus_file,registerNum);
	printf("I am bus %.02x\n",result);
	return result;
}

float calculate_temperature(s16 combined_temperature){
	float temperature = ((float)(combined_temperature/340)) + 36.53f;
	return temperature;
}

float get_temperature(int bus_file, u8 register_H, u8 register_L){
	u16 temperature_H = i2c_smbus_read_byte_data(bus_file,register_H);
	u16 temperature_L = i2c_smbus_read_byte_data(bus_file,register_L);
	s16 combined_hex  = (s16)((temperature_H) << 8) | (temperature_L);
	float temperature = calculate_temperature(combined_hex);
	printf("Temp: %f\n",temperature);
	return temperature;
}

void get_gyro_data(int bus_file, u8 reg_x_h, u8 reg_x_l, u8 reg_y_h, u8 reg_y_l, u8 reg_z_h, u8 reg_z_l){
	u16 gyro_x_h = i2c_smbus_read_byte_data(bus_file, reg_x_h);
	u16 gyro_x_l = i2c_smbus_read_byte_data(bus_file, reg_x_l);
	u16 gyro_y_h = i2c_smbus_read_byte_data(bus_file, reg_y_h);
	u16 gyro_y_l = i2c_smbus_read_byte_data(bus_file, reg_y_l);
	u16 gyro_z_h = i2c_smbus_read_byte_data(bus_file, reg_z_h);
	u16 gyro_z_l = i2c_smbus_read_byte_data(bus_file, reg_z_l);

	s16 combined_x = (s16)(gyro_x_h << 8) | (gyro_x_l);
	s16 combined_y = (s16)(gyro_y_h << 8) | (gyro_y_l);
	s16 combined_z = (s16)(gyro_z_h << 8) | (gyro_z_l);
	
	printf("Gyro:  X: %d Y: %d Z: %d\n",combined_x, combined_y, combined_z);
}

void get_accel_data(int bus_file, u8 reg_x_h, u8 reg_x_l, u8 reg_y_h, u8 reg_y_l, u8 reg_z_h, u8 reg_z_l){
	u16 accel_x_h = i2c_smbus_read_byte_data(bus_file, reg_x_h);
	u16 accel_x_l = i2c_smbus_read_byte_data(bus_file, reg_x_l);
	u16 accel_y_h = i2c_smbus_read_byte_data(bus_file, reg_y_h);
	u16 accel_y_l = i2c_smbus_read_byte_data(bus_file, reg_y_l);
	u16 accel_z_h = i2c_smbus_read_byte_data(bus_file, reg_z_h);
	u16 accel_z_l = i2c_smbus_read_byte_data(bus_file, reg_z_l);

	s16 combined_x = (s16)(accel_x_h << 8) | (accel_x_l);
	s16 combined_y = (s16)(accel_y_h << 8) | (accel_y_l);
	s16 combined_z = (s16)(accel_z_h << 8) | (accel_z_l);
	
	printf("Accel:  X: %d Y: %d Z: %d\n",combined_x, combined_y, combined_z);
}

int main(int argc,char** argv){
	int bus_number = 1;
	char filename[20];
	int bus_file = open_bus(bus_number,filename);
	int addr = 0x68;
	int loop_count = atoi(argv[1]);

	/* Setup */

	if (ioctl(bus_file, I2C_SLAVE,addr) < 0){
		printf("Error");	
	}
	
	/* Registers */

	u8 whoamiRegister = 0x75;
	u8 fifo_en = 0x23;
	u8 pwr_management1 = 0x6B;
	/* Temperature Registers */
	u8 temp_h = 0x41;
	u8 temp_l = 0x42;
	
	/* Gyroscope Registers */
	u8 gyro_x_h = 0x43;
	u8 gyro_x_l = 0x44;
	u8 gyro_y_h = 0x45;
	u8 gyro_y_l = 0x46;
	u8 gyro_z_h = 0x47;
	u8 gyro_z_l = 0x48;

	/* Accelerometer Registers */
	u8 accel_x_h = 0x3B;
	u8 accel_x_l = 0x3C;
	u8 accel_y_h = 0x3D;
	u8 accel_y_l = 0x3E;
	u8 accel_z_h = 0x3F;
	u8 accel_z_l = 0x40;


	s32 res = who_am_i(bus_file, whoamiRegister); 

	remove_from_sleep(bus_file);

	if(loop_count == NULL){
		loop_count = 10;
	}


	for(int i =0; i < loop_count; i++){
		get_temperature(bus_file, temp_h,temp_l);
		get_gyro_data(bus_file,gyro_x_h, gyro_x_l, gyro_y_h, gyro_y_l, gyro_z_h, gyro_z_l);
		get_accel_data(bus_file,accel_x_h, accel_x_l, accel_y_h, accel_y_l, accel_z_h, accel_z_l);
		sleep(1);

	}
	return 0;
}

