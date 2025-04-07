#include <stdio.h>
#include <stdlib.h>

typedef struct IMArray {
    int op;
    int addr;
} IMArray;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Open the file specified by first command-line argument
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    int op = 0, addr = 0;
    int IRC = 0;                  // Index into instruction memory
    IMArray IM[129] = {0};       // Instruction memory

    // Read instructions from file until HALT (op == 7) or EOF
    while (fscanf(fp, "%d %d", &op, &addr) == 2) {
        IMArray temp;
        temp.op = op;
        temp.addr = addr;
        IM[IRC++] = temp;

        // If we just read a HALT instruction, stop reading
        if (op == 7) {
            break;
        }
    }

    fclose(fp);

    // We are now storing the instructions in IM[0..(IRC-1)]
    // and run the logic for the tiny vm:
    int PC = 0;         // Program Counter
    int A  = 0;         // Accumulator
    int DM[10] = {0};   // Data Memory
    IMArray IR;
    int neg = 1;
    int MAR = 0;
    int MDR = 0;

    // Execute all instructions
    while ((PC / 2) < IRC) {

        // FETCH
        IR.op = IM[PC / 2].op;
        IR.addr = IM[PC / 2].addr;
        PC += 2;

        // DECODE / EXECUTE
        switch (IR.op) {

        case 1: // LOAD <x>
            MAR = IR.addr;
            MDR = DM[MAR];
            A = MDR;
            break;

        case 2: // ADD <x>
        case 4: // SUB <x>
            if (IR.op == 4) neg = -1;
            MAR = IR.addr;
            MDR = DM[MAR];
            A += (DM[IR.addr]) * neg; 
            neg = 1;
            break;

        case 3: // STORE <x>
            MAR = IR.addr;
            MDR = A;
            DM[MAR] = MDR;
            break;

        case 5: // IN
            printf("INPUT: ");
            scanf("%d", &A);
            break;

        case 6: // OUT
            printf("%d\n", A);
            break;

        case 7: // HALT
            return 0;

        case 8: // JUMP <x>
            PC = IR.addr;
            break;

        // SKIP INSTRUCTIONS
        case 9:  // SKIPZ: skip next if A == 0
            if (A == 0) PC += 2;
            break;
        case 10: // SKIPG: skip next if A > 0
            if (A > 0) PC += 2;
            break;
        case 11: // SKIPL: skip next if A < 0
            if (A < 0) PC += 2;
            break;

        default:
            // Unknown opcode; just ignore or break
            break;
        }
        // Print current state of tinyvm
        printf("PC = %d | A = %d | DM = [", PC, A);
        for (int i = 0; i < 10; i++)
            printf("%d%s", DM[i], (i < 9) ? ", " : "]\n");

    }
    return 0;
}
