#include "Trie.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

TrieNode::TrieNode()
{
    isEndOfWord = false;
}

bool TrieNode::containsKey(char ch)
{
    return (links.find(ch) != links.end());
}

void TrieNode::put(char ch)
{
    links.insert(std::make_pair(ch, new TrieNode()));
}

void TrieNode::put(char ch, TrieNode *node)
{
    links[ch] = node;
}

TrieNode *TrieNode::get(char ch)
{
    return links[ch];
}

void TrieNode::markEnd()
{
    isEndOfWord = true;
}

bool TrieNode::isEnd()
{
    return isEndOfWord;
}

Trie::Trie()
{
    root = new TrieNode();
}

void Trie::Insert(std::string &word)
{
    int len = word.length();
    TrieNode *curr = root;
    for (int i = 0; i < len; i++)
    {
        if (!curr->containsKey(word[i]))
        {
            curr->put(word[i]);
        }
        curr = curr->get(word[i]);
    }
    curr->markEnd();
}

std::vector<std::string> Trie::prefix_search(std::string prefix)
{
    int len = prefix.length();
    TrieNode *curr = root;
    std::vector<std::string> result;
    for (int i = 0; i < len; i++)
    {
        if (!curr->containsKey(prefix[i]))
        {
            return result;
        }
        curr = curr->get(prefix[i]);
    }
    get_all_searches(result, curr, prefix);
    return result;
}

void Trie::get_all_searches(std::vector<std::string> &result, TrieNode *node, std::string prefix)
{
    if (node == nullptr)
        return;
    if (node->isEnd())
    {
        result.push_back(prefix);
    }
    for (auto nxt : node->links)
    {
        get_all_searches(result, nxt.second, prefix + nxt.first);
    }
}

bool Trie::exact_search(std::string target)
{
    int len = target.length();
    TrieNode *curr = root;

    for (int i = 0; i < len; i++)
    {
        if (!curr->containsKey(target[i]))
        {
            return false;
        }
        curr = curr->get(target[i]);
    }
    return curr->isEnd();
}