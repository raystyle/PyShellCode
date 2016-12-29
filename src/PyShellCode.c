/*
 * Thomas Keck 2017
 */

#include "PyShellCode.h"


struct ExecutableCode* create_ExecutableCode_from_File(const char *filename) {

    struct stat sbuf;
    if(stat(filename, &sbuf)) {
		perror("Failed to call stat on provided file");
		return NULL;
	}

    void *raw_code = NULL;
    if(!(raw_code = malloc(sbuf.st_size))) {
		perror("Failed to malloc enough memory");
		return NULL;
	}

	FILE *fp;
    if(!(fp = fopen(filename, "rb"))) {
		perror("Failed to open provided file");
		return NULL;
	}

    if(fread(raw_code, 1, sbuf.st_size, fp) != (size_t)sbuf.st_size) {
		perror("Failed to read provided file");
		return NULL;
	}

    if(fclose(fp)) {
		perror("Failed to close provided file");
		return NULL;
	}

    return create_ExecutableCode_from_ShellCode((char*)raw_code, sbuf.st_size);
}


struct ExecutableCode* create_ExecutableCode_from_ShellCode(const char *shellcode, size_t length) {

    struct ExecutableCode *code = malloc(sizeof(struct ExecutableCode));
    code->length = length;
    code->addr = mmap(NULL, code->length, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

    if(code->addr == MAP_FAILED) {
        perror("Memory mapping failed");
        free(code);
        return NULL;
    }

    // We use memcpy hence \0 bytes in the shellcode won't be a problem.
    memcpy(code->addr, shellcode, code->length);
    return code;
}

void execute_ExecutableCode(struct ExecutableCode* code) {

    ((void (*)(void))code->addr)();

}

void destroy_ExecutableCode(struct ExecutableCode* code) {

    if(code == NULL)
        return;

    if(munmap(code->addr, code->length) == -1) {
        perror("Memory un-mapping failed");
    }
    
    free(code);
}

void print_ExecutableCode(struct ExecutableCode* code) {
	
    for(unsigned int i = 0; i < code->length; ++i) {
    	printf("\\x%02x", ((unsigned char *)code->addr)[i]);
    }
    printf("\n");

}

void PrintVersion() {
  printf("PyShellCode Version: %i.%i", PyShellCode_VERSION_MAJOR, PyShellCode_VERSION_MINOR);
}


int main(int argc, char *argv[]) {
    
    if(argc == 0) {
        printf("Did not receive any arguments");
        return 1;
    }

    if(argc == 1 || argc > 3) {
        printf("Usage %s filename\n", argv[0]);
        printf("Usage %s shellcode length\n", argv[0]);
        return 1;
    }

    struct ExecutableCode* code = NULL;

    if(argc == 2) {
        code = create_ExecutableCode_from_File(argv[1]);
    }
    
    if(argc == 3) {
        code = create_ExecutableCode_from_ShellCode(argv[1], atoi(argv[2]));
    }

    execute_ExecutableCode(code);
    destroy_ExecutableCode(code);

    return 0;
}