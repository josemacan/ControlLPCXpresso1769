// ALUMNO: CANCINOS, JOSÉ MARÍA

int antirebote (int );
int main (void)
{

	static int muestraA = 1;
	antirebote (muestraA); //
	antirebote (muestraA);
	antirebote (muestraA);

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
