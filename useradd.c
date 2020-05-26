#include <stdio.h>
#include "useradd.h"

void 
safeget (char *buf, int maxlen)
{
    int c, i = 0, bad = 0;
    char *bstart = buf;
    while ((c = getc (stdin)) != EOF && (c != '\n') && (++i < maxlen))
    {
        bad = (!isalnum (c) && (c != '_') && (c != ' '));
        *(buf++) = (char) c;
    }
    *buf = '\0';

    if (bad)
    {
        printf ("\nString contained banned character. Please stick to alphanumerics.\n");
        *bstart = '\0';
    }
}

void
getUsrInfo (usr_info_t *usr_info) {

    char foo[32];

    if (getuid () == 0)
    {
        printf ("\nIf home dir ends with a / then '%s' will be appended to it\n", usr_info->usrname);
        printf ("Home Directory [%s/%s]: ", DEFAULT_HOME, usr_info->usrname);
        fflush (stdout);
        safeget (usr_info->dir, sizeof (usr_info->dir));
        if (!strlen (usr_info->dir))
        {
            sprintf (usr_info->dir, "%s/%s", DEFAULT_HOME, usr_info->usrname);
        }
        else if (usr_info->dir[strlen (usr_info->dir) - 1] == '/')
            sprintf (usr_info->dir+strlen(usr_info->dir), "%s", usr_info->usrname);
    }
    else
    {
        printf ("\nHome directory will be %s/%s\n", DEFAULT_HOME, usr_info->usrname);
        sprintf (usr_info->dir, "%s/%s", DEFAULT_HOME, usr_info->usrname);
    }

    printf ("\nShell [%s]: ", DEFAULT_SHELL);
    fflush (stdout);
    safeget (usr_info->shell, sizeof (usr_info->shell));
    if (!strlen (usr_info->shell))
        sprintf (usr_info->shell, "%s", DEFAULT_SHELL);
    else
    {
        char *sh;
        int ok = 0;

        if (!ok)
        {
            if (getuid () == 0)
                printf ("Warning: root allowed non standard shell\n");
            else
            {
                printf ("Shell NOT in /etc/shells, DEFAULT used\n");
                sprintf (usr_info->shell, "%s", DEFAULT_SHELL);
            }
        }
    }

    printf ("\nMin. Password Change Days [%d]: ", DEFAULT_MIN_PASS);
    fflush (stdout);
    safeget (foo, sizeof (foo));
    if (strlen (foo) > 1)
        usr_info->min_pass = DEFAULT_MIN_PASS;
    else
        usr_info->min_pass = atoi (foo);

    printf ("Max. Password Change Days [%d]: ", DEFAULT_MAX_PASS);
    fflush (stdout);
    safeget (foo, sizeof (foo));
    if (strlen (foo) > 1)
        usr_info->max_pass = atoi (foo);
    else
        usr_info->max_pass = DEFAULT_MAX_PASS;

    printf ("Password Warning Days [%d]: ", DEFAULT_WARN_PASS);
    fflush (stdout);
    safeget (foo, sizeof (foo));
    usr_info->warn_pass = atoi (foo);
    if (usr_info->warn_pass == 0)
        usr_info->warn_pass = DEFAULT_WARN_PASS;

    printf ("Days after Password Expiry for Account Locking [%d]: ", DEFAULT_USER_DIE);
    fflush (stdout);
    safeget (foo, sizeof (foo));
    usr_info->user_die = atoi (foo);
    if (usr_info->user_die == 0)
        usr_info->user_die = DEFAULT_USER_DIE;
}

int
validateUsername (char *usr_name) {
    int success = 0;

    printf ("\nLogin to add (^C to quit): ");

    if (!strlen (usr_name))
    {
        printf ("Empty input.\n");
        success = 0;
    }

    if (strlen (usr_name) > MAX_USRNAME)
    {
        printf ("That name is longer than the maximum of %d characters. Choose another.\n", MAX_USRNAME);
        success = 0;
    }
    else if (getpwnam (usr_name) != NULL)
    {
        printf ("That name is in use, choose another.\n");
        success = 0;
    }
    else if (strchr (usr_name, ' ') != NULL)
    {
        printf ("No spaces in username!!\n");
        success = 0;
    }
    else
        success = 1;
        
    return success;
}

int
validateGrpid (char *grp_name) {
    int success = 0;
    int group = 0;
    struct group *grp;

    grp = getgrgid (DEFAULT_GROUP);
    printf ("GID [%d]: ", DEFAULT_GROUP);
    printf("\ngrp is %s\n",grp_name);
    if (!strlen (grp_name)) {
        group = DEFAULT_GROUP;
    }
    else if (isdigit (*grp_name))
    {
        group = atoi (grp_name);
        printf("group is %d\n", group);
            success = 1;
    }
    else if ((grp = getgrnam (grp_name))) {
        group = grp->gr_gid;
    }
    else
    {
        printf ("unknown group %s\n", grp_name);
        group = DEFAULT_GROUP;
        success = 1;
    }
    if (group == 0)
    {		/* You're not allowed to make root group users! */
        printf ("Creation of root group users not allowed (must be done by hand)\n");
        group = DEFAULT_GROUP;
        success = 1;
    };

    if (success)
    {
        return group;
    }
    else
    {
        printf("\n Group ID entered is invalid\n");
        exit (0);
    } 
}
