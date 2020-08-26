/* ALUMNO: CANCINOS JOSÉ MARÍA
 *
// MAT0.1: 			P1.29		PINSEL3: 26-27
 *
 *
 */
/*

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

#define AddrFIO1DIR 0x2009C020;
#define AddrPINSEL3 0x4002C00C;
#define AddrPINMODE3 0x4002C04C;


//#define AddrAD0GDR 0x40034004

*/

// PUERTO 0
unsigned int volatile * const FIO0DIR = (unsigned int *) 0x2009C000;
unsigned int volatile * const FIO0SET = (unsigned int *) 0x2009C018;
unsigned int volatile * const FIO0CLR = (unsigned int *) 0x2009C01C;
unsigned int volatile * const PINMODE1 = (unsigned int *) 0x4002C044;
unsigned int volatile * const PINSEL1 = (unsigned int *) 0x4002C004;

// PUERTO 1
unsigned int volatile * const FIO1DIR =  (unsigned int *) 0x2009C020;
unsigned int volatile * const PINSEL3 =  (unsigned int *) 0x4002C00C;
unsigned int volatile * const PINMODE3 =  (unsigned int *) 0x4002C04C;


// PUERTO 3
unsigned int volatile * const FIO3DIR =  (unsigned int *) 0x2009C060;
unsigned int volatile * const FIO3SET =  (unsigned int *) 0x2009C078;
unsigned int volatile * const FIO3CLR =  (unsigned int *) 0x2009C07C;

// PUERTO 2
unsigned int volatile * const FIO2DIR =  (unsigned int *) 0x2009C040;
unsigned int volatile * const PINSEL4 =  (unsigned int *) 0x4002C010;

// PERIFÉRICO
unsigned int volatile * const PCONP = (unsigned int *) 0x400FC0C4;
unsigned int volatile * const PCLKSEL0 =  (unsigned int *) 0x400FC1A8;

// ADC
unsigned int volatile * const AD0CR = (unsigned int *) 0x40034000;
unsigned int volatile * const AD0INTEN = (unsigned int *) 0x4003400C;
unsigned int volatile * const AD0DR0 = (unsigned int *) 0x40034010;
//unsigned int volatile * const AD0GDR = (unsigned int *) 0x40034004;

// TIMER 1

unsigned int volatile * const T1EMR = (unsigned int *) 0x4000803C;
unsigned int volatile * const T1MCR = (unsigned int *) 0x40008014;
unsigned int volatile * const T1MR1 = (unsigned int *) 0x4000801C;
unsigned int volatile * const T1TCR = (unsigned int *) 0x40008004;
unsigned int volatile * const T1IR = (unsigned int *) 0x40008000;


// INTERRUPCIONES EXTERNAS
unsigned int volatile * const ISER0 =  (unsigned int *) 0xE000E100;

void adcConfig(void);
void timer1Configuracion(void);
void ADC_IRQHandler(void);

int main(void)
{
		unsigned short volatile ADC0Value=0;

		adcConfig();
		timer1Configuracion();

		// LEDS DE PRUEBA
		*FIO0DIR |= (1<<22);
		*FIO3DIR |= (1<<26);
		*FIO0SET |= (1<<22);
		*FIO3SET |= (1<<26);
		//
		while(1)
		{
			ADC0Value = (*AD0DR0>>4) & 0xFFF;



			if(ADC0Value<2047)
			{
				*FIO0SET |= (1 << 22);
				*FIO3CLR |= (1<<26);

			}
			else
			{
				*FIO0CLR |= (1 << 22);
				*FIO3SET |= (1<<26);

			}
		}
	return 0 ;
}
void timer1Configuracion(void)
{
		*PCONP|=(1<<2); // Habilito Timer1
		*PCLKSEL0 |= (0<<4);
		*PCLKSEL0 |= (0<<5); // CCLK/4
		*PINSEL3 |= (3<<26); // Elijo MR0R M0.1 P1.29
		*T1MCR |= (1 << 4); // Si llego a match, reinicio el TC
		*T1EMR |= (1<<1); // Control del MAT0.1
		*T1EMR |= (3<<6); // 11. Toogle de MAT0.1
		*T1MR1 = 20000; // 20 mseg
		*T1TCR |= (1<<0);

}
void adcConfig()
{
		// ENCENDIDO DEL ADC

		*PCONP |=(1<<12);     // Enciende el periferico de ADC
		*AD0CR|= (1<<21); 		  // Modo operacional

		// RELOJ DEL ADC - FRECUENCIAS

		*PCLKSEL0 |= (3<<24); // CCLK/8
		*AD0CR &=~(255<<8);   // DIVIDE EN 255

		// LA FREQ DEBE SER MENOR A 13 [MHz]

		// NO MODO BURST SI UTILIZO START DISTINTO A 000
		*AD0CR &=~ (1<<16);     // NO burst

		/*
		 * SI BURST = 1 ->
		 * 					START: 000
		 * 					ADGINTEN: 000
		 * 					AD0INTEN: 000
		 *
		 */

		// CONFIGURACION DE INICIO DE CONVERSIÓN

		*AD0CR &= ~(1<<24);      //
		*AD0CR &= ~(1<<25);      //
		*AD0CR |= (1<<26);      // 100 - Inicio con MAT0.1
		*AD0CR &= ~(1<<27);      // flanco ascendente MAT0.1


		*FIO1DIR &= ~(1<<29);	// 0 - P1.29 como entrada
		//*PINMODE3 |= (2<<26);	// 10 - Desactivo pull up y pull down

		//*PINSEL3 |= (3<<26);	// 11 - Selecciono MAT0.1 P1.29

		//
		*PINSEL3 |= (1<<26);	// 11 - Selecciono MAT0.1 P1.29
		*PINSEL3 |= (1<<27);	// 11 - Selecciono MAT0.1 P1.29
		//


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
			*FIO3CLR |= (1<<26);
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
