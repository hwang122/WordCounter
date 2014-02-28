#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sys/time.h>
#include <unordered_map>
#include <pthread.h>
//#include "Mstrtok.h"

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

//merge several maps together
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

//count words number and store them is map
void *Counter(void *arg)
{
	FileArg *pArg = (FileArg *)arg;
    wordsMap *tempMap = pArg->Map;
    const char *delimiter;
    delimiter = " \"\\,?:;<>~`!@#^&*()_+=/{}[]|\n\r\v\f";
    //vector<string> words;

    pthread_mutex_lock(&streamMutex);
    ifstream *ifs = pArg->ifs;
	while(!ifs->eof())
	{
		char *file = new char[BUFFER_SIZE+1];
		//ifs.seekg(position, ifs.beg);
		memset(file, 0, BUFFER_SIZE+1);
		//ifs->seekg(position, ifs->beg);
		ifs->read(file, BUFFER_SIZE);
		//position += BUFFER_SIZE;
		
		//unlock the mutex to let other thread read file
		pthread_mutex_unlock(&streamMutex);
	
		/**
		MStrTok(file, delimiter, strlen(file), strlen(delimiter), words);
		vector<string>::iterator iter;
	    for(iter = words.begin(); iter != words.end(); iter++)
	    {
	        (*tempMap)[*iter]++;
	    }
	    words.clear();
		**/
	    
		//count keyword
		char *pch = NULL;
        char *last =NULL;
        //wordsMap::iterator found;

        //strtok is not thread safe!
		pch = strtok_r(file, delimiter, &last);
		while(pch != NULL)
		{
			string str = string(pch);
			(*tempMap)[str]++;
			/*
            found = tempMap->find(str);

            if(found == tempMap->end())
            {
                //not found
                pair<string, int> tempPair (str, 1);
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
		
		delete[] file;
		file = NULL;
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
	
	//variables to count the time
	struct timeval etstart, etstop;
	unsigned long long usecstart, usecstop;
	
	pthread_t tid[numThread];
	pthread_mutex_init(&streamMutex, NULL);
	//position = 0;
	//words number
	wordsMap res[numThread];
	FileArg f[numThread];
	
	ifstream ifs(FileName, ifstream::in);
	if(!ifs.is_open())
    {
        cerr<<"Fail to open file"<<endl;
        exit(-1);
    }
	
	//start time
	gettimeofday(&etstart, NULL);
	
    for(int i = 0; i < numThread; i++)
    {
    	//set arguments
		f[i].ifs = &ifs;
		f[i].Map = &res[i];
    	pthread_create(&tid[i], NULL, Counter, &f[i]);
    }

    for(int i = 0; i < numThread; i++)
        pthread_join(tid[i], NULL);

    wordsMap sum(merge(res, numThread));
	wordsMap::iterator found = sum.find(Keyword);

    if(found != sum.end())
		cout<<"Total number of key words is "<<found->second<<endl;
	else
		cout<<"Key word not found!"<<endl;
	
	//end time
	gettimeofday(&etstop, NULL);
	usecstart = (unsigned long long)etstart.tv_sec * 1000000 + etstart.tv_usec;
	usecstop = (unsigned long long)etstop.tv_sec * 1000000 + etstop.tv_usec;
	
	float elapsed_time = (float)(usecstop - usecstart) / 1000;
	printf("Elapsed time %.3f ms\n", elapsed_time);
	
	ifs.close();
    return 0;
}
