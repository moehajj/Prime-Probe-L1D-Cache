
#include "util.h"

/*
 * Loads from virtual address addr and measure the access time
 */
extern inline __attribute__((always_inline))
void load(ADDR_PTR addr)
{
    asm volatile("mov %0, %%r8\n\t"
            "mov (%%r8), %%r8\n\t"
    :: "r"(addr));
}

extern inline __attribute__((always_inline))
CYCLES measure_one_block_access_time(ADDR_PTR addr)
{
    CYCLES cycles;

    asm volatile("mov %1, %%r8\n\t"
            "lfence\n\t"
            "rdtsc\n\t"
            "mov %%eax, %%edi\n\t"
            "mov (%%r8), %%r8\n\t"
            "lfence\n\t"
            "rdtsc\n\t"
            "sub %%edi, %%eax\n\t"
    : "=a"(cycles) /*output*/
    : "r"(addr)
    : "r8", "edi");

    return cycles;
}

/* 
 * Returns Time Stamp Counter 
 */
extern inline __attribute__((always_inline))
CYCLES rdtscp(void) {
	CYCLES cycles;
	asm volatile ("rdtscp"
	: /* outputs */ "=a" (cycles));

	return cycles;
}

/* 
 * Gets the value Time Stamp Counter 
 */
inline CYCLES get_time() {
    return rdtscp();
}

/* Synchronizes at the overflow of a counter
 *
 * Counter is created by masking the lower bits of the Time Stamp Counter
 * Sync done by spinning until the counter is less than CHANNEL_SYNC_JITTER
 */
extern inline __attribute__((always_inline))
CYCLES cc_sync() {
    while((get_time() & CHANNEL_SYNC_TIMEMASK) > CHANNEL_SYNC_JITTER) {}
    return get_time();
}


/*
 * Flushes the cache block accessed by a virtual address out of the cache
 */
extern inline __attribute__((always_inline))
void clflush(ADDR_PTR addr)
{
    asm volatile ("clflush (%0)"::"r"(addr));
}

/*
 * Returns the 6 bits l1d cache set index of a given address.
 */
uint64_t get_l1d_cache_index(ADDR_PTR addr)
{
    uint64_t mask = ((uint64_t) 1 << 6) - 1;
    return ((uint64_t) addr >> 6) & mask;
}

/*
 * Convert a given ASCII string to a binary string.
 * From:
 * https://stackoverflow.com/questions/41384262/convert-string-to-binary-in-c
 */
char *string_to_binary(char *s)
{
    if (s == NULL) return 0; /* no input string */

    size_t len = strlen(s) - 1;

    // Each char is one byte (8 bits) and + 1 at the end for null terminator
    char *binary = malloc(len * 8 + 1);
    binary[0] = '\0';

    for (size_t i = 0; i < len; ++i) {
        char ch = s[i];
        for (int j = 7; j >= 0; --j) {
            if (ch & (1 << j)) {
                strcat(binary, "1");
            } else {
                strcat(binary, "0");
            }
        }
    }

    return binary;
}

/*
 * Convert 8 bit data stream into character and return
 */
char *conv_char(char *data, int size, char *msg)
{
    for (int i = 0; i < size; i++) {
        char tmp[8];
        int k = 0;

        for (int j = i * 8; j < ((i + 1) * 8); j++) {
            tmp[k++] = data[j];
        }

        char tm = strtol(tmp, 0, 2);
        msg[i] = tm;
    }

    msg[size] = '\0';
    return msg;
}

/*
 * Prints help menu
 */
void print_help() {

	printf("Flush+Reload Covert Channel Sender/Reseiver Flags:\n"
		"\t-f,\tFile to be shared between sender/receiver\n"
		"\t-o,\tSelected offset into shared file\n"
		"\t-i,\tTime interval for sending a single bit\n");

}

/*
 * Parses the arguments and flags of the program and initializes the struct config
 * with those parameters (or the default ones if no custom flags are given).
 */
void init_config(struct config *config, int argc, char **argv)
{
	config->interval = CHANNEL_DEFAULT_INTERVAL;
	config->data = malloc((size_t) 1024*1024*sizeof(size_t));
	
	srand(time(0)); 
	for(int i = 0; i<1024*1024*sizeof(size_t); i++) 
        	config->data[i] = rand()%128;


	config->eviction_set = NULL;
	for (int i=0;i<64*64;i++) {
		ADDR_PTR addr = (ADDR_PTR) config->data + i*64;
		if (get_l1d_cache_index(addr) == 12) {
			config->eviction_set = addNode(config->eviction_set,addr);	
		}
	}	
	config->addr = config->eviction_set->addr;

/*
	// Parse the command line flags
	//      -f is used to specify the shared file 
	//      -i is used to specify the sending interval rate
	//      -o is used to specify the shared file offset
	int option;
	while ((option = getopt(argc, argv, "i:o:f:")) != -1) {
		switch (option) {
			case 'i':
				config->interval = atoi(optarg);
				break;
			case 'o':
				offset = atoi(optarg)*CACHE_BLOCK_SIZE;
				break;
			case 'f':
				filename = optarg;
				break;
			case 'h':
				print_help();
				exit(1);
			case '?':
				fprintf(stderr, "Unknown option character\n");
				print_help();
				exit(1);
			default:
				print_help();
				exit(1);
		}
	}
	config->addr = (ADDR_PTR) mapaddr + offset;
*/
	
}




Node createNode(){
    Node temp; // declare a node
    temp = (Node)malloc(sizeof(struct LinkedList)); // allocate memory using malloc()
    temp->next = NULL;// make next point to NULL
    return temp;//return the new node
}


Node addNode(Node head, ADDR_PTR addr){
    Node temp,p;// declare two nodes temp and p
    temp = createNode();//createNode will return a new node with data = value and next pointing to NULL.
    temp->addr = addr; // add element's value to data part of node
    if(head == NULL){
        head = temp;     //when linked list is empty
    } else {
        p  = head;//assign head to p 
        while(p->next != NULL) {
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;//Point the previous last node to the new node created.
    }
    return head;
}

