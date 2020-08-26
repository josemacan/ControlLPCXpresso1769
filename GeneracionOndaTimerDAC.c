// ALUMNO: CANCINOS, JOSÉ MARÍA
/*
 * A partir del ejemplo 7 y dada la siguiente variable tipo array que define un cuarto de onda senoidal,
 * como se muestra en la figura adjunta, generar una onda analógica senoidal de 400 Hz
 * de frecuencia utilizando un timer y el DAC.
   uint32_t sin_0to90[16] = {0,1045,2079,3090,4067,5000,5877,6691,7431,8090,8660,9135,9510,9781,9945,10000};
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

//----------------------------------------------------------------

void aConvertir (int32_t valor);


uint32_t static sin_0to90[16] = {0,1045,2079,3090,4067,5000,5877,6691,7431,8090,8660,9135,9510,9781,9945,10000};	// valor de 1/4 de onda

unsigned int cuadrante = 1;				// variable que contiene el cuadrante en el que estoy
unsigned int indice = 0;			// indice para recorrer el arreglo
int main (void)
{
	uint32_t a = SystemCoreClock;  // uint32_t es igual a int pero define como entero unsigned y de 32 bits

	// Configuración de Timer0

	LPC_SC -> PCONP |= (1<<1);			// ENCIENDO PEROFERICO DE TIMER
	LPC_SC -> PCLKSEL0 &= ~(3<<22); 		// "00" -> CLKTMR = CCLK/4

	LPC_TIM0 -> MR0 = 250000;		// Valor de carga p 10 [mseg]
	LPC_TIM0 -> MCR |= (3<<0);			// Interrupción por timer y reseteo
	NVIC_EnableIRQ(TIMER0_IRQn);		// Habilito interrupcion de timer
	LPC_TIM0 -> TCR |= (1<<0);			// Habilito registro de control del timer

	// Configuración del DAC

	LPC_GPIO0 -> FIODIR |= (1<<26); 	// P0[26] SALIDA
	LPC_PINCON -> PINSEL1 |= (2<<20);   // "10" P0[26] -> AOUT
	LPC_PINCON -> PINMODE1 &= ~(3<<20); 	// Activo resistencia de pull up

	LPC_SC -> PCLKSEL0 &= ~(3<<22); 		// "00" -> CLKDAC = CCLK/4

	// LED de prueba
	/*
	LPC_GPIO0 -> FIODIR|=(1<<22);		// P0[22] COMO SALIDA
	LPC_GPIO0 -> FIOSET|=(1<<22);		// Apago LED Rojo
	LPC_GPIO3 -> FIODIR|=(1<<26);		// P0[22] COMO SALIDA
	LPC_GPIO3 -> FIOSET|=(1<<26);		// Apago LED azul
	LPC_GPIO3 -> FIODIR|=(1<<25);		// P0[22] COMO SALIDA
	LPC_GPIO3 -> FIOSET|=(1<<25);		// Apago LED verde
	*/

	while (1)
	{
		;
	}
		return 0;
	}

	// FUNCIÓN DEL TIMER

	void TIMER0_IRQHandler(void)		// ISR del Timer0
	{
		if((LPC_TIM0 -> IR >> 0) && 1)
		{
			if(cuadrante == 0)
			{ //LPC_GPIO0 -> FIOCLR|=(1<<22);		// Activo LED Rojo
				if(indice == 15 )
				{
					aConvertir (sin_0to90[indice]);
					cuadrante = 1;
				}
				else
				{
					aConvertir (sin_0to90[indice]);
					indice = indice + 1;
				}
			}
			else if (cuadrante == 1)
			{ //LPC_GPIO3 -> FIOCLR|=(1<<25);		// Activo LED verde
				if(indice == 0 )
				{
					aConvertir (sin_0to90[indice]);
					cuadrante = 2;
				}
				else
				{
					aConvertir (sin_0to90[indice]);
					indice = indice - 1;
				}
			}
			else if (cuadrante == 2)
			{ 	//LPC_GPIO3 -> FIOCLR|=(1<<26);		// Activo LED azul
				if(indice == 15 )
				{
					aConvertir (-1*(sin_0to90[indice]));
					cuadrante = 3;
				}
				else
				{
					aConvertir (-1*(sin_0to90[indice]));
					indice = indice + 1;
				}
			}
			else if (cuadrante == 3)
			{	//LPC_GPIO3 -> FIOCLR|=(1<<26);		// Activo LED azul
			    //LPC_GPIO3 -> FIOCLR|=(1<<25);		// Activo LED verde
				if(indice == 0 )
				{
					aConvertir (-1*(sin_0to90[indice]));
					cuadrante = 0;
				}
				else
				{
					aConvertir (-1*(sin_0to90[indice]));
					indice = indice - 1;
				};
			}
		}

		LPC_TIM0 -> IR|= (1<<0); 	// borro bandera de interrupcion
		return;
	}

void aConvertir (int32_t valor)
{
	LPC_DAC -> DACR = ((valor & 0x3FF) << 6); // Escribo valor en registro para convertir
}
