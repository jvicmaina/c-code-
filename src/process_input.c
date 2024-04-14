#include "process_input.h"
#include <stdio.h>

void process_input(char *input, FILE *output_stream) {

    fprintf(output_stream, "Received input: %s", input);
}
