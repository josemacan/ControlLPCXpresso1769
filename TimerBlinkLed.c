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
#define AddrT0EMR    0x4000403C;	// Registro de match externo. Controla los pines de Match
#define AddrT0MCR	 0x40004014;	// Registro de control de Match
#define AddrT0MR0	 0x40004018;	// Registro 0 de Match

#define AddrT0TCR 	 0x40004004;	// Registro de control de TMR
#define AddrT0IR	 0x40004000;	// Registro de Interrupcion
									// para borrar interrupciones de TMR
#define AddrT0PC	 0x40004010;	//Prescale couter: contador incrementado hasta igual a PR
#define AddrT0TC	 0x40004008;		// Timer counter: incrementado cada vez que PR+1

#define AddrPINSEL4	 0x4002C010;
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
unsigned int volatile *const T0EMR = (unsigned int*)AddrT0EMR ;
unsigned int volatile *const T0MCR = (unsigned int*)AddrT0MCR ;
unsigned int volatile *const T0MR0 = (unsigned int*)AddrT0MR0 ;
unsigned int volatile *const T0TCR = (unsigned int*)AddrT0TCR ;
unsigned int volatile *const T0IR = (unsigned int*)AddrT0IR ;
unsigned int volatile *const T0PR = (unsigned int*)AddrT0PR ;
unsigned int volatile *const T0PC = (unsigned int*)AddrT0PC ;
unsigned int volatile *const T0TC = (unsigned int*)AddrT0TC ;

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
unsigned static int prescaler = 364500;
unsigned static int prueba = 0;
unsigned static int prueba1 = 0;

int main (void)
{
*PCONP |= (1<<1);	// enciende el periférico de Timer

	 /*	------------------ ACTIVIDAD Nº1 -----------------
	// 	ELECCIÓN DE VALOR DEL CLOCK DE PERIFÉRICO:
	 *
	 * 	PIN 2-3 :  00= CCLK/4
	 * 			  01= CCLK
	 * 			  10= CCLK/2
	 *
	 */
		*PCLKSEL0 |= (2<<2); // reloj de periférico: reloj del sistema
	 //---------------------------------------


	*PINSEL3 |= (3<<24); 		//match 0.0
	*T0EMR |= (3<<4);			// conmuta el bit de Match externo
	*T0MCR |= 3; 				// (11) b0 = genera interrupcion cuando MR0 = TC
								//      b1 = TC se resetea si hace match con MR0
								// Reset en MR0: el Timer resetea si MR0 hace match
					 	 	 	// Interrupcion cuando MR0 hace match con el valor de TC

	*T0MR0 = 364500; 			// valor del Match register 0.
	*ISER0 |= (1<<1);			// ACTIVO INTERRUPCIONES EXTERNAS
	*T0TCR |= 1; 				// HABILITO EL REGISTRO DE CONTROL DE TIMER

	*FIO0DIR |= (1<<22);		// P0.22 como SALIDA
	*ISER0 |= (1<<1);

// PULSADOR
	*FIO0DIR &= ~(1<<1);		// P0.1 como ENTRADA
	*PINMODE0 &= ~(3<<2);		// P0.1 con pull up interno activado
	*IO0IntEnF|=(1<<1);			// Activo interrupcion P0.0 por flanco bajada
	*ISER0 |=(1<<21);			// activo las interrupciones

	//*********************
	*FIO0DIR |= (1<<22); // SALIDA LED
	*FIO3DIR |= (1<<26); // SALIDA LED
	*FIO0SET |= (1<<22); // SALIDA LED
	*FIO3SET |= (1<<26); // SALIDA LED
	//*************************



	while(1)
	{
			prueba = prueba + 1;
	}

	return 0;
}

// INTERRUPCION POR TIMER0

void TIMER0_IRQHandler (void)		// ISR del TIMER0
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
	*T0IR |= 1; // BORRA LA INTERRUPCION Bandera de interrupcion para match en canal 0
}

void EINT3_IRQHandler(void)
{
	llamado_antirebote(1);
	if (  (*IO0IntStatF >> 1) & 1  )
	{
		if(valorlogico == 0)
		{
			cambioDePrescaler ();
			//*FIO3CLR |= (1<<26); // SALIDA LED
			//retardado ();
			//retardado ();
			//retardado ();
		//*FIO3SET |= (1<<26); // SALIDA LED
		}
	}

	    *IO0IntClr |= (1 << 1);
	return;
}

void cambioDePrescaler ()
{

	*T0PR= *T0PR+1;		// divido el prescaler en 4;

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

/*
 * *T0TCR    bit 0:  si 1: TC y PC se activan
 * 					 si 0: TC y PC deshabilitados.
 *
 * 			 bit 1:	 si 1: TC y PC se resetean
 * 			 		 si 0: TC y PC salen de reset
 *
 * *T0PR		32 bits: establecen el maximo valor para el Prescale Counter
 *
 * *TMR0 (match register): valor al cual se compara el valor de TC
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

