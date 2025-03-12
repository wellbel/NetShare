#if !defined(__APPLE__) && !defined(__linux__)
#include "WIN32.H"

bool setOutputFile(const string &path, const string &fileName, std::ofstream &fileStream)
{
    if (fileStream.is_open())
        fileStream.close();
    fileStream.open(path + '\\' + fileName, std::ios_base::binary);

    return fileStream.is_open();
}

int main(int argc, char **argv)
{
    std::cout << "Windows" << std::endl;

    if (argc < 2)
    {
        std::cerr << "Please provide an absolute* path to folder.\n*Not Required." << std::endl;
        return 1;
    }
    if (argc > 2) {
        std::cout << "Cleaning WSA..." << std::endl;
        WSACleanup();
        return 0;
    }

    const string Path{argv[1]};
    std::ofstream OutputFile;
    std::cout << Path << std::endl;

    WSADATA wsaData;

    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET ListenSocket = INVALID_SOCKET;

    // Create socket for server to use
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (ListenSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    SOCKET ClientSocket = INVALID_SOCKET;
    cout << "Waiting for client..." << std::endl;
    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        cout << std::endl;
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Client accepted" << std::endl;
    // No longer need server socket after client is accepted
    closesocket(ListenSocket);

    char recvbuf[DEFAULT_BUFLEN];
    // For Recieving packets in orderly fashion where transfer is recv -> send loop instead of recv -> recv loop
    int iSendResult;
    char sendbuf[1] = {'g'};

    const string endFileString {"NetShareEnd"};
    // Receive until the peer shuts down the connection
    do {

        iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
        
        if (iResult > 0)
        {
            string fileData {recvbuf, static_cast<size_t>(iResult)};

            size_t fileNameEnd {fileData.find("\r")};
            size_t fileSizeEnd {fileData.find("\r", fileNameEnd+1)};

            string fileName {fileData.substr(0, fileNameEnd)};

            string fileSizeStr {fileData.substr(fileNameEnd + 1, fileSizeEnd - fileNameEnd)};
            size_t remainingBytes{0};
            size_t recvBytes;
            try {
                remainingBytes = std::stoull(fileSizeStr);
            } catch (...) {
                std::cerr << "Failed to parse file size." << std::endl;
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }

            if (!setOutputFile(Path, fileName, OutputFile))
            {
                std::cerr << "Output file failed.\n\t" << Path << '\\' << fileName << std::endl;

                iResult = shutdown(ClientSocket, SD_BOTH);
                if (iResult == SOCKET_ERROR)
                {
                    printf("shutdown failed: %d\n", WSAGetLastError());
                    closesocket(ClientSocket);
                    WSACleanup();
                }
                return 1;
            }
            std::cout << fileName << ":" << remainingBytes << std::endl;

            iSendResult = send(ClientSocket, sendbuf, 1, 0);
            if (iSendResult == SOCKET_ERROR)
            {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }

            do {
                iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
                recvBytes = static_cast<size_t>(iResult);
                OutputFile.write(recvbuf, iResult);
                if (recvBytes > remainingBytes) {
                    std::cerr << "Receieved more bytes than expected." << std::endl;
                    closesocket(ClientSocket);
                    WSACleanup();
                    return 1;
                } 
                remainingBytes -= static_cast<size_t>(iResult);
            } while (remainingBytes > 0 && iResult > 0); // Whole file

            iSendResult = send(ClientSocket, sendbuf, 1, 0);
            if (iSendResult == SOCKET_ERROR)
            {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
        }
        else if (iResult == 0)
            printf("Client closed connection.\n");
        else
        {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            break;
        }

    } while (iResult > 0);

    iResult = shutdown(ClientSocket, SD_BOTH);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
    OutputFile.close();

    std::cin;

    return 0;
}

#endif