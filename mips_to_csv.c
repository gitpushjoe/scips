#include <stdio.h>

unsigned int flip_endian(unsigned int value) {
    return ((value >> 24) & 0xff) | ((value >> 8) & 0xff00) | ((value << 8) & 0xff0000) | ((value << 24) & 0xff000000);
}

int main(int argc, char* argv[]) {
    if (argc != 3 && argc != 2) {
        printf("Usage: %s <input file> <output file?>\n", argv[0]);
        return 1;
    }

    FILE* in = fopen(argv[1], "r");
    if (!in) {
        perror("fopen");
        return 1;
    }

    unsigned int value;

    FILE* out = argc == 3 ? fopen(argv[2], "w") : stdout;
    if (!out) {
        perror("fopen");
        return 1;
    }

    while (fread(&value, sizeof(value), 1, in) == 1) {
        fprintf(out, "%u\n", flip_endian(value));
    }

    fclose(in);
    if (out != stdout) {
        fclose(out);
    }
    return 0;
}
