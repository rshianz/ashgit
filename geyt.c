#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
int check(){
    char pwd[1001]; // print working directory
    char current[1001]; //current diorectory
    if(getcwd(pwd, sizeof(pwd)) == NULL) return 1; //current working directory
    int exists = 0; 
    int count = 0;
    struct dirent *entry;
    do{
        count++;
        DIR *dir = opendir("."); //open current directory (.)
        if(dir == NULL){
            perror("error opening the current dir..!");
            return 1;
        }
        while((entry = readdir(dir)) != NULL){ //it has something so..
            if(entry->d_type == 4 && strcmp(entry->d_name, ".ashgit") == 0){//is the entry of the current directory is a directory too?
                                                                                            //comparing the entry name with the ashgit.
                exists = count;
            }
        }
        closedir(dir);
        if((getcwd(current, sizeof(current))) == NULL) return 1;
        if(strcmp("/", current))  
            if((chdir("..")) != 0) return 1;  //change directory to parent (..)         
    }while((strcmp("/", current)));
    chdir(pwd);
    return exists;
}
int init(){
    if(check() != 0){
        printf("already initialized before!\n");
    }
    else{
        char cwd[1001];
        getcwd(cwd, sizeof(cwd));
        char s[1001];
        chdir(cwd);

        mkdir(".ashgit", 0777);
        chdir(".ashgit");
        system("touch name.txt");
        system("touch email.txt");
        mkdir("commits", 0777);
        FILE *commitF = fopen("last_commit.txt", "w");
        fprintf(commitF, "0");
        fclose(commitF);
        FILE *branchF = fopen("branches.txt", "w");
        fprintf(branchF, "master");
        FILE *current_branchF = fopen("current_branch.txt", "w");
        fprintf(current_branchF, "master");
        fclose(branchF);
        fclose(current_branchF);
        mkdir("stage", 0777);
        mkdir("undo", 0777);
        mkdir("redo", 0777);
        chdir("undo");
        FILE *undoF = fopen("UndoNumber.txt", "w"); //make this in undo directory
        fprintf(undoF, "0");
        fclose(undoF);
        chdir("/Users/arshianazari/ashgit");
        FILE *addresses = fopen("address.txt", "a");
        fprintf(addresses, "%s/.ashgit\n", cwd);

        printf("initialized successfully\n");
    }
}
void where(char *address){
    system("touch pwd.txt");
    system("pwd > pwd.txt"); //printf working directory in pwd.txt
    FILE *pwd = fopen("pwd.txt", "r");
    fgets(address, 1001, pwd);
    address[strlen(address) - 1] = '\0';//we have the address in char *address by reading from the pwd.txt
}
void global_user(char* type, char* nameORemail){
    char curr_address[1001];
    getcwd(curr_address, sizeof(curr_address));
    where(curr_address);
    chdir("/Users/arshianazari/ashgit");
    if(!strcmp(type, "user.name")){
        FILE *fileN = fopen("name.txt", "w");
        fprintf(fileN, "%s", nameORemail); //here is name.
        fclose(fileN);
        char path[101];
        FILE *address = fopen("address.txt", "r");
        while(fgets(path, 101, address)){  //char *path is read the address from the address.txt 
            path[strlen(path) - 1] = '\0';
            char command[101] = "cp name.txt";
            strcat(command, path); // cp name.txt path(address)
            system(command); //command is -> cp name.txt path(address)
        }
    }
    else if(!strcmp(type, "user.email")){ //type is "user.email" ?
        FILE *fileE = fopen("email.txt", "w");
        fprintf(fileE, "%s", nameORemail); //here is email
        fclose(fileE);
        char path[101];
        FILE *address = fopen("address.txt", "r");
        while(fgets(path, 101, address)){  //char *path is read the address from the address.txt 
            path[strlen(path) - 1] = '\0';
            char command[101] = "cp name.txt";
            strcat(command, path); // cp email.txt path(address)
            system(command); //command is -> cp email.txt path(address)
        }
    }
}
void local_user(char* type, char* nameORemail){
    if(!strcmp(type, "user.name")){
        chdir(".ashgit");
        FILE *Fname = fopen("name.txt", "w");
        fprintf(Fname, "%s", nameORemail); //here is name
    }
    else if(!strcmp(type, "user.email")){
        chdir(".ashgit");
        FILE *Femail = fopen("email.txt", "w");
        fprintf(Femail, "%s", nameORemail);// here is email
    }
}
void MakeFileName(char *directory, char *name){
    for(int i = strlen(directory) - 1; i >= 0; i--){
        if(directory[i] == '/'){
            for(int j = 0; j < i; j++){
                name[j] = directory[i + j + 1];
            }
            directory[i] = '\0';
            return;
        }
    }
    strcpy(name, directory);
    getcwd(directory, 1001);

    // printf("%s", directory);
    // printf("\n%s", name);
}
int stage(char *line, char* name){
    char pwd[1001];
    char cmd[1001];
    char ashgitFolder[1001];
    getcwd(pwd, 1001);
    if(chdir(line) != 0){
        printf("file or address is not available\n");
        return 1;
    }
    DIR *dir = opendir(".");
    struct dirent *entry;
    if(dir == NULL){
        perror("Error opening current directory\n");
        return 1;
    }
    int flag = 1;
    while((entry = readdir(dir)) != NULL){
        if(strcmp(entry->d_name, name) == 0){
            if(entry->d_type == 4)
                strcpy(cmd, "cp -r ");
            else    
                strcpy(cmd, "cp ");
            flag = 0;
            break;
        }
    }
    if(flag){
        printf("file or addressa is not available\n");
        return 0;
    }
    int x = check();
    for(int i = 1; i < x; i++){ 
        if(chdir("..") != 0)
            printf("Errror\n");
    }
        chdir(".ashgit");
        chdir("stage");
        getcwd(ashgitFolder, 1001);
        strcat(cmd, line);
        strcat(cmd, "/");
        strcat(cmd, name);
        strcat(cmd, " ");
        strcat(cmd, ashgitFolder);
        chdir("../undo");
        int undo;
        strcat(ashgitFolder, "/");
        strcat(ashgitFolder, name);
        char FileName[1001];
        char No[1001];
        FILE *UndoNumber = fopen("UndoNumber.txt", "r");
        fscanf(UndoNumber, "%d", &undo);
        fclose(UndoNumber);
        undo++;
        sprintf(FileName, "undo%d.txt", undo);
        FILE *file = fopen(FileName, "w");
        fprintf(file, "%s", ashgitFolder);
        fclose(file);
        FILE *f = fopen("UndoNumber.txt", "w");
        fprintf(f, "%d", undo);
        fclose(f);
        char str[1001];
        strcpy(str, cmd);
        system(str);
        chdir(pwd);
        
    
}

void redo() {
    char cmd[1000] = "ditto ";
    char pwd[1000];
    
    if(getcwd(pwd, sizeof(pwd)) == NULL){
        perror("error");
        return;
    }
    chdir(pwd);
    strcat(cmd, pwd);
    strcat(cmd, "/.ashgit/undo/redo/"); 
    strcat(cmd, " ");
    strcat(cmd, pwd);
    strcat(cmd, "/.ashgit/stage");
    printf(" command > %s", cmd);
    system(cmd);
}

void reset(char *name){
    DIR *dir = opendir(".");
    char cmd[1001];
    char pwd[1001];
    char pwd1[1001];
    int is_found = 0;
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
        if(!strcmp(entry->d_name, name)){
            strcpy(cmd, "rm ");
            if(entry->d_type == 4){
                strcat(cmd, "-r ");
            }
            getcwd(pwd, 1001);
            is_found = 1;
            break;
        }
        else if(entry->d_type == 4 && strcmp(entry->d_name, ".") &&  strcmp(entry->d_name, "..")){
            chdir(entry->d_name);
            reset(name);
            chdir("..");
        }
    }
    closedir(dir);
    if(is_found){
        strcat(cmd, pwd);
        strcat(cmd, "/");
        strcat(cmd, name);
        system(cmd);
    }
}


void run_undo() {
        chdir("../undo");
    FILE *file = fopen("UndoNumber.txt", "r");
    int undo;
    int flag = 0;
    fscanf(file, "%d", &undo);
    fclose(file);
    char fileName[1001], line[1001], cmd[1001], cmdCp[1001], name[1001], pwd[1001], cmdtmp[1001];
    sprintf(fileName, "undo%d.txt", undo);
    FILE *file2 = fopen(fileName, "r");
    if (file2 == NULL) {
        FILE *rechange = fopen("UndoNumber.txt", "w");
        fprintf(rechange, "%d", undo - 1);
        fclose(rechange);
        if (undo) {
            run_undo();
        } else {
            return;
        }
    }
    fgets(line, 1001, file2);
    fclose(file2);
    MakeFileName(line, name);
    getcwd(pwd, 1001);
    chdir(line);
    DIR *dir = opendir(".");
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, name)) {
            if (entry->d_type == 4) {
                strcpy(cmd, "rm -r ");
                strcpy(cmdCp, "cp -r ");
            } else if (entry->d_type == 8) {
                strcpy(cmd, "rm ");
                strcpy(cmdCp, "cp ");
            }
            flag = 1;
        }
    }
    strcpy(cmdtmp, line);
    strcat(cmdtmp, "/../undo/redo");    
    strcat(line, "/");
    strcat(line, name);
    closedir(dir);
    strcat(cmd, line);
    strcat(cmdCp, line); 
    strcat(cmdCp, " ");
    strcat(cmdCp, cmdtmp);
    if (flag) {
        system(cmdCp);
        system(cmd);
        undo--;
        chdir("../undo");
        FILE *file3 = fopen("UndoNumber.txt", "w");
        fprintf(file3, "%d", undo);
        fclose(file3);
    } else {
        FILE *rechange = fopen("UndoNumber.txt", "w");
        fprintf(rechange, "%d", undo - 1);
        fclose(rechange);
        if (undo) {
            run_undo();
        } else {
            return;
        }
    }
}

void comeback(){
    int comeback = check();
    for(int i = comeback; i > 1; i--){
        chdir("..");
    }
}


int main(int argc, char *argv[]){
    char name[1001];
    char pwd[1001];
    char line[1001];
    if(argc < 2)
        printf("invalid command\n");
    if(strcmp(argv[1], "init") == 0){
        return init();
    }
    else if(!strcmp(argv[1], "config")){
        if(!strcmp(argv[2], "user.name"))
            local_user(argv[2] , argv[3]);
        else if(!strcmp(argv[2], "user.email"))
            local_user(argv[2] , argv[3]);
        else if(!strcmp(argv[2], "-global")){
            if(!strcmp(argv[3], "user.name"))
                global_user(argv[2] ,argv[4]);
            else if(!strcmp(argv[3], "user.email"))
                global_user(argv[2], argv[4]);
        }
    }
    else if(!strcmp(argv[1], "add")){
        if(check() == 0){
            printf("not initialized!\n");
            return 1;
        }
        
        if(!strcmp(argv[2], "-f")){
            for(int i = 3; i < argc; i++){
                getcwd(pwd, 1001);
                strcpy(line , argv[i]);
                MakeFileName(line, name);
                stage(line, name);
                line[0] = '\0';
                name[0] = '\0';
                chdir(pwd);
            }
        }
        else if(!strcmp(argv[2], "-redo")){
            comeback();
            redo();
        }
        else{
            MakeFileName(argv[2], name);
            stage(argv[2], name);
        }
        
    }
    else if(!strcmp(argv[1], "reset")){
        if(check == 0){
            printf("not initialized yet!\n");
            return 1;
        }
        getcwd(pwd, 1001);

        comeback();
        chdir(".ashgit/stage");
        if(!strcmp(argv[2], "-f")){
            for(int i = 1 ; i < argc; i++){
                strcpy(line , argv[i]);
                MakeFileName(line, name);
                reset(name);
            }
        }
        else if(!strcmp(argv[2], "-undo")){
            run_undo();
        }
        else{
            MakeFileName(argv[2], name);
            reset(name);
        }
        chdir(pwd);
    }
    else if(!strcmp(argv[1], "commit")){
        if(check() == 0){
            printf("not initialized yet!\n");
            return 1;
        }
        if(argc > 4){
            printf("put your massage on the double quotation!\n");
        }
        if(strlen(argv[3] > 72)){
            printf("your massage is too long, make it short\n");
        }
        if(!strcmp(argv[2], "-m")){
            //give argv[3] as a massage and ..
            //do commit
        }else{
            printf("its not correct command for commit do it right");
        }
    }
    // else if(!strcmp(argv[1], "test")){
       
    // }
}