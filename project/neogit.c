#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int run_init(int argc, char * const argv[]) 
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;

    char tmp_cwd[1024];
    bool exists = false;
    struct dirent *entry;
    do {
        // find .neogit
        DIR *dir = opendir("."); 
        if (dir == NULL) {
            fprintf(stderr,"%s\n","Error opening current directory");
            return 1;
        }
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0)
                exists = true;
        }
        closedir(dir);

        // update current working directory
        if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL) return 1;

        // change cwd to parent  2829 
        if (strcmp(tmp_cwd, "/") != 0) {
            if (chdir("..") != 0) return 1;
        }

    } while (strcmp(tmp_cwd, "/") != 0);

    // return to the initial cwd
    if (chdir(cwd) != 0) return 1;
        
    if (!exists) 
    {
        if (mkdir(".neogit", 0755) != 0) return 1; 
        if (mkdir(".neogit/configs", 0755) != 0) return 1;
        if (mkdir(".neogit/staging_area", 0755) !=0) return 1;
        if (mkdir(".neogit/commit", 0755) !=0) return 1;
    
    } 
    else 
    {
        fprintf(stderr, "%s\n", "neogit repository has already initialized");
    }
    return 0;
}

int create_local_configs_user(char *username)
{
    if()
    FILE *local_user = fopen(".neogit/configs/username", "w");

    if (local_user == NULL) return 1;

    fprintf(local_user, "username: %s\n", username);

    fclose(local_user);

    return 0;
}
int create_local_configs_email(char *email)
{
    FILE *local_email = fopen(".neogit/configs/email", "w");

    if (local_email == NULL) return 1;

    fprintf(local_email, "username: %s\n",email);

    fclose(local_email);

    return 0;   
}
int create_global_configs_user(char *username)
{
    FILE *global_user = fopen("/home/ava/global_configs/global-user", "w");

    if (global_user == NULL) return 1;
  
    fprintf(global_user, "username: %s\n", username);
 
    fclose(global_user);
}
int create_global_configs_email(char *email)
{
    FILE *global_email = fopen("/home/ava/global_configs/global-email", "w");
    
    if (global_email == NULL) return 1;
    
    fprintf(global_email, "email: %s\n", email);
    
    fclose(global_email);
}

int main(int argc, char *argv[]) 
{
    
    if (argc < 2) 
    {
        fprintf(stdout, "please enter a valid command\n");
        return 1;
    }
    
    if (strcmp(argv[1], "init") == 0) 
    {
        return run_init(argc, argv);
    // } else if (strcmp(argv[1], "add") == 0) {
    //     return run_add(argc, argv);
    // } else if (strcmp(argv[1], "reset") == 0) {
    //     return run_reset(argc, argv);
    // } else if (strcmp(argv[1], "commit") == 0) {
    //     return run_commit(argc, argv);
    // } else if (strcmp(argv[1], "checkout") == 0) {
    //     return run_checkout(argc, argv);
    } else if (strcmp(argv[2], "user.name") == 0) {
        return create_local_configs_user(argv[3]);
    } else if (strcmp(argv[2], "user.email") == 0) {
        return create_local_configs_email(argv[3]);
    } else if (strcmp(argv[3], "user.name") == 0 && strcmp(argv[2], "--global") == 0) {
        return create_global_configs_user(argv[4]);
    } else if (strcmp(argv[3], "user.email") == 0 && strcmp(argv[2], "--global") == 0) {
        return create_global_configs_email(argv[4]);
    }
    return 0;
}





