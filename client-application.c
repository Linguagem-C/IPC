#include "queue.h"
#include "shm.h"
#include "fso.h"

int main() {

  struct msgbuffer buffer;  // Estrutura de mensagens
  int queue;                // Fila de mensagens
  key_t queue_key;          // Chave da fila de mensagem

  application_header();

  // Cria a chave para criar a fila de mensagem
  create_key(&queue_key, "client-application.c", 'A');

  // Cria a fila de mensagem (IPC_CREAT)
  get_queue(&queue, queue_key, 0644 | IPC_CREAT);

  while(TRUE) {
    // Insere a mensagem
    printf("Entre com o texto: ");
    fgets(buffer.message, 1024, stdin);
    remove_line_break(buffer.message);

    // Manda a mensagem para a fila
    send_message_to_queue(&queue, buffer);

    printf("Deseja inserir outra mensagem (0 - não)/(1 - sim): ");
    scanf("%d", &buffer.flag);
    clean_buffer();

    if(buffer.flag == 0) {
      break;
    }
  }

  return 0;
}


