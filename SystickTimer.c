// ALUMNO: CANCINOS, JOSÉ MARÍA

// MULTIPLEXADO DE 20 ms de dos displays
/*
 *	NUMEROS=
 *
 *	0 -> 0111111 b = 63
 *	1 -> 0000110 b = 6
 *	2 -> 1011011 b = 91
 *	3 -> 1001111 b = 79
 *	4 -> 1100110 b = 102
 *	5 -> 1101101 b = 109
 *	6 -> 1111101 b = 125
 *	7 -> 0000111 b = 7
 *	8 -> 1111111 b = 127
 *	9 -> 1101111 b = 111
 *
 */

#define AddrFIO0DIR 0x2009C000;
#define AddrFIO0SET 0x2009C018;
#define AddrFIO0CLR 0x2009C01C;
#define AddrFIO0PIN 0x2009C014;
#define AddrSTCTRL 0xE000E010;
#define AddrSTRELOAD 0xE000E014;

#define AddrFIO2DIR 0x2009C040;
#define AddrFIO2SET 0x2009C058;
#define AddrFIO2CLR 0x2009C05C;

#define AddrIO0IntEnR 0x40028090;
#define AddrIO0IntEnF 0x40028094;
#define AddrIO0IntStatR 0x40028084;
#define AddrIO0IntStatF 0x40028088;

#define AddrIO0IntClr 0x4002808C;

#define AddrISER0 0xE000E100;
#define AddrPINMODE0 0x4002C040;

unsigned int volatile *const FIO0DIR = (unsigned int*)AddrFIO0DIR;
unsigned int volatile *const FIO0SET = (unsigned int*)AddrFIO0SET;
unsigned int volatile *const FIO0CLR = (unsigned int*)AddrFIO0CLR;
unsigned int volatile *const FIO0PIN = (unsigned int*)AddrFIO0PIN;
unsigned int volatile *const STCTRL = (unsigned int*)AddrSTCTRL;
unsigned int volatile *const STRELOAD = (unsigned int*)AddrSTRELOAD;

unsigned int volatile *const FIO2DIR = (unsigned int*)AddrFIO2DIR;
unsigned int volatile *const FIO2SET = (unsigned int*)AddrFIO2SET;
unsigned int volatile *const FIO2CLR = (unsigned int*)AddrFIO2CLR;

unsigned int volatile *const ISER0 = (unsigned int*)AddrISER0;
unsigned int volatile *const IO0IntEnR = (unsigned int*)AddrIO0IntEnR;
unsigned int volatile *const IO0IntStatR = (unsigned int*)AddrIO0IntStatR;
unsigned int volatile *const IO0IntClr = (unsigned int*)AddrIO0IntClr;
unsigned int volatile *const IO0IntStatF = (unsigned int*)AddrIO0IntStatF;
unsigned int volatile *const IO0IntEnF = (unsigned int*)AddrIO0IntEnF;



unsigned int volatile *const PINMODE0 = (unsigned int*)AddrPINMODE0;

////////////////////// VARIABLES

volatile unsigned int SysTickCnt; // Contador de interrupciones
static unsigned int modo = 0;
static unsigned int cclk = 0;
//unsigned static int segDisplay [] = { 63, 6, 91, 79, 102,
//										109, 125, 7, 127, 111};
unsigned static int segDisplay [] = { 64, 121, 36, 48, 25,
										18, 2, 120, 0, 24};
unsigned static int tiempompx = 50;
unsigned static int tiempomax = 95;
unsigned static int tiempomin = 5;
unsigned static int unidad = 0;
unsigned static int decena = 0;
unsigned static int prueba = 0;

static int valorlogico = 0;

static int muestraB = 0;
static int muestraC = 0;
static int UltimoValorAntirebote = 0;
static int valor = 0;


// PROTOTIPOS DE LAS FUNCIONES

void SysTick_Handler (void);
void EINT3_IRQHandler (void);
void retardo (unsigned int tick);
void retardado (void);
void encenderDisplay (void);
void cambio (unsigned int numero);
void cambioSystick (unsigned int tmpx);
void llamado_antirebote (int );
int antirebote (int );

int main (void)
{
	// CONFIGURACIÓN DE SYSTICK

	tiempompx = 50;   // 50 Define el tiempo en mseg del SysTick
	cclk = 4000000; // Freq de funcionamiento de reloj del CPU
	unsigned int tick = 1;


	*STCTRL |= (1 << 2); // define la fuente de reloj del SysTick
	*STRELOAD = (cclk/1000)*tiempompx-1; // valor de recarga del temp Sys Tick
	*STCTRL |= (1 << 1);


	// CONFIGURACIÓN P0.15	PULSADOR
	*FIO0DIR&=~(1<<15); 	// define al pin 15 del puerto 0 como entrada	// PULSADOR
	*PINMODE0&=~(3<<30); 	//pull up interno de puerto 0.15
	*IO0IntEnF|=(1<<15);	// interrupcion por flanco de bajada	// PULSADOR

	//	CONFIGURACION P0.0 PULSADOR
	*FIO0DIR&=~(1<<0); 	// define al pin 0 del puerto 0 como entrada	// PULSADOR
	*PINMODE0&=~(3<<0); 	//pull up interno de puerto 0.0
	*IO0IntEnF|=(1<<0);	// interrupcion por flanco de bajada	// PULSADOR
	//*************

	//	CONFIGURACIÓN OTROS PUERTOS
	*FIO0DIR |= ( 1 << 9); // define al pin 9 del puerto 0 como salida		// DISPLAY
	*FIO0DIR |= ( 1 << 8); // define al pin 8 del puerto 0 como salida		// DISPLAY
	*FIO2DIR |= ( 127 << 0); // define a pines del puerto 2 como salida		// SEGMENTOS

	// ACTIVACIÓN DE INTERRUPCIONES EINT3

	*ISER0 |=(1<<21);			// activación de interrupciones externas	// INT EINT3


	while (1)
	{
		encenderDisplay();
		retardo(tick);
		encenderDisplay();
		retardo(tick);
	}

	return 0;
}

void EINT3_IRQHandler(void) // Manejador de interrupciones SysTick
{

	if ( (*IO0IntStatF >> 15) & 1 )
		{
		llamado_antirebote (15);
		if(valorlogico == 0)
			{
						if(tiempompx == tiempomax)
						{
							;
						}
						else
						{
							tiempompx = tiempompx + 1;
						}
		}
			*IO0IntClr |= (1 << 15);
		}

	if ( (*IO0IntStatF >> 0) & 1 )
		{
		llamado_antirebote (0);
		if(valorlogico == 0)
			{
			*FIO0CLR |= ( 1 << 22); // define al pin 22 del puerto 0 como salida	// LED

					if(tiempompx == tiempomin)
					{
						;
					}
					else
					{
						tiempompx = tiempompx - 1;
					}
			}
			*IO0IntClr |= (1 << 0);
		}
	cambioSystick (tiempompx);
}

void SysTick_Handler(void) // Manejador de interrupciones SysTick
{
	SysTickCnt++;
}
void retardo (unsigned int tick)
{
	SysTickCnt = 0;
	*STCTRL |= 1;   // habilito contador de SysTick

	while (SysTickCnt < tick)
	{
	}

	*STCTRL |= 0;   // Deshabilito contador de SysTick


}

void encenderDisplay (void )
	{

	cambio (tiempompx);

	if (modo == 0)
		{
			*FIO2CLR |= (127 << 0);			// enciendo 8 en display 1
			*FIO0SET |= (1 << 8);			// apago display 2
			*FIO0CLR |= (1 << 9);			// enciendo display 1
			*FIO2SET |= (segDisplay[unidad] << 0);			// enciendo numero en display 0

			modo = 1;
		}
		else
		{
			*FIO2CLR |= (127 << 0);			// enciendo 8 en display 1
			*FIO0SET |= (1 << 9);			// apago display 1
			*FIO0CLR |= (1 << 8);			// enciendo display 2
			*FIO2SET |= (segDisplay[decena] << 0);			// enciendo 8 en display 1

			modo = 0;
		}


		return;
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

void cambioSystick (unsigned int tmpx)
{
	*STCTRL |= (1 << 2);
	*STRELOAD = (cclk/1000)*tmpx-1; // valor de recarga del temp Sys Tick
	*STCTRL |= (1 << 1);
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
	valorlogico = antirebote (*FIO0PIN&(1<<pin));
	retardado();
	valorlogico =	antirebote (*FIO0PIN&(1<<pin));
	retardado();
	valorlogico =	antirebote (*FIO0PIN&(1<<pin));
	retardado();
}

void retardado (void)
{
	for (int j = 0; j < 20000; j++)
	{
		;
	}
}
