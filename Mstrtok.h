#ifndef MSTRTOK_H
#define MSTRTOK_H

#include <vector>
#include <string>

bool MStrTok(const char* src, const char *sep, int srcLen, int sepLen, std::vector<std::string> &sa);

#endif