//btnode.cpp
#include <iostream>
#include "simpind.h"
#include "indnode.h"
#include "fixfld.h"

#ifndef INDNODE_TC
#define INDNODE_TC

template <class keyType>
IndexNode<keyType>::IndexNode(int maxKeys, int unique)
	:SimpleIndex<keyType>(maxKeys + 1, unique)
{
	Init();
}

template <class keyType>
IndexNode<keyType>::~IndexNode() {}

template <class keyType>
int IndexNode<keyType>::Insert(const keyType key, int recAddr)
{
	int result;
	result = SimpleIndex<keyType>::Insert(key, recAddr);
	if (!result) return 0; // insert failed
	if (NumKeys >= MaxKeys) return -1; // node overflow
	return 1;
}

template <class keyType>
int IndexNode<keyType>::Remove(const keyType key, int recAddr)
{
	int result;
	result = SimpleIndex<keyType>::Remove(key, recAddr);
	if (!result) return 0; // remove failed
	if (NumKeys < MinKeys) return -1; // node underflow
	return 1;
}

template <class keyType>
void IndexNode<keyType>::Print(ostream & stream) const
{
	SimpleIndex<keyType>::Print(cout);

}

template <class keyType>
int IndexNode<keyType>::SmallestKey()
// returns value of smallest key
{
	return Keys[0];
}

template <class keyType>
int IndexNode<keyType>::Split(IndexNode<keyType> * newNode)
{
	// check for sufficient number of keys
	if (NumKeys < MaxKeys) return 0;
	// find the first Key to be moved into the new node
	int midpt = (NumKeys + 1) / 2;
	int numNewKeys = NumKeys - midpt;
	// check that number of keys for newNode is ok

	if (numNewKeys > newNode->MaxBKeys
		|| numNewKeys < newNode->MinKeys)
		return 0;
	// move the keys and recaddrs from this to newNode
	for (int i = midpt; i< NumKeys; i++)
	{
		newNode->Keys[i - midpt] = Keys[i];
		newNode->RecAddrs[i - midpt] = RecAddrs[i];
	}
	// set number of keys in the two Nodes
	newNode->NumKeys = numNewKeys;
	NumKeys = midpt;
	// Link the nodes together
	newNode->NextNode = NextNode;
	NextNode = newNode->RecAddr;
	return 1;
}


template <class keyType>
int IndexNode<keyType>::Merge(IndexNode<keyType> * fromNode)
{
	// check for too many keys
	if (NumKeys + fromNode->NumKeys > MaxKeys - 1) return 0;
	// move keys and recaddrs from fromNode to this
	for (int i = 0; i<fromNode->NumKeys; i++)
	{
		Keys[NumKeys + i] = fromNode->Keys[i];
		RecAddrs[NumKeys + i] = fromNode->RecAddrs[i];
	}
	// adjust number of keys
	NumKeys += fromNode->NumKeys;
	// adjust links
	NextNode = fromNode->NextNode;
	return 1;
}

template <class keyType>
int IndexNode<keyType>::UpdateKey(keyType oldKey, keyType newKey, int recAddr)
{
	// look for the old key
	int recaddr = Search(oldKey, recAddr);
	if (recaddr < 0) return 0; // key and recaddr not found
	Remove(oldKey, recAddr);
	Insert(oldKey, recaddr);
}


template <class keyType>
int IndexNode<keyType>::Init()
{
	NextNode = -1;
	RecAddr = -1;
	MaxBKeys = MaxKeys - 1;
	MinKeys = MaxBKeys / 2;
	return 1;
}

template <class keyType>
IndexNode<keyType> * CreateIndexNode(int maxKeys, int unique)
{
	return new IndexNode<keyType>(maxKeys, unique);
}

template <class keyType>
int IndexNode<keyType>::Pack(IOBuffer& buffer) const
{
	int result;
	buffer.Clear();
	result = buffer.Pack(&NumKeys);
	for (int i = 0; i < NumKeys; i++)
	{// note only pack the actual keys and recaddrs
		result = result && buffer.Pack(&Keys[i]);
		result = result && buffer.Pack(&RecAddrs[i]);
	}
	return result;
}

template <class keyType>
int IndexNode<keyType>::Unpack(IOBuffer& buffer)
{
	int result;
	result = buffer.Unpack(&NumKeys);
	for (int i = 0; i < NumKeys; i++)
	{// note only pack the actual keys and recaddrs
		result = result && buffer.Unpack(&Keys[i]);
		result = result && buffer.Unpack(&RecAddrs[i]);
	}
	return result;
}

template <class keyType>
int IndexNode<keyType>::InitBuffer(FixedFieldBuffer & buffer, int maxKeys, int keySize)
{// initialize a buffer for the Index node
	buffer.AddField(sizeof(int));
	for (int i = 0; i < maxKeys; i++)
	{
		buffer.AddField(keySize); buffer.AddField(sizeof(int));
	}
	return 1;
}

#endif
