//#pragma warning(disable:4996)
//tstbtree.cc
//#include "btnode.h"

#include <iostream>
#include <string>
#include "btree.h"
using namespace std;

const char * keys = "CSDTAMPIBWNGURKEHOLJYQZFXV";

const int BTreeSize = 4;
int main(int argc, char * argv) {
	int result, i,select;
	BTree <char> bt(BTreeSize);
	
	
	


	while (1)
	{
		cout << "\nSelect command 1:: Create File, 2:: Insert & Display characters, 3:InOrder Traversal, 4::Search Function, 5::Remove Function , 6.Quit => ";
		cin >> select;

		switch (select) {
		case 1:
		{
			cout << "input data = " << keys << endl;
			result = bt.Create("btree.dat", ios::in | ios::out);
			if (result = 1)
			{
				cout << "File Create Successfull !" << endl;
			}
			if (!result) {
				cout << "Please delete testbt.dat" << endl;
				system("pause");
				return 0;
			}
			break;
		}

		case 2: {
			
			for (i = 0; i < 26; i++)
			{
				cout << "Inserting " << keys[i] << endl;
				result = bt.Insert(keys[i], i);
				
			}
			bt.Print(cout);
		
			break;
		}
		case 3:
		{
			cout << endl;
			cout << "The Order Traversal is given below" << endl;
			cout << endl;
			bt.PrintInOrderTraversal(cout); //Printing IN order Traversal
			break;
		}
		case 4:
		{
			char serial;
			cout << "Enter key to search" << endl;
			cin >> serial;
			result = bt.Search(serial);
			cout << "Key of the " << serial << " having address " << result<< endl;


			/*for (int i = 0; i <1; i++)
			{
				result=bt.Search(keys[1], 1);
				bt.Print(cout);
			}*/
			
			if (result = 1) cout << "Search Success!" << endl;
			else cout << "Searching Function Error!!" << endl;
			break;
		}
		case 5: 
		{
			for (i = 0; i < 26; i++)
			{
				cout << "Removing " << keys[i] << endl;
				result = bt.Remove(keys[i], i);
				//bt.Print(cout);
			}
			cout << "The final output is given below " << endl;
			bt.Print(cout);
		
			break;
		}
		
		default:
			// others
			exit(0);
			break;

		}
	}
	system("pause");
	return 1;
}