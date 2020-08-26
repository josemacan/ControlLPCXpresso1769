// ALUMNO: CANCINOS JOSÉ MARÍA

/*
 * P1.18 -> PULSADOR  CAP1.0
 * TIMER0 -> CAPTURE
 * TIMER1 -> MATCH
 * P2.0 a P2.7 -> 7 segmentos
 * P0.8 y P0.9 -> control multiplexacion de displays
 *
 */

// PUERTO 0
unsigned int volatile *const FIO0DIR = (unsigned int*)0x2009C000;
unsigned int volatile *const FIO0SET = (unsigned int*)0x2009C018;
unsigned int volatile *const FIO0CLR = (unsigned int*)0x2009C01C;
//unsigned int volatile *const PINSEL0 = (unsigned int*)0x4002C000;
//unsigned int volatile *const PINMODE0 = (unsigned int*)0x4002C040;

// PUERTO 1

unsigned int volatile *const FIO1DIR = (unsigned int*)0x2009C020;
/*
unsigned int volatile *const FIO1SET = (unsigned int*)0x2009C038;
unsigned int volatile *const FIO1CLR = (unsigned int*)0x2009C03C;

*/
unsigned int volatile *const FIO1PIN = (unsigned int*)0x2009C034;
unsigned int volatile *const PINMODE3 = (unsigned int*)0x4002C04C;
unsigned int volatile *const PINSEL3 = (unsigned int*)0x4002C00C;

// PUERTO 2
unsigned int volatile *const FIO2DIR = (unsigned int*)0x2009C040;
unsigned int volatile *const FIO2SET = (unsigned int*)0x2009C058;
unsigned int volatile *const FIO2CLR = (unsigned int*)0x2009C05C;
//unsigned int volatile *const PINMODE4 = (unsigned int*)0x2009C040;
//unsigned int volatile *const PINSEL4 = (unsigned int*)0x4002C010;

// PUERTO 3
unsigned int volatile *const FIO3DIR = (unsigned int*)0x2009C060;
unsigned int volatile *const FIO3SET = (unsigned int*)0x2009C078;
unsigned int volatile *const FIO3CLR = (unsigned int*)0x2009C07C;


// INTERRUPCIONES
unsigned int volatile *const ISER0 = (unsigned int*)0xE000E100;

// PERIFÉRICO
unsigned int volatile *const PCONP = (unsigned int*)0x400FC0C4;
unsigned int volatile *const PCLKSEL0 = (unsigned int*)0x400FC1A8 ;

// TIMER 0
unsigned int volatile *const T0EMR = (unsigned int*)0x4000403C ;
unsigned int volatile *const T0MCR = (unsigned int*)0x40004014 ;
unsigned int volatile *const T0MR0 = (unsigned int*)0x40004018 ;
unsigned int volatile *const T0TCR = (unsigned int*)0x40004004 ;
unsigned int volatile *const T0IR = (unsigned int*)0x40004000 ;

// TIMER 1

//unsigned int volatile *const T1EMR = (unsigned int*)0x4000803C ;
unsigned int volatile *const T1MCR = (unsigned int*)0x40008014 ;
unsigned int volatile *const T1MR0 = (unsigned int*)0x40008018 ;
unsigned int volatile *const T1TCR = (unsigned int*)0x40008004 ;
unsigned int volatile *const T1CCR = (unsigned int*)0x40008028 ;
unsigned int volatile *const T1CR0 = (unsigned int*)0x4000802C;
unsigned int volatile *const T1IR = (unsigned int*)0x40008000;


//// VARIABLES

unsigned static int modo = 0;
unsigned static int prueba = 0;
unsigned static int unidad = 0;
unsigned static int decena = 0;
unsigned static int segDisplay [] = { 64, 121, 36, 48, 25,
											18, 2, 120, 0, 24};
unsigned static int valorlogico = 0;

unsigned static int muestraB = 0;
unsigned static int muestraC = 0;
unsigned static int UltimoValorAntirebote = 0;
unsigned static int valorcapturado = 0;

//// PROTOTIPOS DE FUNCIONES

void cambio (unsigned int );
//void encenderDisplay (unsigned int );
void TIMER0_IRQHandler (void);
void retardado (void);
void llamado_antirebote (int );
int antirebote (int );


int main (void)
{
	// PUERTOS
		*FIO0DIR |= ( 1 << 9); // define al pin 9 del puerto 0 como salida		// DISPLAY
		*FIO0DIR |= ( 1 << 8); // define al pin 8 del puerto 0 como salida		// DISPLAY
		*FIO2DIR |= ( 127 << 0); // define a pines del puerto 2 como salida		// SEGMENTOS
		*FIO1DIR &= ~(1<<18);		// P1.18 / CAP1.0 como ENTRADA
		*PINMODE3 &= ~(3<<4);		// Explicito pull up interno

	// TIMER 0
		*PCONP |= (1<<1);			// enciende el periférico de Timer0
		*PCLKSEL0 |= (2<<2); 		// reloj de periférico: reloj del sistema CCLK/ 2 -> 1 / 2MHz = 0,5 [useg]
		*PINSEL3 |= (3<<24); 		//	match 0.0
		*T0MCR |= 3; 				// (11) b0 = genera interrupcion cuando MR0 = TC
		*T0MR0 = 400000;			    // valor del Match register 0. 							si busco multiplexado de 20 [mseg] -> Match: 40.000

	// 	   TIMER 1
		*PCONP |= (1<<2);			// enciende el periférico de Timer1
		*PCLKSEL0 |= (2<<4); 		// reloj de periférico: reloj de timer 1 CCLK = 4 [MHz]
		*PINSEL3 |= (3<<4);			// Capture 1.0
		*T1CCR |= (1<<1);			// captura por flanco de bajada en CAP1.0.
		*T1CCR |= (1<<2);			// una carga de CR0 genera una interrupcion
		*T1MCR |= (1<<1);			// Reinicia cuando llega llega a MATCH.
		*T1MR0 = 99;			// Timer1 Match0


	//		INTERRUPCIONES
	*ISER0 |= (1<<1);			// Activo interrupcion por Timer0
	*ISER0 |= (1<<2);			// Activo interrupcion por Timer1
	//

	// ACTIVO TIMERS
	*T1TCR |= 1;  //TIMER1 ENABLE
	*T0TCR |= 1; //TIMER0 ENABLE

	//********LEDS DE PRUEBA*********
	*FIO0DIR |= (1<<22);		// P0.22 como SALIDA
	*FIO0SET |= (1<<22);		// P0.22 como SALIDA
	*FIO3DIR |= (1<<26); 		// SALIDA LED
	*FIO3SET |= (1<<26);
	//*****************************
	while(1)
	{
		;
	}

	return 0;
}


void TIMER1_IRQHandler (void)		// ISR del TIMER0
{
	if((*T1IR >> 4) & 1)
	{

		llamado_antirebote(18);
		if (valorlogico == 0)
		{
			valorcapturado = *T1CR0;
			cambio(valorcapturado);

			*FIO0CLR |= (1<<22);		// P0.22 como SALIDA
			retardado();
			retardado();
			*FIO0SET |= (1<<22);		// P0.22 como SALIDA
		}
	}
	*T1IR |= (1 << 4); // BORRA LA INTERRUPCION Bandera de interrupcion para captura en canal 0
}


void cambio (unsigned int numero)
{
	unidad = 0;
	decena = 0;
	// CONVIERTO DECENA
	while (numero >= 10)
	{
		numero = numero - 10;
		decena = decena + 1;
	}
	while(numero > 0)
	{
		numero = numero - 1;
		unidad = unidad + 1;
	}
}


void TIMER0_IRQHandler (void)		// ISR del TIMER0
{

	if (modo == 0)
		{

			*FIO2CLR |= (127 << 0);			// enciendo 8 en display 1
			*FIO0SET |= (1 << 8);			// apago display 2
			*FIO0CLR |= (1 << 9);			// enciendo display 1
			*FIO2SET |= (segDisplay[unidad] << 0);			// enciendo numero en display 0

			modo = 1;

			//////////////
			*FIO3CLR |= (1<<26); 		// LED AZUL
			//////////////
		}
		else
		{
			*FIO2CLR |= (127 << 0);			// enciendo 8 en display 1
			*FIO0SET |= (1 << 9);			// apago display 1
			*FIO0CLR |= (1 << 8);			// enciendo display 2
			*FIO2SET |= (segDisplay[decena] << 0);			// enciendo 8 en display 1

			modo = 0;

			////////////////
			*FIO3SET |= (1<<26);		// LED AZUL
			//////////
		}

	*T0IR |= (1<<0); // BORRA LA INTERRUPCION Bandera de interrupcion para match en canal 0
}

int antirebote (int muestraA)
{

UltimoValorAntirebote = (UltimoValorAntirebote) & ((muestraA) | (muestraB) | (muestraC)) | ((muestraA) & (muestraB) & (muestraC));
muestraC = muestraB;
muestraB = muestraA;

return UltimoValorAntirebote;
}

void llamado_antirebote (int pin)
{
	valorlogico = antirebote (*FIO1PIN&(1<<pin));
	retardado();
	valorlogico =	antirebote (*FIO1PIN&(1<<pin));
	retardado();
	valorlogico =	antirebote (*FIO1PIN&(1<<pin));
	retardado();
}

void retardado (void)
{
	for (int j = 0; j < 20000; j++)
	{
		;
	}
}


