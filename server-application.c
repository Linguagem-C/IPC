#include <unistd.h>
#include <sys/wait.h>
#include "queue.h"
#include "shm.h"

#define MSG_SIZE 100

void application_header();
void transport_header();

int main() {

  pid_t process_son;        // Processo filho
  struct msgbuffer buffer;  // Estrutura de mensagens
  int queue;                // Fila de mensagem
  key_t queue_key;          // Chave da fila de mensagem
  int segment_id;           // Identificador do segmento
  char *shared_memory;      // Endereço da memória compartilhada
  key_t shm_key = 1234;     // Chave do segmento da memória compartilhada
  application_header();

  // Cria um processo filho exatamente igual ao processo pai
  process_son = fork();

  if(process_son != 0) {

    // Cria a chave para a fila de mensagem
    create_key(&queue_key, "server.c", 'A');

    // Abre a fila de mensagem se existir
    get_queue(&queue, queue_key, 0644);

    do {
      buffer = receive_message_from_queue(&queue, buffer);

      printf("Mensagem: %s\n", buffer.message);
    } while(number_of_messages(&queue) != 0);

  } else {

    // Cria a chave para criar a fila de mensagem
    create_key(&queue_key, "server.c", 'A');

    // Cria a fila de mensagem (IPC_CREAT)
    get_queue(&queue, queue_key, 0644 | IPC_CREAT);

    // IPC_CREATE cria um novo segmento
    // 0666 são as permissões de escrita e leitura para qualquer usuário
    segment_id = get_shared_memory(shm_key, IPC_CREAT | 0666);

    // Realoca a memória compartilhada com um endereço diferente.
    shared_memory = attach_shared_memory(&segment_id, (void*) 0x5000000);

    do {
      // Manda a mensagem da memória para a fila
      strcpy(buffer.message, shared_memory);
      send_message_to_queue(&queue, buffer);
    } while(number_of_messages(&queue) != 0);

    // Desaloca a memória compartilhada
    detach_shared_memory(shared_memory);

    // Remove a memória compartilhada
    remove_shared_memory(&segment_id);
  }

  return 0;
}


// Cabeçalho da camada de aplicação do host A.
void application_header() {
  printf("\n*******************\n");
  printf("Camada de aplicação\n");
  printf("*******************\n");
}

// Cabeçalho da camada de transporte do host A.
void transport_header() {
  printf("\n********************\n");
  printf("Camada de transporte\n");
  printf("********************\n");
}
