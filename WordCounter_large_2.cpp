#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unordered_map>
#include <pthread.h>
#include <cmath>

using namespace std;
typedef unordered_map<string,int> wordsMap;

typedef struct fileArg
{
	char *file;
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
    char *file = pArg->file;
    const char *delimiter;
    delimiter = " \"\\,?:;<>~`!@#^&*()_+=/{}[]|\n\r\v\f";

    char *pch;
    char *last;
    pch = strtok_r(file, delimiter, &last);
    while(pch != NULL)
    {
    	(*tempMap)[pch]++;
    	pch = strtok_r(NULL, delimiter, &last);
    }
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
	//words number
	wordsMap res[numThread];
    FileArg f[numThread];

	ifstream ifs(FileName, ifstream::in);
	if(!ifs.is_open())
    {
        cerr<<"Fail to open file"<<endl;
        exit(-1);
    }
    //tell file's length
   	ifs.seekg(0, ifs.end);
   	long long length = ifs.tellg();
   	//unsigned int length = ifs.tellg();
   	ifs.seekg(0, ifs.beg);

   	//malloc memory to store file
   	char **file = (char**)malloc(sizeof(char*) * numThread);
   	for(int i = 0; i < numThread; i++)
   	{
   		long long temp = (long long)ceil((double)length/numThread);
   		long long s_length = \
   		(temp < (length - temp * i))? temp : (length - temp * i);

   		//unsigned int temp = (unsigned int)ceil((double)length/numThread);
   		//unsigned int s_length = \
   		//(temp < (length - temp * i))? temp : (length - temp * i);
   		file[i] = (char*)malloc(sizeof(char) * s_length);
   		memset(file[i], 0, s_length);
   		ifs.read(file[i], s_length);
   	}

    for(int i = 0; i < numThread; i++)
    {
    	//set arguments
		f[i].file = file[i];
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

	ifs.close();
	ifs.clear();
	for(int i = 0; i < numThread; i++)
		free(file[i]);
	free(file);

    return 0;
}
