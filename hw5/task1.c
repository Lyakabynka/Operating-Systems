#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <getopt.h>

// Mutexes and condition variables for synchronization
pthread_mutex_t stand_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t traveler_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t driver_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t traveler_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t driver_cond = PTHREAD_COND_INITIALIZER;

int waiting_drivers = 0;
int waiting_travelers = 0;

char *program_name;

// Data structure to represent a person
typedef struct
{
    char type; // 't' for traveler, 'd' for driver
    int id;
} Person;

void log_taxi(Person *person, char *message)
{
    printf("%s:\t []\t\t %c%d %s \n", program_name, person->type, person->id, message);
}

// Function to simulate a traveler's life
void *traveler_life(void *arg)
{
    Person *traveler = (Person *)arg;
    while (1)
    {
        usleep(rand() % 1000000); // Simulate random arrival time

        pthread_mutex_lock(&stand_mutex);

        log_taxi(traveler, "entering");

        if (waiting_drivers > 0)
        {
            pthread_cond_signal(&driver_cond);
        }
        else
        {
            waiting_travelers++;
            log_taxi(traveler, "waiting...");
            pthread_cond_wait(&traveler_cond, &stand_mutex);

            pthread_mutex_lock(&traveler_mutex);
            log_taxi(traveler, "waking up...");
            waiting_travelers--;
            pthread_mutex_unlock(&traveler_mutex);
        }

        log_taxi(traveler, "leaving");

        pthread_mutex_unlock(&stand_mutex);
    }
    return NULL;
}

// Function to simulate a driver's life
void *driver_life(void *arg)
{
    Person *driver = (Person *)arg;
    while (1)
    {
        usleep(rand() % 1000000); // Simulate random arrival time

        pthread_mutex_lock(&stand_mutex);

        log_taxi(driver, "entering");

        if (waiting_travelers > 0)
        {
            pthread_cond_broadcast(&traveler_cond);

            log_taxi(driver, "picking traveler t");
        }
        else
        {
            printf("Waiting drivers before: %d\n", waiting_drivers);
            waiting_drivers++;
            printf("Waiting drivers: %d\n", waiting_drivers);
            log_taxi(driver, "waiting...");
            pthread_cond_wait(&driver_cond, &stand_mutex);

            pthread_mutex_lock(&driver_mutex);
            log_taxi(driver, "waking up...");
            log_taxi(driver, "picking traveler t");
            waiting_drivers--;
            pthread_mutex_unlock(&driver_mutex);
        }

        log_taxi(driver, "leaving");
        pthread_mutex_unlock(&stand_mutex);
    }
    return NULL;
}

void init_persons(Person *arr, int length, char type)
{
    for (int i = 0; i < length; i++)
    {
        arr[i].id = i;
        arr[i].type = type;
    }
}

int main(int argc, char *argv[])
{
    program_name = argv[0];

    int num_travelers = 1;
    int num_drivers = 1;

    int opt;
    while ((opt = getopt(argc, argv, "t:d:")) != -1)
    {
        switch (opt)
        {
        case 't':
            num_travelers = atoi(optarg);
            break;
        case 'd':
            num_drivers = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Usage: %s -t [num_travelers] -d [num_drivers]\n", argv[0]);
            return 1;
        }
    }

    pthread_t *travelers = malloc(num_travelers * sizeof(pthread_t));
    pthread_t *drivers = malloc(num_drivers * sizeof(pthread_t));

    Person *travelers_persons = calloc(num_travelers, sizeof(Person));
    Person *drivers_persons = calloc(num_drivers, sizeof(Person));

    init_persons(travelers_persons, num_travelers, 't');
    init_persons(drivers_persons, num_drivers, 'd');

    srand(time(NULL));

    // Create traveler threads
    for (int i = 0; i < num_travelers; i++)
    {
        pthread_create(&travelers[i], NULL, traveler_life, &travelers_persons[i]);
    }

    // Create driver threads
    for (int i = 0; i < num_drivers; i++)
    {
        pthread_create(&drivers[i], NULL, driver_life, &drivers_persons[i]);
    }

    sleep(100);

    // Join traveler threads (never reached)
    for (int i = 0; i < num_travelers; i++)
    {
        pthread_join(travelers[i], NULL);
    }

    // Join driver threads (never reached)
    for (int i = 0; i < num_drivers; i++)
    {
        pthread_join(drivers[i], NULL);
    }

    free(travelers);
    free(drivers);

    return 0;
}