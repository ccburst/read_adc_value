#include <Wire.h>
#define Addr 0x50 //  I2C address
int data;

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  Serial.println("start!");

  REG_write(0x11, 0x01);
  REG_write(0x00, 0x00);
  
  // 0x01 0x02 為PGA暫存器 用於差分輸入信號進行信號放大及∆𝐷𝐼±控制
  REG_write(0x01, 0x00); // VREFGN=1 , ADGN=1 , PGAGN=1
  REG_write(0x02, 0x00); // DCSET=+0V
 
  REG_write(0x03, 0x00); // 選擇通道 CH1為反相輸入端 CH0為正相輸入端
  REG_write(0x07, 0x06); // VREFS=1 調整為外部參考電壓比較
  REG_write(0x09, 0x1f);
  REG_write(0x08, 0x00);
  
}

void loop()
{
  delay(1000);

  Serial.println("檢測EOC"); 
  do{
      
    REG_read(0x08);
    Serial.print("0x08="); 
    Serial.println(data); 

    delay(1000);
    
  }  while((data&2)==0);  // 檢測ADCR1中EOC位
  
  Serial.println("EOC為1"); 

  Read_AD_Value();
  
}

void REG_write(int a,int b){

  Wire.beginTransmission(Addr);
  Wire.write(a);  // 進入 暫存器
  Wire.write(b);  // 寫入控制內容
  //Serial.println("write sucess");

  Wire.endTransmission();  // Stop I2C Transmission

}

void REG_read(int a){
  
  Wire.beginTransmission(Addr);
  Wire.write(a);  // 進入 暫存器

  Wire.endTransmission(0);  // Stop I2C Transmission
  
  Wire.requestFrom(Addr, 1);

  if (Wire.available())
  {
    data = Wire.read();
    //Serial.println(data);   //顯示暫存器的內部狀態
  }
  
  
}

void Read_AD_Value(void) 
{
  unsigned char AD_L,AD_M,AD_H;
  unsigned long value;
  float LSB,SI_I,VR_I,ADC_data,ADC_value,K;
  
  REG_read(0x04);     AD_L=data;
  REG_read(0x05);     AD_M=data;
  REG_read(0x06);     AD_H=data;
  REG_write(0x08, 0x00);
  
  value=AD_H;
  value=value<<8;
  value=value+AD_M;  
  value=value<<8;
  value=value+AD_L;
  
  ADC_value=(float)value*(3.3/8388608);
 
  VR_I=1*3.3;
  LSB=VR_I/8388608;
  SI_I=(64*2*3.3)+0;
  K=1 << 23;
  ADC_data=(SI_I/VR_I)*K;

  if(value >> 23 ==0){
    Serial.println("正數");
    Serial.println(value);
    ADC_value=(float)value*(3.3/8388608);
    Serial.print("ADC_value=");
    Serial.println(ADC_value);
  }
  else{
    Serial.println("負數");
    ADC_value=( (value^0x00FFFFFF) +1);
    Serial.println(value);
    Serial.println(ADC_value);
    ADC_value=(float)ADC_value*(3.3/8388608);
    Serial.print("ADC_value=-");
    Serial.println(ADC_value);
  } 
 
}
