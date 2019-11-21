#include "task_struct.hh"

#ifndef QUEUE_INCLUDE_GUARD
#define QUEUE_INCLUDE_GUARD 1

#define NUM_MAX 15


/*!
  Classe virtuale utilizzata per implementare successivamente le varie strutture dati delle code di scheduling (a seconda del tipo di scheduler, si usano strutture dati diverse)
 */
class queue {
public:
  typedef task_struct type;

  queue()
    : size_(0), head(0), tail(0) {};
  
  virtual ~queue() {};
  
  //! Restituisce vero se la coda e' vuota
  bool empty() {
    return size_ == 0;
  };

  //! Ritorna il numero di elementi della coda
  int size() {
    return size_;
  };

  //! Ritorna un riferimento all'i-esimo elemento dell'array
  type& ret_val(int i) {
    return vect[i];
  };

  //! Ritorna l'elemento successivo a quello passato come parametro
  int next(int i) {
    return (i + 1) % NUM_MAX;
  };

  //! Ritorna l'indice dell'inizio
  int begin() {
    return head;
  };

  //! Ritorna l'indice della fine
  int end() {
    return tail;
  };

  virtual void push_back(const type& ) = 0;
  virtual void pop_front() = 0;
  virtual type front() = 0;


  type vect[NUM_MAX];
  int  size_;
  int  head;
  int  tail;
};

#endif
