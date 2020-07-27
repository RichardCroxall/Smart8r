
#pragma once
#ifdef MAIN_PROGRAM
       char * messageDescription(x10_message_t x10_received_message);
#else
extern char * messageDescription(x10_message_t x10_received_message);
#endif