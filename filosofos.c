#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <ncurses.h>

#define NUM_FILOSOFOS 5

int pos_x[NUM_FILOSOFOS] = {40, 60, 50, 30, 20};
int pos_y[NUM_FILOSOFOS] = {5, 15, 25, 25, 15};

sem_t garfos[NUM_FILOSOFOS];
int estado_garfos[NUM_FILOSOFOS];  // 1 = livre (🥄), 0 = ocupado (❌)
pthread_mutex_t mutex;

void *filosofo(void *num) {
    int id = *(int *)num;
    int garfo_esquerdo = id;
    int garfo_direito = (id + 1) % NUM_FILOSOFOS;

    while (1) {
        mvprintw(pos_y[id], pos_x[id], "F%d 🧠 ", id);
        refresh();
        sleep(1);

        pthread_mutex_lock(&mutex);
        sem_wait(&garfos[garfo_esquerdo]);
        sem_wait(&garfos[garfo_direito]);
        estado_garfos[garfo_esquerdo] = 0;
        estado_garfos[garfo_direito] = 0;
        for (int i = 0; i < NUM_FILOSOFOS; i++) {
            mvprintw(30 + i, 0, "Garfo %d: %s", i, estado_garfos[i] ? "🥄 livre " : "❌ ocupado");
        }
        pthread_mutex_unlock(&mutex);

        mvprintw(pos_y[id], pos_x[id], "F%d 🍝 ", id);
        refresh();
        sleep(1);

        sem_post(&garfos[garfo_direito]);
        sem_post(&garfos[garfo_esquerdo]);
        estado_garfos[garfo_direito] = 1;
        estado_garfos[garfo_esquerdo] = 1;
        for (int i = 0; i < NUM_FILOSOFOS; i++) {
            mvprintw(30 + i, 0, "Garfo %d: %s", i, estado_garfos[i] ? "🥄 livre " : "❌ ocupado");
        }
    }
}

int main() {
    pthread_t threads[NUM_FILOSOFOS];
    int ids[NUM_FILOSOFOS];

    initscr();
    noecho();
    curs_set(FALSE);

    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        sem_init(&garfos[i], 0, 1);
        estado_garfos[i] = 1;
    }

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, filosofo, &ids[i]);
    }

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        sem_destroy(&garfos[i]);
    }
    pthread_mutex_destroy(&mutex);

    endwin();

    return 0;
}
