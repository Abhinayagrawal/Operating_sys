/*
 * Creates a new file and hard link to some existing file
 * A simple program to test hard-linking
 */

#include "Kernel.h"
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 4096

int main(int argc, char ** argv)
{
	char PROGRAM_NAME[8];
	memset(PROGRAM_NAME, '\0', 8);
	strcpy(PROGRAM_NAME, "ln");

	// initialize the file system simulator kernel
	if(Kernel::initialize() == false)
	{
		cout << "Failed to initialized Kernel" << endl;
		Kernel::exit(1);
	}

	if( argc < 2 )
	{
		cout << PROGRAM_NAME << ": usage: " << PROGRAM_NAME << " input-file ..." << endl;
		Kernel::exit(1) ;
	}

        char file_name[64];

        strcpy(file_name, argv[1]);

        // open the input file
        int fd = Kernel::open(file_name , Kernel::O_RDONLY);

        if(fd < 0)
        {
            Kernel::perror(PROGRAM_NAME);
            cout << PROGRAM_NAME << ": unable to open file \"" << file_name << "\"";
            Kernel::exit( 2 ) ;
        }

        Kernel::close(fd);
        int status = Kernel::unlink(file_name);

        if(status < 0)
        {
            cout << PROGRAM_NAME << ": error" << file_name << endl;
            Kernel::exit( 2 ) ;
        }

        
        Kernel::exit(0);
}
