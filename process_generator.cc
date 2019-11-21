#include <sys/msg.h>
#include <unistd.h>
#include "task_struct.hh"
#include "queue.hh"
#include "mem.hh"


/*!
 * \file process_generator.cc
 */

//! Funzione definita in sem.cc
void down(int sem_id, int n_sem = 0);

//! Funzione definita in sem.cc
void up(int sem_id, int n_sem = 0);

//! Funzione definita in execute.cc
void execute(task_struct, mem*);


/*!
    \brief Generatore di processi
    \param cpu_queue Puntatore all'area di memoria condivisa contenente la struttura dati per la coda di ready
    \param shmem Puntatore alla zona condivisa contenente la struttura dati mem
    \param cpu_queue_sem_id Identificatore del semaforo per la mutua esclusione sulla coda di cpu
    \param io_queue_sem_id Identificatore del semaforo per la mutua esclusione sulla coda di io

    A comando dell'utente genera un processo attraverso la funzione fork().
    Il processo cosi' generato, chiama la funzione execute() che simula la sua esecuzione
*/
void process_generator(queue *cpu_queue, mem *shmem, int cpu_queue_sem_id, int io_queue_sem_id) {
  
  char c = '1';
  while(c != '0') {
    std::cout << "MENU" << '\n';
    std::cout << "   Premi 1: Genera un processo" << '\n';
    std::cout << "   Premi 2: Genera un processo con statistiche a scelta" << '\n';
    std::cout << "   Premi 0: Esci" << std::endl;
    std::cout << "Scelta: ";
    std::cin >> c;
    
    if(c == '1' || c == '2') {
      task_struct process;
      
      if(c == '2') {
	int io_use;
	int cpu_burst;
	std::cout << "Numero di accessi al dispositivo di i/o: " << std::endl;
	std::cin >> io_use;
	std::cout << "Burst iniziale di cpu: " << std::endl;
	std::cin >> cpu_burst;
	
	process.stats.io_use = io_use;
	process.stats.cpu_burst = cpu_burst;
      };
      
      process.msg_id = msgget(IPC_PRIVATE, IPC_CREAT | 0666 );
      process.pid = fork();
      
      if(process.pid == 0) 
	//mando in esecuzione il figlio
	execute(process, shmem);
      
      std::cout << "Creato processo " << process.pid << std::endl;
    
      //Il generatore mette in coda di cpu il processo creato
      down(cpu_queue_sem_id);
      cpu_queue -> push_back(process);
      up(cpu_queue_sem_id);
    }
    else if(c == '0')
      std::cout << "Uscita ..." << std::endl;
    else 
      std::cout << "Errore!" << std::endl;
    
  };
  
  shmem -> exit = 0;

  exit(0);
};

