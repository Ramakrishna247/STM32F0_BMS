//CODE:SIMPLE BMS WITH STM32F030F4P6 FOR SINGLE 18650 CELL
//AUTHOR:K.V.Rama Krishna
//DATE:27-05-2019

#include "stm32f0xx.h"
void delay(void);


void print(unsigned char *ptr);            //uart print function
void hex2char(int num,unsigned char *str); //converts hexadecimal number to ascii character
void shut_ind(void);                       //over voltage and lower voltage cutoff

float vol_5;
int x;

int main()
{
	
	
	
RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // enable clock for usart
RCC->AHBENR |=RCC_AHBENR_GPIOAEN;   // enable clock for gpioa
	
	/*------------cutoff signal pin------------*/
	
	GPIOA->MODER  |=GPIO_MODER_MODER4_0;
	GPIOA->MODER  &=~GPIO_MODER_MODER4_1;
	
	GPIOA->OTYPER &=~GPIO_OTYPER_OT_4;
	
	GPIOA->OSPEEDR |=GPIO_OSPEEDER_OSPEEDR4_0;
	GPIOA->OSPEEDR &=~GPIO_OSPEEDER_OSPEEDR4_1;
	
	GPIOA->PUPDR &=~GPIO_PUPDR_PUPDR4_0;
	GPIOA->PUPDR &=~GPIO_PUPDR_PUPDR4_1;
	
	
/*----------------analog pin configuration------------------*/

//configure PA0 TO ANALOG PIN
GPIOA->MODER |=GPIO_MODER_MODER0_0;
GPIOA->MODER |=GPIO_MODER_MODER0_1;	
GPIOA->PUPDR &=~GPIO_PUPDR_PUPDR0_0;
GPIOA->PUPDR &=~GPIO_PUPDR_PUPDR0_1;
	
RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; 
	
	//ENABLE CLOCK FOR ADC	ADC
	RCC->CFGR2 &=~RCC_CFGR2_PREDIV_3;
	RCC->CFGR2 &=~RCC_CFGR2_PREDIV_2;
	RCC->CFGR2 |=RCC_CFGR2_PREDIV_1;
	RCC->CFGR2 |=RCC_CFGR2_PREDIV_0;
	
	
/*---------------END OF ADC pin CONFIGURATRION-------------*/	
	
/*----------------start of uart pin confugrations --------------*/	
	
	
GPIOA->AFR[1]=0X00000110;   // select alternate function for pin 9 and pin 10
	
//for pins 0 to 8 we have to use afr[0] and for pins 9 to 15 use afr[1]  
//for stm32f030 alternate function number for usart is 1 
	
GPIOA->MODER |=GPIO_MODER_MODER10_1;  //select alternate function mode
GPIOA->MODER &= ~(GPIO_MODER_MODER10_0);
	
	
GPIOA->MODER |=GPIO_MODER_MODER9_1;
GPIOA->MODER &=~(GPIO_MODER_MODER9_0);
GPIOA->OTYPER &=~(GPIO_OTYPER_OT_10);	
GPIOA->OTYPER &=~(GPIO_OTYPER_OT_9);
	
GPIOA->OSPEEDR |=GPIO_OSPEEDER_OSPEEDR10;//select speed 50 MHZ
GPIOA->OSPEEDR |=GPIO_OSPEEDER_OSPEEDR9;	
	
GPIOA->PUPDR |=GPIO_PUPDR_PUPDR9_0;	// select pull up
GPIOA->PUPDR &=~(GPIO_PUPDR_PUPDR9_1);		
	
GPIOA->PUPDR |=GPIO_PUPDR_PUPDR10_0;	 // select pull up
GPIOA->PUPDR &=~(GPIO_PUPDR_PUPDR10_1);	// select pull up 
		
 //SELECT ALTERNATE FUNCTION HIGH REGISTER AND AF1 FOR PIN 9 AND PIN 10 FOR  UART
/*----------------------- end of uart pin configuration------------------------*/


/*------------------------------START OF UART peripheral CONFIGURATION----------------------*/


USART1->CR1 &=~(USART_CR1_UE); // DISABLE UASRT PHERIPHERAL
USART1->CR1 &=~(USART_CR1_M);  //SET DATA LENGTH TO 8 BITS
USART1->CR1 &=~USART_CR1_PCE;  // no parity check
USART1->CR1 |= USART_CR1_OVER8; // to select over sampling by 8


USART1->CR2 &=~(USART_CR2_STOP); // TO SELECT ONE STOP BIT

USART1->CR3 &=~USART_CR3_CTSE; // hardware flow control disabled
USART1->CR3 &=~USART_CR3_RTSE; // hardware flow control disabled
USART1->BRR =0X681;//  set baud rate to 9600 and oversampling by 8 , for clock 8MHZ


 
USART1->CR1 |=USART_CR1_RE; //enable receive 
USART1->CR1 |=USART_CR1_TE; // enable transmit
USART1->CR1 |=USART_CR1_UE;  // enable uart peripheral 



unsigned char a[10]="ADC_V";

float vol_3;

print(a); //test print


/*----------------*END OF UART peripheral Configuration----------------*/


/*--------------START OF ADC peripheral  CONFIGURATION-------------------*/

ADC1->CR &=~ADC_CR_ADSTART;  // MAKE ADC START AS LOW

// ENABLE ADC PERIPHERAL
ADC1->CR |=ADC_CR_ADEN;

//SELECTING SAMPLE RATE 41.5 ADC CLOCK
ADC1->SMPR |=ADC_SMPR1_SMPR_2;
ADC1->SMPR &=~ADC_SMPR1_SMPR_1;
ADC1->SMPR &=~ADC_SMPR1_SMPR_0;

//SELECT CLOCK SOURCE PCLK/4
ADC1->CFGR2 &=~ADC_CFGR2_CKMODE_1;
ADC1->CFGR2 &=~ADC_CFGR2_CKMODE_0;

//Enabling irq for ADC1
//NVIC_EnableIRQ(ADC1_IRQn);
//NVIC_SetPriority(ADC1_IRQn,0);



//ENABLE END OF CONVERSION AND ADC READY INTERRUPTS
ADC1->IER |=ADC_IER_EOCIE;
ADC1->IER |=ADC_IER_ADRDYIE;



ADC1->CHSELR |=ADC_CHSELR_CHSEL0; //CHANNEL SELECTED AS ZERO
ADC1->CFGR1 |=ADC_CFGR1_DISCEN; // DISCONTINOUS MODE
ADC1->CFGR1 &=~ADC_CFGR1_CONT; //SINGLE CONVERSION
ADC1->CFGR1 &=~ADC_CFGR1_ALIGN;//DATA ALIGNMENT TO RIGHT
ADC1->CFGR2 &=~ADC_CFGR1_SCANDIR;// scan upward

ADC1->CFGR1 &=~ADC_CFGR1_RES_0; //SELECT RESOLUTION AS 12 BIT
ADC1->CFGR1 &=~ADC_CFGR1_RES_1;

ADC1->CHSELR |=ADC_CHSELR_CHSEL0; //CHANNEL SELECTED AS ZERO	

ADC->CCR |=ADC_CCR_VREFEN;

/*----------------end of ADC peripheral configuration--------------*/


//super loop 
while(1)
{


ADC1->CR |=ADC_CR_ADSTART; //START ADC CONVERSION
	
while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {;}

while((ADC1->ISR & ADC_ISR_EOSMP) == 0){;} //POLL TO CHECK END OF CONVERSION

x=ADC1->DR;		
		
ADC1->CR |=ADC_CR_ADSTP; // DISABLE ADC
	
vol_3 =(x/4095.0)*3.3;
	
vol_5 =(vol_3/3.3)*5;	
	
hex2char(x,a);

print(a);
	
	shut_ind();
	
delay();
	
}




}


void delay(void){

volatile unsigned int i=0;
	
	for(i=0; i<1000000; i++)
	{
	;
	}

}

void print(unsigned char *ptr)
{
int i;
	
for(i=0;i<5;i++)
{
	
USART1->TDR =ptr[i];
while (!(USART1->ISR & USART_ISR_TXE));
//while(!(USART1->ISR & USART_ISR_TC));
	
	
}

}

void hex2char(int num,unsigned char *str)
{
	
	
	int temp;
	int i=0;
	if(num==0)
	{
		for(i=4;i>=0;i--)
	{
		
		str[i]='0';   
		
	}
	
		str[5]='\0';
	}
	
	else{
		
	for(i=4;i>=0;i--)
	{
		temp=num%10;
		
		num=num/10;
		
		str[i]=temp+48;   
		
	}
	}
	
	
	str[5]='\0';
	return;
	
}

void shut_ind(void)
{
if(vol_5>4.1) //check if battery voltage is greater than 4.1 volts
{
	
GPIOA->ODR |=GPIO_ODR_4; //turn on shut_down circuit if battery voltage is above 4.1
	
	
}
else
{

	GPIOA->ODR &=~GPIO_ODR_4; //turn of shut down circuit

}

return;

}
	