#ifndef TRIE_H
#define TRIE_H

#include "TrieNode.h"
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class Trie
{
public:
    TrieNode *root;

    Trie();
    void Insert(std::string &word);
    std::vector<std::string> prefix_search(std::string prefix);
    bool exact_search(std::string target);
    void get_all_searches(std::vector<std::string> &result, TrieNode *node, std::string prefix);
    void serializeTrie(TrieNode *root, const std::string &filename);
    TrieNode *deserializeTrie(const std::string &filename);
};

#endif // TRIE_H
