#include "name_server.h"
#include "syscall.h"
#include "hash_table.h"
#include "printf.h"
#include "task_scheduler.h"
#include "debug.h"

enum NameServerAction {
    REGISTER_AS,
    WHO_IS,

    NAME_SERVER_ACTION_NUM
};

STRUCT(NameServerMsg) {
    enum NameServerAction action;
    cstring name;
    i32 tid;
};

static i32 name_server_tid;

void name_server() {
  HashTable hash_table;
  hash_table_init(&hash_table);

  name_server_tid = MyTid();

  i32 tid;
  NameServerMsg msg;

  while (Receive(&tid, (cstring) &msg, sizeof(NameServerMsg))) {
    printf("    [name_server]: Received tid: %d, msg(%d, %s, %d)\r\n",
           tid, msg.action, msg.name, msg.tid);
    switch (msg.action) {
      case REGISTER_AS: {
        // TODO: check for collision
        // should we do something other than Hash?
        hash_table_add(&hash_table, msg.name, tid);
        break;
      }
      case WHO_IS: {
//       printf("WHO_IS: %s?\n", msg.name);
        if (hash_table_has(&hash_table, msg.name)) {
          msg.tid = hash_table_get(&hash_table, msg.name);
//         printf("WHO_IS: %s : %d\n", msg.name, msg.tid);
        } else {
          msg.tid = -1;
        }
        break;
      }
      default:
        break;
    }

//    printf("    [name_server][from %d]: calling Reply(%d, msg[%d, %s, %d])\r\n",
//           MyTid(), tid, msg.action, msg.name, msg.tid);
    Reply(tid, (cstring) &msg, sizeof(NameServerMsg));
  }
}

i32 RegisterAs(const cstring name) {

//  debug30("Begin of RegisterAs");
//  asm volatile("ldp %[x29], %[x30], [sp]"
//  : [x29]"=r"(x29), [x30]"=r"(x30)
//  );
//  printf("loaded from stack: x29=%p, x30=%p\r\n", x29, x30);

//  asm volatile("mov %[result], x30"
//  : [result]"=r"(x30)
//  );
//  printf("x30=%p\r\n", x30);
  printf("pc=%p,x[30]=%p,x30c=%p\r\n", current_task->pc, current_task->x[30], current_task->x30_copy);
  printf("In name server(including one above)\r\n");
  MyTid();
  printf("pc=%p,x[30]=%p,x30c=%p\r\n", current_task->pc, current_task->x[30], current_task->x30_copy);

  printf("called RegisterAs(%s)\r\n", name);
  NameServerMsg msg;
  {
    msg.action = REGISTER_AS;
    msg.name = name;
    msg.tid = MyTid();
  }

//  printf("    [RegisterAs][from %d]: Send(%d, ...)\r\n", MyTid(), name_server_tid);

  printf("Before calling Send, pc=%p,x[30]=%p,x30c=%p\r\n", current_task->pc, current_task->x[30],
         current_task->x30_copy);
  debug30("After call MyTid, before Send");
  if (Send(name_server_tid, (cstring) &msg, sizeof(NameServerMsg), (cstring) &msg, sizeof(NameServerMsg)) < 0) {
    return -1;
  }
  debug30("After Send");
  Yield();
  printf("After calling Send, pc=%p,x[30]=%p,x30c=%p\r\n", current_task->pc, current_task->x[30],
         current_task->x30_copy);
  printf("RegisterAs send finished\r\n");

  return 0;
}

i32 WhoIs(const char *name) {
  printf("called WhoIs(%s)\r\n", name);
  NameServerMsg msg;
  {
    msg.action = WHO_IS;
    msg.name = name;
    msg.tid = MyTid();
  }
//  printf("    [WhoIs][from %d]: Send(%d, msg[%d, %s, %d])\n",
//         MyTid(), name_server_tid, msg.action, msg.name, msg.tid);

  if (Send(name_server_tid,
           (cstring) &msg,
           sizeof(NameServerMsg),
           (cstring) &msg,
           sizeof(NameServerMsg)) < 0) {
    return -1;
  }

//   printf("%d calls [%p] WhoIs(%s): %d\n", MyTid(), &msg, msg.name, msg.tid);

  return msg.tid;
}
