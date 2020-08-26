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

int retardo(unsigned int); // prototipo de la función retardo
void EINT3_IRQHandler(void);
unsigned static int prueba = 0;

int main(void)
	{
	 	 prueba = 0;
		unsigned int rutina = 0;
		unsigned int duracion = 5;
		unsigned int modo = 1;   // 1: descendente 0: ascendente
		unsigned int tiempo = 364500; // control de tiempo de retardo
		*FIO0DIR |= (1<<22);			  // define al pin 22 del puerto 0	como de SALIDA (SALIDA = 1)
		*FIO0DIR &= ~(1<<1);			  // define al pin 22 del puerto 0	como de ENTRADA (SALIDA = 1)
		*PINMODE0 &= ~(3<<2);			  // explicito pull up interno pin 1 puerto 0
		*FIO0MASK0 &= ~(1<<1);			  //
		*FIO0MASK0 &= ~ (1<<22);			  // Enmascaro el resto de los bits de excepto 0.1 y 0.22

		*IO0IntEnF|=(1<<1);			// activo interrupcion del puerto 1 por flanco de bajada
		*IO0IntEnR|=(1<<1);
		*ISER0 |=(1<<21);			// activo las interrupciones



		while(1)
		{

			switch (rutina)
			{
				case 0 :
					tiempo = 500000;
						if(modo == 1)
						{ rutina++; }
						else
						{modo = 1; rutina = 0;}
					break;
				case 1 :
					tiempo =250000;
						if(modo == 1)
						{ rutina++; }
						else {rutina--;}
					break;
				case 2 :
					tiempo = 125000;
						if(modo == 1)
						{ rutina++; }
						else {rutina--;}
					break;
				case 3:
					tiempo = 75000;
						if(modo == 1)
						{ rutina++; }
						else {rutina--;}
					break;
				case 4:
					tiempo = 37500;
						if(modo == 1)
						{ rutina++; }
						else {rutina--;}
					break;
				case 5:
					tiempo = 12500;
						if(modo == 1)
						{ modo = 0;
						  rutina = 5; }
						else {rutina--;}
					break;
				default:
				tiempo = 12500;
						if(modo == 1)
						{ modo = 0;
						rutina = 5; }
						else {rutina--;}

			}

			while (duracion > 0)
			{
				if(prueba != 1)
				{
				*FIO0SET|=(1<<22);   // pone en alto al pin 22, encendiendo el led
				retardo(tiempo);
				*FIO0CLR|=(1<<22);	 // pone en cero al pin 22, apagando el led
				retardo(tiempo);

				duracion = duracion - 1;
				}

			}
				duracion = 5;
		}


		return 0 ;
	}

int retardo (unsigned int tiempo)
	{
		unsigned int i;
		for(i = 0; i < tiempo; i++);

		return 0;
	}

void EINT3_IRQHandler(void)
{


	if((*IO0IntStatR >> 1) & 1)
	{
		prueba = 0;
		*FIO0CLR |= (1 << 22);
		*IO0IntClr |= (1 << 1);


	}

	if((*IO0IntStatF >> 1) & 1)
		{
			prueba = 1; // no parpadear;
			*FIO0SET |= (1 << 22);
			*IO0IntClr |= (1 << 1);

		}

   return;
}
