#include <sys/msg.h>

#include "queue.hh"
#include "mem.hh"
#include "msg_struct.hh"

/*!
 * \file scheduler.cc
 */

//! Funzione definita in sem.cc
void down(int sem_id, int n_sem = 0);

//! Funzione definita in sem.cc
void up(int sem_id, int n_sem = 0);

/*!

  \brief Scheduler di tipo First In First Out o Shortest Job First  
  \param cpu_queue: puntatore a queue. Queue e' una struttura dati astratta, a seconda dell'implementazione, ha comportamenti diversi. Se si istanzia una coda standard si ha uno scheduler di tipo FIFO, se invece si istanzia una struttura di tipo Heap, si ha uno scheduler di tipo SJF. 
  \param shmem Puntatore alla zona condivisa contenente la struttura dati mem
  \param cpu_queue_sem_id Identificatore del semaforo per la mutua esclusione sulla coda di cpu
  \param io_queue_sem_id Identificatore del semaforo per la mutua esclusione sulla coda di io
 
  Quando la coda dei processi di cpu non e' vuota, estrae il primo elemento e lo manda in esecuzione risvegliandolo dallo stato di sleep attraverso l'invio di un messaggio sulla relativa coda.
  Dopo averlo risvegliato, lo scheduler si mette in attesa di un messaggio da parte del processo, e a seconda dello stato di quest'ultimo, lo scheduler effettua l'opportuna operazione e aggiorna le statistiche.
*/
void scheduler(queue *cpu_queue, mem *shmem, int cpu_queue_sem_id, int io_queue_sem_id) {

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

    msg.mtext = Execute;

    while(msg.mtext == Execute) {
      //Faccio partire il processo
      msgsnd(process.msg_id, &msg, MSGBUF_MSG_LENGHT, 0);

      //Segno la statistica
      ++process.stats.cpu_time;

      //Aspetto che il processo dica cosa intende fare
      msgrcv(shmem -> process_msg_id, &msg, MSGBUF_MSG_LENGHT, 0, 0);
    
      switch(msg.mtext) { 
      case Execute: 
	//Mantiene in esecuzione lo stesso processo, quindi attendo un colpo di clock e lo faccio
	//continuare nella sua esecuzione 

	//Riceve segnale
	msgrcv(shmem -> scheduler_msg_id, &signal, MSGBUF_SIGNAL_LENGHT, 0, 0);

	break;
	
      case Wait:
	//Metto il processo nella coda del dispositivo di i/o
	down(io_queue_sem_id);
	shmem -> io_queue.push_back(process);
	up(io_queue_sem_id);
	
	//Attendo un colpo di clock e aggiorno il contatore 
	++(shmem -> context_switch);
	
	//Riceve segnale
	msgrcv(shmem -> scheduler_msg_id, &signal, 1, 0, 0);
	
	break;
	
      case Exit:
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
      }
    };
  };
  
  //Segnalo che lo scheduler ha finito
  shmem -> scheduler_exit = 0;
  
  exit(0);
    
};
