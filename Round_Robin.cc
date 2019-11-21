#include <sys/msg.h>

#include "task_struct.hh"
#include "queue.hh"
#include "mem.hh"
#include "msg_struct.hh"


/*!
 * \file Round_Robin.cc
 */
//! Funzione definita in sem.cc
void down(int sem_id, int n_sem = 0);

//! Funzione definita in sem.cc
void up(int sem_id, int n_sem = 0);


/*!
  \brief Scheduler di tipo Round Robin
  
  \param cpu_queue Puntatore all'area di memoria condivisa contenente la struttura dati per la coda di ready
  \param shmem Puntatore alla zona condivisa contenente la struttura dati mem
  \param cpu_queue_sem_id Identificatore del semaforo per la mutua esclusione sulla coda di cpu
  \param io_queue_sem_id Identificatore del semaforo per la mutua esclusione sulla coda di io


  Algoritmo di scheduling di tipo Round Robin.
  Il numero di cicli di clock dedicato ad ogni processo e' contenuto nella costante RR_TIME


 */
void round_robin(queue *cpu_queue, mem *shmem, int cpu_queue_sem_id, int io_queue_sem_id) {

  //Numero di cicli di clock che vengono dedicati ad ogni processo
  const int RR_TIME = 5;

  task_struct process;

  msgbuf_signal signal;

  while(shmem -> exit ||
	!(cpu_queue -> empty()) ||
	!(shmem -> io_queue.empty()) ) {  
  
    //Riceve segnale dal clock
    msgrcv(shmem -> scheduler_msg_id, &signal, MSGBUF_SIGNAL_LENGHT, 0, 0);
    
    if(cpu_queue -> empty()) 
      continue;
    
    down(cpu_queue_sem_id);
    process = cpu_queue -> front();  
    cpu_queue -> pop_front();
    up(cpu_queue_sem_id);
    
    msgbuf_msg msg;
    
    std::cout << "Lo scheduler ha scelto il processo " << process.pid << std::endl;
    for(int i = 0; i < RR_TIME; ++i) {
      //Manda in esecuzione il processo mandando un messaggio
      msgsnd(process.msg_id, &msg, MSGBUF_MSG_LENGHT, 0);

      //Segno la statistica
      ++process.stats.cpu_time;

      //Aspetto che il processo dica cosa intende fare
      msgrcv(shmem -> process_msg_id, &msg, MSGBUF_MSG_LENGHT, 0, 0);

      if(msg.mtext == Wait) {
	//Se il processo necessita di fare i/o, effettuo prelazione, mandandolo nella coda di wait
	down(io_queue_sem_id);
	shmem -> io_queue.push_back(process);
	up(io_queue_sem_id);
	
	//Attendo un colpo di clock e aggiorno il contatore 
	++(shmem -> context_switch);
      
	//Riceve segnale
	msgrcv(shmem -> scheduler_msg_id, &signal, 1, 0, 0);

	break;
      }
      else if (msg.mtext == Exit) {
	//Se il processo ha terminato, aggiorno le statistiche e lo rimuovo
	std::cout << process.pid << " termina" << std::endl;
	
	process.stats.turnaround = process.stats.io_time + process.stats.cpu_time
	  + process.stats.time_waiting;
	
	//Salva le statistiche del processo
	shmem -> stats.push_back(process);
	
	++(shmem -> context_switch);
	
	//Elimina la coda messaggi del processo
	msgctl(process.msg_id, IPC_RMID, NULL);
	
	//Riceve segnale
	msgrcv(shmem -> scheduler_msg_id, &signal, MSGBUF_SIGNAL_LENGHT, 0, 0);

	break;
      }; //Fine if

      //Se il processo deve continuare l'esecuzione, non faccio nulla e lo lascio eseguire
      //attendendo un ciclo di clock
      msgrcv(shmem -> scheduler_msg_id, &signal, MSGBUF_SIGNAL_LENGHT, 0, 0);
      
    }; //fine for
    
    //Controllo se il processo deve utilizzare ancora la cpu, e in tal caso lo metto tra i procesi di ready
    if(msg.mtext == Execute) {
      down(cpu_queue_sem_id);
      cpu_queue -> push_back(process);
      up(cpu_queue_sem_id);
    };

  }; //fine while

  //Segnalo che lo scheduler ha finito
  shmem -> scheduler_exit = 0;
    
  exit(0);

};
