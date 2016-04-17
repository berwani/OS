#include<stdio.h>
#include<list>
#include<string>
#include<iostream>
#include<cstdlib>
#include<math.h>

#define DISKSIZE 20000000
#define BLOCKSIZE 1000

using namespace std;

char virtualDisk[DISKSIZE] = {0};
int FAT[DISKSIZE/BLOCKSIZE] = {0};
list <int> freeBlockList;



class Something{
	
public:
	string created;
	int size ;
	
	Something()
	{
		size =  0;
		created = "";
	}
};

class Node{
public:
	Something* data ;
	Node* aTOz[26];

	Node()
	{
		data = NULL;
		for(int i=0;i<26;i++)
			aTOz[i] = NULL;
	}
};

class Trie{
public:
	Node* root;

	Trie(){
		root = new Node();
	}

	bool insert(Node* roo,string name,Something* d,int index)
	{

		if(index==name.size()-1)
		{
			if(roo->aTOz[name[index]-97] != NULL){
				cout << "Name already exists in Trie, insert again with another name" << endl;
				return false;
			}

			roo->aTOz[name[index]-97] = new Node();
			roo->aTOz[name[index]-97]->data = d;
						//cout<<(roo->aTOz[name[index]-97]->data)->size<<" check\n";
			return true;
		}
		else
		{
			if(roo->aTOz[name[index]-97] == NULL){
				roo->aTOz[name[index]-97] = new Node();
						//cout<<"created new node "<<name[index]<<endl;
			}

			return insert(roo->aTOz[name[index]-97],name,d,index+1);	
		}
	}

	bool insert(string name,Something* d)
	{
		return insert(root,name,d,0);
	}

	Something* search(Node* roo, string name,int index)
	{
		if(index == name.size()-1)
		{
			if(roo->aTOz[name[index]-97] != NULL){
						//cout<<"chek "<<(roo->aTOz[name[index]-97])->data->size<<endl;
				return roo->aTOz[name[index]-97]->data;
			}
			else{
				cout<< "Name dosen't exist in Trie, search again with another name" << endl;
				return NULL;	
			} 
		}
		else
		{
			if(roo->aTOz[name[index]-97] != NULL){
							//cout<<"searching towards "<<name[index]<<endl;
				return search(roo->aTOz[name[index]-97],name,index+1);
			}
			else{
				cout<< "Name dosen't exist in Trie, search again with another name" << endl;
				return NULL;	
			}
		}
	}

	Something* search(string name)
	{
		return search(root,name,0);
	}

	
};


class DirectoryTree : public Something{
public:
	Trie folders,files;
	DirectoryTree* parentDirectory;
	string directoryName;
	int size;
	DirectoryTree()
	{
		parentDirectory = NULL;
		directoryName = "";
		size = 0;
	}
};

class MyFile : public Something{
public:
	int FATentry ;
	bool readOnly ;
	string fileName;
	DirectoryTree* parentDirectory;

	MyFile()
	{
		FATentry=0;
		readOnly = false;
		fileName = "";
		parentDirectory = NULL;
	}
};

class FileSystem{
public:
	DirectoryTree* backSlash;
	DirectoryTree* currentDirectory;

	FileSystem()
	{
		backSlash = new DirectoryTree();
		backSlash->directoryName = "\\";
		for(int i=1;i<DISKSIZE/BLOCKSIZE;i++)
		{
			freeBlockList.push_back(i);
		}

		currentDirectory = backSlash;
	}

	int allocateSpace(int size)
	{
		srand (time(NULL));
		int x = rand()%freeBlockList.size();
		std::list<int>::iterator it = freeBlockList.begin();

		for(int i=0;i<x;i++)
			it++;
		int toReturn = *it,nextValue = *it;
		freeBlockList.erase(it);

		for(int i=1;i<ceil(size/BLOCKSIZE);i++){

			x = rand()%freeBlockList.size();
			it = freeBlockList.begin();

			for(int i=0;i<x;i++)
				it++;
								cout<<"allocating "<<nextValue<<" "<<freeBlockList.size()<<endl;
			FAT[nextValue] = *it;
			nextValue = *it;

			freeBlockList.erase(it);		
		}

		return toReturn;
	}

	void newFile(int fileSize,string fileName,bool readOnly)
	{
		MyFile* newf = new MyFile();
		bool success = currentDirectory->files.insert(fileName,newf);
		if(!success)
		{
			cout<<"New file not created"<<endl;
			return;
		}

		newf->fileName = fileName;
		newf->size = fileSize;

		newf->FATentry = allocateSpace(fileSize);
						//cout<<"FATentry = "<<newf->FATentry<<endl;
		newf->parentDirectory = currentDirectory;
					//Increament Size in all parent directories
		DirectoryTree* p = currentDirectory;

		while(p!=backSlash)
		{
			p->size = p->size + fileSize;
			p = p->parentDirectory;
		}

		backSlash->size = backSlash->size + fileSize;
					//
		cout<<"File "<<fileName<<" created and first block is at "<<newf->FATentry<<endl;
	}

	void 


};

int main()
{
	FileSystem filesystem;
	filesystem.newFile(5000,"hello",false);
	return 0;
}
