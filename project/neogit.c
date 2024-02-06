#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int isDirectory(const char *path)
{
    struct stat pathStat;

    if (stat(path, &pathStat) != 0)
    {
        // Error in stat
        fprintf(stderr, "%s\n", "Error in stat");
        return -1; // Indicate error
    }

    if (S_ISDIR(pathStat.st_mode))
    {
        // It's a directory
        return 1;
    }
    else
    {
        // It's not a directory
        return 0;
    }
}
int run_init(int argc, char *const argv[])
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return 1;

    char tmp_cwd[1024];
    bool exists = false;
    struct dirent *entry;
    do
    {
        // find .neogit
        DIR *dir = opendir(".");
        if (dir == NULL)
        {
            fprintf(stderr, "%s\n", "Error opening current directory");
            return 1;
        }
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".neogit") == 0)
                exists = true;
        }
        closedir(dir);

        // update current working directory
        if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL)
            return 1;

        // change cwd to parent  2829
        if (strcmp(tmp_cwd, "/") != 0)
        {
            if (chdir("..") != 0)
                return 1;
        }

    } while (strcmp(tmp_cwd, "/") != 0);
    // return to the initial cwd
    if (chdir(cwd) != 0)
        return 1;

    if (!exists)
    {
        if (mkdir(".neogit", 0755) != 0)
            return 1;
        if (mkdir(".neogit/configs", 0755) != 0)
            return 1;
        if (mkdir(".neogit/staging_area", 0755) != 0)
            return 1;
        if (mkdir(".neogit/commits", 0755) != 0)
            return 1;
    }
    else
    {
        fprintf(stderr, "%s\n", "neogit repository has already initialized");
    }
    return 0;
}
int create_local_configs_user(char *username)
{
    bool exists = false;
    struct dirent *entry;
    DIR *dir = opendir(".");
    if (dir == NULL)
    {
        fprintf(stderr, "%s\n", "Error opening current directory");
        return 1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".neogit") == 0)
            exists = true;
    }
    closedir(dir);
    if (exists)
    {

        FILE *local_username = fopen(".neogit/configs/username", "w");

        if (local_username == NULL)
            return 1;

        fprintf(local_username, "username: %s\n", username);

        fclose(local_username);
        return 0;
    }
    else
    {
        fprintf(stderr, "neogit repository is not existed\n");
        return 1;
    }
}
int create_local_configs_email(char *email)
{
    bool exists = false;
    struct dirent *entry;
    DIR *dir = opendir(".");
    if (dir == NULL)
    {
        fprintf(stderr, "%s\n", "Error opening current directory");
        return 1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".neogit") == 0)
            exists = true;
    }
    closedir(dir);
    if (exists)
    {

        FILE *local_email = fopen(".neogit/configs/email", "w");

        if (local_email == NULL)
            return 1;

        fprintf(local_email, "email: %s\n", email);

        fclose(local_email);

        return 0;
    }
    else
    {
        fprintf(stderr, "neogit repository is not existed\n");
        return 1;
    }
}
int create_global_configs_user(char *username)
{
    FILE *global_user = fopen("/home/ava/global_configs/global-user", "w");

    if (global_user == NULL)
        return 1;

    fprintf(global_user, "username: %s\n", username);

    fclose(global_user);
}
int create_global_configs_email(char *email)
{
    FILE *global_email = fopen("/home/ava/global_configs/global-email", "w");

    if (global_email == NULL)
        return 1;

    fprintf(global_email, "email: %s\n", email);

    fclose(global_email);
}
int add(char file_name[])
{
    bool exists = false;
    struct dirent *entry;
    DIR *dir = opendir(".");
    if (dir == NULL)
    {
        fprintf(stderr, "%s\n", "Error opening current directory");
        return 1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".neogit") == 0)
            exists = true;
    }
    closedir(dir);
    if (exists)
    {
        FILE *local_username = fopen(".neogit/configs/username", "r");
        FILE *local_email = fopen(".neogit/configs/email", "r");
        if (local_email == NULL && local_username == NULL)
        {
            fclose(local_email);
            fclose(local_username);
            FILE *global_user = fopen("/home/ava/global_configs/global-user", "r");
            FILE *global_email = fopen("/home/ava/global_configs/global-email", "r");
            if (global_email == NULL && global_user == NULL)
            {
                fprintf(stderr, "please enter your username and email first!\n");
                return 1;
            }
            else
            {
                FILE *local_username = fopen(".neogit/configs/username", "w");
                FILE *local_email = fopen(".neogit/configs/email", "w");
                char read_e[1000];
                char read_u[1000];
                while (fgets(read_e, sizeof(read_e), global_email) != NULL)
                {
                    fputs(read_e, local_email);
                }
                while (fgets(read_u, sizeof(read_e), global_user) != NULL)
                {
                    fputs(read_u, local_username);
                }
                fclose(local_username);
                fclose(local_email);
            }
        }
        else
        {
            int flag;
            flag = isDirectory(file_name);
            if (flag == 1)
            {
                DIR *dir = opendir(file_name);
                char *path = ".neogit/staging_area";
                char literal_dirname[100];
                int start = 0;
                for (int i = strlen(file_name) - 1; i >= 0; i--)
                {
                    if (file_name[i] == '/')
                    {
                        start = i + 1;
                        break;
                    }
                }
                for (int i = start; i < strlen(file_name); i++)
                {
                    literal_dirname[i - start] = file_name[i];
                }
                literal_dirname[strlen(file_name) - start] = '\0';
                struct dirent *entry;
                while ((entry = readdir(dir)) != NULL)
                {
                    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    {
                        continue;
                    }
                    char source_path[1024];
                    char destination_path[1024];
                    snprintf(source_path, sizeof(source_path), "%s/%s", file_name, entry->d_name);
                    snprintf(destination_path, sizeof(destination_path), "%s/%s", path, entry->d_name);

                    if (rename(source_path, destination_path) != 0)
                    {
                        fprintf(stderr, "%s\n", "Error moving file");
                        closedir(dir);

                        return 1;
                    }
                    else if (rename(source_path, destination_path) == 0)
                    {
                        return 0;
                    }
                }
                closedir(dir);
            }
            else if (flag == 0)
            {
                FILE *FILE_NAME = fopen(file_name, "r");
                char *path = ".neogit/staging_area";
                char literal_filename[100];
                int start = 0;
                for (int i = strlen(file_name) - 1; i >= 0; i--)
                {
                    if (file_name[i] == '/')
                    {
                        start = i + 1;
                        break;
                    }
                }
                for (int i = start; i < strlen(file_name); i++)
                {
                    literal_filename[i - start] = file_name[i];
                }
                literal_filename[strlen(file_name) - start] = '\0';
                char buffer[1000];
                sprintf(buffer, ".neogit/staging_area/%s", literal_filename);
                FILE *staging_area_name = fopen(buffer, "w");
                char temp[1000];
                while (fgets(temp, sizeof(temp), FILE_NAME) != NULL)
                {
                    fputs(temp, staging_area_name);
                }
                fclose(FILE_NAME);
                fclose(staging_area_name);
                fclose(local_username);
                fclose(local_email);
            }
        }
    }
    else
    {
        fprintf(stderr, "neogit repository is not existed\n");
        return 1;
    }
}

int reset(char *filename)
{
    bool exists = false;
    struct dirent *entry;
    DIR *dir = opendir(".");
    if (dir == NULL)
    {
        fprintf(stderr, "%s\n", "Error opening current directory");
        return 1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".neogit") == 0)
            exists = true;
    }
    closedir(dir);
    if (exists)
    {
        FILE *local_username = fopen(".neogit/configs/username", "r");
        FILE *local_email = fopen(".neogit/configs/email", "r");
        if (local_email == NULL && local_username == NULL)
        {
            fclose(local_email);
            fclose(local_username);
            FILE *global_user = fopen("/home/ava/global_configs/global-user", "r");
            FILE *global_email = fopen("/home/ava/global_configs/global-email", "r");
            if (global_email == NULL && global_user == NULL)
            {
                fprintf(stderr, "please enter your username and email first!\n");
                return 1;
            }
            else
            {
                FILE *local_username = fopen(".neogit/configs/username", "w");
                FILE *local_email = fopen(".neogit/configs/email", "w");
                char read_e[1000];
                char read_u[1000];
                while (fgets(read_e, sizeof(read_e), global_email) != NULL)
                {
                    fputs(read_e, local_email);
                }
                while (fgets(read_u, sizeof(read_e), global_user) != NULL)
                {
                    fputs(read_u, local_username);
                }
                fclose(local_username);
                fclose(local_email);
            }
        }
        else
        {
            DIR *dir = opendir(".neogit/staging_area");
            while ((entry = readdir(dir)) != NULL)
            {
                if (strcmp(filename, entry->d_name) == 0)
                {
                    char path[1024];
                    snprintf(path, sizeof(path), "%s/%s", ".neogit/staging_area", entry->d_name);
                    if (remove(path) == 0)
                    {
                        return 0;
                    }
                    else
                    {
                        printf("Error removing file: %s\n", entry->d_name);
                        return 1;
                    }
                }
            }
            closedir(dir);
        }
    }
}
int commit(int argc, int argv[])
{
    bool exists = false;
    struct dirent *entry;
    DIR *dir = opendir(".");
    if (dir == NULL)
    {
        fprintf(stderr, "%s\n", "Error opening current directory");
        return 1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".neogit") == 0)
            exists = true;
    }
    closedir(dir);
    if (exists)
    {
        FILE *local_username = fopen(".neogit/configs/username", "r");
        FILE *local_email = fopen(".neogit/configs/email", "r");
        if (local_email == NULL && local_username == NULL)
        {
            fclose(local_email);
            fclose(local_username);
            FILE *global_user = fopen("/home/ava/global_configs/global-user", "r");
            FILE *global_email = fopen("/home/ava/global_configs/global-email", "r");
            if (global_email == NULL && global_user == NULL)
            {
                fprintf(stderr, "please enter your username and email first!\n");
                return 1;
            }
            else
            {
                FILE *local_username = fopen(".neogit/configs/username", "w");
                FILE *local_email = fopen(".neogit/configs/email", "w");
                char read_e[1000];
                char read_u[1000];
                while (fgets(read_e, sizeof(read_e), global_email) != NULL)
                {
                    fputs(read_e, local_email);
                }
                while (fgets(read_u, sizeof(read_e), global_user) != NULL)
                {
                    fputs(read_u, local_username);
                }
                fclose(local_username);
                fclose(local_email);
            }
        }
        else
        {
            if (argc > 4)
            {
                fprintf(stderr, "please put your comment between a double quotation\n");
                return 1;
            }
            if (strlen(argv[3]) > 72)
            {
                fprintf(stderr, "your comment should not be more than 72 characters\n");
                return 1;
            }
            if (argc < 4)
            {
                fprintf(stderr, "please enter a comment for your commit\n");
                return 1;
            }
            FILE *counter;
            counter = fopen(".neogit/counter", "r");
            int number;
            char temp[1];
            if (counter == NULL)
            {
                fclose(counter);
                counter = fopen(".neogit/counter", "w");
                number = 1;
                fprintf(counter, "%d", number);
                fclose(counter);
            }
            else
            {
                fgets(temp, 1, counter);
                temp[0] += 1;
            }
            DIR *dir = opendir(".neogit/staging_area");
            if (dir == NULL)
            {
                fprintf(stderr, "%s\n", "Error opening current directory");
                return 1;
            }
            while ((entry = readdir(dir)) != NULL)
            {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                {
                    continue;
                }
                if (entry->d_type == DT_REG)
                {
                    char *stage_path = ".neogit/staging_area";
                    char *commit_path = ".neogit/commit";
                    char commit_num[1024];
                    snprintf(commit_num, sizeof(commit_num), "%s%s", ".neogit/commit/commit", temp[0]);
                    mkdir(commit_num , "0755");
                    char source_path[1024];
                    char destination_path[1024];
                    snprintf(source_path, sizeof(source_path), "%s/%s", stage_path, entry->d_name);
                    snprintf(destination_path, sizeof(destination_path), "%s/%s", commit_num, entry->d_name);
                    if (rename(source_path, destination_path) != 0)
                    {
                        fprintf(stderr, "%s\n", "Error moving file");
                        closedir(dir);

                        return 1;
                    }
                    else if (rename(source_path, destination_path) == 0)
                    {
                        return 0;
                    }
                }
            }
            closedir(dir);
            counter = fopen(".neogit/counter", "w");
            fprintf(counter, "%d", temp[0]);
            fclose(counter);
        }
    }
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
    }
    else if (strcmp(argv[1], "add") == 0)
    {
        for (int i = 2; i < argc; i++)
        {
            return add(argv[i]);
        }
        if (strcmp(argv[2], "-f"))
        {
            for (int i = 3; i < argc; i++)
            {
                return add(argv[i]);
            }
        }
    }
    else if (strcmp(argv[1], "reset") == 0)
    {
        if (strcmp(argv[2], "-undo"))
        {
            return reset(argv);
        }
        else
        {
            return reset(argv[2]);
        }
    }
    else if (strcmp(argv[1], "commit") == 0)
    {
        return commit(argc, argv);
        // } else if (strcmp(argv[1], "checkout") == 0) {
        //     return run_checkout(argc, argv);
    }
    else if (strcmp(argv[2], "user.name") == 0)
    {
        return create_local_configs_user(argv[3]);
    }
    else if (strcmp(argv[2], "user.email") == 0)
    {
        return create_local_configs_email(argv[3]);
    }
    else if (strcmp(argv[3], "user.name") == 0 && strcmp(argv[2], "--global") == 0)
    {
        return create_global_configs_user(argv[4]);
    }
    else if (strcmp(argv[3], "user.email") == 0 && strcmp(argv[2], "--global") == 0)
    {
        return create_global_configs_email(argv[4]);
    }
    return 0;
}