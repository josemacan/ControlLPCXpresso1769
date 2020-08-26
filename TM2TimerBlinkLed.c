// ALUMNO: CANCINOS JOSÉ MARÍA
/*
 * 1) ENCENDIDO DEL TIMER:
 * PCONP = encender o apagar el TIMER 0, 1, 2 o 3.
 * Después de un reset, Timer0 y 1 están activados (1)
 *
 * 2) ELECCIÓN DEL CLOCK DEL TIMER
 * PCLKSEL0: config Timer0 y Timer1
 * PCLKSEL1: config Timer2 y Timer3
 *
 * 3) PINES DE TIMER
 * PINSEL: elegir los pines de timer en PINSEL y PINMODE
 *
 * 4) INTERRUPCIONES:
 * T0/1/2/3MCR: config match
 * T0/1/2/3CCR: config. capture
 */

#define AddrFIO0DIR 0x2009C000;
#define AddrFIO0SET 0x2009C018;
#define AddrFIO0CLR 0x2009C01C;
#define AddrFIO0PIN 0x2009C014;

#define AddrIO0IntEnR 0x40028090;
#define AddrIO0IntEnF 0x40028094;
#define AddrIO0IntStatR 0x40028084;
#define AddrIO0IntStatF 0x40028088;
#define AddrIO0IntClr 0x4002808C;
#define AddrISER0 0xE000E100;
#define AddrPINMODE0 0x4002C040;
#define AddrPINSEL0	 0x4002C000;

#define AddrFIO3DIR 0x2009C060;
#define AddrFIO3SET 0x2009C078
#define AddrFIO3CLR 0x2009C07C;

#define AddrFIO2DIR 0x2009C040;
#define AddrPINMODE4 0x4002C050;


#define AddrFIO0MASK0 0x2009C010;

#define AddrT0PR 0x4000400C
#define AddrT0CTCR 0x40004070

#define AddrPCONP    0x400FC0C4;
#define AddrPCLKSEL0 0x400FC1A8;
#define AddrPINSEL3  0x4002C00C;
#define AddrT1EMR    0x4000803C;	// Registro de match externo. Controla los pines de Match
#define AddrT1MCR	 0x40008014;	// Registro de control de Match
#define AddrT1MR0	 0x40008018;	// Registro 0 de Match

#define AddrT1TCR 	 0x40008004;	// Registro de control de TMR
#define AddrT1IR	 0x40008000;	// Registro de Interrupcion
									// para borrar interrupciones de TMR
#define AddrT1PC	 0x40008010;	//Prescale couter: contador incrementado hasta igual a PR
#define AddrT1TC	 0x40008008;		// Timer counter: incrementado cada vez que PR+1
#define AddrT1PR	 0x4000800C;

#define AddrPINSEL4	 	 0x4002C010;
#define AddrIO2IntEnF	 0x400280B4;
#define AddrIO2IntClr	 0x400280AC;
#define AddrIO2IntStatF	 0x400280A8;

// PUNTEROS

unsigned int volatile *const FIO0DIR = (unsigned int*)AddrFIO0DIR;
unsigned int volatile *const FIO0SET = (unsigned int*)AddrFIO0SET;
unsigned int volatile *const FIO0CLR = (unsigned int*)AddrFIO0CLR;
unsigned int volatile *const FIO0PIN = (unsigned int*)AddrFIO0PIN;
unsigned int volatile *const IO0IntEnR = (unsigned int*)AddrIO0IntEnR;
unsigned int volatile *const IO0IntEnF = (unsigned int*)AddrIO0IntEnF;
unsigned int volatile *const IO0IntStatR = (unsigned int*)AddrIO0IntStatR;
unsigned int volatile *const IO0IntStatF = (unsigned int*)AddrIO0IntStatF;
unsigned int volatile *const IO0IntClr = (unsigned int*)AddrIO0IntClr;
unsigned int volatile *const ISER0 = (unsigned int*)AddrISER0;
unsigned int volatile *const PINSEL0 = (unsigned int*)AddrPINSEL0;
unsigned int volatile *const PINMODE0 = (unsigned int*)AddrPINMODE0;
unsigned int volatile *const T0CTCR = (unsigned int*)AddrT0CTCR;
unsigned int volatile *const PCONP = (unsigned int*)AddrPCONP;
unsigned int volatile *const PCLKSEL0 = (unsigned int*)AddrPCLKSEL0 ;
unsigned int volatile *const PINSEL3 = (unsigned int*)AddrPINSEL3 ;
unsigned int volatile *const T1EMR = (unsigned int*)AddrT1EMR ;
unsigned int volatile *const T1MCR = (unsigned int*)AddrT1MCR ;
unsigned int volatile *const T1MR0 = (unsigned int*)AddrT1MR0 ;

unsigned int volatile *const T1TCR = (unsigned int*)AddrT1TCR ;

unsigned int volatile *const T1IR = (unsigned int*)AddrT1IR ;
unsigned int volatile *const T1PR = (unsigned int*)AddrT1PR ;
unsigned int volatile *const T1PC = (unsigned int*)AddrT1PC ;
unsigned int volatile *const T1TC = (unsigned int*)AddrT1TC ;

unsigned int volatile *const FIO3DIR = (unsigned int*)AddrFIO3DIR;
unsigned int volatile *const FIO3SET = (unsigned int*)AddrFIO3SET;
unsigned int volatile *const FIO3CLR = (unsigned int*)AddrFIO3CLR;
unsigned int volatile *const PINSEL4 = (unsigned int*)AddrPINSEL4;
unsigned int volatile *const IO2IntClr = (unsigned int*)AddrIO2IntClr;
unsigned int volatile *const IO2IntEnF = (unsigned int*)AddrIO2IntEnF;

unsigned int volatile *const FIO2DIR = (unsigned int*)AddrFIO2DIR;
unsigned int volatile *const PINMODE4 = (unsigned int*)AddrPINMODE4;
unsigned int volatile *const IO2IntStatF = (unsigned int*)AddrIO2IntStatF;



// VARIABLES

static int valorlogico = 0;
static int muestraB = 0;
static int muestraC = 0;
static int UltimoValorAntirebote = 0;

// PROTOTIPOS DE FUNCIONES

void EINT3_IRQHandler(void);
void cambioDePrescaler (void);
void retardado (void);
void llamado_antirebote (int );
int antirebote (int );

// PROGRAMA PRINCIPAL

int main (void)
{
*PCONP |= (1<<2);	// enciende el periférico de Timer1

	 /*	------------------ ACTIVIDAD Nº3 -----------------
	// 	ELECCIÓN DE VALOR DEL CLOCK DE TIMER1
	 *
	 * 	PIN 5-4 : 00= CCLK/4
	 * 			  01= CCLK
	 * 			  10= CCLK/2
	 * 			  11 = CCLK/8
	 *
	 */
		*PCLKSEL0 |= (2<<4); // reloj de periférico: reloj de timer 1
	 //---------------------------------------


	*PINSEL3 |= (3<<24); 		//match 0.0
	*T1EMR |= (3<<4);			// conmuta el bit de Match externo
	*T1MCR |= 3; 				// (11) b0 = genera interrupcion cuando MR0 = TC
								//      b1 = TC se resetea si hace match con MR0
								// Reset en MR0: el Timer resetea si MR0 hace match
					 	 	 	// Interrupcion cuando MR0 hace match con el valor de TC

	*T1MR0 = 364500; 			// valor del Match register 0.
	*ISER0 |= (1<<2);			// ACTIVO INTERRUPCIONES EXTERNAS
	*T1TCR |= 1; 				// HABILITO EL REGISTRO DE CONTROL DE TIMER

	*FIO0DIR |= (1<<22);		// P0.22 como SALIDA

// PULSADOR
	*FIO0DIR &= ~(1<<1);		// P0.1 como ENTRADA
	*PINMODE0 &= ~(3<<2);		// P0.1 con pull up interno activado
	*IO0IntEnF|=(1<<1);			// Activo interrupcion P0.0 por flanco bajada
	*ISER0 |=(1<<21);			// activo las interrupciones

	while(1)
	{
		;
	}

	return 0;
}

// INTERRUPCION POR TIMER1

void TIMER1_IRQHandler (void)		// ISR del TIMER0
{
	static int i = 0;

	if (i == 0)
	{
		*FIO0SET |= (1<<22);
		i = 1;
	}
	else if (i==1)
	{
		*FIO0CLR |= (1<<22);
		i = 0;
	}
	*T1IR |= 1; // BORRA LA INTERRUPCION Bandera de interrupcion para match en canal 0
}

void EINT3_IRQHandler(void)
{
	llamado_antirebote(1);
	if (  (*IO0IntStatF >> 1) & 1  )
	{
		if(valorlogico == 0)
		{
			cambioDePrescaler ();
		}
	}

	    *IO0IntClr |= (1 << 1);
	return;
}

void cambioDePrescaler ()
{
 if (*T1PR== 0)
 {
	*T1PR = 1;		// divido el prescaler en 4;
 }
 if (*T1PR==64)
 {
	 *T1PR = 0;
 }
 else
 {
	 *T1PR = *T1PR * 4;
 }
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

