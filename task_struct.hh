#include "state.hh"
#include <stdlib.h>

#ifndef TASK_STRUCT_INCLUDE_GUARD
#define TASK_STRUCT_INCLUDE_GUARD 1

#define MAX_IO_USE_ 3
#define MAX_BURST_ 15
#define MIN_BURST_ 1

/*!
  \brief Statistiche del processo
  - time_waiting: Tempo totale passato nella coda di ready e del dispositivo di i/o
  - cpu_time: Tempo di uso della cpu
  - io_time: Tempo di uso del dispositivo di i/o
  - cpu_burst: Quantita' di tempo (espresso in numero di clock) di utilizzo di cpu da parte del processo prima di fare i/o
  - io_use: numeri di accessi al dispositivo di i/o che il processo deve fare
  - turnaround: tempo di completamento
 */
struct statistics {
  int time_waiting;
  int cpu_time;
  int io_time;
  int io_use;
  int cpu_burst;
  int turnaround;
  statistics() 
    : time_waiting(0), io_time(0), cpu_time(0),
      io_use(rand() % MAX_IO_USE_), 
      cpu_burst( (rand() % MAX_BURST_) + MIN_BURST_), 
      turnaround(0) {};
};

/*!
  \brief Struttura dati per la rappresentazione dei processi
  - pid: Pid del processo
  - status: Descrive in che stato si trova il processo
  - msg_id: Id della coda di messaggi per la comunicazione con lo scheduler
  - stats: Statistiche del processo
 */
struct task_struct {
  int        pid;
  state      status;
  int        msg_id;
  statistics stats;
  
};

#endif
