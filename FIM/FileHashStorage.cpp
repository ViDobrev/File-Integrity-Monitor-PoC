#include "FileHashStorage.h"
#include <openssl/sha.h>

typedef unsigned char byte;

bool FileHashStorage::add_file_hash(std::string file_path, std::string hash) {
    if (hashes.find(file_path) != hashes.end()) return false; // file already mapped

    hashes[file_path] = hash;

    std::cout << file_path << " is now being monitored." << std::endl;
    return true;
}

bool FileHashStorage::add_file(std::string file_path) {
    if (hashes.find(file_path) != hashes.end()) // file is already in mapped
    {
        std::cout << file_path << " is already being monitored." << std::endl;
        return false;
    }

    std::string file_contents;
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) return false;


    hashes[file_path] = calculate_file_sha256(file);
    file.close();

    std::cout << file_path << " is now being monitored." << std::endl;
    return true;
}

void FileHashStorage::add_folder(std::string folder_path) {
    std::string file_path;
    for (auto& entry : std::filesystem::directory_iterator(folder_path)) {
        if (!entry.is_directory()) {
            file_path = entry.path().string();
            std::transform(file_path.begin(), file_path.end(), file_path.begin(), ::tolower);
            this->add_file(file_path);
        }
    }
}

void FileHashStorage::remove_file(std::string file_path) {
    if (hashes.find(file_path) == hashes.end()) // file is not mapped;
    {
        std::cout << file_path << " is not a monitored file." << std::endl;
        return;
    }

    hashes.erase(file_path);

    std::cout << file_path << " is no longer being monitored." << std::endl;
    return;
}

void FileHashStorage::remove_folder(std::string folder_path) {
    std::vector<std::string> files_to_remove;
    for (auto [file_path, hash] : hashes) {
        if (file_path.size() > folder_path.size() && file_path.substr(0, folder_path.size()) == folder_path) {
            files_to_remove.emplace_back(file_path);
        }
    }

    for (std::string file_to_remove : files_to_remove) {
        hashes.erase(file_to_remove);

        std::cout << file_to_remove << " is no longer being monitored." << std::endl;
    }
}

void FileHashStorage::update_file(std::string file_path) {
    if (hashes.find(file_path) == hashes.end()) // file is not mapped
    {
        std::cout << file_path << " is not a monitored file." << std::endl;
        return;
    }

    std::string file_contents;
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) return;


    hashes[file_path] = calculate_file_sha256(file);
    file.close();

    std::cout << file_path << "'s hash has been updated." << std::endl;
}

void FileHashStorage::update_folder(std::string folder_path) {
    std::string file_path;
    for (auto& entry : std::filesystem::directory_iterator(folder_path)) {
        if (!entry.is_directory()) {
            file_path = entry.path().string();
            std::transform(file_path.begin(), file_path.end(), file_path.begin(), ::tolower);
            this->update_file(file_path);
        }
    }
}