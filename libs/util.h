#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


#ifndef UTIL_H_
#define UTIL_H_

#define ADDR_PTR uint64_t
#define CYCLES uint32_t

//#define CHANNEL_DEFAULT_INTERVAL        0x00008000
//#define CHANNEL_SYNC_TIMEMASK           0x000FFFFF
//#define CHANNEL_SYNC_JITTER             0x0100

#define CHANNEL_DEFAULT_INTERVAL        0x00800000
#define CHANNEL_SYNC_TIMEMASK           0x00FFFFFFF
#define CHANNEL_SYNC_JITTER             0x010000


#define DEFAULT_FILE_NAME "/bin/ls"
#define DEFAULT_FILE_OFFSET	0x0
#define DEFAULT_FILE_SIZE	4096
#define CACHE_BLOCK_SIZE	64
#define MAX_BUFFER_LEN	1024


struct LinkedList{
    ADDR_PTR addr;
    struct LinkedList *next;
 };

typedef struct LinkedList *Node; //Define node as pointer of data type struct LinkedList



struct config {
	ADDR_PTR addr;
	int interval;
	Node eviction_set;
	char *data;
};

void load(ADDR_PTR addr);
CYCLES measure_one_block_access_time(ADDR_PTR addr);
CYCLES rdtscp(void);
CYCLES get_time();
CYCLES cc_sync();
 
void clflush(ADDR_PTR addr);

uint64_t get_l1d_cache_index(ADDR_PTR phys_addr);

char *string_to_binary(char *s);

char *conv_char(char *data, int size, char *msg);

void init_config(struct config *config, int argc, char **argv);



Node createNode();
Node addNode(Node head, ADDR_PTR addr);





#endif
