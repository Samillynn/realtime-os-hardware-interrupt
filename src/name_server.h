#ifndef __NAME_SERVER_H__
#define __NAME_SERVER_H__

#include "common.h"

void name_server();

i32 RegisterAs(const cstring name);
i32 WhoIs(const char* name);

#endif
