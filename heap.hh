#include "queue"

#ifndef TASK_STRUCT_HEAP_INCLUDE_GUARD
#define TASK_STRUCT_HEAP_INCLUDE_GUARD 1

//! Struttura a heap per lo scheduler Shortest Job First
class task_struct_heap : public queue{

public:
  //! Costruttore
  task_struct_heap(){};

  //! Distruttore
  ~task_struct_heap(){};

  //! Restituisce il primo elemento
  task_struct front() {
    return vect[0];
  };

  //! Restituisce l'indice del padre dell'elemento il cui indice e' passato alla funzione
  int father(int index) {
    if(index%2 == 0) 
      return (index/2) - 1;
    else 
      return index/2;
  };

  //! Ritorna l'indice del figlio sinistro dell'elemento il cui indice e' passato alla funzione
  int left(int index) {
    return 2*index + 1;
  };
  
  //! Ritorna l'indice del figlio destro dell'elemento il cui indice e' passato alla funzione
  int right(int index){
    return index + 1;
  };

  //! Inserisce un elemento nell'heap
  void push_back(const task_struct& process) {
    int i = size_;
    ++size_;
    tail = size_;

      while(i > 0 && vect[father(i)].stats.cpu_burst > process.stats.cpu_burst) {
	vect[i] = vect[father(i)];
	i = father(i);
      };

    vect[i] = process; 
  };

  //! Estrae il primo elemento 
  void pop_front() {
    tail = (tail - 1)%NUM_MAX;
    vect[0] = vect[size_ - 1];
    --size_;
    heapify(0);
    return;
  };

  //! Mantiene la struttura di heap a partire dall'elemento il cui indice e' passato alla funzione
  void heapify(int index) {
    if(index >= (size_ / 2))
      return;

    int left_ = 0;
    int right_ = 0;
    int min = index;

    if (size_%2 != 0)
      right_ = right(index);

    left_ = left(index);
    
    if(vect[min].stats.cpu_burst > vect[left_].stats.cpu_burst)
      min = left_;
    else if(right_ != 0 && vect[min].stats.cpu_burst > vect[right_].stats.cpu_burst)
      min = right_;
    
    if(min != index) {
      task_struct tmp;
      
      tmp = vect[min];
      vect[index] = vect[min];
      vect[min] = tmp;

      heapify(min);
    };

  }; 

};
#endif
