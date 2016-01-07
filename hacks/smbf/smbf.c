/*
 * ANSI C implementation of self-modifying brainfuck.
 * 
 * By Simon Howard <fraggle@removethisbit.gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char *tape;
unsigned int tape_size;
unsigned int tape_center;

void tape_init(void)
{
    tape_size = 1000;
    tape_center = tape_size / 2;
    tape = malloc(tape_size);
}

unsigned char tape_read(signed int index)
{
    index += tape_center;

    if (index < 0 || index >= tape_size) {
        return 0;
    } else {
        return tape[index];
    }
}

void tape_write(signed int index, unsigned char value)
{
    signed int index2;

    index2 = index + tape_center;

    if (index2 < 0 || index2 >= tape_size) {
        unsigned int new_tape_size;
        unsigned char *new_tape;
        unsigned int new_tape_center;

        /* Need to resize the tape to be larger */
        
        new_tape_size = tape_size;
        
        do {
            new_tape_size *= 2;
            new_tape_center = new_tape_size / 2;
            index2 = index + new_tape_center;
        } while (index2 < 0 || index2 >= new_tape_size);

        /* Now resized big enough */

        new_tape = malloc(new_tape_size);
        memset(new_tape, 0, new_tape_size);
        
        /* Copy old data */
        
        memcpy(new_tape + new_tape_center - tape_center, tape, tape_size);
        
        tape = new_tape;
        tape_size = new_tape_size;
        tape_center = new_tape_center;
    
        index2 = index + tape_center;
    }

    tape[index2] = value;
}

void interpret(signed int entrypoint)
{
    signed int pc;
    signed int ptr;
    unsigned char cmd;
    int loop_level;

    pc = entrypoint;
    ptr = 0;

    while (pc < 0) {

        cmd = tape_read(pc);

        switch(cmd) {
            case ',':
                tape_write(ptr, getchar());
                break;

            case '.':
                putchar(tape_read(ptr));
                break;

            case '<':
                ptr -= 1;
                break;

            case '>':
                ptr += 1;
                break;

            case '-':
                tape_write(ptr, tape_read(ptr) - 1);
                break;

            case '+':
                tape_write(ptr, tape_read(ptr) + 1);
                break;

            case '[':
                if (tape_read(ptr) == 0) {
                    loop_level = 1;

                    while (loop_level > 0) {
                        pc += 1;
                        if (tape_read(pc) == '[')
                            loop_level += 1;
                        else if (tape_read(pc) == ']')
                            loop_level -= 1;
                    }
                }
                break;

            case ']':
                /* Rewind to the loop start */

                loop_level = 1;

                while (loop_level > 0) {
                    pc -= 1;
                    if (tape_read(pc) == '[')
                        loop_level -= 1;
                    else if (tape_read(pc) == ']')
                        loop_level += 1;
                }

                pc -= 1;

                break;

            default:
                break;
        }

        pc += 1;
    }
}

signed int load_script(char *filename)
{
    FILE *fstream;
    signed int file_size;
    int i;
    
    fstream = fopen(filename, "rb");

    if (fstream == NULL) {
        fprintf(stderr, "Error loading '%s'\n", filename);
        exit(-1);
    }

    /* Find the file size */
     
    fseek(fstream, 0, SEEK_END);
    file_size = ftell(fstream);
    fseek(fstream, 0, SEEK_SET);

    /* Read in all data */

    for (i=0; i<file_size; ++i) {
        unsigned char value;

        fread(&value, 1, 1, fstream);

        tape_write(i - file_size, value);
    }

    return -file_size;
}

int main(int argc, char *argv[])
{
    signed int entrypoint;

    if (argc < 2) {
        printf("Usage: %s <script name>\n", argv[0]);
        exit(-1);
    }

    tape_init();
    entrypoint = load_script(argv[1]);
    interpret(entrypoint);

    return 0;
}

