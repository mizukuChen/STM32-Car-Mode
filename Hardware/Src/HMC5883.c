

#include "i2c.h"
#include "HMC5883.h"

#define	SlaveAddress   0x3C	  // HMC5883L�ĵ�ַ
//#define	SlaveAddress   0x1a	  //

unsigned char BUF[8];
short Bx,By,Bz;
float hyaw=0,hyawb=0,hyawa=0;
short offsetx=20,offsety=387,offsetz=-295;

void IIC_Write(unsigned char dev_addr, unsigned char mem_addr, 
								unsigned char data)//-selfwrite
{
	HAL_I2C_Mem_Write(&hmci2c, dev_addr, mem_addr,
						I2C_MEMADD_SIZE_8BIT, &data, 1, 2);
}

void IIC_Read(unsigned char dev_addr, unsigned char mem_addr, 
	unsigned char *buf, unsigned char len)
{
	HAL_I2C_Mem_Read(&hmci2c, dev_addr, mem_addr, 
					I2C_MEMADD_SIZE_8BIT, buf, len, 2);
}

void Single_Write_HMC5883(unsigned char  REG_Address, unsigned char  REG_data)
{
	IIC_Write(SlaveAddress, REG_Address, REG_data);
}//only read

unsigned char Single_Read_HMC5883(unsigned char REG_Address)
{  unsigned char  REG_data;

	IIC_Read(SlaveAddress, REG_Address, &REG_data, 1);
    return REG_data; 
}


//******************************************************
//
//��ȡ5883�Ĵų����ݣ��Ĵ�����ַ��03��ʼ��6�ֽ�
//
//******************************************************
void HMC5883_getdata(void){
		Multiple_Read_HMC5883(); 
	Bx = (short)(BUF[0] << 8) | BUF[1];
	Bz = (short)(BUF[2] << 8) | BUF[3];
  By = (short)(BUF[4] << 8) | BUF[5];

	}
void HMC5883_cyclecheck(void)
{

	printf("check start!");
    short xmax,xmin,ymax,ymin,zmax,zmin;
		uint16_t time=0;
	  xmin = 4096;
    xmax = -4096;
    ymin = 4096;
    ymax = -4096;
    zmin = 4096;
    zmax = -4096;
	for(time=0;time<40000;time++){
	
		HMC5883_getdata();
		   if ((Bx> - 2048) && (Bx< 2048))
    {
        if (Bx > xmax)
        {
            xmax = Bx;
        }
        if (Bx < xmin)
        {
            xmin = Bx;
        }    
    }
     if ((By> - 2048) && (By< 2048))
    {
        if (By > ymax)
        {
            ymax = By;
        }
        if (By < ymin)
        {
            ymin = By;
        }    
    }
		    if ((Bz> - 2048) && (Bz< 2048))
    {
        if (Bz > zmax)
        {
            zmax = Bz;
        }
        if (Bz < zmin)
        {
            zmin = Bz;
        }    
    }
		Delay_Ms(1);
	}
	offsetx=(xmax+xmin)/2;
	offsety=(ymax+ymin)/2;
	offsetz=(zmax+zmin)/2;
  printf("check ending\n");  
	
}
void Multiple_Read_HMC5883(void)
{   
	IIC_Read(SlaveAddress, 0x03, BUF, 6);
}


/*
// ��ʼ��5883
 00��Configuration Register A  
 01��Configuration Register B
 02��Mode Register 
 09��Status Register 
*/
void Init_HMC5883()
{
	
	Single_Write_HMC5883(0x00,				//00��Configuration Register A  
												(3<<5)|			//Ĭ�ϣ�8��ƽ��ֵ�˲�
												(1<<4)|			//Ĭ�ϣ�15Hz
												(0)					//00�������ԣ�01�ڲ���������10�ڲ�������
											);  
	
	
  Single_Write_HMC5883(0x01,
												(1<<5)|		//�������ã�Ĭ��+-1.3Ga
												0					//����Ϊ0
											);  

	Single_Write_HMC5883(0x02,0x00);  // 00����ת��ģʽ��01���Σ�����idle
	
	
	//Single_Write_HMC5883(0x00,0x10|(0));  //	
	//Single_Write_HMC5883(0x01,0x40);  // 
	//Single_Write_HMC5883(0x02,0x00);
}





/*
�Լ�ģʽ�������ڲ�����������ת��
������Ĳ���ֵ����ԼΪ��
x��951
y��951
z��886
�Լ���ɺ���������Ϊ��������ģʽ������ת��
*/
unsigned char Init_HMC5883_SelfTestMode()//selfcheck
{
	Single_Write_HMC5883(0x00,0x70|(1));  //	�ڲ�����
	HAL_Delay(10);
	//Single_Write_HMC5883(0x01,0x40);  // Ĭ������
  Single_Write_HMC5883(0x02,0x01);  // �ϵ�Ĭ���ǵ���ת��
	
	HAL_Delay(2000);
	
	Multiple_Read_HMC5883();
	
	Bx = (short)(BUF[0] << 8) | BUF[1]; //Combine MSB and LSB of X Data output register
  Bz = (short)(BUF[2] << 8) | BUF[3]; //Combine MSB and LSB of Z Data output register
  By = (short)(BUF[4] << 8) | BUF[5]; //Combine MSB and LSB of Y Data output register
	
	Single_Write_HMC5883(0x00,0x70|(0)); 
	Single_Write_HMC5883(0x02,0x00);	
	
	return 0;
}
void HMC5883_getyaw(void){
   float dy;
	HMC5883_getdata();
	Bx=Bx-offsetx;
	By=By-offsety;
//	Bz=Bz-offsetz;
	hyawa= -atan2((double)By,(double)Bx) * 57.296; 
	if(hyawa-hyawb<-180)dy=hyawa-hyawb+360;
	else if(hyawa-hyawb>180)dy=hyawa-hyawb-360;
	else dy=hyawa-hyawb;
	hyaw=hyaw+dy;
	hyawb=hyawa;
  hyawb=hyawb;
	}
void HMC5883_first(){
	HMC5883_getdata();
	Bx=Bx-offsetx;
	By=By-offsety;
//	Bz=Bz-offsetz;
	hyawb= -atan2((double)By,(double)Bx) * 57.296;
	hyawa=hyawb;
	

}

