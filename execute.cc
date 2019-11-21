#include <sys/msg.h>

#include "task_struct.hh"
#include "mem.hh"
#include "msg_struct.hh"

/*
 * \file execute.cc
 */

/*!
  Note per i messaggi mandati allo scheduler:
   - Wait   : Il processo va in Wait per fare i/o
   - Exit   : Il processo ha terminato la sua esecuzione
   - Execute: Il processo intende continuare l'esecuzione

 */
//! Simula l'esecuzione del processo comunicando con lo scheduler attraverso la memoria condivisa
void execute(task_struct process, mem *shmem) {
  msgbuf_msg msg;
  
  /*
    Siccome la coda di messaggi all'inizio e' vuota, il processo va in stato di sleep, 
    verra' poi risvegliato al momento giusto dallo schedueler
   */
  while(true) {
    
    msgrcv(process.msg_id, &msg, MSGBUF_MSG_LENGHT, 0, 0);
  
    if (process.stats.cpu_burst > 1) {
      //Segnalo allo scheduler che il processo intende continuare l'esecuzione
      msg.mtext = Execute;
      std::cout << "Il processo usa la cpu" << std::endl;
      
      --process.stats.cpu_burst;
	
      //Segnalo allo scheduler che il processo deve ancora utilizzare la cpu
      msgsnd(shmem -> process_msg_id, &msg, MSGBUF_MSG_LENGHT, 0);
      
    }
    //Controllo se il processo deve fare i/o
    else if(process.stats.io_use != 0) {
      --process.stats.io_use;
    
      std::cout << "Il processo deve fare io" << std::endl;
    
      //Segnalo allo scheduler che il processo deve fare i/o
      msg.mtext = Wait;
      
      //Manda messaggio allo scheduler
      msgsnd(shmem -> process_msg_id, &msg, MSGBUF_MSG_LENGHT, 0);
    }
    else 
      break;
  }


  //Segnalo allo scheduler che il processo ha terminato
  msg.mtext = Exit;
  msgsnd(shmem -> process_msg_id, &msg, MSGBUF_MSG_LENGHT, 0);
  
  exit(0);
 
};
