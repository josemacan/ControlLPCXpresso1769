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
	*dir1 = 0xD412D824;

	dir2 = (unsigned int *)0x10000104;
	*dir2 = 0xFAF02468;

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

    LPC_PINCON -> PINSEL1 |= (3 << 4); // MOSI
    LPC_PINCON -> PINSEL1 |= (3 << 2); // MISO

    LPC_PINCON -> PINSEL1 &= ~(3 << 0); //SS
    LPC_GPIO0 -> FIODIR |= (1 << 16); // SS control de esclavo
    LPC_GPIO0 -> FIOSET |= (1 << 16); // 1 en P0[|6] -> desactivo SS

    // CONFIGURACIÓN DE FREQ DE TRABAJO
    // SPCCR = Val_pclk_spi / sck_freq = 4 [MHz] / 8 = 500 [KHz]
    LPC_SPI -> SPCCR = 8;

    // CONFIGURACIÓN DE MODOS
    LPC_SPI -> SPCR |= (1 << 3); // CPHA = 1
    LPC_SPI -> SPCR |= (1 << 4); // CPOL = 1

    // SELECCIÓN DE MOODO ESCLAVO/MAESTRO
    LPC_SPI -> SPCR |= (1 << 5); // Modo MAESTRO

    // SELECCIÓN DE TAMAÑO DE PALABRA
    LPC_SPI -> SPCR &= ~(0 << 2); // Tx y Rx a 8 bits.

    // BAJADA DE BANDERAS
    redundante = LPC_SPI -> SPSR;
    redundante = LPC_SPI -> SPDR;

    // ACTIVO INTERRUPCIONES
    LPC_SPI -> SPCR |= (1 << 7); // habilito interrupciones por
    							// fin de transmisión o problema
    NVIC_EnableIRQ(SPI_IRQn);   // activo interrrupcion en NVIC


}

void configEINT0 (void)
{
	LPC_GPIO2 -> FIODIR &= ~(1 << 10); // P2[10] como entrada
	LPC_PINCON -> PINSEL4 |= (2 << 10); // P2[10] como EINT0

	LPC_SC -> EXTMODE &= ~(1 << 0); // EINT0 por nivel
	LPC_SC -> EXTPOLAR  &= ~(1 << 0); // EINT0 por nivel bajo

	NVIC_EnableIRQ (EINT0_IRQn);
}

void EINT0_IRQHandler (void)
{
	uint8_t i = 0;
	if ((LPC_SC -> EXTINT & (1 << 1)) == 1) // chequeo de bandera
	{
	    LPC_GPIO0 -> FIOCLR |= (1 << 16); // 1 en P0[|6] ->
	    								  // COMIENZA LA COMUNICACION
	    for (i = 0; i < 8; i++)
	    {
	    	direccion = *puntero_dir;	// datosalida tiene el valor del dato
	    								// de la dirección a que apunta puntero
	    	datosalida = direccion;
	    	direccion = enviar(datosalida); 	//
	    	*puntero_dir = &puntero_dir + 0x01;	// aumento en 1 la dirección a
	    									// la que apunta
	    }

	    LPC_GPIO0 -> FIOSET |= (1 << 16); // 1 en P0[|6] -> desactivo SS

	}

	LPC_SC -> EXTINT |= (1 << 0); // bajo bandera de interrupcion
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
