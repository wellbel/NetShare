# NetShare

Raw file transfer from POSIX compliant systems to Windows machines using TCP/IP with a custom protocol.

## Purpose

Windows machines, Apple devices, and Linux machines most commonly use NTFS, APFS, and ext4 respectively for their primary drives. These files systems can't read and write with each other directly since they're mostly proprietary. If one were to looking to transfer files from their MacBook to an NTFS drive they not be able to. NetShare acts as a bridge from APFS or ext4 to NTFS- but not vice versa. For my use, I only needed this specific direction but a reverse implementation of the concept is simple.

### Features
- True raw file transfer. System metadata is not omitted or converted to NTFS. 
