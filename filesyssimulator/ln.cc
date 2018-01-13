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

	if( argc != 3 )
	{
		cout << PROGRAM_NAME << ": usage: " << PROGRAM_NAME << " input-file ..." << endl;
		Kernel::exit(1) ;
	}

        char base_file_name[64];
        char new_file_name[64];

        strcpy(base_file_name, argv[1]);
        strcpy(new_file_name, argv[2]);

        // open the input file
        int base_fd = Kernel::open(base_file_name , Kernel::O_RDONLY);

        if(base_fd < 0)
        {
            Kernel::perror(PROGRAM_NAME);
            cout << PROGRAM_NAME << ": unable to open base file \"" << base_file_name << "\"";
            Kernel::exit( 2 ) ;
        }

        int status = Kernel::link(base_file_name, new_file_name);

        if(status < 0)
        {
            //error
            cout << "error: link function" << endl;
            Kernel::exit( 2);
        }

        Kernel::close(base_fd);
        
        Kernel::exit(0);
}

