/*
  Group-13:
          Deval Shah - 1401060
          Maharshi Bhavsar - 1401062
          Pratham Solanki - 1401049
   
   Project Title: Gesture Controlled Robot(Wired)
   
   Code Abstract:The code written below does the following:
        1)First fetch accelerometer values by reading actual X and Y co-ordinates from analog channel 0 and 1.
        2)ADC conversion method converts that values into digital values
        3)The digital value of X and Y is then compared to threshold value which is determined by X and Y co-ordinates to set left,right,forward and backward directions.
        4)After comparing to threshold value ,the atmega32 gives appropriate values to input of motor driver of robot to move in respective direction.
        5)The values of X and Y coordinates and direction in which robot is moving is diplayed on runtime in LCD kept on robot.
*/

//LCD Connections
#define LCD_RS  PORTA.B2   // RS
#define LCD_EN  PORTD.B6   //Enable
#define LCD_D4  PORTC.B4   //Data Bit 4
#define LCD_D5  PORTC.B5   //Data Bit 5
#define LCD_D6  PORTC.B6   //Data Bit 6
#define LCD_D7  PORTC.B7   //Data Bit 7

#define           X_MIN           320
/*Defines the lower threshold for the x-axis value of adxl335*/

#define          X_MAX           400
/*Defines the upper threshold for the x-axis value of adxl335*/

#define          Y_MIN           320
/*Defines the lower threshold for the y-axis value of adxl335*/

#define           Y_MAX          400
/*Defines the upper threshold for the y-axis value of adxl335*/

int x_axis=0,y_axis=0;
/*Decalring temporary variables for storing digital values of X and Y*/

/*This function is declared to initialize the Analog to Digital Converter of AVR microcontrollers. The Analog to Digital Converter is initialized with the following features:-
	Auto Triggering mode is enabled,
	Free running mode is the source of trigger,
	ADC conversion is started,
	ADC frequency is 62.5 KHz at 1 MHz system frequency.*/

void adc1_init();


/*This function is declared to read the digital value of the ADC conversion of the selected channel with follwoing conditions:-
AVCC is selected as the refence voltage and ADC output is right adjusted.*/

int read_adc_channel(unsigned char channel);



/*Function definations*/
void adc1_init()
{
ADCSRA=(1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADPS2);
SFIOR=0x00;
}


int read_adc_channel(unsigned char channel)
{
        int adc_value;
        unsigned char temp;
        ADMUX=(1<<REFS0)|channel;
        delay_ms(1);
        temp=ADCL;
        adc_value=ADCH;
        adc_value=(adc_value<<8)|temp;
        return adc_value;
}
/*The function is declared to convert the analog x-axis value of ADXL335 to digital value.*/
int read_adxl335_x_value(unsigned char channel);


/*The function is declared to convert the analog y-axis value of ADXL335 to digital value.*/

int read_adxl335_y_value(unsigned char channel);


/*The function is declared to convert the analog z-axis value of ADXL335 to digital value.*/

int read_adxl335_z_value(unsigned char channel);


/*Function definations*/
int read_adxl335_x_value(unsigned char channel)
{
int x_value;
x_value=read_adc_channel(channel);
return x_value;
}

int read_adxl335_y_value(unsigned char channel)
{
int y_value;
y_value=read_adc_channel(channel);
return y_value;
}

void LCD_data(unsigned char Data)
{
PORTC=Data&0xF0; // Send Higher nibble (D7-D4)
LCD_RS=1;       // Register Select =1 (for data select register)
LCD_EN=1;      //Enable=1 for H to L pulse
delay_us(5);
LCD_EN=0;


PORTC=((Data<<4)&0xF0); // Send Lower nibble (D3-D0)
LCD_EN=1;               //Enable=1 for H to L pulse
delay_us(5);
LCD_EN=0;

delay_us(100);
}


//LCD Print
void LCD_Print(char * str)
{
 unsigned char i=0;

 // Till NULL charecter is reached, take each character
 while(*(str+i)!=0)

 {
  LCD_data(*(str+i)); // Data sent to LCD data register
  i++;
  delay_ms(10);
 }
}

//LCD Command
void lcdcommand(unsigned char command)
{
PORTC=command&0xF0; // Send Higher nibble (D7-D4)
LCD_RS=0; // Register Select =0 (for Command register)
LCD_EN=1; //Enable=1 for H to L pulse
delay_us(5);
LCD_EN=0;
delay_us(100);

PORTC=((command<<4)&0xF0);  // Send Lower nibble (D3-D0)
LCD_EN=1; //Enable=1 for H to L pulse
delay_us(5);
LCD_EN=0;

delay_us(40);
}

// Cursor Posotion
void Cursor_Position(unsigned short int x,unsigned short int y)
{
 unsigned char firstcharadd[] ={0x80,0xC0}; // First line address 0X80
                                            //Second line address 0XC0
 lcdcommand((firstcharadd[x-1]+y-1));

}
 //Function for clearing LCD Screen
void clear()
{
  lcdcommand(0x01);
  delay_ms(2);
}

 //LCD Iniatialize
void LCD_Initialize()
{
LCD_EN=0;

lcdCommand(0x33); // Initialize LCD for 4 bit mode
lcdCommand(0x32); // Initialize LCD for 4 bit mode
lcdCommand(0x28); // Initialize LCD for 5X7 matrix mode
lcdCommand(0x0E); //Display on,cursor blinking
clear();
lcdCommand(0x06); //Shift cursor to right
lcdCommand(0x82);
}


void main() {
  /*Storing different directions in array of characters to be displayed on LCD*/
  char disp_left[]=".!.Left.!.";
  char disp_right[]=".!.Right.!.";
  char disp_forward[]="..!!Forward.!.";
  char disp_backward[]=".!.Backward.!.";
  char disp_stationary[]=".!.Relaxing.!.";
  
  /*Storing different coordinates in array of characters to be displayed on LCD*/
  char disp2[]="X:";
  char disp3[]="Y:";

 //Set-up PORTS for LCD
  DDRD = 0x0F;
  PORTD = 0x00;
  DDRC=0xF0;  // For D3-D0
  DDRA.B2=1;  //For RS
  DDRD.B6=1;  //For Enable

 /*Initializing the ADC conversion*/
 adc1_init();

/*Initializing LCD*/
 LCD_Initialize();

 while(1){
   x_axis = read_adxl335_x_value(0);
   /*Reading x-axis value of Accelerometer Sensor*/
   y_axis = read_adxl335_x_value(1);
   /*Reading y-axis value of Accelerometer Sensor*/

   Cursor_Position(2,1);//Using Cursor Position fn to display on row 2 //col 1
   LCD_Print(disp2);//Prints X:

   /*At cursor position 3,4,5 it prints X coordinate value at runtime afte acsii conversion*/
   Cursor_Position(2,3);//Using Cursor Position fn to display on row 2 //col 3
   LCD_Data((x_axis/100)|0x30);

   Cursor_Position(2,4);//Using Cursor Position fn to display on row 2 //col 4
   LCD_Data(((x_axis/10)%10)|0x30);

   Cursor_Position(2,5);//Using Cursor Position fn to display on row 2 //col 5
   LCD_Data(x_axis%10|0x30);

   /*Prints blank space*/
   Cursor_Position(2,6);//Using Cursor Position fn to display on row 2 //col 6
   LCD_Data(20);

   Cursor_Position(2,7);//Using Cursor Position fn to display on row 2 //col 7
   LCD_Print(disp3);//Prints Y:
   
   /*At cursor position 9,10,11 it prints Y coordinate value at runtime after acsii conversion*/
   Cursor_Position(2,9);//Using Cursor Position fn to display on row 2 //col 9
   LCD_Data((y_axis/100)|0x30);

   Cursor_Position(2,10);//Using Cursor Position fn to display on row 2 //col 5
   LCD_Data(((y_axis/10)%10)|0x30);

   Cursor_Position(2,11);//Using Cursor Position fn to display on row 2 //col 5
   LCD_Data(y_axis%10|0x30);
   

   if(x_axis>X_MAX)
    {
       PORTD=0x0A;
       /*Transmitter will transmit 0x0A to drive Robot in foward direction*/
       Cursor_Position(1,1);
       /*Using Cursor Position fn to display on row 1 //col 1*/
       LCD_Print(disp_forward);
       /*Prints Moving Forward on LCD*/
    }
    else if(x_axis<X_MIN)
    {
       PORTD=0x05;
       /*Transmitter will transmit 0x05 to drive Robot in reverse direction*/
       Cursor_Position(1,1);
       /*Using Cursor Position fn to display on row 1 //col 1*/
       LCD_Print(disp_backward);
       /*Prints Moving Backward on LCD*/

    }
    else if(y_axis>Y_MAX)
    {
       PORTD=0x08;
       /*Transmitter will transmit 0x08 to drive Robot in right direction*/
       Cursor_Position(1,1);
       /*Using Cursor Position fn to display on row 1 //col 1*/
       LCD_Print(disp_right);
       /*Prints Moving Right on LCD*/
    }
    else if(y_axis<Y_MIN)
    {
       PORTD=0x02;
       /*Transmitter will transmit 0x02 to drive Robot in left direction*/
       Cursor_Position(1,1);
       /*Using Cursor Position fn to display on row 1 //col 1*/
       LCD_Print(disp_left);
       /*Prints Moving Left on LCD*/
    }

    else
    {
        PORTD=0x0F;
        /*Transmitter will transmit 0x0F to keep in staitonary position*/
        Cursor_Position(1,1);        
        /*Using Cursor Position fn to display on row 1 //col 1*/
        LCD_Print(disp_stationary);
        /*Prints Moving Stationary on LCD*/
    }
  }
}