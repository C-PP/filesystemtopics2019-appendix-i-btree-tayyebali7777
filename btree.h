#pragma once
// btree.h

#ifndef BTREE_H
#define BTREE_H

#include "btnode.h"
#include "recfile.h"
#include "fixfld.h"
#include "indbuff.h"

// btree needs to be able to pack, unpack, read and
// 	write index records
// 	also, handle the insert, delete, split, merge,
//	growth of the tree
//	a pool of nodes will be managed
//	keep at least a branch of the tree in memory
//	
template <class keyType> class BTreeNode;
template <class keyType> class BTree
	// this is the full version of the BTree
{
public:
	BTree(int order, int keySize = sizeof(keyType), int unique = 1);
	~BTree();
	int Open(char * name, int mode);
	int Create(char * name, int mode);
	int Close();
	int Insert(const keyType key, const int recAddr);
	int Remove(const keyType key, const int recAddr = -1);
	int Search(const keyType key, const int recAddr = -1);
	void PrintInOrderTraversal(ostream &);
	void PrintInOrder(ostream &, int nodeAddr, int level);
	void Print(ostream &);
	void Print(ostream &, int nodeAddr, int level);
protected:
	typedef BTreeNode<keyType> BTNode;// useful shorthand
	BTNode * FindLeaf(const keyType key);
	// load a branch into memory down to the leaf with key
	BTNode * NewNode();
	BTNode * Fetch(const int recaddr);
	int Store(BTNode *thisNode);
	BTNode Root;
	int Height; // height of tree
	int Order; // order of tree
	int PoolSize;
	BTNode ** Nodes; // pool of available nodes
					 // Nodes[1] is level 1, etc. (see FindLeaf)
					 // Nodes[Height-1] is leaf
	FixedFieldBuffer Buffer;
	RecordFile<BTNode> BTreeFile;
};

#endif

const int MaxHeight = 5;
template <class keyType>
BTree<keyType>::BTree(int order, int keySize, int unique)
	: Buffer(1 + 2 * order, sizeof(int) + order * keySize + order * sizeof(int)),
	BTreeFile(Buffer), Root(order) {
	Height = 1;
	Order = order;
	PoolSize = MaxHeight * 2;
	Nodes = new BTNode *[PoolSize];
	BTNode::InitBuffer(Buffer, order);
	Nodes[0] = &Root;
}

template <class keyType>
BTree<keyType>::~BTree()
{
	Close();
	delete Nodes;
}

template <class keyType>
int BTree<keyType>::Open(char * name, int mode)
{
	int result;
	result = BTreeFile.Open(name, mode);
	if (!result) return result;
	// load root
	BTreeFile.Read(Root);
	Height = 1; // find height from BTreeFile!
	return 1;
}

template <class keyType>
int BTree<keyType>::Create(char * name, int mode) {
	int result;
	result = BTreeFile.Create(name, mode);
	if (!result) return result;
	// append root node
	result = BTreeFile.Write(Root);
	Root.RecAddr = result;
	return result != -1;
}

template <class keyType>
int BTree<keyType>::Close()
{
	int result;
	result = BTreeFile.Rewind();
	if (!result) return result;
	result = BTreeFile.Write(Root);
	if (result == -1) return 0;
	return BTreeFile.Close();
}


template <class keyType>
int BTree<keyType>::Insert(const keyType key, const int recAddr)
{
	int result; int level = Height - 1;
	int newLargest = 0;
	keyType prevKey, largestKey;
	BTNode * thisNode = nullptr, *newNode = nullptr, *parentNode = nullptr;
	thisNode = FindLeaf(key);

	// test for special case of new largest key in tree
	if (key > thisNode->LargestKey())
	{
		newLargest = 1;
		prevKey = thisNode->LargestKey();
	}

	result = thisNode->Insert(key, recAddr);

	// handle special case of new largest key in tree
	if (newLargest)
		for (int i = 0; i < Height - 1; i++)
		{
			Nodes[i]->UpdateKey(prevKey, key);
			if (i > 0) Store(Nodes[i]);
		}

	while (result == -1) // if overflow and not root
	{
		//remember the largest key
		largestKey = thisNode->LargestKey();
		// split the node
		newNode = NewNode();
		thisNode->Split(newNode);
		Store(thisNode);
		Store(newNode);
		level--; // go up to parent level
		if (level < 0) break;
		// insert newNode into parent of thisNode
		parentNode = Nodes[level];
		result = parentNode->UpdateKey(largestKey, thisNode->LargestKey());
		result = parentNode->Insert(newNode->LargestKey(), newNode->RecAddr);
		thisNode = parentNode;
	}
	Store(thisNode);
	if (level >= 0) return 1;// insert complete
							 // else we just split the root
	int newAddr = BTreeFile.Append(Root); // put previous root into file
										  // insert 2 keys in new root node
	Root.Keys[0] = thisNode->LargestKey();
	Root.RecAddrs[0] = newAddr;
	Root.Keys[1] = newNode->LargestKey();
	Root.RecAddrs[1] = newNode->RecAddr;
	Root.NumKeys = 2;
	Height++;
	return 1;
}

template <class keyType>
int BTree<keyType>::Remove(const keyType key, const int recAddr)
{
	int result; int level = Height - 1;
	int newLargest = 0;
	keyType prevKey;
	keyType largestKey;
	BTNode * thisNode, *newNode = nullptr, *parentNode = nullptr;
	thisNode = FindLeaf(key);


	// test for special case of new largest key in tree
	if (key > thisNode->LargestKey())
	{
		newLargest = 1;
		prevKey = thisNode->LargestKey();
	}

	result = thisNode->Remove(key, recAddr);
	largestKey = thisNode->LargestKey();
	result = parentNode->UpdateKey(largestKey, thisNode->LargestKey());
	cout << "1st cout" << endl;

	if(thisNode<numKeys())
	//thisNode->Print(cout);
	//parentNode->Print(cout);
	Store(thisNode);
	/*if (newLargest)
		for (int i = 0; i < Height - 1; i++)
		{
			Nodes[i]->UpdateKey(prevKey, key);
			if (i > 0) Store(Nodes[i]);
		}*/


	//While(result==-1){

	//largestKey = thisNode->LargestKey();

	//parentNode = Nodes[level];
 //   result = parentNode->UpdateKey(largestKey, thisNode->LargestKey());
	//result = parentNode->Remove(thisNode->LargestKey(), thisNode->RecAddr);
	//thisNode = parentNode;
	////parentNode->Print(cout);
	//level--;
	//
	//}

	return 1;
}

template <class keyType>
int BTree<keyType>::Search(const keyType key, const int recAddr)
{
	BTNode * leafNode;
	leafNode = FindLeaf(key);
	return leafNode->Search(key, recAddr);
}


template<class keyType>
inline void BTree<keyType>::PrintInOrderTraversal(ostream & stream)
{
	stream << "BTree of height " << Height << " is " << endl;
	//Root.Print(stream);
	if (Height > 1)
		for (int i = 0; i < Root.numKeys(); i++)
		{
			PrintInOrder(stream, Root.RecAddrs[i], 2);
		}
	stream << "end of BTree" << endl;
}

template<class keyType>
inline void BTree<keyType>::PrintInOrder(ostream &stream, int nodeAddr, int level)
{
	BTNode * thisNode = Fetch(nodeAddr);
	
	if(level > 2)
		thisNode->Print(stream);

	else {
		stream << "BTree::Print() ->Node at level " << level << " address " << nodeAddr << ' ' << endl;
		if (Height > level)
		{
			level++;
			for (int i = 0; i < thisNode->numKeys(); i++)
			{

				PrintInOrder(stream, thisNode->RecAddrs[i], level);

			}

			stream << "end of level " << level << endl;
		}
	}
}

template <class keyType>
void BTree<keyType>::Print(ostream & stream)
{
	stream << "BTree of height " << Height << " is " << endl;
	Root.Print(stream);
	if (Height > 1)
		for (int i = 0; i < Root.numKeys(); i++)
		{
			Print(stream, Root.RecAddrs[i], 2);
		}
	stream << "end of BTree" << endl;
}

template <class keyType>
void BTree<keyType>::Print
(ostream & stream, int nodeAddr, int level)
{
	BTNode * thisNode = Fetch(nodeAddr);
	stream << "BTree::Print() ->Node at level " << level << " address " << nodeAddr << ' ' << endl;
	thisNode->Print(stream);
	if (Height > level)
	{
		level++;
		for (int i = 0; i < thisNode->numKeys(); i++)
		{
			Print(stream, thisNode->RecAddrs[i], level);
		}
		stream << "end of level " << level << endl;
	}
}

template <class keyType>
BTreeNode<keyType> * BTree<keyType>::FindLeaf(const keyType key)
// load a branch into memory down to the leaf with key
{
	int recAddr, level;
	for (level = 1; level < Height; level++)
	{
		recAddr = Nodes[level - 1]->Search(key, -1, 0);//inexact search
		Nodes[level] = Fetch(recAddr);
	}
	return Nodes[level - 1];
}

template <class keyType>
BTreeNode<keyType> * BTree<keyType>::NewNode()
{// create a fresh node, insert into tree and set RecAddr member
	BTNode * newNode = new BTNode(Order);
	int recAddr = BTreeFile.Append(*newNode);
	newNode->RecAddr = recAddr;
	return newNode;
}

template <class keyType>
BTreeNode<keyType> * BTree<keyType>::Fetch(const int recaddr)
{// load this node from File into a new BTreeNode
	int result;
	BTNode * newNode = new BTNode(Order);
	result = BTreeFile.Read(*newNode, recaddr);
	if (result == -1) return NULL;
	newNode->RecAddr = result;
	return newNode;
}
template<class keyType>
int BTree<keyType>::Store(BTNode * thisNode)
{
	return BTreeFile.Write(*thisNode, thisNode->RecAddr);
}
