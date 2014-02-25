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
//long position;
typedef unordered_map<string,int> wordsMap;

typedef struct fileArg
{
	ifstream *ifs;
	wordsMap *Map;
} FileArg;

wordsMap merge(wordsMap wordsCount[], int size)
{
	wordsMap temp(wordsCount[0]);
	wordsMap::iterator iter;
	for(int i = 1; i < size; i++)
	{
		for(iter = wordsCount[i].begin(); iter != wordsCount[i].end(); iter++)
		{
			temp[iter->first] += iter->second;
		}
	}
	return temp;
}

void *Counter(void *arg)
{
	FileArg *pArg = (FileArg *)arg;
    //char *FileName = pArg->FileName;
    wordsMap *tempMap = pArg->Map;
    ifstream *ifs = pArg->ifs;
    
    pthread_mutex_lock(&streamMutex);
	while(!ifs->eof())
	{
		//ifs.seekg(position, ifs.beg);
		char *file = (char*)malloc(BUFFER_SIZE+1);
		memset(file, 0, BUFFER_SIZE+1);
		//ifs->seekg(position, ifs->beg);
		ifs->read(file, BUFFER_SIZE);
		//position += BUFFER_SIZE;
		
		//unlock the mutex to let other thread read file
		pthread_mutex_unlock(&streamMutex);

		//count keyword
		char *pch = NULL;
        char *last =NULL;
        wordsMap::iterator found;
		const char *delimiter;
        delimiter = " ,?\":;<>~`!@#^&*()_+=/\\{}[]|\n\r\v\f";

        //strtok is not thread safe!
		pch = strtok_r(file, delimiter, &last);
		while(pch != NULL)
		{
			string pstr = string(pch);
			
			(*tempMap)[pstr]++;
			/**
            found = tempMap->find(pstr);
            if(found == tempMap->end())
            {
                //not found
                pair<string, int> tempPair (pstr, 1);
                tempMap->insert(tempPair);
            }
            else
            {
                //found
                found->second++;
            }
			**/

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
	//position = 0;
	//words number
	wordsMap res[numThread];
	
	ifstream ifs(FileName, ifstream::in);
	if(!ifs.is_open())
    {
        cerr<<"Fail to open file"<<endl;
        exit(-1);
    }

    for(int i = 0; i < numThread; i++)
    {
    	//set arguments
    	FileArg f;
		f.ifs = &ifs;
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

	ifs.close();
    return 0;
}
