#pragma once
//btnode.h
#ifndef BTNODE_H
#define BTNODE_H

#include "simpind.h"
#include "recfile.h"
#include "fixfld.h"
#include "btree.h"

using namespace std;

template <class keyType> class BTree;
template <class keyType> class SimpleIndex;
template <class keyType>
class BTreeNode : public SimpleIndex<keyType> {// this is the in-memory version of the BTreeNode
public:
	BTreeNode(int maxKeys, int unique = 1);
	~BTreeNode();
	// Insert and Remove return
	//	0 for failure
	//	-1 for overflow
	//	1 for success
	int Insert(const keyType key, int recAddr);
	int Remove(const keyType key, int recAddr = -1);
	//int Search (const keyType key) const;
	void Print(ostream &) const;
	int LargestKey(); // returns value of largest key
	int Split(BTreeNode<keyType> * newNode); // move keys into newNode
	int Merge(BTreeNode<keyType> * fromNode); // move keys from fromNode
	int UpdateKey(keyType oldKey, keyType newKey, int recAddr = -1);
	int Pack(IOBuffer& buffer) const;
	int Unpack(IOBuffer& buffer);
	static int InitBuffer(FixedFieldBuffer & buffer,int maxKeys, int keySize = sizeof(keyType));
protected:
	int NextNode; // address of next node at same level
	int RecAddr; // address of this node in the BTree file
	int MinKeys; // minimum number of keys in a node
	int MaxBKeys; // maximum number of keys in a node
	int Init();
	void Clear() { NumKeys = 0; RecAddr = -1; }
	friend class BTree<keyType>;
};



#endif


template <class keyType>
BTreeNode<keyType>::BTreeNode(int maxKeys, int unique)
	:SimpleIndex<keyType>(maxKeys + 1, unique)
{
	Init();
}

template <class keyType>
BTreeNode<keyType>::~BTreeNode() {}

template <class keyType>
int BTreeNode<keyType>::Insert(const keyType key, int recAddr)
{
	int result;
	result = SimpleIndex<keyType>::Insert(key, recAddr);
	if (!result) return 0; // insert failed
	if (NumKeys >= MaxKeys) return -1; // node overflow
	return 1;
}

template <class keyType>
int BTreeNode<keyType>::Remove(const keyType key, int recAddr)
{
	int result;
	result = SimpleIndex<keyType>::Remove(key, recAddr);
	if (!result) return 0; // remove failed
	if (NumKeys < MinKeys) return -1; // node underflow
	return 1;
}

template <class keyType>
void BTreeNode<keyType>::Print(ostream & stream) const
{
	SimpleIndex<keyType>::Print(cout);
}

template <class keyType>
int BTreeNode<keyType>::LargestKey()
// returns value of largest key
{
	if (NumKeys > 0) return Keys[NumKeys - 1];
	else return Keys[0];
}

template <class keyType>
int BTreeNode<keyType>::Split(BTreeNode<keyType> * newNode)
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
	for (int i = midpt; i < NumKeys; i++)
	{
		newNode->Keys[i - midpt] = Keys[i];
		newNode->RecAddrs[i - midpt] = RecAddrs[i];
	}
	// set number of keys in the two Nodes
	newNode->NumKeys = numNewKeys;
	NumKeys = midpt;
	// Link the nodes together
	return 1;
}

template <class keyType>
int BTreeNode<keyType>::Merge(BTreeNode<keyType> * fromNode)
{
	// check for too many keys
	if (NumKeys + fromNode->NumKeys > MaxKeys - 1) return 0;
	// move keys and recaddrs from fromNode to this
	for (int i = 0; i < fromNode->NumKeys; i++)
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
int BTreeNode<keyType>::UpdateKey(keyType oldKey, keyType newKey, int recAddr)
{
	// look for the old key
	int recaddr = Search(oldKey, recAddr);
	if (recaddr < 0) return 0; // key and recaddr not found
	Remove(oldKey, recAddr);
	Insert(newKey, recaddr);
	return 1;
}


template <class keyType>
int BTreeNode<keyType>::Init()
{
	NextNode = -1;
	RecAddr = -1;
	MaxBKeys = MaxKeys - 1;
	MinKeys = MaxBKeys / 2;
	return 1;
}

template <class keyType>
BTreeNode<keyType> * CreateBTreeNode(int maxKeys, int unique)
{
	return new BTreeNode<keyType>(maxKeys, unique);
}

template <class keyType>
int BTreeNode<keyType>::Pack(IOBuffer& buffer) const
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
int BTreeNode<keyType>::Unpack(IOBuffer& buffer)
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
int BTreeNode<keyType>::InitBuffer
(FixedFieldBuffer & buffer, int maxKeys, int keySize)
{// initialize a buffer for the btree node
	buffer.AddField(sizeof(int));
	for (int i = 0; i < maxKeys; i++)
	{
		buffer.AddField(keySize); buffer.AddField(sizeof(int));
	}
	return 1;
}

