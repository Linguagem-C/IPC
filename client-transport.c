#include "queue.h"
#include "shm.h"
#include "fso.h"

int main() {

  struct msgbuffer buffer;          // Estrutura de mensagens
  struct msgbuffer *shared_memory;  // Estrutura de mensagens compartilhado
  int queue;                        // Fila de mensagens
  key_t queue_key;                  // Chave da fila de mensagem
  int segment_id;                   // Identificador do segmento
  key_t shm_key = 1234;             // Chave do segmento da memória compartilhada

  transport_header();

  // Cria a chave para a fila de mensagem
  create_key(&queue_key, "client-application.c", 'A');

  // Abre a fila de mensagem se existir
  get_queue(&queue, queue_key, 0666 | IPC_CREAT);

  // Cria um novo segmento (IPC_CREAT) e verifica se ele foi realmente criado e é único (IPC_EXCL)
  // 0666 - Permissão de escrita e leitura para todos os tipos de usuário
  segment_id = get_shared_memory(shm_key, buffer, IPC_CREAT | 0666);

  // Deixar a memória compartilhada acessível
  // NULL (linux vai escolher o endereço disponivel)
  shared_memory = attach_shared_memory(&segment_id, (void*) 0x5000000);

  // Recebe as mensagens que estão na fila
  printf("Mensagens recebidas da fila e armazenadas na memória\n");
  do {
    buffer = receive_message_from_queue(&queue, buffer);

    // Insere uma string na memória compartilhada
    memcpy(shared_memory, &buffer, sizeof(buffer));
    printf("Mensagem: %s\n", shared_memory->message);
  } while(buffer.flag == 1);

  if(number_of_messages(&queue) == 0) {
    // Remove a fila de mensagem
    destroy_queue(&queue);
  }

  // Desaloca o segmento da memória compartilhada
  detach_shared_memory(shared_memory);

  return 0;
}
