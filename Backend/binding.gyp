{
  "targets": [
    {
      "target_name": "trie_search",
      "sources": ["src/trie-search/trie_search.cpp", "src/trie-search/Trie.cpp", "src/trie-search/KMP.cpp"],
      "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
      "libraries":    [],
      "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
      "cflags_cc": ["-fexceptions"]
    }
  ]
}
