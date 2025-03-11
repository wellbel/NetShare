#if defined(__APPLE__) || defined(__linux__)
#include "POSIX.h"

fileInfo nextFile(std::fstream& stream, const string& path, std::ifstream& file) {
    fileInfo returnInfo; 
    string fileName;

    if (file.is_open()) file.close();
    if (!std::getline(stream, fileName)) return returnInfo; // Defined for error to be true // eof 
    returnInfo.eof = false;

    file.open(path + '/' + fileName, std::ios_base::binary); // Just in case path does not end with a slash
    if (file.is_open()) {
        returnInfo.error = false;
        returnInfo.fileName = fileName;
        returnInfo.path = path + '/' + fileName;
        file.seekg(0, std::ios_base::end);
        returnInfo.fileSize = static_cast<size_t>(file.tellg());
        file.seekg(0);
    } else {
        returnInfo.fileName = fileName;
        returnInfo.path = path + '/' + fileName;

        std::cout << "Failed.\t" << returnInfo.path << std::endl;
    }
    return returnInfo;
}

bool gotoLine(std::fstream& stream, size_t lineNumber) {
    --lineNumber;
    std::string nothing;
    for(;lineNumber > 0; lineNumber--) {
        if(!std::getline(stream, nothing)) return false;
    }
    return true;
}

bool recieve(int sockfd, char* pb) {
    return recv(sockfd, pb, 1, 0) < 1;
}

int main(int argc, char** argv) {
    cout << "Apple/Linux" << std::endl;

    if (argc < 5) {
        std::cerr << "Please provide:\n>\tServer Address\n>\tPort\n>\tPath to Folder\n>\tPath to File list" << std::endl;
        return EXIT_FAILURE;
    }

    const char* serverAddr = argv[1];
    unsigned short portNum = atoi(argv[2]);
    const string Path = argv[3];
    std::fstream FileList {argv[4]};
    std::ifstream ActiveFile{};

    size_t startLine = argc > 5 ? atoi(argv[5]) : 0;
    if (startLine != 0 && gotoLine(FileList, static_cast<size_t>(startLine))) {
        cout << "Starting at line: " << startLine << std::endl;
    }

    if (!FileList.is_open()) {
        std::cerr << "Could not open File list" << std:: endl;
        return EXIT_FAILURE;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server {
        .sin_family = AF_INET,
        .sin_port = htons(portNum),
        .sin_addr.s_addr = inet_addr(serverAddr)
    };

    int cv = connect(sockfd, (struct sockaddr *) &server, sizeof(struct sockaddr_in));
    if(cv == -1) {
        std::cerr << "Connection failed; errno: " << errno << std::endl;
        return EXIT_FAILURE;
    } else {
        cout << "Connection successful to: " << inet_ntoa(server.sin_addr) << ':' << ntohs(server.sin_port) << std::endl;
    }

    char buffer[BUFFERSIZE];
    bool fileExists = true;
    int fileFails{0};

    char endFileSeq[11] {'N','e','t','S','h','a','r','e','E','n','d'};

    char placeboffer[1];

    std::cout << "Files Ready to transfer" << fileFails << std::endl;

    while(fileExists && fileFails <= FILEFAILLIMIT) {
        fileInfo fInfo = nextFile(FileList, Path, ActiveFile);
        if (fInfo.error) {
            if (!fInfo.eof) {
                std::cout << "File failed to open.\t" << fInfo.fileName << std::endl;
                fileFails++;
                continue;
            } else {
                std::cout << "Reached eof on FileList." << std::endl;
                break;
            }
        } 
        send(sockfd, fInfo.fileName.c_str(), fInfo.fileName.length(), 0);
        std::cout << fInfo.fileName << '\t' << fInfo.fileSize << std::endl;

        if (recieve(sockfd, placeboffer)) {
            std::cout << "Server closed connection" << std::endl;
            break;
        }
        // Transfer file data
        while(ActiveFile.read(buffer, BUFFERSIZE) && !ActiveFile.fail() && !ActiveFile.eof())
            send(sockfd, buffer, static_cast<size_t>(ActiveFile.gcount()), 0);
        if (ActiveFile.gcount() > 0) send(sockfd, buffer, static_cast<size_t>(ActiveFile.gcount()), 0);
        send(sockfd, endFileSeq, 11, 0);
        
        if (recieve(sockfd, placeboffer)) {
            std::cout << "Server closed connection" << std::endl;
            break;
        }
    }

    FileList.close();
    close(sockfd);

    if (fileFails >= FILEFAILLIMIT) {
        std::cerr << "Maximum file failures reached. " << fileFails << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif