/*
Dillon Irish
4/20/16
Operating Systems
Homework 6: Virtual Memory Manager 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// number of characters to read for each line from input file
#define BUFFER_SIZE         32

// number of bytes to read
#define CHUNK               256

//Constants for the address extraction
#define MASK 0x000000ff
#define FRONT_MASK 0x0000ffff

FILE    *address_file;
FILE    *backing_store;

// how we store reads from input file
signed char    address[BUFFER_SIZE];
int     logical_address;

//Page Table = 256 entry array of ints
int pageTable[CHUNK];

//Physcal Address Space = arry of strings
signed char pa[CHUNK][CHUNK];
int paCounter = 0;

// the buffer containing reads from backing store
signed char     buffer[CHUNK];

// the value of the byte (signed char) in memory
signed char value;

//TLB and queue to record FIFO
int tlb[16][2];
int queue = 0;

//Boolean for TLB, 1 yes 0 no
int inTLB = 0;

int main(int argc, char *argv[])
{

int *tlbptr =malloc(4096);
signed char *paptr = malloc(4096);

int i; //Counter for for loops
int offSet; //Holds the offset for the address
int pageNumber; //Holds the page number for the logical address
int pageFrame; //Holds the page frame for the physical address
int physicalAddress; //Holds the physical address
int pageFaults = 0; //Records the number of page Faults
int tlbHits = 0; //Records the number of tlb Hits

	//Initialize the pageTable and tlb to 256, value that will never actually be reaced	
	for(i = 0;i < 256; i = i + 1){
		pageTable[i] = 256;
	}
	
	for(i = 0; i < 16; i = i + 1){
		tlb[i][0] = 256;
		tlb[i][1] = 256;
	}
	

    // perform basic error checking
    if (argc != 3) {
        fprintf(stderr,"Usage: ./vm [backing store] [input file]\n");
        return -1;
    }

    // open the file containing the backing store
    backing_store = fopen(argv[1], "r+b");
    
    if (backing_store == NULL) { 
        fprintf(stderr, "Error opening %s\n",argv[1]);
        return -1;
    }

    // open the file containing the logical addresses
    address_file = fopen(argv[2], "r");

    if (address_file == NULL) {
        fprintf(stderr, "Error opening %s\n",argv[2]);
        return -1;
    }

    // read through the input file and output each logical address
    while ( fgets(address, BUFFER_SIZE, address_file) != NULL) {
	inTLB = 0;
        logical_address = atoi(address);
	printf("Logical Address = %d ", logical_address);
	

	//Get offset and page number from logical address	
	offSet = logical_address & MASK;
	
	pageNumber = logical_address >> 8;
	pageNumber = pageNumber & MASK;

	//Check if the page number is in the tlb
	for(i = 0; i < 16; i = i + 1){
		if(tlb[i][0] == pageNumber){
			pageFrame = tlb[i][1]; //Page frame was found, set to holder
			inTLB = 1;		//Change "boolean" to found
			tlbHits = tlbHits + 1;	//Increment the number of tlbhits
			
		}
	}
	if(inTLB == 1){
		//Look for Page Frame in Page Table
		if(pageTable[pageNumber] != 256){
			pageFrame = pageTable[pageNumber]; //Page frame was found, set to holder
			tlb[queue][0] = pageNumber;	//Place in tlb
			tlb[queue][1] = pageFrame;
			if(queue == 15)			//Increment the queue
				queue = 0;
			else
				queue = queue + 1;
		}else{
			
			//Seek and read chunk of bytes from the BL
			if (fseek(backing_store, CHUNK * pageNumber, SEEK_SET) != 0) {
	 	           fprintf(stderr, "Error seeking in backing store\n");
	 	           return -1;
	 	       }
		
			if (fread(buffer, sizeof(signed char), CHUNK, backing_store) == 0) {
			    fprintf(stderr, "Error reading from backing store\n");
			    return -1;
			}

			//Store bytes in physical memory
			for(i=0; i < 256; i = i + 1){
				pa[paCounter][i] = buffer[i];
			}
			//Place in Page Table			
			pageTable[pageNumber] = paCounter;
			
			//Update TLB
			tlb[queue][0] = pageNumber;
			tlb[queue][1] = paCounter;
			if(queue == 15)
				queue = 0;
			else
				queue = queue + 1;	
			
			pageFrame = paCounter;		//Place in holders and increment counters
			paCounter = paCounter + 1;
			pageFaults = pageFaults + 1;

		}//close else
	}//close if
	
	value = pa[pageFrame][offSet];
	pageFrame = pageFrame << 8;
	physicalAddress = pageFrame | offSet; //Get physical address from pageframe and offset
	printf("Physical Address = %d ", physicalAddress);
	printf("Value = %d\n", value);

    }

	//Close streams and print stats	
    fclose(address_file);
    fclose(backing_store);
	printf("Page Faults: %d\n", pageFaults);
	//printf("Page Fault Rate: %f\n", pageFaults/1000);
	printf("TLB Hits: %d\n", tlbHits);
    return 0;
}

