// ALUMNO: CANCINOS, JOSÉ MARÍA


/*	EINT0 Flanco de subida -> cambio de frecuencia de LED
 *
 *  EINT1 Flanco de bajada -> cuenta desde 0 a 9 de display 7 segmentos
 *  			-> Cuando llega a 9 CAMBIAR A MODO INTERRUPCION POR NIVEL BAJO
 *  			-> Decremento de 9 a 0
 *
 *	P2.0 -> Segmento a
 *	P2.1 -> Segmento b
 *	P2.2 -> Segmento c
 *	P2.3 -> Segmento d
 *	P2.4 -> Segmento e
 *	P2.5 -> Segmento f
 *	P2.6 -> Segmento g
 *
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

// PUERTO 0

#define AddrFIO0DIR 0x2009C000;
#define AddrFIO0SET 0x2009C018;
#define AddrFIO0CLR 0x2009C01C;
#define AddrFIO0PIN 0x2009C014;
#define AddrPINSEL0 0x2009C020;  // P0.0 a P0.15
#define AddrPINSEL1 0x2009C024;  // P0.16 a P0.32
#define AddrPINMODE0 0x4002C040;
#define AddrPINMODE1 0x4002C044;

// Interrupciones puerto 0
#define AddrIO0IntEnR 0x40028090;
#define AddrIO0IntEnF 0x40028094;
#define AddrIO0IntStatR 0x40028084;
#define AddrIO0IntStatF 0x40028088;
#define AddrIO0IntClr 0x4002808C;

#define AddrISER0 0xE000E100;

// PUERTO 2

#define AddrPINSEL4 0x4002C010 // P2.0 a P2.15
#define AddrFIO2DIR 0x2009C040
#define AddrFIO2SET 0x2009C058
#define AddrFIO2CLR	0x2009C05C
#define AddrFIO2PIN 0x2009C054
#define AddrPINMODE4 0x4002C050

#define AddrIO2IntEnR 0x400280B0;
#define AddrIO2IntEnF 0x400280B4;
#define AddrIO2IntStatR 0x400280A4;
#define AddrIO2IntStatF 0x400280A8;
#define AddrIO2IntClr 0x400280AC;


// INTERRUPCIONES EXTERNAS

#define AddrEXTINT 0x400FC140
#define AddrEXTMODE 0x400FC148
#define AddrEXTPOLAR 0x400FC14C

// DEFINICIÓN DE PUNTEROS

unsigned int volatile *const FIO0DIR = (unsigned int*)AddrFIO0DIR;
unsigned int volatile *const FIO0SET = (unsigned int*)AddrFIO0SET;
unsigned int volatile *const FIO0CLR = (unsigned int*)AddrFIO0CLR;
unsigned int volatile *const FIO0PIN = (unsigned int*)AddrFIO0PIN;
unsigned int volatile *const PINSEL0 = (unsigned int*)AddrPINSEL0;
unsigned int volatile *const PINSEL1 = (unsigned int*)AddrPINSEL1;
unsigned int volatile *const ISER0 = (unsigned int*)AddrISER0;
unsigned int volatile *const PINMODE0 = (unsigned int*)AddrPINMODE0;
unsigned int volatile *const PINMODE1 = (unsigned int*)AddrPINMODE1;
unsigned int volatile *const IO0IntEnR = (unsigned int*)AddrIO0IntEnR;
unsigned int volatile *const IO0IntEnF = (unsigned int*)AddrIO0IntEnF;
unsigned int volatile *const IO0IntStatR = (unsigned int*)AddrIO0IntStatR;
unsigned int volatile *const IO0IntStatF = (unsigned int*)AddrIO0IntStatF;
unsigned int volatile *const IO0IntClr = (unsigned int*)AddrIO0IntClr;

unsigned int volatile *const PINSEL4 = (unsigned int*)AddrPINSEL4;
unsigned int volatile *const FIO2DIR = (unsigned int*)AddrFIO2DIR;
unsigned int volatile *const FIO2SET = (unsigned int*)AddrFIO2SET;
unsigned int volatile *const FIO2CLR = (unsigned int*)AddrFIO2CLR;
unsigned int volatile *const FIO2PIN = (unsigned int*)AddrFIO2PIN;
unsigned int volatile *const PINMODE4 = (unsigned int*)AddrPINMODE4;
unsigned int volatile *const IO2IntEnR = (unsigned int*)AddrIO2IntEnR;
unsigned int volatile *const IO2IntEnF = (unsigned int*)AddrIO2IntEnF;
unsigned int volatile *const IO2IntStatR = (unsigned int*)AddrIO2IntStatR;
unsigned int volatile *const IO2IntStatF = (unsigned int*)AddrIO2IntStatF;
unsigned int volatile *const IO2IntClr = (unsigned int*)AddrIO2IntClr;

unsigned int volatile *const EXTMODE = (unsigned int*)AddrEXTMODE;
unsigned int volatile *const EXTPOLAR = (unsigned int*)AddrEXTPOLAR;
unsigned int volatile *const EXTINT = (unsigned int*)AddrEXTINT;

unsigned static int modoled = 0;
unsigned static int mododisp = 0;

unsigned static int segDisplay [] = { 64, 121, 36, 48, 25, 18, 2, 120, 0, 24};

static int valorlogico = 0;

static int muestraB = 0;
static int muestraC = 0;
static int UltimoValorAntirebote = 0;
static int valor = 0;
static int prueba = 0;

int retardo (unsigned int);
void encenderDisplay (unsigned int unidad);

void llamado_antirebote (int );
int antirebote (int );
void retardado (void);


unsigned static int tiempo = 500000;

int main (void)
{
	//	CONFIGURACIÓN PUERTO 0 (LED, DISPLAY Y SEGMENTOS)
	*FIO0DIR |= ( 1 << 9); // define al pin 9 del puerto 0 como salida		// DISPLAY
	*FIO0DIR |= (1 << 8); // P0.8 como salida del LED
	*FIO2DIR |= ( 127 << 0); // define a pines del puerto 2 como salida		// SEGMENTOS

	*PINSEL0 &= ~( 15 << 16); // uso como GPIO P0.8 y P0.9
	*PINSEL4 &= ~(127 << 0); //P2.0 P2.1 P2.2 P2.3 P2.4 P2.5 P2.6 P2.7 seleccionado como GPIO
	*PINSEL0 &= ~( 15 << 16); // uso como GPIO

	// ENCIENDO DISPLAY
	*FIO0CLR|= (1<<9); 		// enciendo el display con un 0
	encenderDisplay(0);

	//	CONFIGURACIÓN PUERTOS EINT0
	*FIO2DIR &= ~( 1 << 10);		// P2.10 como entrada
	*PINSEL4 |= (1 << 20);     		// P2.10 como EINT0
	*EXTMODE |= (1<<0);				// Interrup EINT0 por flanco
	*EXTPOLAR |= (1<<0);			// Interrup EINT0 por subida

	// CONFIGURACION PUERTOS EINT1
	*FIO2DIR &= ~( 1 << 11);		// P2.11 como entrada
	*PINSEL4 |= (1 << 22);     		// P2.11 como EINT1
	*EXTMODE |= (1<<1);				// Interrup EINT1 por flanco
	*EXTPOLAR &= ~(1<<1);			// Interrup EINT1 por bajada

	// ACTIVACIÓN DE INTERRUPCIONES

	*ISER0 |= (1<<18);	// habilito EINT0
	*ISER0 |= (1<<19);	// habilito EINT1

	while(1)
	{
		*FIO0SET |= (1 << 8);
		retardo(tiempo);
		*FIO0CLR |= (1 << 8);
		retardo(tiempo);
	}

	return 0;
}


void EINT0_IRQHandler(void)
{
	if ( (*EXTINT >> 0) & 1)   // estado para interrupcion
	{

				if(modoled == 0)
				{
					tiempo = 500000;		//	modo 1 = 200000
					modoled = 1;
				}
				else
				{
					tiempo = 25000;
					modoled = 0;				//	modo 0 = 8000
				}

	}
	*EXTINT |= (1 << 0);// borro el flag de interrupcion EXTINT;

	return;
}

void EINT1_IRQHandler(void)
{

	unsigned int index = 0;
	if ( (*EXTINT >> 1) & 1)
	{
		if( mododisp == 0)
		{
				// estado para interrupcion por flanco de subida P2.10
			encenderDisplay(1);
			index = 0;

			while (index <= 9)
			{
					encenderDisplay(index);
					//encenderDisplay(index);
					retardo(250000);
					index = index + 1;
			}

			// CAMBIO DE MODO DE INTERRUPCION
			*ISER0 &= ~(1<<19);	// deshabilito EINT1
			*EXTINT |= (1 << 1);
			*EXTMODE|=~(1<<1); // EINT1 activo por nivel
			*EXTPOLAR&=~(1<<1);  //EINT1 por nivel bajo
			*ISER0 |= (1<<19);	// habilito EINT1

			mododisp = 1;

			*EXTINT |= (1 << 1); // borro bandera de interrupcion EINT1

		}
		if( mododisp == 1)
		{
			llamado_antirebote (11);
			if (valorlogico == 0)
			{
				index = 9;
				while(index > 0 && valorlogico == 0 )
					{
						index = index - 1;
						encenderDisplay(index);
						retardo(250000);
						llamado_antirebote (11);
					}
				index = 0;

						// RETORNO A MODO DE INTERRUPCION ORIGINAL
				*ISER0 &= ~(1<<19);	// deshabilito EINT1
				*EXTINT |= (1 << 1);
				*EXTMODE|=(1<<1); // EINT1 activo por nivel
				*EXTPOLAR=~(1<<1);  //EINT1 por bajo
				*ISER0 |= (1<<19);	// habilito EINT1

				mododisp = 0;

				*EXTINT |= (1 << 1); // borro bandera de interrupcion EINT1
			}

		}

		//*EXTINT |= (1 << 1); // borro bandera de interrupcion EINT1

	}


	return;
}


int retardo (unsigned int tiempo)
{
	unsigned int i;
	for (i = 0; i <  tiempo; i++)  // lazo de demora
	{}
	return 0;
}

void encenderDisplay (unsigned int unidad )
	{

	*FIO2CLR |= (127 << 0);			// enciendo 8 en display 1
	*FIO0SET |= (1 << 8);			// apago display 2
	*FIO0CLR |= (1 << 9);			// enciendo display 1
	*FIO2SET |= (segDisplay[unidad] << 0);			// enciendo numero en display 0

		return;
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
	valorlogico = antirebote (*FIO2PIN&(1<<pin));
	retardado();
	valorlogico =	antirebote (*FIO2PIN&(1<<pin));
	retardado();
	valorlogico =	antirebote (*FIO2PIN&(1<<pin));
	retardado();
}

void retardado (void)
{
	for (int j = 0; j < 20000; j++)
	{
		;
	}
}


