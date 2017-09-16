#include "queue.h"
#include "shm.h"
#include "fso.h"

int main() {

  struct msgbuffer buffer;  // Estrutura de mensagens
  int queue;                // Fila de mensagem
  key_t queue_key;          // Chave da fila de mensagem

  application_header();

  // Cria a chave para a fila de mensagem
  create_key(&queue_key, "server-application.c", 'A');

  // Abre a fila de mensagem se existir
  get_queue(&queue, queue_key, 0666 | IPC_CREAT);

  while(TRUE) {
    buffer = receive_message_from_queue(&queue, buffer);

    printf("Mensagem: %s\n", buffer.message);
  }

  return 0;
}
