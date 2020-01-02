#line 1 "C:/Users/nguye/OneDrive/This PC/Do AN/rs485/pic18f_pic16f/Slaver.c"
#line 1 "d:/program files/mikroelektronika/mikroc pro for pic/include/stdint.h"




typedef signed char int8_t;
typedef signed int int16_t;
typedef signed long int int32_t;


typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long int uint32_t;


typedef signed char int_least8_t;
typedef signed int int_least16_t;
typedef signed long int int_least32_t;


typedef unsigned char uint_least8_t;
typedef unsigned int uint_least16_t;
typedef unsigned long int uint_least32_t;



typedef signed char int_fast8_t;
typedef signed int int_fast16_t;
typedef signed long int int_fast32_t;


typedef unsigned char uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned long int uint_fast32_t;


typedef signed int intptr_t;
typedef unsigned int uintptr_t;


typedef signed long int intmax_t;
typedef unsigned long int uintmax_t;
#line 12 "C:/Users/nguye/OneDrive/This PC/Do AN/rs485/pic18f_pic16f/Slaver.c"
sbit rs485_rxtx_pin at RA4_bit;
sbit rs485_rxtx_pin_direction at TRISA4_bit;





int comando_mestre = 0;
int comando_mestre1 = 0;
int comando_mestre4 = 0;
unsigned char dat[20];

sbit AM2302_Bus_In at RB5_bit;
sbit AM2302_Bus_Out at LATB5_bit;
sbit AM2302_Bus_Direction at TRISB5_bit;

unsigned short cnt;

void SPI_Write_Byte(unsigned short num)
{
unsigned short t, Mask, Flag;
  PORTC.B2  = 0;
 Mask = 128;
 for (t=0; t<8; t++)
 {
 Flag = num & Mask;
 if(Flag == 0)
 {
  PORTC.B0  = 0;
 }
 else
 {
  PORTC.B0  = 1;
 }
  PORTC.B2  = 1;
  PORTC.B2  = 0;
 Mask = Mask >> 1;
 }
}

void max7219(char adres, char deger)

{

  PORTC.B1 =0;
 SPI_Write_Byte(adres);
 SPI_Write_Byte(deger);
  PORTC.B1 =1;
}
void max7219_init()

{

 max7219(0x0C,0x01);

 max7219(0x09,0xFFFF);

 max7219(0x0A,0x09);

 max7219(0x0B,0x07);

 max7219(0x0F,0x01);

 Delay_ms(100);
 max7219(0x0F,0x00);



}




char AM2302_Data[5] = {0, 0, 0, 0, 0};
unsigned humidity = 0, temperature = 0;
const char degree[] = {14,10,14,0,0,0,0,0};
void degreeChar(char pos_row, char pos_char) {
 char i;
 Lcd_Cmd(64);
 for (i = 0; i<=7; i++) Lcd_Chr_CP(degree[i]);
 Lcd_Cmd(_LCD_RETURN_HOME);
 Lcd_Chr(pos_row, pos_char, 0);
}

char AM2302_Read(unsigned *humidity, unsigned *temperature) {
 char i = 0, j = 1;
 char timeout = 0;
 char sensor_byte;
 AM2302_Bus_Out = 0;

 AM2302_Bus_Direction = 1;
 AM2302_Bus_Direction = 0;
 AM2302_Bus_Out = 0;
 Delay_ms(18);
 AM2302_Bus_Out = 1;
 Delay_us(20);
 AM2302_Bus_Direction = 1;


 timeout = 30;
 while (AM2302_Bus_In == 1) {
 Delay_us(1);
 if (!timeout--) {
 return 1;
 }
 }


 while (!AM2302_Bus_In) {
 Delay_us(80);
 }

 while (AM2302_Bus_In) {
 Delay_us(50);
 }
#line 135 "C:/Users/nguye/OneDrive/This PC/Do AN/rs485/pic18f_pic16f/Slaver.c"
 i = 0;
 for (i = 0; i < 5; i++) {
 j = 1;
 for (j = 1; j <= 8; j++) {
 while (!AM2302_Bus_In) {
 Delay_us(1);
 }
 Delay_us(30);
 sensor_byte <<= 1;
 if (AM2302_Bus_In) {
 sensor_byte |= 1;
 delay_us(45);
 while (AM2302_Bus_In) {
 Delay_us(1);
 }
 }
 }
 AM2302_Data[i] = sensor_byte;
 }

 *humidity = (AM2302_Data[0] << 8) + AM2302_Data[1];
 *temperature = (AM2302_Data[2] << 8) + AM2302_Data[3];

 return 0;
}

void processValue1 (unsigned humidity, unsigned temperature) {



 if(humidity>=1000)
 {
 max7219(5,(humidity/1000)%10);
 max7219(6,(humidity/100)%10);
 max7219(7,(humidity/10)%10+0x80);
 max7219(8,(humidity%10));
 }
 else
 {
 max7219(5,15);
 max7219(6,(humidity/100)%10);
 max7219(7,(humidity/10)%10+0x80);
 max7219(8,(humidity%10));
 }

 if(temperature&0x8000)
 {
 max7219(1,0x11);
 max7219(2,(temperature/100)%10);
 max7219(3,(temperature/10)%10+0x80);
 max7219(4,(temperature%10));
 }
 else max7219(1,15);
 temperature&=0x7FFF;
 max7219(2,(temperature/100)%10);
 max7219(3,(temperature/10)%10+0x80);
 max7219(4,(temperature%10));

}

 void dislay(){


 max7219(1,0);
 max7219(2,0);
 max7219(3,0);
 max7219(4,0);

 max7219(5,0);
 max7219(6,0);
 max7219(7,0);
 max7219(8,0);
 }
void InitTimer1(){
 T1CON = 0x21;
 RCIE_bit = 1;
 TXIE_bit = 0;


 TMR1IF_bit = 0;
 TMR1H = 0x3C;
 TMR1L = 0xB0;
 TMR1IE_bit = 1;

 cnt = 0;
 INTCON = 0xC0;
}
void interrupt() {
 RS485Slave_Receive(dat);
 if (TMR1IF_bit){
 cnt++;
 TMR1IF_bit = 0;
 TMR1H = 0x3C;
 TMR1L = 0xB0;

 }
}
void main() {
 int i;
 ANSELB = 0;
 ANSELA = 0;
 OSCCON=0b01110010;
 TRISC0_bit=0;
 TRISC1_bit=0;
 TRISC2_bit=0;
 RC0_bit=0;
 RC1_bit=0;
 RC2_bit=0;


 UART1_Init(14400);
 Delay_ms(100);
 RS485Slave_Init( 160 );






 InitTimer1();
 dat[4] = 0;
 dat[5] = 0;
 dat[6] = 0;

 max7219_init();
 dislay();
 while (1) {
 if(cnt>=20)
 {
 if( dat[4] )
 {
 dat[4] = 0;
 if (AM2302_Read(&humidity, &temperature) == 0)
 {
 processValue1(humidity, temperature);
 comando_mestre=humidity/10;

 dat[0]=comando_mestre;
 comando_mestre1=humidity%10;
 dat[1]=comando_mestre1;
 dat[2]=2;
 RS485Slave_Send(dat, 3 );

 comando_mestre=temperature/10;

 dat[0]=comando_mestre;
 comando_mestre1=temperature%10;
 dat[1]=comando_mestre1;
 dat[2]=1;
 RS485Slave_Send(dat, 3 );
 cnt=0;
 }
 }




 }
 }
}
