// Para ver a fila de mensagem use ipcs -q e para remover use ipcrm -q id
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define ERROR -1

struct msgbuffer {
  long message_type;
  char message[1024];
  int flag;
};

struct msqid_ds buffer;

void create_key(key_t *key, char *file, char letter) {
  // Cria uma chave aleatória através do nome de algum arquivo e um caracter qualquer (B)
  // ftok usa informações sobre algum arquivo e o caractere para gerar a chave provavelmente única
  if((*key = ftok(file, letter)) == ERROR) {
    perror("ftok");
    exit(1);
  }
}

void get_queue(int *queue, key_t key, int permissions) {
  // Cria (IPC_CREAT) a fila no modo leitura (0644) e retorna seu ID utilizando um chave (key)
  if ((*queue = msgget(key, permissions)) == ERROR) {
    perror("msgget");
    exit(1);
  }
}

void destroy_queue(int *queue) {
  // Destroi a fila de mensagem (IPC_RMID)
  if(msgctl(*queue, IPC_RMID, NULL) == ERROR) {
    perror("msgctl");
    exit(1);
  }
}

void send_message_to_queue(int *queue, struct msgbuffer buffer) {
  // Usado para recupera a próxima mensagem da fila
  buffer.message_type = 1;

  // Envia a mensagem para a fila.
  if(msgsnd(*queue, &buffer, sizeof(buffer), 0) == ERROR) {
    perror("msgsnd");
    exit(1);
  }
}

struct msgbuffer receive_message_from_queue(int *queue, struct msgbuffer buffer) {

    // Recebe as mensagem que estão armazenadas na fila de mensagens
    // 0: Recupera a próxima mensagem da fila, independente de seu mtype (message_type)
    // > 0: Pega a próxima mensagem com mtype igual ao msgtyp especificado.
    // < 0: Recupera a primeira mensagem na fica cujo mtype é menor ou igual ao valor absoluto de msgtyp.
    if(msgrcv(*queue, &buffer, sizeof(buffer), 0, 0) == ERROR) {
      perror("msgrcv");
      exit(1);
    }

    return buffer;
}

int number_of_messages(int *queue) {

  if(msgctl(*queue, IPC_STAT, &buffer) == ERROR) {
    perror("msgctl");
    exit(1);
  }

  return buffer.msg_qnum;
}
