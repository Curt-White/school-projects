//
// Created by curt white on 2020-02-14.
//
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <math.h>

#define MAX_PHILOSOPHERS 5

typedef enum p_state {
    THIRSTY,
    DRUNK
} p_state;

pthread_mutex_t mugs[MAX_PHILOSOPHERS];
p_state states[MAX_PHILOSOPHERS];
pthread_t philosophers[MAX_PHILOSOPHERS];
unsigned long long int stats[MAX_PHILOSOPHERS];

/**
 * Pick up a mug
 * @param id - ID of the mug which we will pick up
 */
void grab_mug(int id) {
    int next = (id + 1) % MAX_PHILOSOPHERS;
    // If you are am even philosopher pick up the drink to the left first else right
    if (id % 2 == 0) {
        pthread_mutex_lock(&mugs[next]);
        pthread_mutex_lock(&mugs[id]);
    } else {
        pthread_mutex_lock(&mugs[id]);
        pthread_mutex_lock(&mugs[next]);
    }
}

/**
 * Put the mug back on the table
 * @param id  - The id of the mug to put back on the floor
 */
void set_down_mug(int id) {
    int next = (id + 1) % MAX_PHILOSOPHERS;
    pthread_mutex_unlock(&mugs[id]);
    pthread_mutex_unlock(&mugs[next]);
}

/**
 * After eating the philosophers need to wait a bit until they
 * become hungry/thirsty again. Additionally, they will check the
 * eating count and bring themselves to the minimum spot and let the
 * other philosophers eat.
 * @param id - Id of the current philosopher
 */
void stumble_around(int id) {
    int min = 0;
    int init = 0;

    // Find the philosopher with the smallest amount of time eating
    for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
        if (init == 0) {
            min = stats[i];
            init = -1;
        } else if (min > stats[i]) {
            min = stats[i];
        }
    }

    /* Wait the philosopher for the amount of time he is ahead in microseconds
       gives hungrier philosophers more of a chance to eat. Unsure if this counts
       as the philosophers "talking" because so many of the other solutions involve
       having them talk to each other.
    */
    for (int i = 0; i < (stats[id] - min); i++) {
        usleep(1);
    }

    states[id] = THIRSTY;
}

/**
 * This is called by each thread where each philosophers
 * constantly loops through here
 * @param arg - The only arg is the philosopher ID
 */
void *phil(void *arg) {
    int id = *(int *)arg;
    while(1) {
        if (states[id] == DRUNK) stumble_around(id);

        grab_mug(id);
        usleep(50 + rand() % 100);
        states[id] = DRUNK;
        stats[id] += 1;

        set_down_mug(id);
     }
}

/**
 * Handle the exit via ctrl-c by printing out stats
 * @param sig Unused parameter
 */
void handle_sigint(int sig) {
    printf("\nStats:\n");
    sleep(1);
    unsigned long long total_drinks = 0;
    for (int i = 0; i < MAX_PHILOSOPHERS; i++) total_drinks += stats[i];
    for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
        double perc = (stats[i] / total_drinks) * 100;
        printf("ID: %i   Time Drunk: %.1f%% Times: %llu\n", i, perc, stats[i]);
    }

    for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&mugs[i]);
    }

    exit(EXIT_SUCCESS);
}

int main() {
    int args[MAX_PHILOSOPHERS];
    signal(SIGINT, handle_sigint);
    srand(time(0));

    for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
        pthread_mutex_init(&mugs[i], NULL);
        args[i] = i;
        pthread_create(&philosophers[i], NULL, phil, &args[i]);
        states[i] = THIRSTY;
    }

    for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    return 0;
}

