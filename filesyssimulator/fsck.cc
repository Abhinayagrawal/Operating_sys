/**
 * a sinple fsck program
 * prints the errors in the filesystem.
 * checks the following:
 * 1. all the inodes mentioned in the directory entries have correct number of nlinks
 * 2. all blocks mentioned in the inodes are marked as allocated blocks
 * 3. all blocks not mentioned in the inodes are amrked as free blocks
 */

#include "Kernel.h"
#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>

map<int, int> inodeMap;

int populateMap(char *dirname) {

    FileSystem * fileSystem = Kernel::openFileSystems;
    int dir = Kernel::open(dirname, Kernel::O_RDWR);

    if( dir < 0 )
    {
        cout << "fsck: unable to open directory" << endl;
        return EXIT_FAILURE;
    }

    int status = 0;
    DirectoryEntry currentDirectoryEntry;

    while(true) {
        status = Kernel::readdir(dir, currentDirectoryEntry);

        if(status == 0)
        {
            break;
        }

        IndexNode currIndexNode;
        fileSystem->readIndexNode(&currIndexNode, currentDirectoryEntry.getIno());
        if((currIndexNode.getMode() & Kernel::S_IFMT ) == Kernel::S_IFDIR)
        {
            ++inodeMap[currentDirectoryEntry.getIno()];
            char *curDirName = currentDirectoryEntry.getName();

            char tempDirName[100];
            memset(tempDirName, '\0', 100);
            strcpy(tempDirName, dirname);

            char tempCurrDirName[100];
            memset(tempCurrDirName, '\0', 100);
            strcpy(tempCurrDirName, curDirName);

            if((strcmp(curDirName, ".") == 0) ||
                    (strcmp(curDirName, "..") == 0)) {
                continue;
            }
            if(strcmp(dirname, "/") == 0) {
                strcat(tempDirName, tempCurrDirName);
            } else {
                strcat(tempDirName, "/");
                strcat(tempDirName, tempCurrDirName);
            }

            int s = populateMap(tempDirName);
            if (s == EXIT_FAILURE) {
                cout << "error: some issue in populate method" << endl;
                return EXIT_FAILURE;
            } else {
                continue;
            }
        }
        if(status < 0)
        {
            cout << "error:" << endl;
            return EXIT_FAILURE;
        } else if (status > 0) {
            ++inodeMap[currentDirectoryEntry.getIno()];
        }
    }

    Kernel::close(dir);

    return EXIT_SUCCESS;
}

void checkNlinkCount() {
    FileSystem * fileSystem = Kernel::openFileSystems;
    IndexNode indexNode;
    for (map<int,int>::iterator it=inodeMap.begin(); it!=inodeMap.end(); ++it) {
        fileSystem->readIndexNode(&indexNode, it->first);
        if(indexNode.getNlink() != it->second)
        {
            cout << "Wrong nlink for Inode number:" << it->first << " [nlinks in fileSystem:" << 
                indexNode.getNlink() << " but total links found are:" << it->second << "]" << endl; 
        }
    } 
}


void checkBlockAllocation() {
    FileSystem * fileSystem = Kernel::openFileSystems;
    vector<int> allocatedBlockList;

    for (map<int,int>::iterator it=inodeMap.begin(); it!=inodeMap.end(); ++it) 
    {
        fileSystem->blockAllocationCheck(it->first, &allocatedBlockList);
    }

    fileSystem->blockFreeCheck(allocatedBlockList);
}


int main (int argc, char ** argv) {
    char PROGRAM_NAME[8];
    strcpy(PROGRAM_NAME, "fsck");

    // initialize the file system simulator kernel
    if(Kernel::initialize() == false)
    {
        cout << "Failed to initialized Kernel" << endl;
        Kernel::exit(1);
    }

    if (argc != 1) {
        cout << PROGRAM_NAME << ": usage: " << PROGRAM_NAME << endl;
        return -1;
    }

    // First check for filesystem is available or not.
    ifstream is("filesys.dat");

    if(is.good() == false) {
        cout << "error: filesystem filesys.dat not present " << argv[1] << endl;
        return -1;
    }

    is.close();

    //Checking for each directory entry whther nlinks are correct or not.

    char *str = "/";
    char temp[100];
    memset(temp, '\0', 100);
    strcpy(temp, str);
    inodeMap[0] = 1;
    int status = populateMap(temp);

    if (status == EXIT_FAILURE) {
        cout << "error in populating map" << endl;
        return EXIT_FAILURE;
    }

    /*
    // Code to print the hashmap created above for (inode, nlinks)     
    for (map<int,int>::iterator it=inodeMap.begin(); it!=inodeMap.end(); ++it) {
        cout << it->first << " => " << it->second << endl;
    }
    */
   
#if testNlink
    /* Code utility to test the incorrect number of nlinks.
     * To test this change the inum to some inode number from your created filesystem
     * and enjoy the error.
     */
    FileSystem * fileSystem = Kernel::openFileSystems;
    IndexNode indexNode;
    int inum = 2;
    fileSystem->readIndexNode(&indexNode, inum);
    indexNode.setNlink(indexNode.getNlink()+2);
    fileSystem->writeIndexNode(&indexNode, inum);
#endif

    checkNlinkCount();

#if testBlockAllocation_WithFreedBlock
    /* Code utility to test the allocation of blocks by making free an allocated block.
     * To test this change the inum and block number to some inode number from your 
     * created filesystem and enjoy the error.
     */
    int inum = 2;
    int blockNum = 0;
    FileSystem * fileSystem = Kernel::openFileSystems;
    IndexNode indexNode;
    fileSystem->readIndexNode(&indexNode, inum);
    int address = indexNode.getBlockAddress(blockNum);
    fileSystem->freeBlock(address);
#endif

#if testFreeBlockAllocation_WithExtraAllocatedBlock    
    /* Code utility to test if all blocks NOT mentioned in the inodes are marked as 
     * free blocks. To test this we allocate a block without seeting it in any inode.
     * 
     * NOTE: To run this utility either create a new filessystem or if you have
     * already tested the block allocation utility then to see the correct results run
     * fsck twice. Reason: This happens as the block which was freed earlier is allocated
     * back in the first run and will show the issue in second run.
     */
    FileSystem * fileSystem = Kernel::openFileSystems;
    int allocatedBlock = fileSystem->allocateBlock();
    
#endif

    //Checking the allocated and free blocks in the filesystem
    checkBlockAllocation();
}

