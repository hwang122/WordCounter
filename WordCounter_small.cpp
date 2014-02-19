#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <queue>
#include <unordered_map>
#include <dirent.h>
#include <pthread.h>

using namespace std;

queue<string> FilePath;
pthread_mutex_t queueMutex;
typedef unordered_map<string,int> wordsMap;

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

void initPath(char *DirName)
{
    DIR *dir;
    struct dirent *ptr;

    if((dir = opendir(DirName)) == NULL)
    {
        cerr<<"Fail to open directory"<<endl;
        exit(-1);
    }

    while((ptr = readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)
            continue;
        else if(ptr->d_type == 8)
        {
            char *temp = (char*)malloc(sizeof(char)*40);
            memset(temp, 0, 40);
            strcat(temp, DirName);
            strcat(temp, ptr->d_name);
            string path = temp;
            FilePath.push(path);
        }
    }

    closedir(dir);
}

/**
void removePunct(char *str)
{
    int i, j;
    for (i = j = 0; str[i] != '\0'; i++) {
        if (!ispunct(str[i])) {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}
**/

void *Counter(void *arg)
{
    char *KeyWord = (char*)arg;
    int wordLength = strlen(KeyWord);
    //int *numWords = (int*)malloc(sizeof(int));
	//*numWords = 0;
	wordsMap tempMap;
    fstream ifs;
    pthread_mutex_lock(&queueMutex);

    while(!FilePath.empty())
    {
        char *path = (char*)malloc(sizeof(char)*100);
        memset(path, 0, 100);
        strcpy(path, FilePath.front().c_str());
        FilePath.pop();

        ifs.open(path, ifstream::in);

        if(!ifs.is_open())
        {
            cerr<<"Fail to open file"<<endl;
            exit(-1);
        }
        //get file's length
        ifs.seekg(0, ifs.end);
        int length = ifs.tellg();
        ifs.seekg(0, ifs.beg);

        char *file = (char*)malloc(sizeof(char)*length);
        ifs.read(file, length);

        ifs.close();
        ifs.clear();

        //unlock the mutex to let other thread read file
        pthread_mutex_unlock(&queueMutex);

        //count keyword
		char *pch;
		char *delimiter = " ?��;<>~`!@#^&*()_+=/\:;{}[]|";
		wordsMap::iterator found;
		pch = strtok(file, delimiter);
		while(pch != NULL)
		{
			found = tempMap.find(string(pch));		
			if(found == tempMap.end())
			{
				//not found
				tempMap.insert(make_pair<string, int>(string(pch), 1));
			}
			else
			{
				//found
				found->second++;
			}
			pch = strtok(NULL, delimiter);
		}
		/**
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
		**/
        free(file);
        //lock to handle next file
        pthread_mutex_lock(&queueMutex);
    }
    //all files have been handled, unlock
    pthread_mutex_unlock(&queueMutex);

    //return number of words
    return &tempMap;
}

int main(int argc, char *argv[])
{
	//get arguments from command line
    if(argc != 4){
        cout<<"Usage: "<<argv[0]<<" [directory name] [searching words] \
            [number of thread]"<<endl;
        return -1;
    }

    char *DirName = argv[1];
	char *Keyword = argv[2];
	int numThread = atoi(argv[3]);
	pthread_t tid[numThread];
	pthread_mutex_init(&queueMutex, NULL);
	//words number
	wordsMap res[numThread];
	//int *res[numThread];
	int numWords = 0;

	//remove punctuation character from keyword
	//removePunct(Keyword);

    //init file queue
    initPath(DirName);

    for(int i = 0; i < numThread; i++)
        pthread_create(&tid[i], NULL, Counter, (void*)Keyword);

    for(int i = 0; i < numThread; i++)
        pthread_join(tid[i], (void**)(res[i]));

    //for(int i = 0; i < numThread; i++)
    //    numWords += *res[i];
	wordsMap sum(merge(res, numThreads));
	wordsMap::iterator found = sum.find(string(keyWord));
	if(found != sum.end())
		cout<<"Total number of key words is "<<found->second<<endl;
	else
		cout<<"Key word not found!"<<endl;

    return 0;
}
