#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <pthread.h>

#define BUFFER_SIZE 250000000

using namespace std;

pthread_mutex_t streamMutex;

typedef struct fileArg
{
	char *Keyword;
	ifstream &ifs;
} File;

void *removePunct(char *str)
{
    int i, j;
    for (i = j = 0; str[i] != '\0'; i++) {
        if (!ispunct(str[i])) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

void *Counter(void *arg)
{
	File *pFile = (File *)arg;
    char *KeyWord = pFile->Keyword;
    int wordLength = strlen(KeyWord);
	ifstream ifs = pFile->ifs;
	//result of counter
    int *numWords = (int*)malloc(sizeof(int));
	*numWords = 0;
	
    pthread_mutex_lock(&streamMutex);
	
	while(!ifs.eof())
	{
		//ifs.seekg(position, ifs.beg);
		char *file = (char*)malloc(BUFFER_SIZE);
		ifs.read(file, BUFFER_SIZE);

		//unlock the mutex to let other thread read file
		pthread_mutex_unlock(&streamMutex);

		//count keyword
		char *pch = file;
		while(pch != NULL)
		{
			pch = strstr(pch, KeyWord);
			if(pch != NULL)
			{
				(*numWords)++;
				pch += wordLength;
			}
		}
		free(file);
		//lock to handle next file
		pthread_mutex_lock(&streamMutex);
	}
		
    //all files have been handled, unlock
    pthread_mutex_unlock(&streamMutex);

    //return number of words
    return (void*)numWords;
}

int main(int argc, char *argv[])
{
	//get arguments from command line
    if(argc != 4){
        cout<<"Usage: "<<argv[0]<<" [file name] [searching words] \
            [number of thread]"<<endl;
        return -1;
    }

    char *FileName = argv[1];
	char *Keyword = argv[2];
	int numThread = atoi(argv[3]);
	
	pthread_t tid[numThread];
	pthread_mutex_init(&streamMutex, NULL);
	//words number
	int *res[numThread];
	int numWords = 0;

	//remove punctuation character from keyword
	removePunct(Keyword);
	
	//set arguments
	File f;
	f.Keyword = Keyword;
	ifstream ifs(Filename, ifstream::in);
	if(!ifs.is_open())
    {
        cerr<<"Fail to open file"<<endl;
        exit(-1);
    }
	f.ifs = ifs;
	
    for(int i = 0; i < numThread; i++)
        pthread_create(&tid[i], NULL, Counter, &f);

    for(int i = 0; i < numThread; i++)
        pthread_join(tid[i], (void**)(&res[i]));

    for(int i = 0; i < numThread; i++)
        numWords += *res[i];

    cout<<"Total number of key words is "<<numWords<<endl;

    return 0;
}
