

#include <sys/wait.h>
#include <sys/msg.h>
#include <unistd.h>


#include "msg_struct.hh"
#include "queue.hh"
#include "mem.hh"

/*!
 * \file clock.cc
 */

//! Funzione definita in sem.cc
void down(int sem_id, int n_sem = 0);

//! Funzione definita in sem.cc
void up(int sem_id, int n_sem = 0);

/*! 
  \brief Simula il clock del sistema
  \param cpu_queue Puntatore all'area di memoria condivisa contenente la struttura dati per la coda di ready
  \param shmem Puntatore alla zona condivisa contenente la struttura dati mem
  \param cpu_queue_sem_id Identificatore del semaforo per la mutua esclusione sulla coda di cpu
  \param io_queue_sem_id Identificatore del semaforo per la mutua esclusione sulla coda di io
  \return 

  Ogni quanto di tempo (stabilito dalla costante CLOCK, manda un messaggio allo scheduler e al dispositivo di io per attivarli.
  Inoltre aggiorna le statistiche dei processi nelle code.
 */
void clock(queue *cpu_queue, mem *shmem, int cpu_queue_sem_id, int io_queue_sem_id) {
  const int CLOCK_ = 4;
  msgbuf_signal signal;

  while(shmem -> scheduler_exit ||
	shmem -> io_device_exit ) {
    
    sleep(CLOCK_);
    
    // Manda segnale
    msgsnd(shmem -> scheduler_msg_id, &signal, MSGBUF_SIGNAL_LENGHT, 0);
    
    msgsnd(shmem -> io_device_msg_id, &signal, MSGBUF_SIGNAL_LENGHT, 0);
    
    //Aggiorno statistiche nella coda di cpu
    down(cpu_queue_sem_id);
    if(!cpu_queue -> empty()) {

    int i = cpu_queue -> begin();
    int i_end = cpu_queue -> end();
    
    std::cout << '\n' << "Coda di ready: " << '\n' 
	      << "PID" << '\t' << "TIME_WAITING" << std::endl;
    
    while(i != i_end) {
      ++( (cpu_queue -> vect[i]).stats.time_waiting );
      std::cout << cpu_queue -> vect[i].pid << '\t'
		<< cpu_queue -> vect[i].stats.time_waiting << '\n';
      i = cpu_queue -> next(i);
    };

    std::cout << std::endl;
    
    };
    up(cpu_queue_sem_id);


    //Aggiorno statistiche nella coda di io
    down(io_queue_sem_id);
    if(!shmem -> io_queue.empty()) {
      
      int i = shmem -> io_queue.begin();
      int i_end = shmem -> io_queue.end();
      
      std::cout << "Coda di wait: " << '\n' << "PID" << '\t' << "TIME_WAITING" << std::endl;
      
      while(i != i_end) {
	++( (shmem -> io_queue.vect[i]).stats.time_waiting );
	std::cout << shmem -> io_queue.vect[i].pid << '\t' 
		  << shmem -> io_queue.vect[i].stats.time_waiting << '\n';
	i = shmem -> io_queue.next(i);
      };
      std::cout << std::endl;
    };
    up(io_queue_sem_id);
  };
  
  exit(0);
};

