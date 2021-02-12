// First read lines from .SEEshrc file from HOME directory
// print out each line of the .SEEshrc file immediately
// print a ? and a space at the beginning of each line
// exit on control-D or by typing exit

// read line
// tokenize to form token array
// parse the tokens to form (command option arg) style
// execute the command

// tokens are sequence of non-whitespace characters
// 6 commands needed cd, help, exit, pwd, set, unset

// if not built in, use PATH to locate binary and fork a process and pass filename
// forked processes run in the foreground, use control-C to terminate the current process
// handle erroneous line by writing message to std err, user errors should never crash SEEsh
// ALWAYS FREE YOUR MEMORY !!!!!!!

// ALSO HISTORY COMMAND for bonus marks
// ability to execute a previous command by prefixing with an !commandprefix
#define _POSIX_C_SOURCE 200112L
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#define MAX_LINE_LENGTH 512
#define TOKEN_BUFFER_SIZE 50
#define PROMPT_CURSOR "? "
#define DELIMITER_CHARS " \t\r\n\a"
#define HOME_ENV_VAR "HOME"
#define DEFAULT_HISTORY_SIZE 50

typedef struct history {
    char **items;
    int length;
    int count;
} history;

int curr_pid;
history c_history;


/**
 * Get the SEEsh configuration file from the HOME directory
 */
FILE *get_config() {
    char *home = getenv(HOME_ENV_VAR);
    if (home == NULL) {
        fprintf(stderr, "HOME Variable Not Defined\n");
        return NULL;
    }

    char *file = "/.SEEshrc";
    char *path = malloc(strlen(home) + strlen(file) + 1);
    if (path == NULL) {
        fprintf(stderr, "Error Allocating Memory\n");
        return NULL;
    }

    strncpy(path, home, strlen(home) + 1);
    strncat(path, file, strlen(file) + 1);
    FILE *config = fopen(path, "r");
    if (config == NULL) {
        fprintf(stderr, "Config File Error: %s\n", strerror(errno));
        return NULL;
    }
    free(path);

    return config;
}

/**
 * Initialize the global SEEsh history
 */
void init_history() {
    char **new = (char **)malloc(DEFAULT_HISTORY_SIZE * sizeof(char*));
    if (new == NULL) {
        fprintf(stderr, "Could Not Allocation Memory For History\n");
        return;
    }

    struct history new_hist = { new, DEFAULT_HISTORY_SIZE, 0 };
    c_history = new_hist;
}

/**
 * Free the history structure from memory
 */
void free_history() {
    int i = 0;
    while (i < c_history.count) {
        free(c_history.items[i]);
        i ++;
    }

    free(c_history.items);
}

/**
 * Exit the program and perform any memory freeing
 */
void clean_and_exit() {
    free_history();
    exit(0);
}

/**
 * Tokenize the string of characters provided in line and return an array of
 * strings. This array needs to be freed with free().
 * @param line A string of characters to tokenize
 * @return An array of strings parsed from the line input delimited by whitespace
 */
char** tokenize_line(char *line) {
    int length = TOKEN_BUFFER_SIZE, position = 0;
    char** tokens = (char**)malloc(length * sizeof(char*));
    if (tokens == NULL) {
        fprintf(stderr, "Error Allocating Memory\n");
        return NULL;
    }

    char* token = strtok(line, DELIMITER_CHARS);
    while (token) {
        tokens[position] = token;
        position++;

        if (position >= length) {
            length += TOKEN_BUFFER_SIZE;
            // Allocate a temp because if realloc fails can still free
            char **temp = realloc(tokens, length * sizeof(char*));
            if (temp == NULL) {
                free(tokens);
                fprintf(stderr, "Error Allocating Memory\n");
                return NULL;
            }

            tokens = temp;
        }

        token = strtok(NULL, DELIMITER_CHARS);
    }

    tokens[position] = NULL;
    return tokens;
}

/**
 * Clear the rest of the data either up to a newline character
 * or until the EOF marker
 * @param src a file to clear a line in
 */
void clear_line(FILE *src) {
    int c;
    do {
        c = fgetc(src);
    } while (c != '\n' && c != EOF);
}

/**
 * Reads the next line from stdin and place it in the buffer. Forces
 * the program to exit if EOF is found and returns nothing in buffer if
 * the maximum buffer size is exceeded.
 * @param buffer A buffer to place the data in
 * @param buffer_size The size of the provided buffer argument
 * @returns 1 if the EOF is reached or 0 otherwise
 */
int get_next_line(char* buffer, size_t buffer_size, FILE *src) {
    char* res = fgets(buffer, buffer_size, src);
    if (res == NULL) {
        if (feof(src)) return 1;
        if (ferror(src)) {
            fprintf(src, "Error Reading From stdin\n");
            buffer[0] = '\0';
            return 0;
        }
    }

    // Check if there is a newline preset, if not then max len is exceeded
    char* loc = strchr(buffer, '\n');
    if (loc == NULL && !feof(src)) {
        fprintf(stderr, "Maximum Line Length Exceeded\n");
        buffer[0] = '\0';
        clear_line(src);
    } else if (loc != NULL) {
        *loc = '\0';
    }

    return 0;
}

/**
 * Change the current working directory of the program, if no directory
 * is specified then the program will move to the HOME directory.
 * @param args A single argument specifying path, or 0 args
 */
void change_dir(char **args) {
    if (args[0] != NULL && args[1] != NULL) {
        fprintf(stderr, "Too Many Arguments\n");
        return;
    }

    char *new_dir = NULL;
    if (args[0] == NULL) {
        char *home = getenv(HOME_ENV_VAR);
        if (home != NULL) new_dir = home;
        else {
            fprintf(stderr, "HOME Environment Variable Is Not Defined\n");
            return;
        }
    } else {
        new_dir = args[0];
    }

    if (chdir(new_dir) != 0) {
        printf("%s\n", strerror(errno));
    }
}

/**
 * Print the current working directory, no arguments should be passed
 * in the args array or the function will return immediately.
 * @param args Should be an array with NULL
 */
void print_working_directory(char **args) {
    if (args[0] != NULL) {
        fprintf(stderr, "Too Many Arguments\n");
        return;
    }

    char* path_name = getcwd(NULL, 0);
    if (path_name == NULL) {
        printf("%s\n", strerror(errno));
    }

    printf("%s\n", path_name);
    free(path_name);
}

/**
 * Display the help command with information about how to use built in
 * terminal commands.
 * @param args 0 args should be provided, NULL in first array position
 */
void display_help(char **args) {
    if (args[0] != NULL) {
        fprintf(stderr, "Too Many Arguments\n");
        return;
    }

    printf("cd [dir]          - Navigate to the directory given by [dir]\n");
    printf("pwd               - Print the current working directory\n");
    printf("help              - Display this information\n");
    printf("exit              - Exit the SEEsh shell\n");
    printf("set [var] [value] - Sets an environment variable named [var] with [value], "
           "[var] is created if does not exist\n");
    printf("unset [var]       - Destroy the environment variable named [var]\n");
    printf("history           - View recently used commands\n");
    printf("** Prefacing a command with ! (Exclamation Point) will run the most recent command "
           "with a matching prefix\n");
}

/**
 * Set an environment with the name provided in the first argument, to the value
 * which is provided in the second argument., If the variable does not already exist
 * then the variable is created and set to the value provided.
 * @param args A name and value to set
 */
void set_env_var(char **args) {
    if (args[0] == NULL || args[1] == NULL || args[2] != NULL) {
        fprintf(stderr, "Incorrect Number of Arguments Provided\n");
        return;
    }

    if (setenv(args[0], args[1], 1) != 0) {
        printf("%s\n", strerror(errno));
    }
}

/**
 * Remove the variable with the presented name from the env variables
 * @param args Name of env var to be removed in first argo
 */
void remove_env_var(char **args) {
    if (args[0] == NULL || args[1] != NULL) {
        fprintf(stderr, "Incorrect Number of Arguments Provided\n");
        return;
    }

    if (unsetenv(args[0]) != 0) {
        printf("%s\n", strerror(errno));
    }
}

/**
 * A handler to handle the SIGINT trap/interrupt. This will
 * terminate the program running inside of SEEsh instead of
 * terminating SEEsh.
 * @param sigint Unused code handed in by the signal function
 */
void sig_int_handler(int sigint) {
    if (curr_pid != 0) {
        kill(curr_pid, SIGINT);
    }

    // Set this to handler again fro next time
    signal(SIGINT, sig_int_handler);
}

/**
 * Fork a new process and execute the program which is provided
 * in the path arg
 * @param path The path of the program to be execute
 * @param args The args to pass to the program
 */
void spawn_process(char *path, char **args) {
    int pid = fork();

    if (pid == 0) { // In the child process
        int res = execvp(path, args);
        if (res == -1) { // Failed to start process
            if (errno == 2) {
                printf("Invalid Command\n");
            } else {
                printf("%s\n", strerror(errno));
            }
        }

        _exit(EXIT_FAILURE);
    } else if (pid < 0) { // Failed to spawn child process
        fprintf(stderr, "Failed To Process At: %s\n", path);
        return;
    } else {
        int status = 0;

        do {
            int res = waitpid(pid, &status, 0);
            if (res == -1) {
                printf("%s\n", strerror(errno));
                return;
            }

            if (status != 0) { // Did not end properly
                return;
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

/**
 * Print the history of the commands that were recently used
 * @param args Args are for future extensions of the function
 */
void print_history(char **args) {
    if (args[0] != NULL) {
        fprintf(stderr, "Too Many Arguments Provided\n");
        return;
    }
    
    for (int i = 0; i < c_history.count; i++) {
        printf("%i. %s\n", i + 1, c_history.items[i]);
    }
}

/**
 * Exit the shell
 * @param args empty array
 */
void exit_shell(char **args) {
    if (args[0] != NULL) {
        fprintf(stderr, "Too Many Arguments Provided\n");
        return;
    }

    clean_and_exit();
}


/**
 * Run the process provided in the tokens first position. If the process is built in
 * the process is run immediately, otherwise a process is forked and the process
 * is searched for in the PATH.
 * @param tokens A string of tokens containing the arguments and name of process
 */
void run_process(char **tokens) {
    if (strcmp(tokens[0], "cd") == 0) change_dir(tokens + 1);
    else if (strcmp(tokens[0], "pwd") == 0) print_working_directory(tokens + 1);
    else if (strcmp(tokens[0], "help") == 0) display_help(tokens + 1);
    else if (strcmp(tokens[0], "set") == 0) set_env_var(tokens + 1);
    else if (strcmp(tokens[0], "unset") == 0) remove_env_var(tokens + 1);
    else if (strcmp(tokens[0], "history") == 0) print_history(tokens + 1);
    else if (strcmp(tokens[0], "exit") == 0) exit_shell(tokens + 1);
    else spawn_process(tokens[0], tokens);
}

/**
 * Add a new command to the history array. The array will
 * automatically resize if it grows beyond its current size.
 * @param command A command to be added to the list
 */
void add_to_history(char *command) {
    size_t length = strlen(command) + 1;
    char *loc = (char *)malloc(length * sizeof(char));
    if (loc == NULL) {
        fprintf(stderr, "Error Allocating Memory");
        return;
    }
    // Copy the command to the new place in memory
    loc = strncpy(loc, command, length);
    loc[length - 1] = '\0';

    // resize accordingly
    if (c_history.count == c_history.length) {
        c_history.length += DEFAULT_HISTORY_SIZE;
        char **temp = realloc(c_history.items, c_history.length * sizeof(char*));
        if (temp == NULL) {
            free(c_history.items);
            fprintf(stderr, "Error Allocating Memory\n");
            return;
        }

        c_history.items = temp;
    }

    c_history.items[c_history.count] = loc;
    c_history.count++;
}

/**
 * Check if the provided string is a prefix of a another string
 * @param prefix The item to check if prefix
 * @param str The string to check if has prefix prefix
 * @return o if not prefix, 1 if is prefix
 */
int is_prefix(const char *prefix, const char *str) {
    for (int i = 0; i < strlen(prefix); i++) {
        if (prefix[i] != str[i]) return 0;
    }

    return 1;
}

/**
 * If using the ! operator to quick select a previous command this
 * function will search for the command in the history list
 * @param command
 * @return
 */
char *match_command(char *command) {
    if (strlen(command) == 0) return NULL;

    int i = 0;
    while (i < c_history.count) {
        char *curr = c_history.items[i];
        int cmp = is_prefix(command, curr);
        if (cmp != 0) {
            return curr;
        }
        i++;
    }

    return NULL;
}

/**
 * Process a single line of data from the file provided in src. If the line is longer than
 * the max line length then the remainder of the line is ignored and the computer will
 * attempt to run the command.
 * @param src A source to read commands from
 * @param v Verbose mode, 0 is off
 * @return 1 if the src file is empty, 0 otherwise
 */
int process_line(FILE *src, int v) {
    char *command;
    char **tokens;
    char line_buffer[MAX_LINE_LENGTH + 1];
    int res = get_next_line(line_buffer, MAX_LINE_LENGTH + 1, src);

    if (res) return 1;
    if (strlen(line_buffer) == 0) return 0;
    else if (v) printf("%s\n", line_buffer);

    if (line_buffer[0] == '!') {
        command = match_command(line_buffer + 1);
        if (command == NULL) return 0;
    } else {
        command = line_buffer;
    }

    add_to_history(command);
    tokens = tokenize_line(command);
    run_process(tokens);
    free(tokens);

    return  0;
}

/**
 * Run the command line in standard mode using
 * the stdin is a source for commands. Will run indefinitely
 * or until an EOF is detected via control-D.
 */
void run_std_mode() {
    while (1) {
        printf(PROMPT_CURSOR);
        int status = process_line(stdin, 0);
        if (status == 1) exit(EXIT_SUCCESS);
    }
}

/**
 * Run a list of commands from a file.
 * @param src File to read commands from
 */
void run_script(FILE *src) {
    while(1) {
        int status = process_line(src, 1);
        if (status == 1) return;
    }
}

int main(int argc, char **argv) {
    signal(SIGINT, sig_int_handler);
    init_history();

    FILE *src = get_config();
    if (src) {
        run_script(src);
    }

    run_std_mode();
}
