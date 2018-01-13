/*
 * Main program for the virtual memory project.
 * The header files page_table.h and disk.h explain how to use the page
 * table and disk interfaces.
 *
 * Author: Abhinay Agrawal
 */

#include "page_table.h"
#include "disk.h"
#include "program.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <assert.h>

struct disk *disk;
const char *replace_policy;
int *free_frames;
struct t_queue *fifoQ, *customQ;
int *pfault_count_arr = NULL;

int pfault_count = 0, dread_count = 0, dwrite_count = 0;


//This function is used to find the page for FIFO policy
int fifo_replace() {
    int page = fifoQ->head->val;
    t_queue_dequeue(fifoQ);
    return page;
}

//This function initializes all the arracy values to -1
void arr_init(int arr[], int sz) {
    for (int i=0; i<sz; ++i)
        arr[i] = -1;
}

//This function is used to find the page to be evicted using custom algorithm
int custom_replace(struct page_table *pt) {
    //Iterating over customQ and for each page in our Q we find out the page with
    //minimum page faults till now (from the start of the program)

    assert(customQ->head != NULL);

    struct node *iter = customQ->head;
    int min = INT_MAX, min_index;
    int nframes = page_table_get_nframes(pt);
    int equal_fault_pages[nframes];
    int arr_iter = 0;

    while (iter != NULL) {
        if(pfault_count_arr[iter->val] < min) {
            arr_iter = 0;
            arr_init(equal_fault_pages, nframes);
            equal_fault_pages[arr_iter++] = iter->val;
            
            min = pfault_count_arr[iter->val];
        } else if (pfault_count_arr[iter->val] == min) {
            equal_fault_pages[arr_iter++] = iter->val;
        }
        iter = iter->next;
    }

    min_index = rand() % (arr_iter);
    
    remove_node(customQ, equal_fault_pages[min_index]);
    return equal_fault_pages[min_index];
}

//This function finds the page to be evicted depending on the replacment policy
int get_page_to_evict(struct page_table *pt)
{
    int evict_page;
    if(!strcmp(replace_policy, "rand")){
        int npages = page_table_get_npages(pt);
        while(1){
            evict_page = rand() % npages;
            if(pt->page_bits[evict_page] != 0)
                break;
        }
        return evict_page;
    }
    else if(!strcmp(replace_policy, "fifo")){
        evict_page = fifo_replace();
        return evict_page;
    }
    else if(!strcmp(replace_policy, "custom")) {
        evict_page = custom_replace(pt);
        return evict_page;
    } else {
	fprintf(stderr,"unknown replacement policy: %s\n", replace_policy);
        exit(1);
    }
}

void page_fault_handler( struct page_table *pt, int page )
{
        ++pfault_count;

        // If page is present in physical memory then it is a fault for RW
        if (pt->page_bits[page] == PROT_READ) {
            page_table_set_entry(pt, page, pt->page_mapping[page], PROT_READ|PROT_WRITE);
            
            if(!strcmp(replace_policy, "fifo")) {
                int rem_page = remove_node(fifoQ, page);
                t_queue_enqueue(fifoQ, page);
            } else if (!strcmp(replace_policy, "custom")) {
                int rem_page = remove_node(customQ, page);
                t_queue_enqueue(customQ, page);
                ++pfault_count_arr[page];
            }
            return;
        }

        /* 
         * Now we go for the part where page is not present in the phy. memory
         * Check if PT has some empty location or not
         */

        int nframes = page_table_get_nframes(pt);
        
        for (int i = 0; i<nframes; ++i) {
            //check for empty frame in physical memory
            if(free_frames[i] == 0) {
                page_table_set_entry(pt, page, i, PROT_READ);
                disk_read(disk, page, &pt->physmem[i * PAGE_SIZE]);
                ++dread_count;

                free_frames[i] = 1;

                if(!strcmp(replace_policy, "fifo") ) {
                    t_queue_enqueue(fifoQ, page);
                } else if (!strcmp(replace_policy, "custom")) {
                    t_queue_enqueue(customQ, page);
                    ++pfault_count_arr[page];
                }
                return;
            }
        }

        
        //Now, we are here because physical memory is not empty and kicking is required.
        //Next, we get the page that will be kicked out.

	int page_evicted = get_page_to_evict(pt);
        int frame_evicted = pt->page_mapping[page_evicted];


        //Check if the page to be evicted is dirty.
        if(pt->page_bits[page_evicted] == PROT_WRITE || 
                pt->page_bits[page_evicted] == (PROT_READ|PROT_WRITE)) {
            //If yes, write modified page to disk.
            disk_write(disk, page_evicted, &pt->physmem[frame_evicted * PAGE_SIZE]);
            ++dwrite_count;
        }
        
        //Read new page into physical memory and set entry in page table
        disk_read(disk, page, &pt->physmem[frame_evicted * PAGE_SIZE]);
        ++dread_count;
        page_table_set_entry(pt, page, frame_evicted, PROT_READ);

        if(!strcmp(replace_policy, "fifo")) {
            t_queue_enqueue(fifoQ, page);
        } else if(!strcmp(replace_policy, "custom")) {
            t_queue_enqueue(customQ, page);
            ++pfault_count_arr[page];
        }

        //Void entry of evicted page.
        page_table_set_entry(pt, page_evicted, 0, 0);

#ifdef orig
        exit(1);
#endif
}

int main( int argc, char *argv[] )
{
	if(argc!=5) {
		printf("use: virtmem <npages> <nframes> <rand|fifo|custom> <sort|scan|focus>\n");
		return 1;
	}
	int npages = atoi(argv[1]);
	int nframes = atoi(argv[2]);
        replace_policy = argv[3];
	const char *program = argv[4];

        srand(time(0));

#ifdef orig
        // This was the original code. We have made disk a global variable
	struct disk *disk = disk_open("myvirtualdisk",npages);
#endif
        disk = disk_open("myvirtualdisk",npages);
	if(!disk) {
		fprintf(stderr,"couldn't create virtual disk: %s\n",strerror(errno));
		return 1;
	}

        free_frames = (int *)calloc(nframes, sizeof(int));
        if (!strcmp(replace_policy, "fifo"))
                fifoQ = t_queue_init();
        else if (!strcmp(replace_policy, "custom")) {
                pfault_count_arr = (int *)calloc(npages, sizeof(int));
                customQ = t_queue_init();
                assert(pfault_count_arr[0] == 0);
        }

	struct page_table *pt = page_table_create( npages, nframes, page_fault_handler );
	if(!pt) {
		fprintf(stderr,"couldn't create page table: %s\n",strerror(errno));
		return 1;
	}

	char *virtmem = page_table_get_virtmem(pt);

#ifdef orig
	char *physmem = page_table_get_physmem(pt);
#endif

	if(!strcmp(program,"sort")) {
		sort_program(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"scan")) {
		scan_program(virtmem,npages*PAGE_SIZE);

	} else if(!strcmp(program,"focus")) {
		focus_program(virtmem,npages*PAGE_SIZE);

	} else {
		fprintf(stderr,"unknown program: %s\n",argv[4]);
	}

        printf("page faults = %d \t disk reads = %d \t disk writes = %d\n",
                pfault_count, dread_count, dwrite_count);
	page_table_delete(pt);
	disk_close(disk);

	return 0;
}
