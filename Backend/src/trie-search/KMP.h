#ifndef KMP_H
#define KMP_H

#include <string>
#include <vector>

void computeLPSArray(std::string pat, int M, int *lps);
int KMPSearch(std::string pat, std::string txt);
std::vector<std::string> Patter_search(std::vector<std::string> &data, std::string pattern);

#endif // KMP_H
