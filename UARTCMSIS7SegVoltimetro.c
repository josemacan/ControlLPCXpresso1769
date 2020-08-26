//ALUMNO: CANCINOS, JOSÉ MARÍA
/*
===============================================================================
 Name        : Ejercicio19.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

void aConvertir (int8_t valor);
void clockConfig (void);
void uartConfig (void);
void dacConfig(void);
void displayConfig (void);
void display (unsigned int num);
void enviar (char c);
void eco (void);

unsigned static int segDisplay [] = { 18, 6 };
unsigned static int dummy = 0;
int8_t valorentrada = 0;
unsigned static int bandera = 0;
int main(void) {

	uint32_t a = SystemCoreClock;  // uint32_t es igual a int pero define como entero unsigned y de 32 bits

	uartConfig();
	dacConfig();
	displayConfig();

	// ENCIENDO LED ROJO DE PRUEBA
	LPC_GPIO0 -> FIODIR |= ( 1 << 22); // define LED ROJO como salida.
	LPC_GPIO0 -> FIOSET |= ( 1 << 22); // APAGO LED

	//LPC_GPIO0 -> FIOCLR |= ( 1 << 22); // ENCIENDO LED

    while(1)
    {
       dummy = dummy + 1;
    }
    return 0 ;
}

void uartConfig (void)
{

	LPC_SC -> PCONP |= (1 << 3); 	    // Energizo UART 0
	LPC_SC -> PCLKSEL0 &= !(3 << 6);   // CCLK/4

	LPC_UART0 -> LCR |= (3 << 0); 		// Tamaño de palabra: 8 bits
	LPC_UART0 -> LCR &= ~(1 << 2);		// 1 Bit de parada
	LPC_UART0 -> LCR &= ~(1 << 6);		// Desactivado break control
	LPC_UART0 -> LCR |= (1 << 3);		// Paridad activada
	LPC_UART0 -> LCR |= ~(3 << 4);		// Paridad impar

	LPC_UART0 -> LCR |= (1 << 7);		// DLSB = 1

	LPC_UART0 -> DLL = 13;				// UnDLL = 13
	LPC_UART0 -> DLM = 0;				// UnDLM = 0
	LPC_UART0 -> FDR = (1 << 0);		// DivAddVal = 0;
	LPC_UART0 -> FDR = (15 << 4);		// MulVal = 14;

	LPC_UART0 -> LCR &= ~(1 << 7);		// DLSB = 0

	LPC_PINCON -> PINSEL0 |= (5 << 4);	// P0[2]= Tx, P0[3]= Rx

	LPC_UART0 -> IER |= (1<<0); 		// Activa la interrupción al recibir el dato
	LPC_UART0 -> IER |= (1<<2); 		// Activa la interrupción ante error en la recepción

	NVIC_EnableIRQ(UART0_IRQn);		// Habilito interrupcion de timer
}

void UART0_IRQHandler (void)
{
	valorentrada = (LPC_UART0 -> RBR);
	if((LPC_UART0 -> LSR & ( 1 << 2)) == 0) // Flag de error de paridad
	{
		LPC_GPIO0 -> FIOCLR |= ( 1 << 22); // ENCIENDO LED
		//display(1);
		LPC_GPIO2 -> FIOCLR |= 0x7F; // limpio segmentos
		LPC_GPIO2 -> FIOSET |= 18; // enciendo S

		aConvertir(valorentrada);


		dummy = LPC_UART0 -> LSR; // leo y bajo bandera;
	}
	else
	{
		valorentrada = 0;
		//display(0);
		LPC_GPIO2 -> FIOCLR |= 0x7F; // limpio segmentos
		LPC_GPIO2 -> FIOSET |= 6; //enciendo E;
		LPC_GPIO0 -> FIOSET |= ( 1 << 22); // ENCIENDO LED
		aConvertir(valorentrada);

		dummy = LPC_UART0 -> LSR; // leo y bajo bandera;

	}

}

void dacConfig (void)
{
	LPC_GPIO0 -> FIODIR |= (1<<26); 	// P0[26] SALIDA
	LPC_PINCON -> PINSEL1 |= (1<<21);   // "10" P0[26] -> AOUT
}

void aConvertir (int8_t valor)
{
	valor = (int) (valor);
	LPC_DAC -> DACR = (valor & 0xFF << 8); // Escribo valor en registro para convertir
}

void displayConfig (void)
{
	LPC_GPIO0 -> FIODIR |= ( 1 << 9); // define al pin 9 del puerto 0 como salida DISPLAY
	LPC_GPIO2 -> FIODIR |= (127<<0); // 7 segmentos
	// Enciendo display
	LPC_GPIO0 -> FIOCLR |= ( 1 << 9);
	// Enciendo 0 en segmentos
	LPC_GPIO2 -> FIOCLR |= 0x7F;
	LPC_GPIO2 -> FIOSET |= 64;
}




