/*
    SUIDWGEN - set uid wrapper generator
    Copyright (C) 2018  Mario Aichinger <aichingm@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


/*
#define SCRIPT "suidwgen.php"
#define INTERPRETER "/usr/bin/php"
#define CHECK_SCRIPT_NOT_WRITABLE 1
#define CHECK_SCRIPT_NOT_READABLE 1
#define CHECK_SCRIPT_ONLY_USR_EXEC 1
#define CHECK_SAME_OWN 1
#define CHECK_SAME_GRP 1
#define CHECK_NOT_ROOT_U 1
#define CHECK_NOT_ROOT_G 1
#define CHECK_SCRIPT_SUM "715f5d5a2faa41a941b6c4e01a72b238a53f7acf"
#define MAX_ARGS 6
#define UNSET_ENV 1;
#define DEBUG 1
#define ENABLE_HELP 1
 */

#ifdef CHECK_SCRIPT_SUM 
#include <openssl/sha.h>
#endif

#define NEW_ARGC argc + 1



#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>


#ifdef CHECK_SCRIPT_SUM 
int shaf(FILE *f, unsigned char *sum, int buf_size);
int sha_sum_hex(unsigned char *sum, char *hex_hash);
#endif


int main(int argc, char * argv[], char ** envp) {

#ifdef ENABLE_HELP
    if(strncmp(argv[1], "--help", 6) == 0){
        printf(
        "SUIDWGEN - set uid wrapper generator\n"
        "Copyright (C) 2018  Mario Aichinger <aichingm@gmail.com>\n"
        "\n"
        "This program is free software: you can redistribute it and/or modify\n"
        "it under the terms of the GNU General Public License as published by\n"
        "the Free Software Foundation, either version 3 of the License, or\n"
        "(at your option) any later version.\n"
        "\n"
        "This program is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        "GNU General Public License for more details.\n"
        "\n"
        "You should have received a copy of the GNU General Public License\n"
        "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n"
        );
        return 0;
    }
#endif

#ifdef DEBUG
    printf("Starting checks\n");
#endif 

#ifdef MAX_ARGS
    if (argc - 1 > MAX_ARGS && MAX_ARGS != -1) {
        return 1;
    }
#endif
#ifdef CHECK_SCRIPT_NOT_WRITABLE
#ifdef SCRIPT
    struct stat fileStat_w;
    if (stat(SCRIPT, &fileStat_w) < 0) {
        return 6;
    }
    if (fileStat_w.st_mode & S_IWOTH || fileStat_w.st_mode & S_IWGRP) {
        return 6;
    }
#endif
#endif

#ifdef CHECK_SCRIPT_NOT_READABLE
#ifdef SCRIPT
    struct stat fileStat_r;
    if (stat(SCRIPT, &fileStat_r) < 0) {
        return 7;
    }
    if (fileStat_r.st_mode & S_IROTH || fileStat_r.st_mode & S_IRGRP) {
        return 7;
    }
#endif
#endif
#ifdef CHECK_SCRIPT_ONLY_USR_EXEC
#ifdef SCRIPT
    struct stat fileStat_x;
    if (stat(SCRIPT, &fileStat_x) < 0) {
        return 8;
    }
    if (fileStat_x.st_mode & S_IXOTH || fileStat_x.st_mode & S_IXGRP || (fileStat_x.st_mode & S_IXUSR) == 0) {
        return 8;
    }
#endif
#endif
#ifdef CHECK_SAME_OWN
#ifdef SCRIPT
    struct stat fileStat_u;
    struct stat fileStat_su;
    if (stat(SCRIPT, &fileStat_u) < 0 || stat(argv[0], &fileStat_su) < 0) {
        return 9;
    }
    if (fileStat_u.st_uid != fileStat_su.st_uid) {
        return 9;
    }
#endif
#endif
#ifdef CHECK_SAME_GRP
#ifdef SCRIPT
    struct stat fileStat_g;
    struct stat fileStat_sg;
    if (stat(SCRIPT, &fileStat_g) < 0 || stat(argv[0], &fileStat_sg) < 0) {
        return 10;
    }
    if (fileStat_g.st_gid != fileStat_sg.st_gid) {
        return 10;
    }
#endif
#endif
#ifdef CHECK_NOT_ROOT_U
    if (geteuid() == 0) {
        return 11;
    }
#endif
#ifdef CHECK_NOT_ROOT_G
    if (getegid() == 0) {
        return 12;
    }
#endif
#ifdef DEBUG
    printf("Basic checks done\n");
#endif  
    if (setuid(geteuid())) {
        return 4;
    }

    if (setgid(getegid())) {
        return 5;
    }

#ifdef DEBUG 
    printf("Real user is: %d\n", getuid());
    printf("Effective user id: %d\n", geteuid());
    printf("Checking hash\n");
#endif 
#ifdef CHECK_SCRIPT_SUM
    FILE *f = fopen(SCRIPT, "rb");
    if (f == 0) {
        return 15;
    }
    unsigned char *sum = malloc(sizeof (unsigned char)*200);

    if (shaf(f, sum, 4096) != 0) {
        return 13;
    }
    fclose(f);
    char hex[20];
    sha_sum_hex(sum, hex);
#ifdef DEBUG 
    printf("This checksum: %s\n", CHECK_SCRIPT_SUM);
    printf("Should equal this checksum: %s\n", hex);
#endif
    if (strncmp(CHECK_SCRIPT_SUM, hex, strlen(CHECK_SCRIPT_SUM)) != 0) {
        return 14;
    }
    free(sum);
#endif

#ifdef DEBUG
    printf("All checks done\n");
#endif 


#ifdef UNSET_ENV
    envp = 0; // blocks IFS attack on non-bash shells 
#endif

#ifdef DEBUG
    printf("Debug argv\n");
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
#endif

    char *new_argv[NEW_ARGC + 1];

    for (int i = 0; i < NEW_ARGC - 1; i++) {
        new_argv[i] = NULL;
    }
    new_argv[0] = INTERPRETER;
    new_argv[1] = SCRIPT;
    for (int i = 1; i < argc; i++) {
        new_argv[i + 1] = argv[i];
    }
    new_argv[NEW_ARGC] = NULL;

#ifdef DEBUG
    printf("Debug new_argv\n");
    for (int i = 0; i < NEW_ARGC; i++) {
        printf("%s\n", new_argv[i]);
    }
#endif

    execve(INTERPRETER, new_argv, envp);

#ifdef DEBUG
    printf("Exit abnormal\n");
#endif
    return errno;

}

#ifdef CHECK_SCRIPT_SUM 
int shaf(FILE *f, unsigned char *sum, int buf_size) {
    unsigned char buffer[buf_size];
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    size_t len = 0;
    for (;;) {
        len = fread(buffer, 1, sizeof buffer, f);
        if (len == 0) {
            break;
        }
        SHA1_Update(&ctx, buffer, len);
    }
    if (ferror(f)) {
        return 1;
    }
    SHA1_Final(sum, &ctx);
    return 0;
}

int sha_sum_hex(unsigned char *sum, char *hex_hash) {
    hex_hash[0] = 0;
    for (int i = 0; i < 20; i++) {
        unsigned char str[2] = "\0\0";
        sprintf(str, "%02x", sum[i]);
        strcat(hex_hash, str);
    }
    return 1;
}
#endif
