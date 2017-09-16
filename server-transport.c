#include "queue.h"
#include "shm.h"
#include "fso.h"

int main() {

  struct msgbuffer buffer;          // Estrutura de mensagens
  struct msgbuffer *shared_memory;  // Estrutura de mensagens compartilhado
  int queue;                        // Fila de mensagem
  key_t queue_key;                  // Chave da fila de mensagem
  int segment_id;                   // Identificador do segmento
  key_t shm_key = 1234;             // Chave do segmento da memória compartilhada
  buffer.flag = 1;                  // Execução do programa

  transport_header();

  // Cria a chave para criar a fila de mensagem
  create_key(&queue_key, "server-application.c", 'A');

  // Cria a fila de mensagem (IPC_CREAT)
  get_queue(&queue, queue_key, 0666 | IPC_CREAT);

  // IPC_CREATE cria um novo segmento
  // 0666 são as permissões de escrita e leitura para qualquer usuário
  segment_id = get_shared_memory(shm_key, buffer, IPC_CREAT | 0666);

  // Realoca a memória compartilhada com um endereço diferente.
  /* shared_memory = attach_shared_memory(&segment_id, (void*) 0x5000000); */
  shared_memory = attach_shared_memory(&segment_id, (void*) 0x5000000);

  // Manda a mensagem da memória para a fila
  printf("Mensagem: %s\n", shared_memory->message);
  send_message_to_queue(&queue, *shared_memory);

  // Desaloca a memória compartilhada
  detach_shared_memory(shared_memory);

  // Remove a memória compartilhada
  /* remove_shared_memory(&segment_id); */

  return 0;
}
