#include <thread>
#include <openssl/sha.h>
#include "FileHashStorage.h"

typedef unsigned char byte;

// globals
std::string hash_save_file_location = "hashes.txt";
FileHashStorage file_hashes;
std::string command_add_folder = "add folder";
std::string command_add_file = "add";
std::string command_remove_folder = "remove folder";
std::string command_remove_file = "remove";
std::string command_update_folder = "update folder";
std::string command_update_file = "update";
bool s_monitor_active = true;
bool s_monitor_paused = true;



std::string calculate_file_sha256(std::ifstream& file)
{
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<byte> fileData(fileSize);
    file.read((char*)&fileData[0], fileSize);

    std::string file_contents;
    for (byte b : fileData) file_contents += (char)b;

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)file_contents.c_str(), file_contents.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

void monitor() {
    using namespace std::literals::chrono_literals;
    std::string hash;
    std::ifstream file;

    while (s_monitor_active) {
        while (s_monitor_paused) {
            std::this_thread::sleep_for(1s);
        }

        if (!s_monitor_active) break;

        std::this_thread::sleep_for(1s);
        for (auto [file_path, verified_hash] : file_hashes.hashes) {
            if (s_monitor_paused) break;
            file.open(file_path, std::ios::binary);
            if (file.is_open()) {
                hash = calculate_file_sha256(file);
                file.close();
                if (hash != verified_hash) {
                    std::cout << file_path << " has changed." << std::endl;
                }
                continue;
            }

            std::cout << file_path << " could not be found anymore. Removing file from memory." << std::endl;
            file_hashes.remove_file(file_path);
            break;
        }
    }
}

void add_folder(std::string command) {
    std::string folder_path = command.substr(command_add_folder.size());
    folder_path.erase(0, folder_path.find_first_not_of(' '));
    std::replace(folder_path.begin(), folder_path.end(), '/', '\\');

    if (!std::filesystem::exists(folder_path)) {
        std::cout << "Folder could not be found." << std::endl;
        return;
    }

    if (!std::filesystem::is_directory(folder_path)) {
        std::cout << "Specified path is not a folder." << std::endl;
        return;
    }

    file_hashes.add_folder(folder_path);
}
void add_file(std::string command) {
    std::string file_path = command.substr(command_add_file.size());
    file_path.erase(0, file_path.find_first_not_of(' '));
    std::replace(file_path.begin(), file_path.end(), '/', '\\');

    if (!std::filesystem::exists(file_path)) {
        std::cout << "File could not be found." << std::endl;
        return;
    }

    if (std::filesystem::is_directory(file_path)) {
        std::cout << "Specified path is a folder, not a file." << std::endl;
        return;
    }

    file_hashes.add_file(file_path);
}

void remove_folder(std::string command) {
    std::string folder_path = command.substr(command_remove_folder.size());
    folder_path.erase(0, folder_path.find_first_not_of(' '));
    std::replace(folder_path.begin(), folder_path.end(), '/', '\\');

    file_hashes.remove_folder(folder_path);
}
void remove_file(std::string command) {
    std::string file_path = command.substr(command_remove_file.size());
    file_path.erase(0, file_path.find_first_not_of(' '));
    std::replace(file_path.begin(), file_path.end(), '/', '\\');

    file_hashes.remove_file(file_path);
}

void update_file(std::string command) {
    std::string file_path = command.substr(command_update_file.size());
    file_path.erase(0, file_path.find_first_not_of(' '));
    std::replace(file_path.begin(), file_path.end(), '/', '\\');

    file_hashes.update_file(file_path);
}
void update_folder(std::string command) {
    std::string folder_path = command.substr(command_update_folder.size());
    folder_path.erase(0, folder_path.find_first_not_of(' '));
    std::replace(folder_path.begin(), folder_path.end(), '/', '\\');

    file_hashes.update_folder(folder_path);
}


void save_hashes() {
    std::cout << "Saving hashes.." << std::endl;

    std::fstream file;

    file.open(hash_save_file_location, std::ios::out);
    if (!file.is_open()) {
        std::cout << "Error in opening hashes.txt!" << std::endl;
        return;
    }

    int i = 0;
    for (auto [file_path, hash] : file_hashes.hashes) {
        i++;
        file << file_path << ":" << hash;
        if (i == file_hashes.hashes.size()) break;
        file << std::endl;
    }

    file.close();
}
bool load_previous_file_hashes() {
    std::ifstream file;
    file.open(hash_save_file_location);
    if (!file.is_open()) return false;

    std::cout << "Load previous session's hashes? [y/n] ";
    std::string input;
    while (true)
    {
        std::cin >> input;
        if (input == "y") break;
        //if (input == "n") return;
        return false;
    }

    std::string line, file_path, hash;
    while (std::getline(file, line)) {
        file_path = line.substr(0, line.find_last_of(':'));
        hash = line.substr(line.find_last_of(':') + 1);
        file_hashes.add_file_hash(file_path, hash);
    }

    return true;
}


int process_command(std::string& command) {
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    command.erase(0, command.find_first_not_of(' '));
    command.erase(command.find_last_not_of(' ') + 1);

    if (command == "start") {
        //start monitoring
        return 3;
    }
    else if (command.size() > command_add_folder.size() && command.substr(0, command_add_folder.size()) == command_add_folder) {
        // add files in a folder
        add_folder(command);
        return 0;
    }
    else if (command.size() > command_add_file.size() && command.substr(0, command_add_file.size()) == command_add_file) {
        // add specified file
        add_file(command);
        return 0;
    }
    else if (command.size() > command_remove_folder.size() && command.substr(0, command_remove_folder.size()) == command_remove_folder) {
        // add specified file
        remove_folder(command);
        return 0;
    }
    else if (command.size() > command_remove_file.size() && command.substr(0, command_remove_file.size()) == command_remove_file) {
        // add specified file
        remove_file(command);
        return 0;
    }
    else if (command.size() > command_update_folder.size() && command.substr(0, command_update_folder.size()) == command_update_folder) {
        // add specified file
        update_folder(command);
        return 0;
    }
    else if (command.size() > command_update_file.size() && command.substr(0, command_update_file.size()) == command_update_file) {
        // add specified file
        update_file(command);
        return 0;
    }
    else if (command == "exit" || command == "quit") {
        // exit program
        return 1;
    }
    else if (command == "") {
        // no command -> no output
        return 0;
    }

    // incorrect command
    std::cout << "Incorrect command syntax!" << std::endl;
    return 2;
}

int main() {
    load_previous_file_hashes();

    std::thread monitor_thread(monitor);

    std::string input;
    while (true) {
        std::getline(std::cin, input);

        int result = process_command(input);

        if (result == 1) {
            save_hashes();
            std::cout << "Exiting.." << std::endl;
            break;
        }
        if (result == 3) {
            // resume monitoring thread
            std::cout << "Monitoring.." << std::endl;
            s_monitor_paused = false;
        }

        if (!s_monitor_paused) {
            // pause monitoring thread so user input does not get interrupted
            std::cin.get();
            s_monitor_paused = true;
            std::cout << "Monitoring paused" << std::endl;
        }
    }
    s_monitor_active = false;
    s_monitor_paused = false;
    monitor_thread.join();
    return 0;
}