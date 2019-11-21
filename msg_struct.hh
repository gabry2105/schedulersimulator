/*
  Contiene le strutture dati per lo scambio di messaggi
 */

#ifndef MSGBUF_SIGNAL_INCLUDE_GUARD
#define MSGBUF_SIGNAL_INCLUDE_GUARD 1
//! Struttura dati per i messaggi mandati dal clock come segnale
struct msgbuf_signal{
  long mtype;
  bool msg;

  msgbuf_signal()
    : mtype(1), msg(1) {};
};

const int MSGBUF_SIGNAL_LENGHT = sizeof(msgbuf_signal) - sizeof(long);

#endif

#include "state.hh"

#ifndef MSGBUF_MSG_INCLUDE_GUARD
#define MSGBUF_MSG_INCLUDE_GUARD 1

//! Struttura dati per la comunicazione tra scheduler e processo
struct msgbuf_msg{
  long mtype;
  state mtext;

  msgbuf_msg()
    : mtype(1), mtext(New) {};
};

const int MSGBUF_MSG_LENGHT = sizeof(msgbuf_msg) - sizeof(long);
#endif
