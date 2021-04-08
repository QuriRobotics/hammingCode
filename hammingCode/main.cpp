#include "encoder.h"
#include "decoder.h"
#include "scrumbler.h"
#include "main.h"

int main()
{
	bool retflag = 0;
	int retval = 0;
	//retval = testEncoder(retflag);
	if (retflag) return retval;
	//retval = Scrumble(retflag);
	if (retflag) return retval;
	retval = testDecoder(retflag);
	if (retflag) return retval;

	return 0;
}

int Scrumble(bool& retflag)
{
	retflag = true;


	FILE* fin, * fout;

	fin = fopen("encoded.dat", "rb");
	fout = fopen("scrumbled.dat", "wb");

	if (fin == NULL)
	{
		printf("Error with input file");
		return 0;
	}
	if (fout == NULL)
	{
		printf("Error with output file");
		return 0;
	}

	scrumbler(fin, fout);

	fclose(fin);
	fclose(fout);
	retflag = false;
	return {};
}


int testDecoder(bool& retflag)
{
	retflag = true;


	FILE* fin, * fout;

	fin = fopen("result.dat", "rb");
	fout = fopen("decoded.dat", "wb");

	if (fin == NULL)
	{
		printf("Error with input file");
		return 0;
	}
	if (fout == NULL)
	{
		printf("Error with output file");
		return 0;
	}

	decoder(fin, fout, NULL);

	fclose(fin);
	fclose(fout);
	retflag = false;
	return {};
}

int testEncoder(bool& retflag)
{
	retflag = true;

	FILE* fin, * fout;

	fin = fopen("input.dat", "rb");
	fout = fopen("encoded.dat", "wb");

	if (fin == NULL)
	{
		printf("Error with input file");
		return 0;
	}
	if (fout == NULL)
	{
		printf("Error with output file");
		return 0;
	}

	encoder(fin, fout);

	fclose(fin);
	fclose(fout);
	retflag = false;
	return {};
}
