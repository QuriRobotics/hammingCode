#include "encoder.h"
#include "decoder.h"
#include "scrumbler.h"
#include "main.h"

#define INPUT "full.dat"
#define ENCODED "encoded.dat"
#define SCRUMBLED "encoded.dat"
#define DECODED "decoded.dat"

int main()
{
	//testEncoder();
	//Scrumble();
	testDecoder();

	return 0;
}

int Scrumble()
{
	FILE* fin, * fout;

	fin = fopen(ENCODED, "rb");
	fout = fopen(SCRUMBLED, "wb");

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
	return 0;
}

int testDecoder()
{
	FILE* fin, * fout;

	fin = fopen(SCRUMBLED, "rb");
	fout = fopen(DECODED, "wb");

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
	return 0;
}

int testEncoder()
{
	FILE* fin, * fout;

	fin = fopen(INPUT, "rb");
	fout = fopen(ENCODED, "wb");

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
	return 0;
}
