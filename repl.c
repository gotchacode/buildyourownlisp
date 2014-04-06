#include <stdio.h>

/* declare a static buffer for user input of maximum size 2048 */
static char input[2048];

int main(int argc, char **argv) {
    /* Print Version and exit information */
    puts("Lispy version 0.0.0.0.1");
    puts("Press ctrl + c to Exit\n");

    while(1) {
        /* output our prompt */
        fputs("Lipsy>", stdout);

        fgets(input, 2048, stdin);

        /* Echo input back to the user */
        fputs("No you're a %s", input);

    }
    return 0;
}
