// ALUMNO: CANCINOS, JOSÉ MARÍA

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

uint32_t redundante = 0;
unsigned int *dir1;
unsigned int *dir2;

uint8_t *puntero_dir;

uint8_t direccion;
uint8_t datosalida;
uint8_t datoentrada;

void configSPIMaestro (void);
void configEINT0 (void);
uint8_t enviar (uint8_t);

int main(void) {

	dir1 = (unsigned int *)0x10000100;
	*dir1 = 0xC1357931;

	dir2 = (unsigned int *)0x10000104;
	*dir2 = 0x12345678;

	puntero_dir = &dir1;


	LPC_SC -> CCLKCFG = 25; // Divisor PLL = 25 -> CCLK = 32 [MHz]
	LPC_GPIO0 -> FIODIR &= ~(1 << 22); // P[0]22 como salida
	LPC_GPIO0 -> FIOSET |= (1 << 22); // apago LED rojo
	unsigned int cclk_actual = SystemCoreClock;
    while(1) {
        ;
    }
    return 0 ;


}

// MAESTRO
void configSPIMaestro (void)
{
	uint32_t valorpclkspi;
	// CONFIGURACIÓN DE RELOJ DE PERIFÉRICO
	valorpclkspi = 4000000; // PCLK = 500 [KHz]
	LPC_SC -> PCONP |= (1 << 8); // Energizo SPI
	LPC_SC -> PCLKSEL0 |= (3 << 16); // PCLKSpi = 4 [MHz]

	// CONFIGURACIÓN DE PUERTOS
	LPC_PINCON -> PINSEL0 |= (3 << 30); // Elijo SCK
	LPC_PINCON -> PINSEL1 |= (3<<0); // SSEL cuando esclavo
    LPC_PINCON -> PINSEL1 |= (3 << 4); // MOSI
    LPC_PINCON -> PINSEL1 |= (3 << 2); // MISO

    // CONFIGURACIÓN DE FREQ DE TRABAJO


    // CONFIGURACIÓN DE MODOS
    LPC_SPI -> SPCR |= (1 << 3); // CPHA = 1
    LPC_SPI -> SPCR |= (1 << 4); // CPOL = 1

    // SELECCIÓN DE MOODO ESCLAVO/MAESTRO
    LPC_SPI -> SPCR &= ~(1 << 5); // Modo ESCLAVO

    // SELECCIÓN DE TAMAÑO DE PALABRA
    // Esclavo siempre envía datos de 8 bits.

    // BAJADA DE BANDERAS
    redundante = LPC_SPI -> SPSR;
    redundante = LPC_SPI -> SPDR;

    // ACTIVO INTERRUPCIONES
    LPC_SPI -> SPCR |= (1 << 7); // habilito interrupciones por
    							// fin de transmisión o problema
    NVIC_EnableIRQ(SPI_IRQn);   // activo interrrupcion en NVIC
}


uint8_t enviar (uint8_t datosalida)
{
	LPC_SPI -> SPDR = datosalida; // cargo el dato
	while ( ((LPC_SPI -> SPSR & (1 << 7)) == 0)  )
	{
		; // espero hasta que el dato se haya enviado y bandera = 1
	}
	datoentrada = LPC_SPI -> SPDR;
	return datoentrada;
}
void SPI_IRQHandler (void)
{
	if ( ((LPC_SPI -> SPSR & (1 << 5)) == 1) || ((LPC_SPI -> SPSR & (1 << 6)) == 1) )
	{
		LPC_GPIO0 -> FIOCLR |= (1 << 22); // enciendo LED rojo
	}

	redundante = LPC_SPI -> SPSR;
}



/* 	Baudrate: 500 [Kbit/seg] ->
 *  PCLKspi = 8 * baudrate = 4 [MHz]
 *  Freq Maestro = SCKFreq = 500 [KHz] -> asigno divisor SPI
 *  Freq Esclavo = SCKFreq = 4 [MHz] -> No asigno prescaler
 */


/*
 * Tramas: 8 bits
 * Baurate: 500 [Kbit/seg]
 * CPOL = 1
 * CPHA = 1 (modo 3)
 * Interrupciones en master y esclavo
 * Direcciones: 0x1000 0100 -> Maestro: 0xD412D824
 * 							-> Esclavo: 0xC1357931
 * 				0x1000 0104 -> Maestro: 0xFAF02468
 * 							-> Esclavo: 0x12345678
 * Uso de EINT0 -> interrupcion por EINT0 -> inicio la comunicacion
 * Si read Overrun o Write collision -> LED ENCENDIDO indefinidamente
 */

