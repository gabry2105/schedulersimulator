#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include "mem.hh"
#include "queue.hh"
#include "heap.hh"
#include "std_queue.hh"


/*!
  \mainpage Scheduler Simulator
  \section Scheduler
  Simulatore di politiche di scheduling

  \author Gabriele Labita

*/

/*!
*
* \file      scheduler_simulator.cc
* \brief     file principale
* \author    Gabriele Labita

*  All'inizio del programma verra' chiesto all'utente quale politica di scheduling utilizzare.
* La scelta e' fatta su:
* - FIFO
* - SJF
* - Round Robin
*
* Dopo avere scelto la politica di scheduling, appare il menu del generatore di processi dal quale e' possibile:
* - Creare un nuovo processo: Viene creato un nuovo processo con statistiche casuali
* - Creare un nuovo processo inserendo le statistiche: L'utente dovra' inserire il numero di volte che il processo dovra' fare operazioni di i/o e il primo burst di cpu (i successivi saranno generati poi con numeri casuali)
* - Uscire dal programma: Selezionando l'uscita, il programma termina mostrando le statistiche sui singoli processi e sullo scheduler in generale. Prima di terminare pero' il programma attendera' la terminazione de processi ancora in esecuzione
*
*/

//! Definita in clock.cc
void clock(queue*, mem*, int, int);

//! Definita in io_device.cc
void io_device(queue*, mem*, int, int);

//! Definita in scheduler.cc
void scheduler(queue*, mem*, int, int);

//! Definita in process_generator.cc
void process_generator(queue*, mem*, int, int);

//! Definita in Round_Robin.cc
void round_robin(queue*, mem*, int, int);

/*!
  \brief Simulatore di scheduling

  L'utente sceglie quale politica di scheduling utilizzare e a comando, vengono creati processi e gestiti dall'algoritmo di scheduling scelto.
  Sono stati implementati i seguenti algoritmi:
    - FIFO
    - SJF
    - Round Robin
  Per gli algoritmi: FIFO e SJF, viene usata la stessa funzione di scheduling, cambia solo la struttura dati utilizzata, infatti nel FIFO viene usata una coda standard, mentre per il SJF viene usato un Heap

  Il programma, usa 4 processi:
    - clock: Ogni quanto di tempo (stabilito a priori), manda un messaggio allo scheduler e al dispositivo di i/o
    - scheduler: Ad ogni battito di clock, effettua l'operazione opportuna (in base al tipo di scheduler)
    - io_device: Simula il funzionamento di un dispositivo di io
    - process_generator: Genera processi a comando dell'utente

  Una volta terminata l'esecuzione del programma, vengono mostrate a video le statistiche
 */
int main() {
 
  //Imposto la chiave
  key_t key;
  const char OUT_CODE = 'o';
  const char MEM_CODE = 'm';
  const char QUEUE_CODE = 'q';
  const char CPU_QUEUE_SEM = 'c';
  const char IO_QUEUE_SEM = 'i';

  key = ftok(".", MEM_CODE);
  
  
  //Creo un area di memoria condivisa
  int mem_id = shmget(key, sizeof(mem), IPC_CREAT | 0666);
  mem *shmem = static_cast<mem*> ( shmat(mem_id, NULL, 0) );

  key = ftok(".", QUEUE_CODE);
  int queue_id = shmget(key, sizeof(queue), IPC_CREAT | 0666);
  queue *cpu_queue = static_cast<queue*> ( shmat(queue_id, NULL, 0) );
  
  //Creo i semafori per la mutua esclusione sulla memoria condivisa
  key = ftok(".", CPU_QUEUE_SEM);
  int cpu_queue_sem_id = semget(key, 1, IPC_CREAT | 0666);
  
  key = ftok(".", IO_QUEUE_SEM);
  int io_queue_sem_id = semget(key, 1, IPC_CREAT | 0666);
  
  //imposto i semafori a 1
  union semun { 
    int val;                  
    struct semid_ds *buf;      
    unsigned short *array;    
    struct seminfo *__buf;     
  }; 
  
  semun arg;
  arg.val = 1;
  semctl(cpu_queue_sem_id, 0, SETVAL, arg);
  semctl(io_queue_sem_id, 0, SETVAL, arg);
  

  //creo un area dati condivisa per l'output
  key = ftok(".", OUT_CODE);
  int out_id = shmget(key, sizeof(std_queue), IPC_CREAT | 0666);
  std_queue *out = static_cast<std_queue*> ( shmat(queue_id, NULL, 0) );
  
  char scelta = '0';
  
  //Creo i dati con una new di piazzamento
  new(shmem) mem();

  while(scelta == '0') {
    std::cout << " menu:" << std::endl;
    std::cout << "Scegli l'algoritmo di scheduling :" << std::endl;
    std::cout << "Premi 1: FIFO" << '\n' 
	      << "Premi 2: SJF"  << '\n' 
	      << "Premi 3: Round Robin" << std::endl;
    std::cout << "Scelta: ";
    std::cin >> scelta;
  
    if(scelta == '1' || scelta == '3')
      new(cpu_queue) std_queue();
    else if(scelta == '2')
      new(cpu_queue) task_struct_heap();
    else {
      std::cout << "Scelta errata!" << std::endl;
      scelta = '0';
    };
  };

  //Creo le code di messaggi per i segnali del clock
  shmem -> scheduler_msg_id = msgget(IPC_PRIVATE, IPC_CREAT | 0666 );
  shmem -> io_device_msg_id = msgget(IPC_PRIVATE, IPC_CREAT | 0666 );
  shmem -> process_msg_id = msgget(IPC_PRIVATE, IPC_CREAT | 0666 );

  //Creo i processi
  int pid;
  
  pid = fork();
  if (pid == 0) 
    clock(cpu_queue, shmem, cpu_queue_sem_id, io_queue_sem_id);

  pid = fork();
  if (pid == 0 && 
      (scelta == '1' || scelta == '2')) 
    scheduler(cpu_queue, shmem, cpu_queue_sem_id, io_queue_sem_id);
  else if(pid == 0)
    round_robin(cpu_queue, shmem, cpu_queue_sem_id, io_queue_sem_id);

  pid = fork();  
  if (pid == 0)
    io_device(cpu_queue, shmem, cpu_queue_sem_id, io_queue_sem_id);
  
  pid = fork();
  if (pid == 0)
    process_generator(cpu_queue, shmem, cpu_queue_sem_id, io_queue_sem_id);

  int ret_val = 1;

  // Aspetta terminazione figli
  wait(&ret_val);
  wait(&ret_val);
  wait(&ret_val);
  wait(&ret_val);

  // Compila statistiche
  std::cout << "STATISTICHE " << std::endl;
  int total_time_waiting = 0;
  int total_cpu_time = 0;
  int total_io_time = 0;
  std::cout << "Dettagli processi:" << std::endl;
  std::cout << "PID " << '\t' << "IO_USE" << '\t' << "IO_TIME" << '\t' 
	    << "CPU_TIME" << " " << "TIME_WAITING" 
	    << "TURNAROUND" << std::endl;   

  int i = shmem -> stats.begin();
  int i_end = shmem -> stats.end();

  while(i != i_end) {
    std::cout << (shmem -> stats.ret_val(i)).pid << '\t' 
	      << (shmem -> stats.ret_val(i)).stats.io_use << '\t'
	      << (shmem -> stats.ret_val(i)).stats.io_time << '\t' 
	      << (shmem -> stats.ret_val(i)).stats.cpu_time << '\t'
	      << (shmem -> stats.ret_val(i)).stats.time_waiting << '\t' << '\t' 
	      << (shmem -> stats.ret_val(i)).stats.turnaround << std::endl;
    total_time_waiting += (shmem -> stats.ret_val(i)).stats.time_waiting;
    total_cpu_time += (shmem -> stats.ret_val(i)).stats.cpu_time;
    total_io_time +=  (shmem -> stats.ret_val(i)).stats.io_time;
    i = shmem -> stats.next(i);
  };

  std::cout << '\n' << "Resoconto scheduling: " << std::endl;

  std::cout << "Tempo di i/o: " << total_io_time << std::endl;
  std::cout << "Tempo di utilizzo di cpu: " << total_cpu_time << std::endl;
  std::cout << "Tempo di inutilizzo cpu: " << shmem -> context_switch << std::endl;

  std::cout << "Uso di cpu: " << (total_cpu_time * 100) / 
    (shmem -> context_switch + total_cpu_time) << "%" << std::endl;

  std::cout << "Tempo di attesa medio: " << total_time_waiting / shmem -> stats.size() 
	    << std::endl;
  //Rimuovo le risorse allocate
  semctl(cpu_queue_sem_id, 0, IPC_RMID);
  semctl(io_queue_sem_id, 0, IPC_RMID);
  
  msgctl(shmem -> scheduler_msg_id, IPC_RMID, NULL);
  msgctl(shmem -> io_device_msg_id, IPC_RMID, NULL);
  msgctl(shmem -> process_msg_id, IPC_RMID, NULL);

  shmctl(queue_id, IPC_RMID, NULL);
  shmctl(mem_id, IPC_RMID, NULL);

  return 0;
};

