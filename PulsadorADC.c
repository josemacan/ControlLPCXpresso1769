// ALUMNO: CANCINOS JOSÉ MARÍA

#define AddrFIO0DIR 0x2009C000    // Define las posiciones de memorias
#define AddrFIO0SET 0x2009C018    // donde se encuentran los registros
#define AddrFIO0CLR 0x2009C01C    // que configuran al GPIO0
#define AddrPCONP 0x400FC0C4
#define AddrAD0CR 0x40034000
#define AddrAD0INTEN 0x4003400C
#define AddrPINMODE1 0x4002C044
#define AddrPINSEL1 0x4002C004
#define AddrAD0DR0 0x40034010
#define AddrPCLKSEL0 0x400FC1A8
#define AddrISER0 0xE000E100 //pagina 79

#define AddrFIO3DIR 0x2009C060;
#define AddrFIO3SET 0x2009C078
#define AddrFIO3CLR 0x2009C07C;

#define AddrFIO2DIR 0x2009C040;
#define AddrPINSEL4 0x4002C010;

//#define AddrAD0GDR 0x40034004

unsigned int volatile * const FIO0DIR = (unsigned int *) AddrFIO0DIR;
unsigned int volatile * const FIO0SET = (unsigned int *) AddrFIO0SET;
unsigned int volatile * const FIO0CLR = (unsigned int *) AddrFIO0CLR;
unsigned int volatile * const PCONP = (unsigned int *) AddrPCONP;
unsigned int volatile * const AD0CR = (unsigned int *) AddrAD0CR;
unsigned int volatile * const AD0INTEN = (unsigned int *) AddrAD0INTEN;
unsigned int volatile * const PINMODE1 = (unsigned int *) AddrPINMODE1;
unsigned int volatile * const PINSEL1 = (unsigned int *) AddrPINSEL1;
unsigned int volatile * const AD0DR0 = (unsigned int *) AddrAD0DR0;
unsigned int volatile * const PCLKSEL0 =  (unsigned int *) AddrPCLKSEL0;
unsigned int volatile * const ISER0 =  (unsigned int *) AddrISER0;

unsigned int volatile * const FIO3DIR =  (unsigned int *) AddrFIO3DIR ;
unsigned int volatile * const FIO3SET =  (unsigned int *) AddrFIO3SET ;
unsigned int volatile * const FIO3CLR =  (unsigned int *) AddrFIO3CLR ;

unsigned int volatile * const FIO2DIR =  (unsigned int *) AddrFIO2DIR ;
unsigned int volatile * const PINSEL4 =  (unsigned int *) AddrPINSEL4 ;


//unsigned int volatile * const AD0GDR = (unsigned int *) AddrAD0GDR;

void adcConfig(void);
void ADC_IRQHandler(void);

int main(void)
{
		unsigned short volatile ADC0Value=0;

		adcConfig();

		*FIO0DIR |= (1<<22);
		while(1)
		{
			ADC0Value = (*AD0DR0>>4) & 0xFFF;

			//*FIO3SET |= (1<<26);

			if(ADC0Value<2047)
			{
				*FIO0SET |= (1 << 22);
				//*FIO3CLR |= (1<<26);
			}
			else
			{
			*FIO0CLR |= (1 << 22);
			}
		}
	return 0 ;
}
void adcConfig()
{
		// ENCENDIDO DEL ADC

		*PCONP |=(1<<12);     // Enciende el periferico de ADC
		*AD0CR|=1<<21; 		  // Modo operacional

		// RELOJ DEL ADC - FRECUENCIAS

		*PCLKSEL0 |= (3<<24); // CCLK/8
		*AD0CR &=~(255<<8);   // DIVIDE EN 255

		// LA FREQ DEBE SER MENOR A 13 [MHz]

		// NO MODO BURST SI UTILIZO START DISTINTO A 000
		*AD0CR &=~ (1<<16);     // MODO BURST

		/*
		 * SI BURST = 1 ->
		 * 					START: 000
		 * 					ADGINTEN: 000
		 * 					AD0INTEN: 000
		 *
		 */

		// CONFIGURACION DEL START

		*AD0CR |= (2<<24);     // 010 - Inicio con P2.10
		*FIO2DIR &= ~(1<<10);	// 0 - P2.10 como entrada
		//*PINMODE4 |= (2<<20);	// 10 - Desactivo pull up y pull down
		*PINSEL4 |= (1<<20);	// 01 - Selecciono EINT0 P2.10


		// SELECCION DEL CANAL

		*AD0CR|= (1<<0);     // AD0.0

		// ACTIVACIÓN DEL ADC - SELECCION DE PINES

		*PINMODE1 |= (2<<14);   // Desactivo resistencias de pull up y pull down
		*PINSEL1 |= (1<<14);	  // P0.23 como AD0.0

		// INTERRUPCIONES

		*AD0INTEN |= (1<<0);  //Interrupcion al finalizar una interrupcion

		*ISER0|=(1<<22); 	  // Activo interrupcion de ADC
		}

	void ADC_IRQHandler(void)
	{
			unsigned short volatile ADC0Value=0;

			ADC0Value = (*AD0DR0>>4) & 0xFFF;

			if(ADC0Value<2047)
			{
			*FIO0SET |= (1 << 22);
			}
			else
			{
			*FIO0CLR |= (1 << 22);
			}
	}
