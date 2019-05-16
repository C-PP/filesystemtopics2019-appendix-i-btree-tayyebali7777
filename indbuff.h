#pragma once
//indbuff.h
#ifndef INDBUFF_H
#define INDBUFF_H

#include <iostream>
#include "simpind.h"
#include "fixfld.h"

using namespace std;

// class IndexBuffer supports reading and writing index records from
//	class SimpIndex by key value.
// each record is consistent in its maximum size
//	
// There is a problem in dealing with the key type. Int and char*
// are very different. presently, this supports char * properly,
// but fails for int! 
// We need assignment and equality on the key values
template <class keyType> class SimpleIndex;
template <class keyType>
class IndexBuffer : public FixedFieldBuffer
{
public:
	IndexBuffer(int maxKeys = 100, int keySize = sizeof(keyType));
	int Pack(SimpleIndex<keyType> &);
	int Unpack(SimpleIndex<keyType> &);
	void Print(ostream &) const;
protected:
	int MaxKeys;
	int KeySize;
};

#endif