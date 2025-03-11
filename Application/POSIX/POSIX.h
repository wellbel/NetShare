#if defined(__APPLE__) || defined(__linux__)
#ifndef NSHOSX
#define NSHOSX

#include <iostream>
#include <fstream>
#include <string>

#define BUFFERSIZE 2048
#define FILEFAILLIMIT 10

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h> 

using std::string;
using std::cout;

struct fileInfo {
    bool error = true;
    string fileName;
    string path;
    size_t fileSize;
};

fileInfo nextFile(std::fstream&, const string&, std::ifstream&);
bool gotoLine(std::fstream&, size_t);
bool recieve(int, char*);
int main(int, char**);

#endif
#endif