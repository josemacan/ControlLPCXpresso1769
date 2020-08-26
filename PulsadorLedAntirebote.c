// ALUMNO: CANCINOS, JOSÉ MARÍA

#define AddrFIO0DIR 0x2009C000;
#define AddrFIO0SET 0x2009C018;
#define AddrFIO0CLR 0x2009C01C;
#define AddrFIO0PIN 0x2009C014;
#define AddrPINSEL0 0x2009C020;
#define AddrPINSEL1 0x2009C024;
#define AddrPINMODE0 0x4002C040;
#define AddrFIO0MASK0 0x2009C010;
#define AddrIO0IntEnR 0x40028090;
#define AddrIO0IntEnF 0x40028094;
#define AddrIO0IntStatR 0x40028084;
#define AddrIO0IntStatF 0x40028088;
#define AddrIO0IntClr 0x4002808C;
#define AddrFIO0PIN1 0x2009C015;

#define AddrISER0 0xE000E100;

unsigned int volatile *const FIO0DIR = (unsigned int*)AddrFIO0DIR;
unsigned int volatile *const FIO0SET = (unsigned int*)AddrFIO0SET;
unsigned int volatile *const FIO0CLR = (unsigned int*)AddrFIO0CLR;
unsigned int volatile *const FIO0PIN = (unsigned int*)AddrFIO0PIN;
unsigned int volatile *const PINSEL0 = (unsigned int*)AddrPINSEL0;
unsigned int volatile *const PINSEL1 = (unsigned int*)AddrPINSEL1;
unsigned int volatile *const ISER0 = (unsigned int*)AddrISER0;
unsigned int volatile *const PINMODE0 = (unsigned int*)AddrPINMODE0;
unsigned int volatile *const FIO0MASK0 = (unsigned int*)AddrFIO0MASK0;
unsigned int volatile *const IO0IntEnR = (unsigned int*)AddrIO0IntEnR;
unsigned int volatile *const IO0IntEnF = (unsigned int*)AddrIO0IntEnF;
unsigned int volatile *const IO0IntStatR = (unsigned int*)AddrIO0IntStatR;
unsigned int volatile *const IO0IntStatF = (unsigned int*)AddrIO0IntStatF;
unsigned int volatile *const IO0IntClr = (unsigned int*)AddrIO0IntClr;
unsigned int volatile *const FIO0PIN1 = (unsigned int*)AddrFIO0PIN1;



int retardo(unsigned int); // prototipo de la función retardo
void EINT3_IRQHandler(void);
unsigned static int tiempo = 0;
unsigned static int prueba = 0;
unsigned static int numpin = 9;
unsigned static int index = 0;
unsigned static int pines [] = {9, 8, 7, 6, 0, 1 };

int antirebote ();

static int muestraB = 0;
static int muestraC = 0;
static int UltimoValorAntirebote = 0;

static int valorant2 = 0;
static int valorant1 = 0;
static int valoract = 0;


int main(void)
	{

	tiempo = 10000;

	// SELECCIONO MODO ENTRADA O SALIDA

	*FIO0DIR &= ~(1<<15);			  // PULSADOR.define al pin 22 del puerto 0	como de ENTRADA (SALIDA = 1)
	*FIO0DIR |= (1<<9);			  // define al pin 22 del puerto 0	como de SALIDA (SALIDA = 1)
	*FIO0DIR |= (1<<8);			  // SALIDA
	*FIO0DIR |= (1<<7);			  // SALIDA
	*FIO0DIR |= (1<<6);			  // SALIDA
	*FIO0DIR |= (1<<0);			  // SALIDA
	*FIO0DIR |= (1<<1);			  // SALIDA

	// SELECCIONO ACTIVACIÓN DE RESISTENCIA DE PULL UP

	*PINMODE0 &= ~(3<<30);		  // explicito pull up interno 0.15

	// ENMASCARAMIENTO DE BITS

	*FIO0MASK0 |= (0xffffffff<<0);	// enmascaro todos los bits
	*FIO0MASK0 &= ~(3<<0);
	*FIO0MASK0 &= ~(15<<6);
	*FIO0MASK0 &= ~(1<<15);			// desenmascaro los puertos seleccionados

	// ACTIVACIÓN DE INTERRUPCIONES

	*ISER0 |=(1<<21);			// activación de interrupciones externas

	// SELECCIÓN DE FLANCO DE INTERRUPCION DEL PULSADOR

	*IO0IntEnF|=(1<<15);
	*IO0IntEnR|=(1<<15);

	while(1)
			{
				;
			}

	return 0 ;

	}

void EINT3_IRQHandler(void)
{
	valorant2 = antirebote ();
	valorant1 = antirebote ();
	valoract = antirebote ();
	//if ( ( (valorant2 == valorant1) == valoract ) )
		{
		if ( ( (*IO0IntStatR >> 15) & 1 ) || ( (*IO0IntStatF >> 15) & 1 ) )
			{
				*FIO0CLR|=( 1 << numpin );

				if ( index == 5 )
				{
					index = 0;
				}
				else
				{
					index = index + 1;
				}

				numpin = pines [index];

				*FIO0SET|=( 1 << numpin );
			}
		}
	*IO0IntClr |= (1 << 15);

   return;
}

int retardo (unsigned int tiempo)
	{
		unsigned int i;
		for(i = 0; i < tiempo; i++);

		return 0;
	}

int antirebote (int muestraA)
{
	static int muestraB = 0;
	static int muestraC = 0;
	static int UltimoValorAntirebote = 0;

	UltimoValorAntirebote = (UltimoValorAntirebote) & ((muestraA) | (muestraB) | (muestraC)) | ((muestraA) & (muestraB) & (muestraC));
	muestraC = muestraB;
	muestraB = muestraA;

return UltimoValorAntirebote;
}
