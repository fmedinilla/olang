#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SPACES 8
#define OUTPUT_SPACES 25

enum OP_CODES {
    INC,
    DEC,
    SET,
    MOF,
    MOB,
    PRT
};

void interpret(char *file);
void parse_line(char *line, int *cmd, int *arg);
void exec(int cmd, int arg);
void print_memory();
void screen_clear();

int memory[SPACES];
char output[OUTPUT_SPACES];
int pointer = 0;
int output_ptr = 0;

int main(int argc, char *argv[])
{
    if (argc == 2) {
        interpret(argv[1]);
        return 0;
    }

    while(1) {
        screen_clear();
        
        puts("Olang Interactive\n\n");
        puts("Type 'EXIT' to exit interactive mode.\n");
        puts("Type 'OUTPUT' to show output buffer and remove after.\n\n");
        
        print_memory();

        printf(" > ");
        char buffer[8];
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            perror("Error reading input");
            continue;
        }

        // Remover el carácter de nueva línea al final del buffer si existe
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "EXIT") == 0) break;
        if (strcmp(buffer, "OUTPUT") == 0) {
            printf("Output: '%s'\n", output);
            output_ptr = 0;
            puts("Press Enter to continue...");
            getchar();  // Pausa para mostrar el buffer
            memset(output, 0, OUTPUT_SPACES);
            continue;
        }

        int cmd = 0;
        int arg = 0;
        parse_line(buffer, &cmd, &arg);
        exec(cmd, arg);
    }

    return 0;
}

void interpret(char *file)
{
    FILE *fh = fopen(file, "r");
    if (!fh) {
        perror("Error opening file");
        return;
    }

    char buffer[8];
    while (fgets(buffer, sizeof(buffer), fh)) {
        // Remover el carácter de nueva línea al final del buffer si existe
        buffer[strcspn(buffer, "\n")] = '\0';

        int cmd = 0;
        int arg = 0;

        parse_line(buffer, &cmd, &arg);
        exec(cmd, arg);
    }

    fclose(fh);
}
void parse_line(char *line, int *cmd, int *arg)
{
    char cmd_raw[4];
    strncpy(cmd_raw, line, 3);
    cmd_raw[3] = '\0';

    char *arg_str = line + 4;
    arg_str[3] = '\0';
    int arg_value = (int) strtol(arg_str, NULL, 16);

    int op_code = -1;
    if (strcmp(cmd_raw, "INC") == 0) op_code = INC;
    if (strcmp(cmd_raw, "DEC") == 0) op_code = DEC;
    if (strcmp(cmd_raw, "SET") == 0) op_code = SET;
    if (strcmp(cmd_raw, "MOF") == 0) op_code = MOF;
    if (strcmp(cmd_raw, "MOB") == 0) op_code = MOB;
    if (strcmp(cmd_raw, "PRT") == 0) op_code = PRT;

    *cmd = op_code;
    *arg = arg_value;
}
void exec(int cmd, int arg)
{
    switch (cmd) {
        case INC:
            memory[pointer]++;
            break;
        case DEC:
            memory[pointer]--;
            break;
        case SET:
            memory[pointer] = arg;
            break;
        case MOF:
            pointer++;
            if (pointer == SPACES) pointer = 0;
            break;
        case MOB:
            pointer--;
            if (pointer == -1) pointer = SPACES - 1;
            break;
        case PRT:
            output[output_ptr] = (char) memory[pointer];
            output_ptr++;
            if (output_ptr == OUTPUT_SPACES) output_ptr = 0;
            printf("%c", memory[pointer]);
            break;
        default:
            // printf("Unknown command\n");
            break;
    }
}
void print_memory()
{
    char border[SPACES * 3 + 2];
    for (int i = 0; i < SPACES * 3; i++) {
        border[i] = '-';
    }
    border[0] = '+';
    border[SPACES * 3] = '+';
    border[SPACES * 3 + 1] = '\0';

    printf("%s\n", border);
    for (int i = 0; i < SPACES; i++) {
        printf("|%02x", memory[i]);
    }
    printf("|\n%s\n", border);

    char pointer_arrow[SPACES * 3 + 1];
    for (int i = 0; i < SPACES * 3; i++) {
        pointer_arrow[i] = ' ';
    }
    pointer_arrow[SPACES * 3] = '\0';
    pointer_arrow[3 * pointer + 1] = '^';
    printf("%s\n\n", pointer_arrow);
}
void screen_clear()
{
    printf("\e[1;1H\e[2J");
}
