#include "common.h"
#include "memory_pool.h"
#include "name_server.h"
#include "printf.h"
#include "rps_server.h"
#include "syscall.h"
#include "task_scheduler.h"
#include "utilities.h"

enum RPSAction {
  SIGNUP,
  PLAY,
  QUIT,

  RPS_ACTION_NUM
};

enum RPSPlayAction {
  ABSENT = 0,
  UNKNOWN,
  ROCK,
  PAPER,
  SCISSORS,

  RPS_PLAY_ACTION_NUM
};

enum RPSPlayResult {
  WON,
  LOST,
  TIE,
  NO_OPPONENT,

  RPS_PLAY_RESULT_NUM
};

STRUCT(RPSMsg) {
  enum RPSAction action;
  enum RPSPlayAction play_action;
  cstring name;
  cstring opponent;
  enum RPSPlayResult play_result;

  RPSMsg* next;
};

STRUCT(RPSMsgQueue) {
  RPSMsg* front;
  RPSMsg* end;

  usize size;
};

void rps_msg_queue_init(RPSMsgQueue* self) {
  self->front = NULL;
  self->end = NULL;
  self->size = 0;
}

void rps_msg_queue_add(RPSMsgQueue* self, RPSMsg* msg) {
  msg->next = NULL;
  if (self->end) {
    self->end->next = msg;
    self->end = self->end->next;
  } else {
    self->end = msg;
    self->front = msg;
  }
  self->size += 1;
}

RPSMsg* rps_msg_queue_pop(RPSMsgQueue* self) {
  RPSMsg* popped = NULL;

  if (self->front) {
    popped = self->front;
    if (self->front->next) {
      self->front = self->front->next;
    } else {
      self->front = NULL;
      self->end = NULL;
    }
  }

  self->size -= 1;

  return popped;
}

void rps_server() {
  printf("pc=%p,x[30]=%p,x30c=%p\r\n", current_task->pc, current_task->x[30], current_task->x30_copy);
  printf("rps_server\r\n");
  printf("rps_server_tid = %d\r\n", MyTid());
  printf("pc=%p,x[30]=%p,x30c=%p\r\n", current_task->pc, current_task->x[30], current_task->x30_copy);
  RegisterAs("rps_server");
  printf("pc=%p,x[30]=%p,x30c=%p\r\n", current_task->pc, current_task->x[30], current_task->x30_copy);
  printf("rps_server successfully registered in name_server\r\n");

  MemoryPool memory_pool;
  memory_pool_init(&memory_pool);
  printf("memory pool init\r\n");

  RPSMsgQueue signup_queue;
  rps_msg_queue_init(&signup_queue);
  printf("rps_msg_queue_init\r\n");

  enum RPSPlayAction play_actions[64];
  for (usize i = 0; i < 64; i ++) {
    play_actions[i] = ABSENT;
  }

  i32 tid;
  RPSMsg msg;

  while (Receive(&tid, (cstring)&msg, sizeof(RPSMsg))) {
    printf("received msg[%d, %s] from %d\r\n", msg.action, msg.name, tid);
    switch (msg.action) {
    case SIGNUP: {
      printf("SIGNUP\r\n");
      RPSMsg* new_msg = (RPSMsg*)memory_pool_allocate(&memory_pool, sizeof(RPSMsg));
      *new_msg = msg;
      rps_msg_queue_add(&signup_queue, new_msg);
      play_actions[WhoIs(new_msg->name)] = UNKNOWN;

      if (signup_queue.size >= 2) {
        RPSMsg *msg1 = rps_msg_queue_pop(&signup_queue);
        RPSMsg *msg2 = rps_msg_queue_pop(&signup_queue);
        msg1->opponent = msg2->name;
        msg2->opponent = msg1->name;
        printf("%s and %s successfully paired\r\n", msg1->name, msg2->name);
        Reply(WhoIs(msg1->name), (cstring)msg1, sizeof(RPSMsg));
        Reply(WhoIs(msg2->name), (cstring)msg2, sizeof(RPSMsg));
        memory_pool_free(&memory_pool, msg1);
        memory_pool_free(&memory_pool, msg2);
      }
      break;
    }
    case PLAY: {
      printf("PLAY\r\n");
      i32 p1 = WhoIs(msg.name);
      i32 p2 = WhoIs(msg.opponent);

      play_actions[p1] = msg.play_action;
      if (play_actions[p2] == ABSENT) {
        RPSMsg *msg1 =
            (RPSMsg *)memory_pool_allocate(&memory_pool, sizeof(RPSMsg));
        {
          msg1->name = msg.name;
          msg1->opponent = msg.opponent;
          msg1->play_result = NO_OPPONENT;
        }
        Reply(p1, (cstring)msg1, sizeof(RPSMsg));
        memory_pool_free(&memory_pool, msg1);
        break;
      }
      if (play_actions[p2] != UNKNOWN) {
        switch (play_actions[p1] - play_actions[p2]) {
        case 0: {
          printf("--> %s == %s\r\n", msg.name, msg.opponent);
          RPSMsg* msg1 =
            (RPSMsg*)memory_pool_allocate(&memory_pool, sizeof(RPSMsg));
          {
            msg1->name = msg.name;
            msg1->opponent = msg.opponent;
            msg1->play_result = TIE;
          }
          Reply(p1, (cstring)msg1, sizeof(RPSMsg));
          RPSMsg *msg2 =
            (RPSMsg *)memory_pool_allocate(&memory_pool, sizeof(RPSMsg));
          {
            msg2->name = msg.opponent;
            msg2->opponent = msg.name;
            msg2->play_result = TIE;
          }
          Reply(p2, (cstring)msg2, sizeof(RPSMsg));
          memory_pool_free(&memory_pool, msg1);
          memory_pool_free(&memory_pool, msg2);
          break;
        }
        case 1:
        case -2: {
          printf("--> %s > %s\r\n", msg.name, msg.opponent);
          RPSMsg *msg1 =
              (RPSMsg *)memory_pool_allocate(&memory_pool, sizeof(RPSMsg));
          {
            msg1->name = msg.name;
            msg1->opponent = msg.opponent;
            msg1->play_result = WON;
          }
          Reply(p1, (cstring)msg1, sizeof(RPSMsg));
          RPSMsg *msg2 =
              (RPSMsg *)memory_pool_allocate(&memory_pool, sizeof(RPSMsg));
          {
            msg2->name = msg.opponent;
            msg2->opponent = msg.name;
            msg2->play_result = LOST;
          }
          Reply(p2, (cstring)msg2, sizeof(RPSMsg));
          memory_pool_free(&memory_pool, msg1);
          memory_pool_free(&memory_pool, msg2);
          break;
        }
        case 2:
        case -1: {
          printf("--> %s < %s\r\n", msg.name, msg.opponent);
          RPSMsg *msg1 =
              (RPSMsg *)memory_pool_allocate(&memory_pool, sizeof(RPSMsg));
          {
            msg1->name = msg.name;
            msg1->opponent = msg.opponent;
            msg1->play_result = LOST;
          }
          Reply(p1, (cstring)msg1, sizeof(RPSMsg));
          RPSMsg *msg2 =
              (RPSMsg *)memory_pool_allocate(&memory_pool, sizeof(RPSMsg));
          {
            msg2->name = msg.opponent;
            msg2->opponent = msg.name;
            msg2->play_result = WON;
          }
          Reply(p2, (cstring)msg2, sizeof(RPSMsg));
          memory_pool_free(&memory_pool, msg1);
          memory_pool_free(&memory_pool, msg2);
        }
        }
        play_actions[p1] = UNKNOWN;
        play_actions[p2] = UNKNOWN;
      }
      break;
    }
    case QUIT: {
      printf("QUIT\r\n");
      play_actions[tid] = ABSENT;
      Reply(tid, (cstring)&msg, sizeof(RPSMsg));
      break;
    }
    default: break;
    }
  }

  Exit();
}

void rps_client() {
  i32 tid = MyTid();

  char name[20]; {
    char CLIENT_NAME_PREFIX[] = "rps_client_";
    for (usize i = 0; i < strlen(CLIENT_NAME_PREFIX); i ++) {
      name[i] = CLIENT_NAME_PREFIX[i];
    }
    char buffer[10];
    i32_to_cstring(tid, buffer, 10);
    for (usize i = 0; i < strlen(buffer); i ++) {
      name[i + strlen(CLIENT_NAME_PREFIX)] = buffer[i];
    }
    name[strlen(CLIENT_NAME_PREFIX) + strlen(buffer)] = '\0';
  }
  RegisterAs(name);

  i32 server_id = WhoIs("rps_server");

  RPSMsg msg; {
    msg.action = SIGNUP;
    msg.name = name;
  }
  RPSMsg reply;
  if (Send(server_id, (cstring)&msg, sizeof(RPSMsg), (cstring)&reply, sizeof(RPSMsg))) {
    printf("%s succesfully signed up\r\n", name);
    for (usize i = 0; i < 3; i ++) { // play 3 rounds
      {
        msg.action = PLAY;
        msg.play_action = get_random_u32(2, 5);
        msg.name = name;
        msg.opponent = reply.opponent;
      }
      printf("%s presents %s to %s\r\n",
             name,
             msg.play_action == ROCK ? "rock" :
             msg.play_action == PAPER ? "paper" :
             msg.play_action == SCISSORS ? "scissors" :
             "UNKNOWN", reply.opponent);
      if (Send(server_id, (cstring)&msg, sizeof(RPSMsg), (cstring)&reply, sizeof(RPSMsg))) {
        printf("%s has %s\r\n",
               name,
               reply.play_result == WON ? "won" :
               reply.play_result == LOST ? "lost" :
               reply.play_result == TIE ? "tie" :
               "no opponent");
      }
    }
  }

  {
    msg.action = QUIT;
    msg.name = name;
  }
  Send(server_id, (cstring)&msg, sizeof(RPSMsg), (cstring)&reply, sizeof(RPSMsg));

  Exit();
}
