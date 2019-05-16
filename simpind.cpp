//simpind.tc
#include "simpind.h"
#include "indbuff.h"
#include <iostream>

template <class keyType>
SimpleIndex<keyType>::SimpleIndex(int maxKeys, int unique)
	: NumKeys(0), Keys(0), RecAddrs(0)
{
	Init(maxKeys, unique);
}

template <class keyType>
SimpleIndex<keyType>::~SimpleIndex()
{
	delete Keys;
	delete RecAddrs;
}

template <class keyType>
void SimpleIndex<keyType>::Clear()
{
	NumKeys = 0;
}

template <class keyType>
int SimpleIndex<keyType>::Insert(const keyType key, int recAddr)
{
	int i;
	int index = Find(key);
	if (Unique && index >= 0) return 0; // key already in
	if (NumKeys == MaxKeys) return 0; //no room for another key
	for (i = NumKeys - 1; i >= 0; i--)
	{
		if (key > Keys[i]) break; // insert into location i+1
		Keys[i + 1] = Keys[i];
		RecAddrs[i + 1] = RecAddrs[i];
	}
	Keys[i + 1] = key;
	RecAddrs[i + 1] = recAddr;
	NumKeys++;
	return 1;
}

template <class keyType>
int SimpleIndex<keyType>::Remove(const keyType key, const int recAddr)
{
	int index = Find(key, recAddr);
	if (index < 0) return 0; // key not in index
	for (int i = index; i < NumKeys; i++)
	{
		Keys[i] = Keys[i + 1];
		RecAddrs[i] = RecAddrs[i + 1];
	}
	NumKeys--;
	return 1;
}

template <class keyType>
int SimpleIndex<keyType>::Search(const keyType key,
	const int recAddr, const int exact) const
{
	int index = Find(key, recAddr, exact);
	if (index < 0) return index;
	return RecAddrs[index];
}

template <class keyType>
void SimpleIndex<keyType>::Print(ostream & stream) const
{
	stream << "Simple Index max keys " << MaxKeys
		<< " num keys " << NumKeys << endl;
	for (int i = 0; i < NumKeys; i++)
		stream << "\tKey[" << i << "] " << Keys[i]
		<< " RecAddr " << RecAddrs[i] << endl;
}

template <class keyType>
int SimpleIndex<keyType>::Find(const keyType key,
	const int recAddr, const int exact) const
{
	for (int i = 0; i < NumKeys; i++)
	{
		if (Keys[i] < key) continue; // not found yet
		if (Keys[i] == key) // exact match
		{
			if (recAddr < 0) return i;
			else if (recAddr == RecAddrs[i]) return i;
			else return -1; //
		}
		// no exact match: Keys[i-1]<key<Keys[i]
		if (!exact) // inexact match with key
			return i;
		return -1;
	}
	// key > all keys in index
	if (exact == 1) return -1; // no exact match
	else return NumKeys - 1; // inexact, matches last key
}

template <class keyType>
int SimpleIndex<keyType>::Init(const int maxKeys, const int unique)
{
	Unique = unique != 0;
	if (maxKeys <= 0)
	{
		MaxKeys = 0;
		return 0;
	}
	MaxKeys = maxKeys;
	Keys = new keyType[maxKeys];
	RecAddrs = new int[maxKeys];
	return 1;
}
