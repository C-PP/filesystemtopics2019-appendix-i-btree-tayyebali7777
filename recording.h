#pragma once
// recordng.h

#ifndef RECORDING_H
#define RECORDING_H

#include <iostream>
#include "iobuffer.h"

// class person with fixed field buffer

class Recording
	// a recording with a composite key
{
public:
	Recording();
	Recording(char * label, char * idNum, char * title,
		char * composer, char * artist);
	char IdNum[7]; char Title[30]; char Composer[30];
	char Artist[30]; char Label[7];
	char * Key() const; // return key of object
	int Unpack(IOBuffer &);
	int Pack(IOBuffer &) const;
	void Print(ostream &, char * label = 0) const;
};

ostream & operator << (ostream &, Recording &);
inline ostream & operator << (ostream & stream, Recording & rec)
{
	rec.Print(stream); return stream;
}

#endif
