/*
* Modificar el ejemplo 15 para que:
* Mediante dos displays de 7 segmentos se muestre el valor de tension
* de un segundo canal del ADC que se encuentra conectado a un
* potenciometro.
*/

unsigned int volatile * const FIO0DIR = (unsigned int *) 0x2009C000;
unsigned int volatile * const FIO0SET = (unsigned int *) 0x2009C018;
unsigned int volatile * const FIO0CLR = (unsigned int *) 0x2009C01C;
unsigned int volatile * const FIO0PIN = (unsigned int *) 0x2009C014;

unsigned int volatile * const FIO2DIR = (unsigned int *) 0x2009C040;
unsigned int volatile * const FIO2SET = (unsigned int *) 0x2009C058;
unsigned int volatile * const FIO2CLR = (unsigned int *) 0x2009C05C;
unsigned int volatile * const FIO2PIN = (unsigned int *) 0x2009C054;

unsigned int volatile * const PINSEL3 = (unsigned int *) 0x4002C00C;

// PERIFÉRICO
unsigned int volatile * const PCONP = (unsigned int *) 0x400FC0C4;
unsigned int volatile * const PCLKSEL0 =  (unsigned int *) 0x400FC1A8;

// TIMER 1

unsigned int volatile * const T0EMR = (unsigned int *) 0x4000403C;
unsigned int volatile * const T0MCR = (unsigned int *) 0x40004014;
unsigned int volatile * const T0MR0 = (unsigned int *) 0x40004018;
unsigned int volatile * const T0TCR = (unsigned int *) 0x40004004;
unsigned int volatile * const T0IR = (unsigned int *) 0x40004000;
unsigned int volatile * const T0PR = (unsigned int *) 0x4000400C;

// ADC
unsigned int volatile * const AD0CR = (unsigned int *) 0x40034000;
unsigned int volatile * const AD0INTEN = (unsigned int *) 0x4003400C;

unsigned int volatile * const PINMODE1 = (unsigned int *) 0x4002C044;
unsigned int volatile * const PINSEL1 = (unsigned int *) 0x4002C004;
unsigned int volatile * const AD0DR0 = (unsigned int *) 0x40034010;
unsigned int volatile * const AD0DR1 = (unsigned int *) 0x40034014;
unsigned int volatile * const ADSTAT = (unsigned int *) 0x40034030;

// INTERRUPCIONES EXTERNAS
unsigned int volatile * const ISER0 =  (unsigned int *) 0xE000E100;

void adcConfig(void);
void ADC_IRQHandler(void);
void display (unsigned int num, unsigned int point);

static int decena = 0, unidad = 0;
float voltaje = 0.0;
unsigned static int segDisplay [] = { 64, 121, 36, 48, 25,
										18, 2, 120, 0, 24};

int main (void)
{
	*PCONP|=(1<<1); // Habilito Timer1
	*PCLKSEL0 |= (1<<2); // cclk timer 0
	*PCLKSEL0 |= (3<<4);  // cclk/8 timer1
	*PINSEL3 |= (3<<4); //config match timer
	*T0EMR |= (3<<4); // Control del MAT0.1
	*T0MCR |= 3;
	*T0MR0 = 4000; // 10 mseg para mux
	*ISER0 |= (3 << 1); // habilito interrupciones timers
	*T0TCR |= 1; // habilito counter timer 0
	adcConfig();
	//*FIO0DIR |= (3<<0); // salida display anodo comun P0.0 y P0.1
	//----
	*FIO0DIR |= ( 1 << 9); // define al pin 9 del puerto 0 como salida		// DISPLAY
	*FIO0DIR |= ( 1 << 8); // define al pin 8 del puerto 0 como salida		// DISPLAY
	//---
	*FIO0DIR |= (1<<22);
	*FIO2DIR |= (127<<0); // 7 segmentos

	while(1)
	{
		;
	}

	return 0;
}

void TIMER0_IRQHandler(void)
{
	static int i = 0;
	if (i == 0)
	{
		*FIO0CLR |= (1<<9);
		display (decena,1);
		*FIO0SET|=(1<<8);
		i = 1;
	}
	else if (i == 1)
	{
		*FIO0CLR |= (1<<8);
		display (unidad,0);
		*FIO0SET|=(1<<9);
		i = 0;
	}
	*T0IR |= 1;
}

void adcConfig(void)
{
	*PCONP |= (1<<12); // Enciende periferico ADC
	*AD0CR |= (1<<21); // Enciende periferico ADC
	*PCLKSEL0 |= (3<<24); // seleccion de clock
	*AD0CR |= (3<<0);
	*AD0CR &= ~(255<<8); // No divido clock
	*AD0CR |= (1<<16); // ADC trabajara en modo burst
	*PINMODE1 |= (1<<15); // deshabilito pull up/down P0.23
	*PINMODE1 |= (1<<17); // deshabilito pull up/down P0.24
	*PINSEL1 |= (1<<14); // selecciono pin como entrada de ADC P0.23
	*PINSEL1 |= (1<<16); // selecciono pin como entrada de ADC P0.24
	*AD0INTEN = 3; // habilito interrupciones por ADC canal 0 y 1
	*ISER0 |= (1<<22);
}

void ADC_IRQHandler(void)
{
	unsigned short volatile ADC0Value = 0;
	unsigned short volatile ADC1Value = 0;

	ADC1Value = (*AD0DR1 >> 4) & 0xFFF;
	voltaje = (ADC1Value/4095.0)*(3.4);
	decena = (int)voltaje;
	unidad = voltaje * 10.0;
	unidad = unidad % 10;

	ADC0Value = (*AD0DR0>> 4) & 0xFFF;
	if (ADC0Value < 2047)
	{
		*FIO0SET |= (1<<22);
	}
	else
	{
		*FIO0CLR|= (1<<22);
	}

}

void display (unsigned int num, unsigned int point)
{
	switch (num)
	{
		case 0:
					*FIO2CLR |= 0x7F;
					*FIO2SET |= segDisplay[0];
					break;
		case 1:
					*FIO2CLR |= 0x7F;
					*FIO2SET |= segDisplay[1];
					break;
		case 2:
					*FIO2CLR |= 0x7F;
					*FIO2SET |= segDisplay[2];
					break;
		case 3:
					*FIO2CLR |= 0x7F;
					*FIO2SET |= segDisplay[3];
					break;
		case 4:
					*FIO2CLR |= 0x7F;
					*FIO2SET |= segDisplay[4];
					break;
		case 5:
					*FIO2CLR |= 0x7F;
					*FIO2SET |= segDisplay[5];
					break;
		case 6:
					*FIO2CLR |= 0x7F;
					*FIO2SET |= segDisplay[6];
					break;
		case 7:
					*FIO2CLR |= 0x7F;
					*FIO2SET |= segDisplay[7];
					break;
		case 8:
					*FIO2CLR |= 0x7F;
					*FIO2SET |= segDisplay[8];
					break;
		case 9:
					*FIO2CLR |= 0x7F;
					*FIO2SET |= segDisplay[9];
					break;
	}

}

