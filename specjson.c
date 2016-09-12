#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#define BUF_SIZE 1024

char* get_kernel_type() {
    FILE* fp;
    char* path = malloc(BUF_SIZE);
    fp = popen("uname","r");
    if(fp == NULL) {
        printf("Something broke.\n");
        exit(1);
    }
    fgets(path, BUF_SIZE, fp);
    fclose(fp);
    return path;
}

void add_tab(FILE* ofp, int size) {
    for(int i = 0; i < size; i++) {
        fprintf(ofp, " ");
    }
}

void new_json_object(FILE * ofp, void (* op)()) {
    add_tab(ofp, 4);
    fprintf(ofp, "{\n");
    op(ofp);
    add_tab(ofp, 4);
    fprintf(ofp, "}\n");
}

void get_cpu_info(FILE* ofp) {
    char* type = get_kernel_type();
    char* path = malloc(BUF_SIZE);
    memset(path, 0, BUF_SIZE);
    char* path1 = malloc(BUF_SIZE);
    memset(path1, 0, BUF_SIZE);
    char* path2 = malloc(BUF_SIZE);
    memset(path2, 0, BUF_SIZE);
    if(strcmp(type, "Darwin\n") == 0) {
        /*sysctlbyname*/
        FILE* ifp =  popen("sysctl -n machdep.cpu.brand_string", "r"); //use sysctl()
        FILE* ifp1 = popen("sysctl -n machdep.cpu.logical_per_package", "r");
        FILE* ifp2 = popen("sysctl -n machdep.cpu.cores_per_package", "r");
        if(!ifp || !ifp1 || !ifp2) {
          perror("popen");
          exit(1);
        }
        fgets(path, BUF_SIZE, ifp); //use sscanf()
        fgets(path1, BUF_SIZE, ifp1);
        fgets(path2, BUF_SIZE, ifp2);
        path[strlen(path)] = '\0';
        path1[strlen(path1)] = '\0';
        path2[strlen(path2)] = '\0';
        printf("%lu", strlen(path));
        add_tab(ofp, 8);
        fprintf(ofp, "\"CPU\": \"%s\";\n", CTL_HW.HW_MACHINE);
        add_tab(ofp, 8);
        fprintf(ofp, "\"Logical Cores\": \"%s\";\n", path1);
        add_tab(ofp, 8);
        fprintf(ofp, "\"Physical Cores\": \"%s\";\n", path2);
        //pclose(ifp);
        pclose(ifp1);
        pclose(ifp2);
        free(path);
        free(path1);
        free(path2);
    }
    free(type);
}

int main(int argc, char* argv[]) {
    char* output_file = "specs.json";
    FILE* ofp = fopen(output_file, "w");
    if(!ofp) {
        printf("Can't open %s", output_file);
        exit(1);
    }
    void* cpu = &get_cpu_info;
    fprintf(ofp, "{\n");
    new_json_object(ofp, cpu);
    fprintf(ofp, "}\n");
    fclose(ofp);
    return 0;
}
