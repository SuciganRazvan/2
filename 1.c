#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>

void recursivedel(const char *directory, const char *flag, int max_size, FILE *deletedTxt) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(directory);
    if (!dir) {
        perror("Error opening directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char subdirectory[256]; // Adjust the buffer size as needed
                snprintf(subdirectory, sizeof(subdirectory), "%s/%s", directory, entry->d_name);
                processDirectory(subdirectory, flag, max_size, deletedTxt);
            }
        } else if (entry->d_type == DT_REG) {
            char filePath[256];  // Adjust the buffer size as needed
            snprintf(filePath, sizeof(filePath), "%s/%s", directory, entry->d_name);

            if (strcmp(flag, "eext") == 0) {
                char extension[] = ".ext";  // Replace with the desired extension
                char *ext = strrchr(entry->d_name, '.');
                if (ext && strcmp(ext, extension) == 0) {
                    remove(filePath);
                    fprintf(deletedTxt, "%s\n", entry->d_name);
                }
            } else if (flag[0] == 'l' && isdigit(flag[1])) {
                struct stat fileStat;
                if (stat(filePath, &fileStat) == 0 && fileStat.st_size <= max_size) {
                    remove(filePath);
                    fprintf(deletedTxt, "%s\n", entry->d_name);
                }
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf(": %s [flag] fisierul unde vrei sa faceti stergerea\n", argv[0]);
        return 1;
    }

    char *flag = argv[1];
    char *directory = argv[2];
    char deletedFile[] = "deleted.txt";

    FILE *deletedTxt = fopen(deletedFile, "w");
    if (!deletedTxt) {
        perror("eroare la deschidere");
        return 1;
    }

    recursivedel(directory, flag, atoi(flag + 1), deletedTxt);
    fclose(deletedTxt);

    return 0;
}