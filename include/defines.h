#pragma once


#define DEBUG 1
#define DEBUG2 0
//#define DEBUG2 1
#define DEBUG3 1
//#define DEBUG4 0
#define DEBUG4 1
#define DEBUG5 1

#ifdef DEBUG5
#define TRACE() printf("trace file %s line %d\n", __FILE__, __LINE__); //(void)getch()
#else
#define TRACE()
#endif

//#define DEBUG44 1
//#define DEBUG_READFILE 1

