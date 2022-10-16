//
// Created by 28379 on 10/5/2022.
//

#ifndef K2_PRINT_H
#define K2_PRINT_H

#include "../src/message_passing.h"
void printf_( const char* format, ... );
void print_task(Task* t);
void print_send(SendArgs* send);
void print_receive(ReceiveArgs* receive);

#endif //K2_PRINT_H
