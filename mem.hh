#include "std_queue.hh"

#ifndef MEM_INCLUDE_GUARD
#define MEM_INCLUDE_GUARD 1

/*!
  \brief Struttura dati che contiene i dati condivisi

  - io_queue: Coda per il dispositivo di i/o
  - stats: Contiene i processi terminati, viene poi utilizzata per compilare le statistiche

  - exit: Segnala che l'utente vuole uscire dal programma
  - scheduler_exit: Una volta che l'utente intende uscire, quando lo scheduler finisce utilizza questa variabile per segnalare che ha finito
  - io_device_exit: Come scheduler_exit, ma per il dispositivo di i/o

  - schedueler_msg_id: Id della coda di messaggi utilizzata dal clock per mandare il segnale allo scheduler
  - io_device_msg_id: Come scheduler_msg_id, ma usata con il dispositivo di i/o
  - process_msg_id: Id delle coda di messaggi utilizzata dai processi per comunicare con lo scheduler

  - context_switch: Tempo totale impiegato per il context switch
 */
struct mem {
  std_queue  io_queue;
  std_queue  stats;

  bool   exit;
  bool   scheduler_exit;
  bool   io_device_exit;

  int scheduler_msg_id; 
  int io_device_msg_id;
  int process_msg_id; // Lo uso per mandare da processi a sched

  
  int context_switch;

  mem()
    : exit(1), scheduler_exit(1), io_device_exit(1), context_switch(0) {};

  ~mem() {};
};

#endif
