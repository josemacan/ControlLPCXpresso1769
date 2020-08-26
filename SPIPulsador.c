// Alumno: Cancinos, José María

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

void configUART0 (void);
void enviar (uint8_t dato);
void configPulsador (void);
void asignacion (void);
void retardo (void);

uint8_t static const arrayval [] = {1, 2, 3, 4, 5};
uint8_t static redundante = 0;
uint8_t static contador = 0;
uint8_t static *punteroDir = 0;
uint8_t static datoEnvio = 0;
uint8_t static direccion = 0;


int main (void)
{

	int prueba = 0;
	contador = 0;
	datoEnvio = 0;
	direccion = 0;
	// CONFIGURACIÓN DE LED

	LPC_GPIO0 -> FIODIR |= (1 << 22); // P0[22] como salida = LED rojo
	LPC_GPIO0 -> FIOSET |= (1 << 22); // apago LED rojo

	LPC_GPIO3 -> FIODIR |= (1 << 26); // P3[26] como salida = LED azul
	LPC_GPIO3 -> FIOSET |= (1 << 26); // apago LED
    //

	/// ASIGNACIÓN
	asignacion();
	configUART0();
	configPulsador();

	LPC_SC -> EXTINT |= (1 << 0); // bajo bandera

	NVIC_EnableIRQ(EINT0_IRQn); // Activo interrupcion en NVIC

	while (1)
	{
		;
	}

	return 0;
}

void asignacion (void)
{
	int* puntero;
	puntero = (int*) 0x10000000;
	*puntero = 0;
		for (int k = 0; k < 160; k++)
		{
			*puntero = k;
			puntero = puntero + 0x1;

		}
		puntero = (int*) 0x10000000;
}
void configPulsador (void)
{
	LPC_SC -> EXTINT |= (1 << 0); // bajo bandera
	LPC_GPIO2 -> FIODIR &= ~(1<<10); // P2[10] -> ENTRADA
	LPC_SC -> EXTINT |= (1 << 0); // bajo bandera
	LPC_PINCON -> PINSEL4 |= (1 << 20); // P2[10] -> EINT0
	LPC_SC -> EXTINT |= (1 << 0); // bajo bandera
	LPC_SC -> EXTMODE |= ( 1 << 0 );  // Por flanco
	LPC_SC -> EXTINT |= (1 << 0); // bajo bandera
	LPC_SC -> EXTPOLAR |= (1 << 0); // EINT flanco de subida
	LPC_SC -> EXTINT |= (1 << 0); // bajo bandera
}

void configUART0 (void)
{
	redundante = LPC_UART0 -> LSR;

	//busco 9600 [Baudios] -> Freq de funcionamiento 16 * Baudios
	// Freq funcionamiento:

	LPC_SC -> PCONP |= (1 << 3); // Energizo UART0
	LPC_SC -> PCLKSEL0 &= ~(3 << 6); // PCLK =  CCLK / 4 = 25 [MHz]
	LPC_UART0 -> LCR |= (3 << 0); // Palabra: 8 bits
	LPC_UART0 -> LCR &= ~(1 << 2); // Bit de parada: 1
	LPC_UART0 -> LCR &= ~(1 << 3);		// Paridad desactivada
	LPC_UART0 -> LCR &= ~(1 << 6);      // Desactivado break control

	//LPC_PINCON -> PINSEL0 |= (1 << 20); 	// P0[10] = TX
	//LPC_PINCON -> PINSEL0 |= (1 << 22);		// P0[11] = RX

	LPC_UART0 -> LCR |= (1 << 7); // DLAB = 1

	LPC_UART0 -> DLL = 163;
	LPC_UART0 -> DLM = 0;

	LPC_UART0 -> LCR &= ~(1 << 7); // DLAB = 0

	LPC_PINCON -> PINSEL0 |= (5 << 20);	// P0[10]= Tx, P0[11]= Rx

	LPC_UART0 -> IER |= (1 << 1); // Activo interrupción por fin de transmisión



	NVIC_EnableIRQ (UART0_IRQn); // Activo interrupción de NVIC


}

void EINT0_IRQHandler (void)
{

	if ((LPC_SC -> EXTINT & (1 << 0)) == 1 )
	{
		//
		LPC_GPIO3 -> FIOCLR |= (1 << 26); // activo LED AZUL
		retardo ();
		LPC_GPIO3 -> FIOSET |= (1 << 26); // apago LED AZUL


		int* puntero;
		puntero = (int*) 0x10000000;
		contador = 0;
		datoEnvio = 0;

			datoEnvio = *puntero;
			enviar (datoEnvio);
			puntero = puntero + 0x01;
			contador = contador + 1;

	}
	redundante = LPC_UART0 -> LSR;

	LPC_SC -> EXTINT |= (1 << 0); // bajo bandera

}

void enviar(uint8_t medicion)								// Envío dato de medición por módulo UART
{
	int datoaenv = medicion;
	while((LPC_UART0->LSR&(1<<5))==0){};					// Espero hasta que el registro de salida esté vacío
	LPC_UART0->THR = medicion;								// Envío de datos
	redundante = LPC_UART0->LSR;
}

void retardo (void)
{
	for( int j = 0; j <10000000; j++)
	{
			;
	}
}

void UART0_IRQHandler (void)
{
	int* puntero;
	puntero = (int*) 0x10000000;

	redundante = LPC_UART0 -> LSR;

	if(contador < 161)
	{
		puntero = puntero + 0x01*(contador+1);
	}
	if(contador == 161)
	{
		redundante = LPC_UART0 -> LSR;
		LPC_GPIO0 -> FIOCLR |= (1 << 22); // apago LED ROJO

	}
	datoEnvio = *puntero;
	enviar (datoEnvio);
	contador = contador + 1;
}
