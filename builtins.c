#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <pwd.h>
#include <ctype.h>

#define MAX_PATH 1024
#define MAX_HISTORY 100

/* ========== FUNCTION PROTOTYPES ========== */
void handle_cd(char **args);
void handle_echo(char **args);
void handle_pwd(void);
void handle_exit(char **args);
void handle_clear(void);
void handle_whoami(void);
void handle_help(void);
void handle_history(void);
void handle_env(void);
void handle_export(char **args);
void handle_unset(char **args);

/* ========== HISTORY HANDLING ========== */
char *history[MAX_HISTORY];
int history_count = 0;

void add_to_history(const char *cmd) {
    if (history_count < MAX_HISTORY) {
        history[history_count++] = strdup(cmd);
    }
}

/* ========== BUILTIN COMMAND CHECK ========== */
int is_builtin(const char *cmd) {
    return strcmp(cmd, "cd") == 0 ||
           strcmp(cmd, "echo") == 0 ||
           strcmp(cmd, "pwd") == 0 ||
           strcmp(cmd, "exit") == 0 ||
           strcmp(cmd, "clear") == 0 ||
           strcmp(cmd, "whoami") == 0 ||
           strcmp(cmd, "help") == 0 ||
           strcmp(cmd, "history") == 0 ||
           strcmp(cmd, "env") == 0 ||
           strcmp(cmd, "export") == 0 ||
           strcmp(cmd, "unset") == 0;
}

/* ========== BUILTIN FUNCTION IMPLEMENTATIONS ========== */

void handle_cd(char **args) {
    char cwd[MAX_PATH];
    const char *dir = args[1] ? args[1] : getenv("HOME");

    if (strcmp(dir, "-") == 0) {
        const char *oldpwd = getenv("OLDPWD");
        if (oldpwd != NULL) {
            if (chdir(oldpwd) == 0) {
                printf("%s\n", oldpwd);
            } else {
                perror("minishell");
            }
        } else {
            fprintf(stderr, "minishell: cd: OLDPWD not set\n");
        }
    } else {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            setenv("OLDPWD", cwd, 1);
        }
        if (chdir(dir) != 0) {
            perror("minishell");
            fprintf(stderr, "minishell: cd: %s: No such file or directory\n", dir);
        }
    }
}

void handle_echo(char **args) {
    int i = 1;
    int newline = 1;
    if (args[1] && strcmp(args[1], "-n") == 0) {
        newline = 0;
        i = 2;
    }
    for (; args[i]; i++) {
        printf("%s", args[i]);
        if (args[i + 1]) printf(" ");
    }
    if (newline) printf("\n");
}

void handle_pwd(void) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("minishell");
    }
}

void handle_exit(char **args) {
    int status = 0;
    if (args[1]) {
        char *endptr;
        status = strtol(args[1], &endptr, 10);
        if (*endptr != '\0') {
            fprintf(stderr, "minishell: exit: %s: numeric argument required\n", args[1]);
            return;
        }
    }
    exit(status);
}

void handle_clear(void) {
    printf("\033[H\033[J"); // ANSI escape code to clear screen
}

void handle_whoami(void) {
    struct passwd *pw = getpwuid(getuid());
    if (pw) {
        printf("%s\n", pw->pw_name);
    } else {
        perror("minishell");
    }
}

void handle_help(void) {
    printf("MiniShell built-in commands:\n");
    printf("  cd [dir]       - Change directory\n");
    printf("  pwd            - Print current directory\n");
    printf("  echo [args]    - Echo arguments\n");
    printf("  exit [status]  - Exit the shell\n");
    printf("  clear          - Clear the screen\n");
    printf("  whoami         - Show current user\n");
    printf("  help           - Show this help message\n");
    printf("  history        - Show command history\n");
    printf("  env            - Show environment variables\n");
    printf("  export VAR=VAL - Set environment variable\n");
    printf("  unset VAR      - Unset environment variable\n");
}

void handle_history(void) {
    for (int i = 0; i < history_count; i++) {
        printf("%d %s\n", i + 1, history[i]);
    }
}

void handle_env(void) {
    extern char **environ;
    for (char **e = environ; *e; ++e) {
        printf("%s\n", *e);
    }
}

void handle_export(char **args) {
    if (!args[1]) {
        handle_env();
        return;
    }
    for (int i = 1; args[i]; i++) {
        char *eq = strchr(args[i], '=');
        if (eq) {
            *eq = '\0';
            setenv(args[i], eq + 1, 1);
        } else {
            setenv(args[i], "", 1);
        }
    }
}

void handle_unset(char **args) {
    for (int i = 1; args[i]; i++) {
        unsetenv(args[i]);
    }
}

/* ========== DISPATCH BUILTIN ========== */

void handle_builtin(char **args) {
    if (!args[0]) return;

    if (strcmp(args[0], "cd") == 0) handle_cd(args);
    else if (strcmp(args[0], "echo") == 0) handle_echo(args);
    else if (strcmp(args[0], "pwd") == 0) handle_pwd();
    else if (strcmp(args[0], "exit") == 0) handle_exit(args);
    else if (strcmp(args[0], "clear") == 0) handle_clear();
    else if (strcmp(args[0], "whoami") == 0) handle_whoami();
    else if (strcmp(args[0], "help") == 0) handle_help();
    else if (strcmp(args[0], "history") == 0) handle_history();
    else if (strcmp(args[0], "env") == 0) handle_env();
    else if (strcmp(args[0], "export") == 0) handle_export(args);
    else if (strcmp(args[0], "unset") == 0) handle_unset(args);
    else fprintf(stderr, "minishell: %s: command not found\n", args[0]);
}
