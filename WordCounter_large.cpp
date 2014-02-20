#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unordered_map>
#include <pthread.h>

#define BUFFER_SIZE 250000000

using namespace std;

pthread_mutex_t streamMutex;
pthread_mutex_t mapMutex;
size_t position;
typedef unordered_map<string,int> wordsMap;

typedef struct fileArg
{
	char *FileName;
	wordsMap *Map;
} FileArg;

wordsMap merge(wordsMap wordsCount[], int size)
{
	wordsMap temp(wordsCount[0]);
	for(int i = 1; i < size; i++)
	{
		for(auto& x: wordsCount[i])
		{
			temp[x.first] += x.second;
		}
	}
	return temp;
}

void *Counter(void *arg)
{
	FileArg *pFile = (FileArg *)arg;
    char *FileName = pFile->FileName;
    wordsMap *tempMap = pFile->Map;

	ifstream ifs;
	ifs.open(FileName, ifstream::in);
	if(!ifs.is_open())
    {
        cerr<<"Fail to open file"<<endl;
        exit(-1);
    }

    pthread_mutex_lock(&streamMutex);
	while(!ifs.eof())
	{
		//ifs.seekg(position, ifs.beg);
		char *file = (char*)malloc(BUFFER_SIZE);
		ifs.seekg(position, ifs.beg);
		ifs.read(file, BUFFER_SIZE);
		position += BUFFER_SIZE;

		//unlock the mutex to let other thread read file
		pthread_mutex_unlock(&streamMutex);

		//count keyword
		char *pch;
        char *last;
        wordsMap::iterator found;
		const char *delimiter;
        delimiter = " ,?\":;<>~`!@#^&*()_+=/\\{}[]|\n\r\v\f";

        //strtok is not thread safe!
		pch = strtok_r(file, delimiter, &last);
		while(pch != NULL)
		{
            found = tempMap->find(pch);

            if(found == tempMap->end())
            {
                //not found
                pair<string, int> tempPair (pch, 1);
                tempMap->insert(tempPair);
            }
            else
            {
                //found
                found->second++;
            }

            pch = strtok_r(NULL, delimiter, &last);
		}

		free(file);
		//lock to handle next file
		pthread_mutex_lock(&streamMutex);
	}
		
    //all files have been handled, unlock
    pthread_mutex_unlock(&streamMutex);
}

int main(int argc, char *argv[])
{
	//get arguments from command line
    if(argc != 4){
        cout<<"Usage: "<<argv[0]<<" [file name] [searching word] [number of threads]"<<endl;
        return -1;
    }

    char *FileName = argv[1];
	char *Keyword = argv[2];
	int numThread = atoi(argv[3]);
	
	pthread_t tid[numThread];
	pthread_mutex_init(&streamMutex, NULL);
	position = 0;
	//words number
	wordsMap res[numThread];
	
    for(int i = 0; i < numThread; i++)
    {
    	//set arguments
    	FileArg f;
		f.FileName = FileName;
		f.Map = &res[i];
    	pthread_create(&tid[i], NULL, Counter, &f);
    }

    for(int i = 0; i < numThread; i++)
        pthread_join(tid[i], NULL);

    wordsMap sum(merge(res, numThread));
	wordsMap::iterator found = sum.find(Keyword);

    if(found != sum.end())
		cout<<"Total number of key words is "<<found->second<<endl;
	else
		cout<<"Key word not found!"<<endl;

    return 0;
}
