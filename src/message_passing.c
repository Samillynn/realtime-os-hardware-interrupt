#include "message_passing.h"
#include "printf.h"
#include "task.h"
#include "utilities.h"
#include "task_scheduler.h"

void print_send(SendArgs *send_args) {
  printf("    [sys_send]: [send_args] tid=%d, msg=%s, msg_len=%d, reply(%p)=%s, reply_len=%d\r\n",
          send_args->tid, send_args->msg, send_args->msg_len, send_args->reply, send_args->reply,
          send_args->reply_len);
}

void print_receive(ReceiveArgs *args) {
  printf("    [sys_receive]: *tid=%d, msg=%p, msg_len=%d\r\n", args->tid, args->msg,
          args->msg_len);
}

void print_reply(ReplyArgs *args) {
  printf("    [sys_reply]: [args] tid=%d, reply(%p)=%s, reply_len=%d\r\n",
          args->tid, args->reply, args->reply, args->reply_len);
}

bool register_sender(Task *receiver, Task *sender) {
  printf("register_sender\r\n");
  return queue_push(&receiver->senders, sender->tid);
}

int next_sender(Task *receiver) {
  printf("next_sender\r\n");
  return queue_pop(&receiver->senders);
}

SendArgs *get_send_args(Task *task) {
  return (SendArgs *) task->x;
}

ReceiveArgs *get_receive_args(Task *task) {
  return (ReceiveArgs *) task->x;
}

ReplyArgs *get_reply_args(Task *task) {
  return (ReplyArgs *) task->x;
}

int copy_message(const char *msg_from, int len_msg_from, char *msg_to, int len_msg_to) {
  int actual_len = (len_msg_from < len_msg_to) ? len_msg_from : len_msg_to;
//  printf("    [copy_message](%p, %d, %p, %d); actual_len: %d\r\n",
//         msg_from, len_msg_from, msg_to, len_msg_to, actual_len);
  memcpy(msg_to, msg_from, actual_len);
  return actual_len;
}

void synchronized_send_receive(Task *sender, const char *msg_from, int len_msg_from, Task *receiver, char *msg_to,
                               int len_msg_to) {
  if (sender == NULL || receiver == NULL) {
    printf("Assertion: Sender | Receiver cannot be NULL\r\n");
    return;
  }

  //    if (receiver->state == Ready) {
  //        printf("Assertion: State of receiver cannot be Ready\r\n");
  //        return;
  //    }

//      *get_receive_args(receiver)->tid = sender->tid;

  int actual_len = copy_message(msg_from, len_msg_from, msg_to, len_msg_to);
  assign_result(receiver, actual_len);

  receiver->state = Ready;
}


void sys_send() {
//  printf("Beginning of send\r\n");
//  print_receive(task_queue_get(1));
//  print_send(current_task);
//  printf("send x[0]=%d\r\n", current_task->x[0]);

  Task *sender = get_current_task();
  SendArgs *args = get_send_args(sender);
  Task *receiver = task_queue_get(args->tid);

  if (receiver == NULL) {
    printf("Receiver tid: %d not found\r\n", args->tid);
    assign_result(sender, -1);
    return;
  }

  if (receiver->state == WaitReceive) {
    ReceiveArgs *receive_args = get_receive_args(receiver);

//    printf("before assign: sender_id:%d, rec_tid:%d\r\n", sender->tid, *(receive_args->tid));
    *(receive_args->tid) = sender->tid;
//    printf("after assign: sender_id:%d, rec_tid:%d\r\n", sender->tid, *(receive_args->tid));

//    printf("Before copy in send\r\n");
//    print_receive(task_queue_get(1));
//    print_send(current_task);
    synchronized_send_receive(sender, args->msg, args->msg_len, receiver, receive_args->msg, receive_args->msg_len);
//    printf("After copy in send\r\n");
//    print_receive(task_queue_get(1));
//    print_send(current_task);
    // block sender
    sender->state = WaitReply;
  } else {
    bool success = register_sender(receiver, sender);
    if (success) {
      // block sender
      sender->state = WaitSend;
    } else {
      printf("Fail to register as sender to receiver(tid:%d)\r\n", receiver->tid);
      assign_result(sender, -2);
    }
  }


//  printf("End of send\r\n");
//  print_receive(task_queue_get(1));
//  print_send(current_task);
}

/** make the return value of Receive be -1 if the sender terminated before receiving **/
void sys_receive() {
//  printf("Beginning of receive\r\n");
//  print_receive(current_task);

  Task *receiver = get_current_task();
  ReceiveArgs *args = get_receive_args(receiver);

  if (queue_is_empty(&receiver->senders)) {
//    printf("sender queue is empty, block receiver\r\n");

//    printf("Before change state in receive\r\n");
//    print_receive(current_task);

    receiver->state = WaitReceive;
//    printf("After change state in receive\r\n");
//    print_receive(get_receive_args(current_task));
    return;
  }

  int sender_id = next_sender(receiver);
  Task *sender = task_queue_get(sender_id);
  if (sender == NULL) {
    printf("Sender tid: %d is invalid", sender_id);
    assign_result(receiver, -1);
    return;
  }

//  printf("Before copy in receive\r\n");
//  print_receive(current_task);

  SendArgs *send_args = get_send_args(sender);

//  printf("After copy in receive\r\n");
//  print_receive(current_task);

  *args->tid = sender->tid;
//  printf("After change *tid in receive\r\n");
//  print_receive(current_task);
  synchronized_send_receive(sender, send_args->msg, send_args->msg_len, receiver, args->msg, args->msg_len);

  sender->state = WaitReply;

  printf("End of receive\r\n");
  print_receive(current_task);
}

void sys_reply() {
//  print_reply(current_task);
//  print_send(task_queue_get(2));
//  printf("Beginning of reply\r\n");
//  print_reply(current_task);
//  print_send(task_queue_get(2));

  Task *receiver = get_current_task();
  ReplyArgs *args = get_reply_args(receiver);
//  printf("    [sys_reply]: [args] tid=%d, reply(%p)=%s, reply_len=%d\r\n",
//         args->tid, args->reply, args->reply, args->reply_len);

  Task *sender = task_queue_get(args->tid);

  if (sender == NULL) {
    printf("Sender tid: %d is invalid", args->tid);
    assign_result(receiver, -1);
    return;
  }

  SendArgs *send_args = get_send_args(sender);
//  printf("    [sys_reply]: [send_args] tid=%d, msg=%s, msg_len=%d, reply(%p)=%s, reply_len=%d\r\n",
//         send_args->tid, send_args->msg, send_args->msg_len, send_args->reply, send_args->reply,
//         send_args->reply_len);

//  printf("before copy in reply\r\n");
//  print_reply(current_task);
//  print_send(task_queue_get(2));
  synchronized_send_receive(receiver, args->reply, args->reply_len, sender, send_args->reply, send_args->reply_len);


//  printf("End of reply\r\n");
//  print_reply(current_task);
//  print_send(task_queue_get(2));
}
