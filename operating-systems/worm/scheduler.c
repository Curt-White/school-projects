#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include "scheduler.h"

#include <assert.h>
#include <curses.h>
#include <ucontext.h>
#include <errno.h>
#include <string.h>

#include "util.h"

// This is an upper limit on the number of tasks we can create.
#define MAX_TASKS 128

// This is the size of each task's stack memory
#define STACK_SIZE 65536

void schedule_task();

// State for the tasks
typedef enum task_state {
    READY,
    RUNNING,
    TERMINATED,
    WAITING,
    WAITING_FOR_INPUT
} task_state_t;

// This struct will hold the all the necessary information for each task
typedef struct task_info {
  ucontext_t context;       // State to switch back
  ucontext_t exit_context;  // Context to enter when exit
  task_state_t state;       // State of the process
  size_t time_to_wake;      // Time at which the process should wake
  int waiting_for_id;       // Default: -1, else task_id
  int input;                // Input read while the task was being blocked
} task_info_t;

int current_task = 0;           //< The handle of the currently-executing task
int num_tasks = 1;              //< The number of tasks created so far
task_info_t tasks[MAX_TASKS];   //< Information for every task

/**
 * Initialize a task info structure
 * @param info - Pointer to structure which needs initialization
 */
void init_task_info(task_info_t *info) {
    info->waiting_for_id = -1;
    info->state = READY;
    info->time_to_wake = 0;
}

/**
 * Exit the program, printing anything from errno
 * @param message A message to print upon exiting the program
 */
void exit_with_error(char *message) {
    fprintf(stderr, "ERROR: %s\n", strerror(errno));
    printf("MESSAGE: %s\n", message);
    exit(0);
}

/**
 * Initialize the scheduler. Programs should call this before calling any other
 * functions in this file.
 */
void scheduler_init() {
  int res = getcontext(&tasks[current_task].context);
  if (res == -1) exit_with_error("could not get context of main task");

  init_task_info(&tasks[current_task]);
}

/**
 * This function will execute when a task's function returns. This allows you
 * to update scheduler states and start another task. This function is run
 * because of how the contexts are set up in the task_create function.
 */
void task_exit() {
    tasks[current_task].state = TERMINATED;
    int res = setcontext(&tasks[0].context);
    if (res == -1) exit_with_error("an error occurred when attempting to switch to the main context");
}

/**
 * Create a new task and add it to the scheduler.
 *
 * \param handle  The handle for this task will be written to this location.
 * \param fn      The new task will run this function.
 */
void task_create(task_t* handle, task_fn_t fn) {
  // Claim an index for the new task
  int index = num_tasks;
  num_tasks++;
  
  // Set the task handle to this index, since task_t is just an int
  *handle = index;
 
  // We're going to make two contexts: one to run the task, and one that runs at the end of the task so we can clean up. Start with the second
  
  // First, duplicate the current context as a starting point
  getcontext(&tasks[index].exit_context);
  
  // Set up a stack for the exit context
  tasks[index].exit_context.uc_stack.ss_sp = malloc(STACK_SIZE);
  tasks[index].exit_context.uc_stack.ss_size = STACK_SIZE;
  
  // Set up a context to run when the task function returns. This should call task_exit.
  makecontext(&tasks[index].exit_context, task_exit, 0);
  
  // Now we start with the task's actual running context
  getcontext(&tasks[index].context);
  
  // Allocate a stack for the new task and add it to the context
  tasks[index].context.uc_stack.ss_sp = malloc(STACK_SIZE);
  tasks[index].context.uc_stack.ss_size = STACK_SIZE;
  
  // Now set the uc_link field, which sets things up so our task will go to the exit context when the task function finishes
  tasks[index].context.uc_link = &tasks[index].exit_context;
  
  // And finally, set up the context to execute the task function
  makecontext(&tasks[index].context, fn, 0);
  init_task_info(&tasks[index]);
}

/**
 * Swap from one task to another
 * @param curr_task_id The current task who's context will be saved
 * @param new_task_id The new task who's context will be loaded
 * @state The state to switch old the task to (might be Blocked, Waiting)
 *
 * @return -1 if argument error
 */
int swap_task(int curr_task_id, int new_task_id, task_state_t state) {
    if (state == RUNNING) {
        fprintf(stderr, "Two tasks cannot simultaneously be in the running state\n");
        return -1;
    }

    if (curr_task_id >= num_tasks || new_task_id >= num_tasks) {
        fprintf(stderr, "either %i or %i is not a valid id\n", curr_task_id, new_task_id);
        return -1;
    }

    tasks[curr_task_id].state = state;
    tasks[new_task_id].state = RUNNING;
    int res = swapcontext(&tasks[curr_task_id].context, &tasks[new_task_id].context);
    if (res == -1) exit_with_error("could not switch to provided context");
    return 0;
}

/**
 * Handle tasks that are waiting and set them to a ready state
 * if the appropriate conditions are met
 * @param task_id The task ID of the task in waiting state
 */
void handle_wait(int task_id) {
    task_info_t *curr = &tasks[task_id];

    if (time_ms() > curr->time_to_wake) {
        if (curr->waiting_for_id >= 0) {
            if (tasks[curr->waiting_for_id].state == TERMINATED) {
                curr -> waiting_for_id = -1;
                curr->state = READY;
            }
        } else {
            curr->state = READY;
        }
    }
}

/**
 * Handle input for tasks which are blocked for user input
 * @param task_id ID of the task in which input should be stored
 */
void handle_input(int task_id) {
    task_info_t *curr = &tasks[task_id];
    int c = getch();

    if (c != ERR) {
        curr->input = c;
        swap_task(0, current_task, WAITING);
    }
}

/**
 * Cycle through tasks and schedule them accordingly
 */
void schedule_task() {
    while(1) {
        task_info_t *curr = &tasks[current_task];
        if (curr->state == WAITING)
            handle_wait(current_task);
        else if (curr->state == READY && current_task != 0)
            swap_task(0, current_task, WAITING);
        else if (curr->state == WAITING_FOR_INPUT)
            handle_input(current_task);

        if (tasks[0].state == READY) return;
        current_task = (current_task + 1) % num_tasks;
    }
}

/**
 * Wait for a task to finish. If the task has not yet finished, the scheduler should
 * suspend this task and wake it up later when the task specified by handle has exited.
 *
 * \param handle  This is the handle produced by task_create
 */
void task_wait(task_t handle) {
    tasks[current_task].waiting_for_id = handle;
    tasks[current_task].state = WAITING;
    schedule_task();
}

/**
 * The currently-executing task should sleep for a specified time. If that time is larger
 * than zero, the scheduler should suspend this task and run a different task until at least
 * ms milliseconds have elapsed.
 * 
 * \param ms  The number of milliseconds the task should sleep.
 */
void task_sleep(size_t ms) {
    size_t wait_until = time_ms() + ms;
    tasks[current_task].time_to_wake = wait_until;
    swap_task(current_task, 0, WAITING);
}

/**
 * Read a character from user input. If no input is available, the task should
 * block until input becomes available. The scheduler should run a different
 * task while this task is blocked.
 *
 * \returns The read character code
 */
int task_readchar() {
    int ret = getch();
    if (ret == ERR) {
        swap_task(current_task, 0, WAITING_FOR_INPUT);
        return tasks[current_task].input;
    }

  return ERR;
}
