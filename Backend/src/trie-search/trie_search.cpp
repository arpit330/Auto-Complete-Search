#include <napi.h>
#include "Trie.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;
using namespace Napi;

using json = nlohmann::json;

Trie *trie = new Trie(); // Initialize the global Trie object

// Function to read an array of strings from a file
vector<string> get_array_from_file(const string &src)
{
    fstream file(src);
    vector<string> words;
    string word;

    if (!file)
    {
        cerr << "\nCan't Open File " << src << " Some Error Occurred.!\n";
        return words;
    }

    while (getline(file, word))
    {
        if (!word.empty())
        {
            words.push_back(word);
        }
    }

    return words;
}

// Function to write JSON data to a file
// void write_json_to_file(const string &filename, const json &j)
// {
//     std::ofstream o(filename);
//     o << j.dump(4); // Pretty print the JSON with an indent of 4
// }

// Function to read JSON data from a file
// json read_json_from_file(const string &filename)
// {
//     std::ifstream i(filename);
//     json j;
//     i >> j;
//     return j;
// }

// Function to write JSON data to a file
// void write_json_to_file(const std::string &filename, const json &j)
// {
//     std::ofstream o(filename);
//     if (!o)
//     {
//         cerr << "Failed to open file for writing: " << filename << endl;
//         return;
//     }
//     o << j.dump(4); // Pretty print the JSON with an indent of 4
// }

// // Function to read JSON data from a file
// json read_json_from_file(const std::string &filename)
// {
//     std::ifstream i(filename);
//     json j;
//     if (!i)
//     {
//         cerr << "Failed to open file for reading: " << filename << endl;
//         return j; // Return an empty json object
//     }
//     try
//     {
//         i >> j;
//     }
//     catch (const json::parse_error &e)
//     {
//         std::cerr << "JSON parsing error: " << e.what() << std::endl;
//     }
//     return j;
// }

// Function to check if a file exists
bool fileExists(const std::string &filename)
{
    std::ifstream file(filename);
    return file.good();
}

// Function to read a JSON file
json read_json_from_file(const std::string &filename)
{
    json j;
    if (fileExists(filename))
    {
        std::ifstream file(filename);
        if (file.is_open())
        {
            try
            {
                file >> j;
            }
            catch (json::parse_error &e)
            {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
                throw;
            }
            file.close();
        }
        else
        {
            std::cerr << "Unable to open file for reading: " << filename << std::endl;
        }
    }
    else
    {
        std::cerr << "File does not exist: " << filename << std::endl;
    }
    return j;
}

// Function to write a JSON object to a file
void write_json_to_file(const std::string &filename, const json &j)
{

    std::ofstream file(filename);
    if (file.is_open())
    {
        try
        {
            file << j.dump(4); // Pretty print with 4 spaces
        }
        catch (std::exception &e)
        {
            std::cerr << "Error writing to file: " << e.what() << std::endl;
            throw;
        }
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file for writing: " << filename << std::endl;
    }
}

// Serialize Trie into a JSON object
void SerializeNode(TrieNode *node, json &j)
{
    if (node == nullptr)
        return;

    j["isEndOfWord"] = node->isEnd();

    for (const auto &link : node->links)
    {
        json childObj;
        SerializeNode(link.second, childObj);
        j["children"][std::string(1, link.first)] = childObj;
    }
}

// Deserialize JSON into a Trie node
TrieNode *DeserializeNode(const json &j)
{
    TrieNode *node = new TrieNode();

    // Check if 'isEndOfWord' exists and is a boolean
    if (j.contains("isEndOfWord") && j["isEndOfWord"].is_boolean())
    {
        if (j["isEndOfWord"].get<bool>())
        {
            node->markEnd();
        }
    }

    // Check if 'children' exists and is an object
    if (j.contains("children") && j["children"].is_object())
    {
        for (auto &[key, value] : j["children"].items())
        {
            // Check that the key is a string and the value is an object
            if (key.length() == 1 && value.is_object())
            {
                char ch = key[0];
                TrieNode *childNode = DeserializeNode(value);
                node->put(ch, childNode);
            }
            else
            {
                std::cerr << "Invalid key or value type in children" << std::endl;
            }
        }
    }
    else
    {
        std::cerr << "'children' key missing or is not an object" << std::endl;
    }

    return node;
}

void SerializeNodeBinary(TrieNode *node, std::ofstream &output)
{
    if (node == nullptr)
        return;

    // Write whether this node is an end of a word
    output.write(reinterpret_cast<const char *>(&node->isEndOfWord), sizeof(bool));

    // Write the number of children
    uint32_t numChildren = node->links.size();
    output.write(reinterpret_cast<const char *>(&numChildren), sizeof(uint32_t));

    // Write each child
    for (const auto &pair : node->links)
    {
        // Write the child character
        char childChar = pair.first;
        output.write(reinterpret_cast<const char *>(&childChar), sizeof(char));

        // Recursively serialize the child node
        SerializeNodeBinary(pair.second, output);
    }
}

// Write entire trie to binary file
void WriteTrieToBinary(TrieNode *root, const std::string &filename)
{
    std::ofstream output(filename, std::ios::binary);
    if (output.is_open())
    {
        SerializeNodeBinary(root, output);
        output.close();
    }
    else
    {
        std::cerr << "Failed to open file for writing." << std::endl;
    }
}

TrieNode *DeserializeNodeBinary(std::ifstream &input)
{
    TrieNode *node = new TrieNode();

    // Read if it's an end of the word
    input.read(reinterpret_cast<char *>(&node->isEndOfWord), sizeof(bool));

    // Read the number of children
    uint32_t numChildren;
    input.read(reinterpret_cast<char *>(&numChildren), sizeof(uint32_t));

    // Read each child
    for (uint32_t i = 0; i < numChildren; ++i)
    {
        char childChar;
        input.read(reinterpret_cast<char *>(&childChar), sizeof(char));

        TrieNode *childNode = DeserializeNodeBinary(input);
        node->put(childChar, childNode);
    }

    return node;
}

// Read entire trie from binary file
TrieNode *ReadTrieFromBinary(const std::string &filename)
{
    std::ifstream input(filename, std::ios::binary);
    if (input.is_open())
    {
        TrieNode *root = DeserializeNodeBinary(input);
        input.close();
        return root;
    }
    else
    {
        std::cerr << "Failed to open file for reading." << std::endl;
        return nullptr;
    }
}

// N-API function to build the Trie from a file or a serialized JSON file
Napi::Value BuildTrie(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "Filename expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string filename = info[0].As<Napi::String>();
    bool forceBuild = false;

    // Check if forceBuild argument is provided
    if (info.Length() > 1 && info[1].IsBoolean())
    {
        forceBuild = info[1].As<Napi::Boolean>().Value();
    }

    std::string jsonFile = filename + ".json";
    std::string textFile = filename + ".txt";
    std::string binaryFile = filename + ".bin";

    if (!forceBuild && std::ifstream(jsonFile))
    {
        // If serialized file exists and forceBuild is not requested, load from JSON
        // json j = read_json_from_file(jsonFile);

        // trie->root = DeserializeNode(j);

        trie->root = ReadTrieFromBinary(binaryFile);
        std::cout << "Trie loaded from " << jsonFile << std::endl;
    }
    else
    {
        // Build Trie from text file and save to JSON for future use
        vector<string> data = get_array_from_file(textFile);
        for (string &word : data)
        {
            trie->Insert(word);
        }

        json j;

        // SerializeNode(trie->root, j);
        // write_json_to_file(jsonFile, j);

        WriteTrieToBinary(trie->root, binaryFile);

        std::cout
            << "Trie built from " << textFile << " and serialized to " << jsonFile << std::endl;
    }

    return env.Null();
}

// N-API function to insert a word into the Trie
Napi::Value Insert(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string word = info[0].As<Napi::String>();
    trie->Insert(word); // Correct usage of pointer notation
    return env.Null();
}

// N-API function to build the Trie from a file
// Napi::Value BuildTrie(const Napi::CallbackInfo &info)
// {
//     Napi::Env env = info.Env();
//     if (info.Length() < 1 || !info[0].IsString())
//     {
//         Napi::TypeError::New(env, "Filename expected").ThrowAsJavaScriptException();
//         return env.Null();
//     }

//     std::string filename = info[0].As<Napi::String>();
//     vector<string> data = get_array_from_file(filename + ".txt");

//     for (string &word : data)
//     {
//         trie->Insert(word);
//     }

//     return env.Null();
// }

// N-API function to perform prefix search in the Trie
Napi::Value PrefixSearch(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "Prefix expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string prefix = info[0].As<Napi::String>();
    std::vector<std::string> results = trie->prefix_search(prefix);

    Napi::Array jsResults = Napi::Array::New(env, results.size());
    for (size_t i = 0; i < results.size(); ++i)
    {
        jsResults[i] = Napi::String::New(env, results[i]);
    }

    return jsResults;
}

// N-API function to perform exact search in the Trie
Napi::Value ExactSearch(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "Target word expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string target = info[0].As<Napi::String>();
    bool exists = trie->exact_search(target);

    return Napi::Boolean::New(env, exists);
}

// void SerializeNode(TrieNode *node, Napi::Object &obj, Env env)
// {
//     if (node == nullptr)
//         return;

//     obj.Set("isEndOfWord", Napi::Boolean::New(env, node->isEnd()));

//     Napi::Object children = Napi::Object::New(env);
//     for (const auto &link : node->links)
//     {
//         Napi::Object childObj = Napi::Object::New(env);
//         SerializeNode(link.second, childObj, env);
//         children.Set(Napi::String::New(env, std::string(1, link.first)), childObj);
//     }
//     obj.Set("children", children);
// }

// TrieNode *DeserializeNode(const Napi::Object &obj, Env env)
// {
//     TrieNode *node = new TrieNode();
//     if (obj.Get("isEndOfWord").As<Napi::Boolean>().Value())
//     {
//         node->markEnd();
//     }

//     Object children = obj.Get("children").As<Object>();
//     Array keys = children.GetPropertyNames();
//     for (size_t i = 0; i < keys.Length(); i++)
//     {
//         String key = keys.Get(i).As<String>();
//         char ch = key.Utf8Value()[0];
//         TrieNode *childNode = DeserializeNode(children.Get(key).As<Object>(), env);
//         node->put(ch, childNode);
//     }
//     return node;
// }

// Napi::Value Serialize(const Napi::CallbackInfo &info)
// {
//     Env env = info.Env();
//     Napi::Object result = Napi::Object::New(env);

//     SerializeNode(trie->root, result, env);
//     return result;
// }

// Napi::Value Deserialize(const Napi::CallbackInfo &info)
// {
//     Env env = info.Env();
//     Napi::Object obj = info[0].As<Napi::Object>();

//     trie->root = DeserializeNode(obj, env);
//     // Use root node here
//     return env.Undefined();
// }

// Initialize and export functions
Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "insert"), Napi::Function::New(env, Insert));
    exports.Set(Napi::String::New(env, "buildTrie"), Napi::Function::New(env, BuildTrie));
    exports.Set(Napi::String::New(env, "prefixSearch"), Napi::Function::New(env, PrefixSearch));
    exports.Set(Napi::String::New(env, "exactSearch"), Napi::Function::New(env, ExactSearch));
    return exports;
}

// Register and initialize the addon
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
