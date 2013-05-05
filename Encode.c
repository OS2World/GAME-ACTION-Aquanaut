#include <stdio.h>
#include <stdlib.h>

/* Crude encryption scheme */
#define ENCODE(c)  (((c - 'A' + 13) % 26) + 'A')

void main (int argc, char *argv[])
{
	int  i, j;
	int  iLen;
	char szName[100];
	char c;

	if (argc != 2)
	{
		printf ("Need to supply a name in quotes to encode\n");
		exit (0);
	}

	/*
	** Get the name into an upper case string - removing spaces and only
	** using every other character 
	*/
	for (i = 0, j = 0;  i < strlen (argv[1]);  i += 2)
	{
	   if (argv[1][i] != '\"' && argv[1][i] != ' ' && argv[1][i] != '\t')
		{
			szName[j++] = toupper (argv[1][i]);
		}
	}
	szName[j] = '\0';

	/* Append AQUA to the name */
	strcat (szName, "AQUA");
	iLen = strlen (szName);

	/* Now switch the characters, encoding as we go along */
	for (i = 0;  i < iLen - 1;  i += 2)
	{
		c = szName[i];
		szName[i] = ENCODE (szName[i+1]);
		szName[i+1] = ENCODE (c);
	}

	/* If there are an odd number of characters then encode the last char */
	if (iLen & 1)
	{
		szName[iLen - 1] = ENCODE (szName[iLen - 1]);
	}

	printf ("ID=%s\n", szName);
}

