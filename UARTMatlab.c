// ALUMNO: CANCINOS, JOSÉ MARÍA

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

void clockConfig(void);
void uartConfig (void);
void enviar(char);

int main(void) {

	uint32_t a = SystemCoreClock;  // uint32_t es igual a int pero define como entero unsigned y de 32 bits

	clockConfig ();
	uartConfig();

    while(1) {
         ;
    }
    return 0 ;
}

void uartConfig (void)
{

	LPC_SC -> PCONP |= (1 << 24); 	    // Energizo UART 2
	LPC_SC -> PCLKSEL1 &= !(3 << 16);   // CCLK/4
	LPC_UART2 -> LCR |= (3 << 0); 		// Tamaño de palabra: 8 bits
										// Bit de parada: 1
										// Paridad activada
										// Desactivado break control
	LPC_UART2 -> LCR |= (1 << 7);		// DLSB = 1
	LPC_UART2 -> DLL = 163;				// UnDLL = 163
	LPC_UART2 -> DLM = 0;				// UnDLM = 0
	LPC_UART2 -> LCR &= ~(1 << 7);		// DLSB = 0
	LPC_PINCON -> PINSEL0 |= (5 << 20);	// P0[10]= Tx, P0[11]= Rx
	LPC_UART2 -> IER = 1; 				// Activa la interrupción al recibir el dato

	NVIC_EnableIRQ(UART2_IRQn);		// Habilito interrupcion de timer
}

void UART2_IRQHandler (void)
{
	char k;
	int i;
	k = (LPC_UART2 -> RBR);
	char c[] = "hola mundo \r\n";
	c[11] = k;
	for(i = 0; c[i]; i++)
	{
		enviar (c[i]);
	}
}

void enviar (char c)
{
	while (((LPC_UART2 -> LSR) & ( 1 << 5)) == 0)
	{
		;
	}
	LPC_UART2 -> THR = c;
}

void clockConfig(void)
{
	 while (((LPC_SC -> SCS) & (1 << 6)) == 0);

	LPC_SC -> PLL0FEED = 0xAA;
	LPC_SC -> PLL0FEED = 0x55;

	LPC_SC -> PLL0CON = 0x01;		// ACTIVO PLL0

	LPC_SC -> PLL0FEED = 0xAA;
	LPC_SC -> PLL0FEED = 0x55;

	while (!((LPC_SC -> PLL0STAT )& (1<<26)))
	{
		;
	}

	LPC_SC -> PLL0CON = 0x03;		// ACTIVO Y CONECTO PLL0

	LPC_SC -> PLL0FEED = 0xAA;
	LPC_SC -> PLL0FEED = 0x55;

	while (!((LPC_SC -> PLL0STAT )& ((1<<25) | (1<<24 ))))
		{
			;
		}
}

/*
 *	*1) Seleccionar MAIN OSCILLATOR   (SCS |= (1 << 5)
 *	2) while (SCS >> 6) == 0)
 *	{
 *		;
 * 	}
 * 	*3) CCLKCFG = 0x3 		// PLLCLK / 4
 * 	*4) PCLKSEL0 = 0
 * 	*5) PCLKSEL1 = 0
 * 	*6) CLKSRCSEL = 1		// Fuente de clock para PLL
 * 	*7) PLL0CFG = 0x50063	// PLL0 -> M = 100; N = 6
 * 	8) PLL0FEED = 0xAA;
 * 	   PLL0FEED = 0x55;
 * 	9) PLL0CON = 0x01		// activo PLL0
 * 	10) PLL0FEED = 0xAA;
 * 	    PLL0FEED = 0x55;
 *
 * 	11)  while(!(PLL0STAT & (1<<26) ) )
 * 	    {
 * 	    	;
 * 	    }
 * 	12) PLL0CON = 0x03		// activo y conecto PLLO
 * 	13) PLL0FEED = 0xAA;
 * 	    PLL0FEED = 0x55;
 * 	  while(!(PLL0STAT & ((1<<25) | (1<<24 )) ))
 * 	  {
 * 	  	;
 * 	  }
 *
 *
 *
 *
 *
 *
 */
