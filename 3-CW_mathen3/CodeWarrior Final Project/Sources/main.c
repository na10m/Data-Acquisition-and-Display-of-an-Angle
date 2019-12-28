//*********************************************************************  
//*                          FINAL PROJECT                         
//*                       McMaster University                         
//*                      2DP4 Microcontrollers                        
//*                          Lab Section 02                          
//*                  Nathen Mathew mathen3 400074896                            
//*********************************************************************
//*********************************************************************
//*                          Description                              
//*                                                                   
//*                                                        
//* Read an analog input of an angle from the accelerometer through the                         
//* ESDX and serially output it to the PC                                                                  
//*                                                                   
//*********************************************************************


#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "SCI.h"

/*Prototypes*/
 void setClk8(void);
 void delay1ms(unsigned int multiple);
 
 unsigned int isOn = 0;
 unsigned int mode = 0;
 unsigned int xout;
 unsigned int theta;
 unsigned int firstDigit;
 unsigned int secndDigit;

void main(void) {

  setClk8(); //set bus clock to 8 MHz
  
  /* setup and enable channel 4 ADC */ 
  ATDCTL1 = 0b00110110; //0x36; 10-bit data transfer 
  ATDCTL3 = 0b10001000; //right justified, 1 sample/sequence (don't really matter tho)
  ATDCTL4 = 0b00000010; //double check this one, sets prescaler to 2
  ATDCTL5 = 0b00100110; //continous conversion on channel 6
  //ATDCTL0 not needed, only one register
  //ATDCTL2 = falling edge v rising edge, default is falling
  
  
  DDRJ = 0xFF;      //set all port J as output
  DDRP = 0b00001110;   //p ports 1,2,3 as output  //for LED outputs
  DDR1AD = 0b00111111; //set port AD as output except 6,7
  PTJ = 0x00;    // start LED off
  PTP = 0x00;


  // The next six assignment statements configure the Timer Input Capture                                                             
  TSCR1 = 0x90;    //Timer System Control Register 1
                    // TSCR1[7] = TEN:  Timer Enable (0-disable, 1-enable)
                    // TSCR1[6] = TSWAI:  Timer runs during WAI (0-enable, 1-disable)
                    // TSCR1[5] = TSFRZ:  Timer runs during WAI (0-enable, 1-disable)
                    // TSCR1[4] = TFFCA:  Timer Fast Flag Clear All (0-normal 1-read/write clears interrupt flags)
                    // TSCR1[3] = PRT:  Precision Timer (0-legacy, 1-precision)
                    // TSCR1[2:0] not used

  TSCR2 = 0x04;    //Timer System Control Register 2
                    // TSCR2[7] = TOI: Timer Overflow Interrupt Enable (0-inhibited, 1-hardware irq when TOF=1)
                    // TSCR2[6:3] not used
                    // TSCR2[2:0] = Timer Prescaler Select: See Table22-12 of MC9S12G Family Reference Manual r1.25 (set for bus/1)
                    
  TIOS = 0b11111100;     //Timer Input Capture or Output capture
                    //set TIC[0] and input (similar to DDR)
  PERT = 0x03;     //Enable Pull-Up resistor on TIC[0] and TIC[1]  IOC0 + IOC1

  TCTL3 = 0x00;    //TCTL3 & TCTL4 configure which edge(s) to capture
  TCTL4 = 0x0A;    //Configured for falling edge on TIC[0]

  /*
 * The next one assignment statement configures the Timer Interrupt Enable                                                   
 */           
  TIE = 0x03;      //Timer Interrupt Enable
  
  SCI_Init(19200);

  EnableInterrupts;


  for(;;) {
  
  
    while(isOn!=0){ //stops serial communication if isOn = 0
    
      xout = ATDDR0;
      theta = (xout-510)*100/121;
      if (xout < 510) theta = 0;
      if (xout > 620) theta = 90;
      SCI_OutUDec(theta);
      SCI_OutChar(CR);
      
      if (mode == 0){  // mode 0 
      
        PTJ = 0x00;
        PTP_PTP1 = 0;
        
        firstDigit = theta/10;
        secndDigit = theta%10;
        
        if(firstDigit == 0) { PT1AD = 0b00000000; // 0000
        } else if (firstDigit == 1) { PT1AD = 0b00001000; // 0001
        } else if (firstDigit == 2) { PT1AD = 0b00000100; // 0010
        } else if (firstDigit == 3) { PT1AD = 0b00001100; // 0011
        } else if (firstDigit == 4) { PT1AD = 0b00000010; // 0100
        } else if (firstDigit == 5) { PT1AD = 0b00001010; // 0101
        } else if (firstDigit == 6) { PT1AD = 0b00000110; // 0110
        } else if (firstDigit == 7) { PT1AD = 0b00001110; // 0111
        } else if (firstDigit == 8) { PT1AD = 0b00000001; // 1000
        } else if (firstDigit == 9) { PT1AD = 0b00001001; // 1001
        }
        
        PT1AD ^= 0b00110000; // PT1AD = 0bXX11XXXX BCD2nd = 11XX
        PTP ^= 0b00001100; // PTP = 0bXXXX11XX BCD2nd = XX11
        if(secndDigit == 0) { PT1AD &= 0b11001111; PTP &= 0b11110011;          
        } else if (secndDigit == 1) { PT1AD &= 0b11001111; PTP &= 0b11111011; 
        } else if (secndDigit == 2) { PT1AD &= 0b11001111; PTP &= 0b11110111; 
        } else if (secndDigit == 3) { PT1AD &= 0b11001111; PTP &= 0b11111111; 
        } else if (secndDigit == 4) { PT1AD &= 0b11101111; PTP &= 0b11110011; 
        } else if (secndDigit == 5) { PT1AD &= 0b11101111; PTP &= 0b11111011; 
        } else if (secndDigit == 6) { PT1AD &= 0b11101111; PTP &= 0b11110111; 
        } else if (secndDigit == 7) { PT1AD &= 0b11101111; PTP &= 0b11111111; 
        } else if (secndDigit == 8) { PT1AD &= 0b11011111; PTP &= 0b11110011; 
        } else if (secndDigit == 9) { PT1AD &= 0b11011111; PTP &= 0b11111011; 
        }
        
      } else {  // mode 1
      
      
        PTJ = 0x01;
        PTP_PTP1 = 1; 
         
        if (theta >80)  {PT1AD=0b00111111; PTP_PTP2=1; PTP_PTP3=1;}
        else if (theta >70)  {PT1AD=0b00111111; PTP_PTP2=1; PTP_PTP3=0;} 
        else if (theta >60)  {PT1AD=0b00111111; PTP_PTP2=0; PTP_PTP3=0;}
        else if (theta >50)  {PT1AD=0b00011111; PTP_PTP2=0; PTP_PTP3=0;}
        else if (theta >40)  {PT1AD=0b00001111; PTP_PTP2=0; PTP_PTP3=0;}
        else if (theta >30)  {PT1AD=0b00000111; PTP_PTP2=0; PTP_PTP3=0;}
        else if (theta >20)  {PT1AD=0b00000011; PTP_PTP2=0; PTP_PTP3=0;} 
        else if (theta >10)  {PT1AD=0b00000001; PTP_PTP2=0; PTP_PTP3=0;}
        else if (theta <=10)  {PT1AD=0b00000000; PTP_PTP2=0; PTP_PTP3=0;} 
        
      }
      

      delay1ms(100);
    
    }
  

 
  } /* loop forever */
  /* please make sure that you never leave main */
  

}

interrupt  VectorNumber_Vtimch0 void ISR_Vtimch0(void)// switches mode
{
  unsigned int temp; 
  
  if (isOn == 0) isOn = 1;  // toggles isOn
  else isOn = 0; 

  temp = TC0;       
}

interrupt VectorNumber_Vtimch1 void ISR_Vtimch1(void)
{
 unsigned int temp;
 
 
 if (mode == 0) mode = 1;   // toggles mode
 else mode = 0;

 temp = TC1; 

}

void setClk8(void){
  CPMUPROT = 0x26;     //Protection of clock configuration is disabled, maybe CPMUPROT=0. 
  
  CPMUCLKS = 0x80;     //PLLSEL=1. Select Bus Clock Source is PLL clock
  CPMUOSC = 0x80;      //OSCE=1. Select Clock Reference for PLLclk as fOSC (8 MHz).
  
  CPMUREFDIV = 0x41;   //fREF= fOSC/(REFDIV+1) -> fREF= fOSC/(2) -> fREF= 4 MHz.  
  
  CPMUSYNR=0x01;       //VCOCLK = fVCO= 2 * fREF* (SYNDIV+1) -> fVCO= 2 * 4 MHz * (1+1) fVCO = 16 MHz.
  
  CPMUPOSTDIV=0x00;    //PLLCLK = VCOCLK/(POSTDIV+1) -> PLLCLK = 16 MHz/(0+1) -> PLLCLK = 16 MHz.
                       // fBUS=fPLL/2=16 MHz/2 = 8 MHz  
  
  while (CPMUFLG_LOCK == 0) {}  //Wait for PLL to achieve desired tolerance of target frequency. NOTE: For use when the source clock is PLL. comment out when using external oscillator as source clock 
 
  CPMUPROT = 1;                 //Protection for clock configuration is reenabled 
 
}

void delay1ms(unsigned int multiple){

  unsigned int i;
  unsigned int j;
  
  for(j = 0; j<multiple; j++){
    for(i = 0; i<100; i++){
      // Delay
      PTJ = PTJ;
      PTJ = PTJ;
      PTJ = PTJ;
      PTJ = PTJ;
      PTJ = PTJ;
    }
  }  

}

