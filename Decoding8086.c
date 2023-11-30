#include "Decoding8086.h"



/* Reg = 010 W = 1 -> reg_field_encoding[2][1] (i.e., "DX") */
const char* decode_register[8][2] = {
    {"al","ax"},
    {"cl","cx"},
    {"dl","dx"},
    {"bl","bx"},
    {"ah","sp"},
    {"ch","bp"},
    {"dh","si"},
    {"bh","di"}
};

const char* decode_opcode(byte instruction)
{
    byte opcode = instruction >> 2;

    switch (opcode)
    {

    case 0x22:
        
        return "mov";

        break;
    default:
        return "Unknown";
    }

}

register_field decode_register_flags(byte byte1, byte byte2)
{
    register_field r_fields;

    r_fields.d_flag = (byte1 >> 1) & 1;
    r_fields.w_flag = (byte1 & 1);
    r_fields.mod = (byte2 >> 6) & 0b11;
    r_fields.reg = (byte2 >> 3) & 0b111;
    r_fields.rm = byte2 & 0b111;

    return r_fields;

}
 

void write_output_file(char* opcode, char* r_dest, char* r_sour, char* output_file, char* asm_code)
{
    // Constructing the full path of the output file
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "outputfiles/%s%s", output_file,".asm");

    // Open the file for writing
    FILE* file = fopen(full_path, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Writing data to the file
    fprintf(file, ";=====================================================================\n");
    fprintf(file, "; %s disassembly:\n", output_file);  // Assuming you want to print the file name here
    fprintf(file, ";=====================================================================\n");
    fprintf(file, "\n");
    fprintf(file, "bits 16\n");
    fprintf(file, "\n");
    fprintf(file, "%s", asm_code);

    // Close the file
    fclose(file);
}



void run(P8086* processor)
{

    size_t memory_size = processor->program_size;

    char* r_dest = NULL;
    char* r_sour = NULL;
    char* opcode = NULL;
    char asm_code_src[1000] = {0};
    char asm_code_des[1000] = {0};
    // Initialize Instruction Pointer (IP) to the start of the memory
    processor->ip = 0;

    while (processor->ip < memory_size) // Ensure there are at least 6 bytes left to read
    {
        processor->pack_instruction.byte1 = processor->memory[processor->ip + 0];
        processor->pack_instruction.byte2 = processor->memory[processor->ip + 1];
        /*processor->pack_instruction.byte3 = processor->memory[processor->ip + 2];
        processor->pack_instruction.byte4 = processor->memory[processor->ip + 3];
        processor->pack_instruction.byte5 = processor->memory[processor->ip + 4];
        processor->pack_instruction.byte6 = processor->memory[processor->ip + 5];*/

        // Process the instruction here...
        // For example, decode and execute it.

        opcode = decode_opcode(processor->pack_instruction.byte1);
        processor->reg_field = decode_register_flags(processor->pack_instruction.byte1,
            processor->pack_instruction.byte2);
       

        byte destination = processor->reg_field.d_flag == 1 ? processor->reg_field.reg : processor->reg_field.rm;

        byte source = processor->reg_field.d_flag == 1 ? processor->reg_field.rm : processor->reg_field.reg;

        char* r_dest = decode_register[destination][processor->reg_field.w_flag];

        char* r_sour = decode_register[source][processor->reg_field.w_flag];


        printf("%s %s,%s\n", opcode, r_dest, r_sour);
        
        snprintf(asm_code_src, sizeof(asm_code_src), "%s %s, %s\n", opcode, r_dest, r_sour);

        strncat(asm_code_des, asm_code_src, sizeof(asm_code_src));
        
        processor->ip += 2;
    }

    
    write_output_file(opcode, r_dest, r_sour, proccessor->output_file, asm_code_des);

    // If you reach here and there are less than 6 bytes left, you can handle the trailing bytes.
    // This part depends on the specification of the instruction set and whether instructions can be less than 6 bytes.
}
 



void load_program(byte* program, size_t size, char* output_file)
{
    
    proccessor->program_size = size;
    proccessor->output_file = malloc(strlen(output_file) + 1);

    if (proccessor->output_file != NULL) {
        strcpy(proccessor->output_file, output_file);
    }

    for (size_t i = 0; i < size; i++)
    {
        proccessor->memory[i] = program[i];        
    }

    

}


void init()
{
    proccessor = (P8086*) malloc(sizeof(P8086));
    memset(proccessor->memory, 0, sizeof(proccessor->memory));
    proccessor->ip = 0;
}


void main(int argc, char** argv)
{


    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("Loading file: %s\n", argv[1]);

    FILE* file;
    if (FOPEN(file, argv[1], "rb") == NULL) {
        perror("Error opening file");
        return 1;
    }

    byte buffer[65536] = {0};

    size_t bytesRead = fread(buffer, sizeof(byte), sizeof(buffer), file);
    if (bytesRead == 0) {
        fputs("Error reading file", stderr);
        fclose(file);
        return EXIT_FAILURE;
    }
    
    init();

    load_program(&buffer, bytesRead, argv[1]);

    run(proccessor);

    free(proccessor->output_file);
    free(proccessor);

    
}

 