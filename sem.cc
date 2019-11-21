#include <sys/ipc.h>
#include <sys/sem.h>


/*! 
  \brief Effettua l'operazione di down su un semaforo
  \param sem_id: identificatore della coda di semafori
  \n_sem: Indice del semaforo sul quale si vuole fare l'operazione, di default e' 0
*/
void down(int sem_id, int n_sem = 0){
  sembuf s_op;
  s_op.sem_num = n_sem;
  s_op.sem_op = -1;
  semop(sem_id, &s_op, 1);
};

/*! 
  \brief Effettua l'operazione di up su un semaforo
  \param sem_id: identificatore della coda di semafori
  \n_sem: Indice del semaforo sul quale si vuole fare l'operazione, di default e' 0
*/
void up(int sem_id, int n_sem = 0){
  sembuf s_op;
  s_op.sem_num = n_sem;
  s_op.sem_op = 1;
  semop(sem_id, &s_op, 1);
};
