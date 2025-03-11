#if defined(__APPLE__) || defined(__linux__)
   #include "POSIX/POSIX.h"
#else 
    // Assume Win32 because there is no definitive macro
    // The Windows program will act as the server in this exchange
    // The main purpose of this program is to recieve files from Apple/Linux machines and write them onto Windows machines
    // The reason for this narrow purpose is that I would like to export my Photo Library on my Macbook onto an SSD I have as it contains my entire iCloud Photo Library and the SSD's drives are currently in NTFS which MacOS cannot write to
    #include "WIN32/WIN32.H"
#endif