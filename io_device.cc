#include <iostream>
#include <sys/msg.h>

#include "msg_struct.hh"
#include "queue.hh"
#include "mem.hh"

/*!
 * \file io_device.cc
 */

//! Funzione definita in sem.cc
void down(int sem_id, int n_sem = 0);

//! Funzione definita in sem.cc
void up(int sem_id, int n_sem = 0);

/*! 
    \brief Simula il comportamento di un dispositivo di i/o
    Estrae un processo alla volta dalla coda, e per un tempo casuale da 1 a MAX_USE cicli di clock, tiene impegnato il processo precedentemente estratto.
    Quando ha terminato, metta il processo nella coda di cpu e riesegue da capo   	
*/
void io_device(queue *cpu_queue, mem *shmem, int cpu_queue_sem_id, int io_queue_sem_id) {
  const int MAX_USE_ = 3;

  msgbuf_signal signal;
  task_struct process;
  while(!(shmem -> io_queue.empty()) ||
	!(cpu_queue -> empty()) ||
	shmem -> exit) { 
    
    //Riceve segnale
    msgrcv(shmem -> io_device_msg_id, &signal, MSGBUF_SIGNAL_LENGHT, 0, 0);

    if(shmem -> io_queue.empty())
      continue;

    down(io_queue_sem_id);
    process = shmem -> io_queue.front();
    shmem -> io_queue.pop_front();
    up(io_queue_sem_id);

    //numero di colpi di clock per l'uso del dispositivo
    int use = rand()%MAX_USE_ + 1;
    
    std::cout << "Il processo " << process.pid << " inizia a fare io per " 
	      << use << " cicli di clock" << std::endl;

    process.stats.io_time += use;

    for(int i = 0; i < use; ++i)
      //Riceve segnale
      msgrcv(shmem -> io_device_msg_id, &signal, MSGBUF_SIGNAL_LENGHT, 0, 0);

    std::cout << "Il processo " << process.pid << " termina di fare io " << std::endl;  

    process.stats.cpu_burst = rand() % MAX_BURST_ + MIN_BURST_;  

    down(cpu_queue_sem_id);
    cpu_queue -> push_back(process);
    up(cpu_queue_sem_id);
  };
  
  //Segnalo che il dispositivo ha finito
  shmem -> io_device_exit = 0;
  exit(0);

};
