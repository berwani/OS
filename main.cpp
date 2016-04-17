#include<stdio.h>
#include<list>
#include<string>
#include<iostream>
#include<cstdlib>
#include<math.h>

// #define DISKSIZE 20000000
// #define BLOCKSIZE 1000

#define DISKSIZE 2000
#define BLOCKSIZE 10

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
	bool isEmpty;
	DirectoryTree()
	{
		parentDirectory = NULL;
		directoryName = "";
		size = 0;
		isEmpty = true;
	}
};

class MyFile : public Something{
public:
	int FATentry ;
	bool readOnly ;
	string fileName;
	DirectoryTree* parentDirectory;
	int lastFilledBlock, lastByteInlastFilledBlock, lastBlock;

	MyFile()
	{
		FATentry=0;
		readOnly = false;
		fileName = "";
		parentDirectory = NULL;
		lastBlock = lastFilledBlock = lastByteInlastFilledBlock = 0;
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

	int allocateSpace(int size,MyFile* file)
	{
		if(freeBlockList.size()==0)
		{
			cout<<"Memory Full, System Crash"<<endl;
			exit(-1);
		}

		
		int x = rand()%freeBlockList.size();
		std::list<int>::iterator it = freeBlockList.begin();

		for(int i=0;i<x;i++)
			it++;
		int toReturn = *it,nextValue = *it;
		freeBlockList.erase(it);

		if(size <= BLOCKSIZE)
		{
			file->lastBlock = toReturn;
			FAT[toReturn] = -1;
			return toReturn;
		}

		for(int i=1;i<ceil(size*1.0/BLOCKSIZE);i++){

			x = rand()%freeBlockList.size();
			it = freeBlockList.begin();

			for(int i=0;i<x;i++)
				it++;
								cout<<"allocating "<<nextValue<<" "<<freeBlockList.size()<<endl;
			FAT[nextValue] = *it;
			nextValue = *it;

			freeBlockList.erase(it);		
		}

		file->lastBlock = nextValue;
		FAT[nextValue] = -1;		//Last Block - no next block
								cout<<"allocating "<<nextValue<<" "<<freeBlockList.size()<<endl;
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

		newf->FATentry = allocateSpace(fileSize,newf);
		newf->lastFilledBlock = newf->FATentry;
						//cout<<"FATentry = "<<newf->FATentry<<endl;
		newf->parentDirectory = currentDirectory;
		currentDirectory->isEmpty = false;
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

	void appendFile(string fileName, string newData)
	{
		MyFile *file = (MyFile*)currentDirectory->files.search(fileName);

		if(file == NULL)
		{
			cout<<"The fie dosen't exists in this directory!"<<endl;
			return;
		}

		if(file->readOnly)
		{
			cout<<"Sorry the file is read only, cannot write anything"<<endl;
			return;
		}

		int currentBlock = file->lastFilledBlock, blockStart = currentBlock*BLOCKSIZE, currentByte = blockStart + file->lastByteInlastFilledBlock;
		int blockEnd = blockStart + BLOCKSIZE ;
						cout<<currentByte<<" "<<currentBlock<<" "<<blockStart<<" "<<blockEnd<<endl;
		for(int i=0;i<newData.size();i++)
		{
			if(FAT[currentBlock]==-1 && currentByte==blockEnd)
			{
								cout<<"Call to allocate new Block"<<endl;
				currentBlock = allocateNewBlock(file);
				currentByte = currentBlock*BLOCKSIZE;
				blockStart = currentBlock*BLOCKSIZE;
				blockEnd = blockStart + BLOCKSIZE;
			}

			else if(currentByte == blockEnd)
			{
								cout<<"Block filled, going to next Block"<<endl;
				currentBlock = FAT[currentBlock];
				currentByte = currentBlock*BLOCKSIZE;
				blockStart = currentBlock*BLOCKSIZE;
				blockEnd = blockStart + BLOCKSIZE;

			}
						cout<<"Writing "<<newData[i]<<" at "<<currentByte<<endl;
			virtualDisk[currentByte++] = newData[i];
		}
		file->lastFilledBlock = currentBlock;
		file->lastByteInlastFilledBlock = (currentByte - blockStart);
					cout<<"AppendCheck "<<file->lastByteInlastFilledBlock<<endl;		
	}

	int allocateNewBlock(MyFile* file)
	{
		if(freeBlockList.size()==0)
		{
			cout<<"Memory Full, System Crash"<<endl;
			exit(-1);
		}

		int x = rand()%freeBlockList.size();
		std::list<int>::iterator it = freeBlockList.begin();

		for(int i=0;i<x;i++)
			it++;
		int nextValue = *it;
		freeBlockList.erase(it);
		FAT[file->lastBlock] = nextValue;
		FAT[nextValue] = -1;
		file->lastBlock = nextValue;
						cout<<"New Block allocated "<<nextValue<<endl;
		return nextValue;
	}

	void readFile(string fileName, int numberOfBytes)
	{
							cout<<"Reading file"<<endl;
		MyFile *file = (MyFile*)currentDirectory->files.search(fileName);

		if(file == NULL)
		{
			cout<<"The file dosen't exists in this directory!"<<endl;
			return;
		}

		int currentBlock = file->FATentry, currentByte = currentBlock*BLOCKSIZE,blockStart = currentBlock*BLOCKSIZE, blockEnd = blockStart + BLOCKSIZE;
		
							
		int test = 25;
		while(!(currentBlock==file->lastFilledBlock && currentByte==(blockStart+file->lastByteInlastFilledBlock+1)))
		{
			if(currentByte == blockEnd)
			{
				if(FAT[currentBlock]==-1){
					printf("\n");
					return;
				}
				currentBlock = FAT[currentBlock];
				currentByte = currentBlock*BLOCKSIZE;
				blockStart = currentBlock*BLOCKSIZE;
				blockEnd = blockStart + BLOCKSIZE;

			}
							//cout<<currentByte<<" "<<currentBlock<<" "<<blockStart<<" "<<blockEnd<<" "<<file->lastByteInlastFilledBlock<<endl;
			printf("%c",virtualDisk[currentByte++]);
		}
		printf("\n");
	}


};

int main()
{
	srand (time(NULL));
	FileSystem filesystem;
	filesystem.newFile(12,"hello",false);
	filesystem.appendFile("hello","helloworldfuckedsobadd");
	filesystem.readFile("hello",12);
	return 0;
}
