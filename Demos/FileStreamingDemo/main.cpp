#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#define BUFFERSIZE 1024

#define INPUTFILE "file.txt"
#define OUTPUTFILE "out.txt"

const std::string Path{"/Users/maxim/CProjects/NetShareClient/Demos/FileStreamingDemo/build/Images/"};

void printProgress(const std::string& fileName, size_t curr, size_t max){ // Implementation and definition look very unoptimized
    std::cout << '\r';
    size_t progress{static_cast<size_t>((curr/(double)max)*10)};
    std::string bar{std::string(progress,'#')+std::string(10-progress,'-')};

    std::cout << fileName << "\t[" << bar << "] " << std::flush;
}

// File streaming demo for base functionality for application 
// The demonstration here will be split between the client and server

int main() {
    std::cout << Path + INPUTFILE << '\n' << Path + OUTPUTFILE << std::endl;
    std::ifstream image{Path + INPUTFILE, std::ios_base::binary};
    std::ofstream outputImage{Path + OUTPUTFILE, std::ios_base::binary};

    char buffer[BUFFERSIZE];
    if(image.is_open() && outputImage.is_open()) {
        image.seekg(0, std::ios_base::end);
        size_t fileSize{static_cast<size_t>(image.tellg())};
        std::cout << "File Size: "<< fileSize << " bytes" << std::endl;

        image.seekg(0);
        
        size_t portions{fileSize/BUFFERSIZE};
        size_t count{0};

        std::cout << std::endl;

        while(image.read(buffer, BUFFERSIZE) && !image.fail() && !image.eof()) { // All the bytes in the size 
            outputImage.write(buffer, image.gcount());
            count++;
            printProgress("ICIMG.JPG", count, portions);
        }
        // Last few bytes
        if (image.gcount() > 0) outputImage.write(buffer, image.gcount());
        printProgress("ICIMG.JPG", portions, portions);
        std::cout << std::endl;

    } else {
        std::cout << "Image " << image.is_open() << std::endl;
        std::cout << "OutputImage " << outputImage.is_open() << std::endl;
        return 1;
    }

    image.close();
    outputImage.close();

    std::cout << "Process Complete" << std::endl;
    return 0;
}