#include "queue.hh"
#include <iostream>
#ifndef STD_QUEUE_INCLUDE_GUARD
#define STD_QUEUE_INCLUDE_GUARD 1

/*!
  \brief Coda standard
  Eredita pubblicamente dalla classe virtuale queue, implemente i metodi push_back e pop_front come una normale coda	
*/
class std_queue : public queue{
public:

  std_queue() {};

  ~std_queue() {};
  
  //! Ritorna il primo elemento
  type front() {
    return vect[head];
  };

  //! Mette in fondo alla coda l'elemento passato come parametro
  void push_back(const task_struct& p) {
    vect[tail] = p;
    ++size_;
    tail = (tail + 1)%NUM_MAX;
  };

  //! Elimina dalla coda il primo elemento
  void pop_front() {
    head = (head + 1)%NUM_MAX; 
    --size_;
  };

};


#endif
