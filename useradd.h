#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <syslog.h>

#define MAX_USRNAME  8

#define DEFAULT_SHELL	"/bin/bash"
#define DEFAULT_HOME	"/home"
#define USERADD_PATH	"/usr/sbin/useradd"
#define CHAGE_PATH	"/usr/bin/chage"
#define PASSWD_PATH	"/usr/bin/passwd"
#define EDQUOTA_PATH	"/usr/sbin/edquota"
#define QUOTA_DEFAULT	"defuser"
#define DEFAULT_GROUP	100

#define DEFAULT_MIN_PASS 0
#define DEFAULT_MAX_PASS 100
#define DEFAULT_WARN_PASS 14
#define DEFAULT_USER_DIE 366

typedef struct usr_info_
{
    char usrname[32];
    char person[32];
    char dir[32];
    char shell[32];
    unsigned int uid;
    unsigned int group;
    unsigned int min_pass;
    unsigned int max_pass;
    unsigned int warn_pass;
    unsigned int user_die;
}usr_info_t;

void safeget (char *, int);
int validateUsername (char *);
int validateGrpid (char *);
void getUsrInfo (usr_info_t *);
