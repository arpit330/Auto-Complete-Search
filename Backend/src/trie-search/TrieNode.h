#ifndef TRIENODE_H
#define TRIENODE_H

#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class TrieNode
{
public:
    std::unordered_map<char, TrieNode *> links;
    bool isEndOfWord;

    TrieNode();
    bool containsKey(char ch);
    void put(char ch);
    void put(char ch, TrieNode* node);
    TrieNode *get(char ch);
    void markEnd();
    bool isEnd();
    void serializeNode(TrieNode *node, json &j);
    void deserializeNode(TrieNode *node, const json &j);
};

#endif // TRIENODE_H
