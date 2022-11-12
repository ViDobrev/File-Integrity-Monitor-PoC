#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <fstream>
#include <filesystem>
#include <algorithm>

// prototype for linker
std::string calculate_file_sha256(std::ifstream& file);

struct FileHashStorage {
    std::unordered_map<std::string, std::string> hashes;

    bool add_file_hash(std::string file_path, std::string hash);
    bool add_file(std::string file_path);
    void add_folder(std::string folder_path);
    void remove_file(std::string file_path);
    void remove_folder(std::string folder_path);
    void update_file(std::string file_path);
    void update_folder(std::string folder_path);
};