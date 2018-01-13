/**
 * A simple defragmentation program for a simulated file system.
 */
#include "Kernel.h"
#include "DirectoryEntry.h"
#include "Stat.h"
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 4096
#define OUTPUT_MODE 0700

int main(int argc, char ** argv)
{
	char PROGRAM_NAME[8];
	strcpy(PROGRAM_NAME, "defrag");

	// initialize the file system simulator kernel
	if(Kernel::initialize() == false)
	{
		cout << "Failed to initialized Kernel" << endl;
		Kernel::exit(1);
	}
	// print a helpful message if no command line arguments are given
	if(argc != 1)
	{
		cout << PROGRAM_NAME << ": usage: " << PROGRAM_NAME << endl;
		Kernel::exit( 1 ) ;
	}
        int status = Kernel::defrag();
        if(status == EXIT_FAILURE)
        {
            cout << "Error in defrag" << endl;
            return EXIT_FAILURE;
        }
        Kernel::exit(0);
}

