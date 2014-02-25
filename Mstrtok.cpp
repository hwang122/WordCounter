#include <cstring>
#include "Mstrtok.h"

using namespace std;

bool MStrTok(const char* src, const char *sep, int srcLen, int sepLen, vector<string> &sa) 
{ 
    if( (src == NULL) || (sep == NULL) ) 
    { 
        return false; 
    } 
     
    //int nMinLenSrc = min(strlen(src), srcLen);
    int nMinLenSrc = strlen(src) < srcLen ? strlen(src) : srcLen;
    //int nMinLensep = min(strlen(sep), sepLen);
    int nMinLensep = strlen(sep) < sepLen ? strlen(sep) : sepLen;
     
    char *pNew = new char[nMinLenSrc + 1]; 
    memset(pNew, 0, nMinLenSrc + 1); 
    memcpy(pNew, src, nMinLenSrc); 
         
    char *p = (char *)pNew; 
    char c;
    int i = 0, n = 0; 
    char *pStr = NULL;
    while(*p != 0 && n < nMinLenSrc) 
    {            
        c = *sep; 
        for(i = 0; (i < nMinLensep) && (c != 0); i++) 
        { 
            c = *(sep + i); 
            if(c == *p) 
            { 
                *p = 0; 
            } 
        }    
         
        if(*p == 0) 
        { 
            if(pStr != NULL) 
            { 
                sa.push_back(pStr); 
                pStr = NULL; 
            } 
        } 
        else 
        { 
            if (pStr == NULL) 
            { 
                pStr = p;                
            }            
        } 
         
        p++; 
        n++; 
    }
    if(pStr != NULL) 
    { 
        sa.push_back(pStr); 
    } 
     
    delete []pNew;  
    return true; 
}