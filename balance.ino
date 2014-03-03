#include <SD.h>
#include <Wire.h>

/*

	//6dof
	SCL -> a5
	SDA -> a4d
	
	//SD-card
	SS -> d10
	mosi -> d11
	miso ->  d12
	sck -> d13

gyro_scale = 14.375;
acc_scale = 128;

*/

#define pln(x) Serial.println(x)
#define p(x) Serial.print(x)



enum {
	//Gyro Stuff
	gyro_dev = (0xD0>>1), //Removing the r/w bit

	dlpf_cfg_0 = 1<<0,
	dlpf_cfg_1 = 1<<1,
	dlpf_cfg_2 = 1<<2,
	dlpf_fs_sel_0 = 1<<3,
	dlpf_fs_sel_1 = 1<<4,
	
	
	gyro_xout_h = 0x1D,
	gyro_xout_l = 0x1E,
	gyro_yout_h = 0x1F,
	gyro_yout_l = 0x20,
	gyro_zout_h = 0x21,
	gyro_zout_l = 0x22,
	
	temp_out_h = 0x1B,
	temp_out_l = 0x1C,
	
	who_am_i = 0x00,
	smplrt_div = 0x15,
	dlpf_fs = 0x16,
	int_cfg = 0x17,
	int_status = 0x1A,
	pwr_mgm = 0x3E,
	
	//Accelerometer Stuff
	acc_dev = (0xA6>>1), //Removing  the r/w bit
	acc_scale = 128,	//Depends on the range and resolution... And temperature
	
	power_ctl = 0x2D,
	data_format = 0x31,
	
	datax0 = 0x32,
	datax1 = 0x33,
	datay0 = 0x34,
	datay1 = 0x35,
	dataz0 = 0x36,
	dataz1 = 0x37,
	
	acc_measure = 1<<3,
	acc_full_res = 1<<3,
	acc_range_2g = 0,
	acc_range_4g = 1,
	acc_range_8g = 1<<1,
	acc_range_16g = (1<<0 | 1<<1),
	
	warn_light = 8,
	warn_time = 2000,
	measure_light = 9,
	
	ss = 10,	//Sd card setting
	logtime = 8*1000,
	
};

int gyro_x_bias = 0;
int gyro_y_bias = 0;
int gyro_z_bias = 0;

int acc_x_bias = 0;
int acc_y_bias = 0;
int acc_z_bias = 0;

unsigned long start_time;
File logfile;

struct Data {
	int x;
	int y;
	int z;
};
typedef struct Data Data;

void setup() 
{

	
	Serial.begin(9600);
	Wire.begin();
        delay(100);
	//We need the 6dof to be placed correctly with no movement on it, lets give a warning.
	//digitalWrite(warn_light, HIGH);
	//delay(warn_time);
	
	//Initialize the 6dof
        pln("Before Acc init");
        delay(100);
	initAcc(100);
        pln("Acc init done, starting gyro init");
        delay(100);
       	initGyro(100);
        pln("Gyro init done");
	delay(100);
	/*
	logfile = initSD();
	if(!logfile)
	{
		delay(200); 	//So the arduino has time to empty the print buffer
		exit(1);
	}
        */

        /*
	digitalWrite(warn_light, LOW);	//Non-movement phase is over
	digitalWrite(measure_light, HIGH);	//We're now measuring
        */
	
	start_time = millis();
}


void loop()
{
	Data gyro,acc;
	gyro = gyroData();
	acc = accData();
	
        logPrint(acc,gyro);
        
	if((millis()-start_time) > logtime)
	{
		pln("logging done");
		//logfile.close();
		//digitalWrite(measure_light, LOW);
		delay(200); //So the arduino has time to empty the print buffer
		exit(0);
	}
	
	
	//Max frequency for the Wire library is 100hz
	delay(10);
}

struct Data accData(void)
{
	byte g[6];
	struct Data d;
	
	i2cReadBytes(acc_dev, datax0, 6, g);
	
	d.x = (((int)g[1])<<8) | (int) g[0];
	d.y = (((int)g[3])<<8) | (int) g[2];
	d.z = (((int)g[5])<<8) | (int) g[4];
	
	d.x -= acc_x_bias;
	d.y -= acc_y_bias;
	d.z -= acc_z_bias;
	
	return d;
}

struct Data gyroData(void)
{
	byte x[2], y[2], z[2];
	struct Data d;
	
	i2cReadBytes(gyro_dev, gyro_xout_h, 2, x);
	i2cReadBytes(gyro_dev, gyro_yout_h, 2, y);
	i2cReadBytes(gyro_dev, gyro_zout_h, 2, z);
	
	d.x = (((int)x[0])<<8) | (int) x[1];
	d.y = (((int)y[0])<<8) | (int) y[1];
	d.z = (((int)z[0])<<8) | (int) z[1];
	
	d.x -= gyro_x_bias;
	d.y -= gyro_y_bias;
	d.z -= gyro_z_bias;
	
	return d;
}

void i2cReadBytes(char device, char reg, int nrBytes, byte *buff)
{
	int i;

	Wire.beginTransmission(device);
	Wire.write(reg);
	Wire.endTransmission(device);
	Wire.requestFrom(device, nrBytes);
	
	for(i=0; i<nrBytes; i++) {
		if(Wire.available()) 
		{
               		buff[i] = Wire.read();
		} else {
			pln("ERROR READING i2cReadBytes");
                        delay(100);
			exit(3);
		}
	}
}

void i2cWrite(char device, char reg, char data)
{
	Wire.beginTransmission(device);
	Wire.write(reg); 	//The register we wanna write to
	Wire.write(data);
	Wire.endTransmission();
}

void initGyro(int mean)
{
	//+/- 2000 degrees per sec. Internal sample rate 1kHz, low pass filter 188Hz
	i2cWrite(gyro_dev, dlpf_fs, (dlpf_fs_sel_0 | dlpf_fs_sel_1 | dlpf_cfg_0));
	i2cWrite(gyro_dev, smplrt_div, 9);	//sample rate divider 1kHz/10 = 100Hz
	
	setGyroBias(mean);
}


void setGyroBias(int mean)
{
	int x,y,z, i;
	struct Data gyro;
	x = y = z = 0;
	
	for(i=0; i<mean; i++)
	{
		gyro = gyroData();
		x += gyro.x;
		y += gyro.y;
		z += gyro.z;
		delay(10);
	}
	gyro_x_bias = x/mean;
	gyro_y_bias = y/mean;
	gyro_z_bias = z/mean;
}

void initAcc(int mean)
{
	i2cWrite(acc_dev, data_format, 0x01 );	//Set data_format, remember to set acc_scale accordingly
	i2cWrite(acc_dev, power_ctl, 0x08);		//We must tell the acc to start measuring
	
	setAccBias(mean);
}

void setAccBias(int mean)
{
	int x,y,z, i;
	struct Data acc;
	x = y = z = 0;
	
	for(i=0; i<mean; i++)
	{
		acc = accData();
		x += acc.x;
		y += acc.y;
		z += (acc.z - acc_scale);
		delay(10);
	}
	acc_x_bias = x/mean;
	acc_y_bias = y/mean;
	acc_z_bias = z/mean;
}

File initSD() 
{
	File root, entry, logfile;

	if(!SD.begin(ss))	//Check that the SD-card works
		return(File());

	root = SD.open("/", FILE_READ);
	for(;;)
	{
		entry = root.openNextFile();
		if(!entry)
			break;

		if(!entry.isDirectory()) //ignores directories on the sdcard
			SD.remove(entry.name());
	}
	logfile = SD.open("/log.txt", FILE_WRITE);
	return logfile;
}

void filePrint(Data acc, Data gyro)
{
        logfile.print(acc.x); logfile.print("\t"); logfile.print(acc.y); logfile.print("\t");logfile.print(acc.z);
	logfile.print("\t\t");
	logfile.print(gyro.x); logfile.print("\t"); logfile.print(gyro.y); logfile.print("\t");logfile.print(gyro.z);
	logfile.print("\t\t");
	logfile.println((millis()-start_time));
}

void logPrint(Data acc,Data gyro)
{
        p(acc.x); p("\t"); p(acc.y); p("\t");pl(acc.z);
	p("\t\t");
        p(gyro.x); p("\t"); p(gyro.y); p("\t");pl(gyro.z);
	p("\t\t");
	pln((millis()-start_time));
}

