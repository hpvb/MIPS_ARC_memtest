#include "arclib/stdio.h"
#include "arclib/string.h"

#include "arclib/stddef.h"
#include "arclib/arc.h"

#define ARRAY_LENGTH(array)     (sizeof(array)/sizeof(array[0]))

void fput_name(FILE *file, const char **table, int table_size, int index) {
        if (index < 0 || table_size <= index) {
                fprintf(file, "#%u", index);
                return;
        }
        fprintf(file, "%s", table[index]);
}

void bios_dump_memory(FILE *file) {
        unsigned int total = 0;            /* Total physical memory size */
        MEMORYDESCRIPTOR *descr = NULL;
        static const char *memory_type_name[] = {
                "ExeceptionBlock",
                "SystemParameterBlock",
                "FreeMemory",
                "BadMemory",
                "LoadedProgram",
                "FirmwareTemporary",
                "FirmwarePermanent",
                "FreeContigous"
        };

        while ((descr = ArcGetMemoryDescriptor(descr)) != NULL) {
                unsigned int
                        seg_start = (descr->BasePage << 12),
                        seg_size = (descr->PageCount << 12),
                        seg_end = (seg_start + seg_size);

                total += seg_size;
                fprintf(file, "memory 0x%x..%x, size: %u bytes, type: ", seg_start, seg_end, seg_size);
                fput_name(file, memory_type_name, ARRAY_LENGTH(memory_type_name), descr->Type);
                fprintf(file, "\r\n");
        }
        fprintf(file, "Total memory = %u bytes\r\n", total);
}

typedef struct {
        unsigned char* start;
        unsigned long size;
        void* end;
} free_region;

LONG arctest(LONG Argc, CHAR *Argv[], CHAR *Envp[]){
    unsigned int i, k, l;
    unsigned int nr = 0;
    free_region regions[10];
    unsigned char patterns[] = { 0x55, 0xAA, 0, 0xff };
    MEMORYDESCRIPTOR *descr = NULL;

    puts("\x1b[32mHello ARC world!!!\x1b[37m\r\n");
    printf("arctest() at: 0x%x\r\n", arctest);

    ArcFlushAllCaches();
    bios_dump_memory(stdout);
    
    puts("------\r\n");


    memset(regions, 0, sizeof(regions));
    while ((descr = ArcGetMemoryDescriptor(descr)) != NULL) {
        if (descr->Type == FreeMemory) {
             unsigned int
                 seg_start = (descr->BasePage << 12),
                 seg_size = ((descr->PageCount - 1) << 12),
                 seg_end = (seg_start + seg_size);

             regions[nr].start = (void*)(seg_start) + 0x80000000;
             regions[nr].size = seg_size;
             regions[nr].end = (void*)(seg_end) + 0x80000000;
             ++nr;
        }
    }

    while(1) {
    for (i = 0; i < sizeof(patterns); ++i) {
        for (k = 0; k < nr; ++k) {
            printf("Writing region 0x%x..%x, size: %u bytes, pattern: 0x%x\r\n", regions[k].start, regions[k].end, regions[k].size, patterns[i]);
            for (l = 0; l < regions[k].size - 1; ++l) {
                regions[k].start[l] = patterns[i];
            }
        }
        for (k = 0; k < nr; ++k) {
            printf("Reading region 0x%x..%x, size: %u bytes, pattern: 0x%x\r\n", regions[k].start, regions[k].end, regions[k].size, patterns[i]);
            for (l = 0; l < regions[k].size - 1; ++l) {
                if (regions[k].start[l] != patterns[i]) {
                    printf("Bad address: %u, pattern: 0x%x\r\n", ((unsigned int)regions[k].start) + l, patterns[i]);
                    getchar();
                }
            }
        }
    }
    printf("-------\r\n");
    }
    puts("All done!\r\n");
    ArcFlushAllCaches();
    getchar();

    return 0;
}

LONG main(LONG Argc, CHAR *Argv[], CHAR *Envp[]);
LONG __gccmain(LONG Argc, CHAR *Argv[], CHAR *Envp[]);

LONG main(LONG Argc, CHAR *Argv[], CHAR *Envp[]){
    puts("Entering main()\r\n");
    return arctest(Argc, Argv, Envp);
}

LONG __gccmain(LONG Argc, CHAR *Argv[], CHAR *Envp[]){
    puts("Entering __gccmain()\r\n");
    return arctest(Argc, Argv, Envp);
}

