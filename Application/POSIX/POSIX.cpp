#if defined(__APPLE__) || defined(__linux__)
#include "POSIX.h"

fileInfo nextFile(std::fstream& stream, const string& path, std::ifstream& file) {
    fileInfo returnInfo; 
    string fileName;

    if (file.is_open()) file.close();
    if (!std::getline(stream, fileName)) return returnInfo; // Defined for error to be true

    file.open(path + '/' + fileName); // Just in case path does not end with a slash
    if (file.is_open()) {
        returnInfo.error = false;
        returnInfo.fileName = fileName;
        returnInfo.path = path + '/' + fileName;
        file.seekg(0, std::ios_base::end);
        returnInfo.fileSize = static_cast<size_t>(file.tellg());
        file.seekg(0);
    } else {
        returnInfo.fileName = fileName;
        returnInfo.path = path + fileName;
    }
    return returnInfo;
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
        cout << "Connection successful to: " << inet_ntoa(server.sin_addr) << ':' << ntohs(server.sin_port) << '\n';
    }

    char buffer[BUFFERSIZE];
    bool fileExists = true;
    int fileFails{0};

    std::cout << "Files Ready to tranfer" << std::endl;

    while(fileExists && fileFails < FILEFAILLIMIT) {
        fileInfo fInfo = nextFile(FileList, Path, ActiveFile);
        if (fInfo.error) {
            std::cerr << "File failed to open.\t" << fInfo.path;
            fileFails++;
            continue;
        }
        send(sockfd, fInfo.fileName.c_str(), fInfo.fileName.length(), 0);
        std::cout << fInfo.fileName << std::endl;

        // Transfer file data
        while(ActiveFile.read(buffer, BUFFERSIZE) && !ActiveFile.fail() && !ActiveFile.eof())
            send(sockfd, buffer, BUFFERSIZE, 0);
        if (ActiveFile.gcount() > 0) send(sockfd, buffer, static_cast<size_t>(ActiveFile.gcount()), 0); 

        // End file data transmission
        buffer[0] = '\r'; // CR char - int 10
        send(sockfd, buffer, 1, 0);
    }

    FileList.close();
    close(sockfd);

    if (fileFails >= FILEFAILLIMIT) {
        std::cerr << "Maximum file failures reached.";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

#endif