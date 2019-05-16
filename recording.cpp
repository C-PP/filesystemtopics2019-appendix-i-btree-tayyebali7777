//Recording.cpp
#include "recording.h"
#include <strstream>
#include <string.h>
#pragma warning(disable : 4996)

Recording::Recording()
{
	IdNum[0] = 0;
	Title[0] = 0;
	Composer[0] = 0;
	Artist[0] = 0;
	Label[0] = 0;
}

Recording::Recording(char * label, char * idNum, char * title,
	char * composer, char * artist)
{
	strcpy(Label, label); strcpy(IdNum, idNum);
	strcpy(Title, title); strcpy(Composer, composer);
	strcpy(Artist, artist);
}

char * Recording::Key() const
{// produce key as concatenation of Label and IdNum
	ostrstream key;
	key << Label << IdNum << ends;
	return key.str();
}

int Recording::Pack(IOBuffer & Buffer) const
{// return TRUE if all succeed, FALSE o/w
	int numBytes;
	Buffer.Clear();
	numBytes = Buffer.Pack(IdNum);
	if (numBytes == -1) return FALSE;
	numBytes = Buffer.Pack(Title);
	if (numBytes == -1) return FALSE;
	numBytes = Buffer.Pack(Composer);
	if (numBytes == -1) return FALSE;
	numBytes = Buffer.Pack(Artist);
	if (numBytes == -1) return FALSE;
	numBytes = Buffer.Pack(Label);
	if (numBytes == -1) return FALSE;
	return TRUE;
}

int Recording::Unpack(IOBuffer & Buffer)
{// unpack with maximum size, and add null termination to strings
	int numBytes;
	numBytes = Buffer.Unpack(IdNum, 6);
	if (numBytes == -1) return FALSE;
	IdNum[numBytes] = 0;
	numBytes = Buffer.Unpack(Title, 29);
	if (numBytes == -1) return FALSE;
	Title[numBytes] = 0;
	numBytes = Buffer.Unpack(Composer, 29);
	if (numBytes == -1) return FALSE;
	Composer[numBytes] = 0;
	numBytes = Buffer.Unpack(Artist, 29);
	if (numBytes == -1) return FALSE;
	Artist[numBytes] = 0;
	numBytes = Buffer.Unpack(Label, 6);
	if (numBytes == -1) return FALSE;
	Label[numBytes] = 0;
	return TRUE;
}

void Recording::Print(ostream & stream, char * label) const
{
	stream << Label << '|' << IdNum << '|' << Title << '|'
		<< Composer << '|' << Artist;
}
