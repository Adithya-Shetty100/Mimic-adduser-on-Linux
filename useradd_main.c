#include <stdio.h>
#include "useradd.h"

void 
main (int argc, char *argv[])
{
    char foo[32];
    /* the group and uid of the new user */
    int bad = 0, done = 0, correct = 0, olduid;
    char cmd[255];
    struct group *grp;
    int opt;
    int system_enable = 0;
    char grp_name[32];
    int inner_loop = 0;
    usr_info_t usr_info;
    int uid;

    while((opt = getopt(argc, argv, "g:d:s:c:r:u:")) != -1) 
    {
      switch(opt)
      {
          case 'g':
              strcpy(grp_name, optarg);
              break;
          case 'c':
              strcpy(usr_info.person, optarg);
              break;
          case 'r':
              system_enable = 1;
              break;
          case 'u':
              uid = atoi(optarg);
              break;
      }
    }

    // optind is for the extra arguments 
    // which are not parsed 
    for(; optind < argc; optind++){
        printf("extra arguments: %s\n", argv[optind]);
        strcpy(usr_info.usrname, argv[optind]);
        printf("value: %s\n", usr_info.usrname);
    }

    if (geteuid () != 0)
    {
        printf ("It seems you don't have access to add a new user.  Try\n");
        printf ("logging in as root or su root to gain superuser access.\n");
        exit (1);
    }

  /* Sanity checks */

    if (!(grp = getgrgid (DEFAULT_GROUP)))
    {
        printf ("Error: the default group %d does not exist on this system!\n",
	        DEFAULT_GROUP);
        printf ("adduser must be recompiled.\n");
        exit (1);
    };

    while (!correct)
    {			/* loop until a "good" usrname is chosen */
        inner_loop = 0;
        while (!done)
	{
	    done = validateUsername (usr_info.usrname);
	    inner_loop++;
	    if (inner_loop >= 3) {
	        printf ("\n You are entering a wrong username. Either it is invalid or already in use\n");
	        exit (0);
	    }
	}

        /* all set, get the rest of the info */
        printf ("\nEditing information for new user [%s]\n", usr_info.usrname);

        printf ("\nFull Name [%s]: ", usr_info.person);
        if (!strlen (usr_info.person))
        {
            bzero (usr_info.person, sizeof (usr_info.person));
            strcpy (usr_info.person, usr_info.usrname);
	};

        if (getuid () == 0)
	{
	    usr_info.group = validateGrpid (grp_name/*, &grp*/);
	}
	else
	{
	    printf ("Group will be default of: %d\n", DEFAULT_GROUP);
	    usr_info.group = DEFAULT_GROUP;
	}
	
        getUsrInfo (&usr_info);
      
        printf ("\nInformation for new user [%s] [%s]:\n", usr_info.usrname, usr_info.person);
        printf ("Home directory: [%s] Shell: [%s]\n", usr_info.dir, usr_info.shell);
        printf ("GID: [%d]\n", usr_info.group);
        printf ("MinPass: [%d] MaxPass: [%d] WarnPass: [%d] UserExpire: [%d]\n",
	        usr_info.min_pass, usr_info.max_pass, usr_info.warn_pass, usr_info.user_die);
        printf ("\nIs this correct? [y/N]: ");
        fflush (stdout);
        safeget (foo, sizeof (foo));

        done = bad = correct = (foo[0] == 'y' || foo[0] == 'Y');

        if (bad != 1)
            printf ("\nUser [%s] not added\n", usr_info.usrname);
    }

    bzero (cmd, sizeof (cmd));
    if (system_enable) {
        printf("Adding system user\n");
        sprintf (cmd, "%s -g %d -u %d -d %s -s %s -c \"%s\" -r -m -k /etc/skel %s",
	         USERADD_PATH, usr_info.group, uid, usr_info.dir, usr_info.shell, usr_info.person, usr_info.usrname);
        printf ("Calling useradd to add new user:\n%s\n", cmd);
        if (system (cmd))
        {
            printf ("User add failed!\n");

           exit (0);
        };
    }
    else {
        printf("Adding normal user\n");
        sprintf (cmd, "%s -g %d -u %d -d %s -s %s -c \"%s\" -m -k /etc/skel %s",
	       USERADD_PATH, usr_info.group, uid, usr_info.dir, usr_info.shell, usr_info.person, usr_info.usrname);
        printf ("Calling useradd to add new user:\n%s\n", cmd);
        if (system (cmd))
        {
            printf ("User add failed!\n");

            exit (0);
        };
    }
    olduid = getuid ();	/* change, passwd, edquota etc. require ruid = root
			 */
    setuid (0);

    bzero (cmd, sizeof (cmd));

    sprintf (cmd, "%s -m %d -M %d -W %d -I %d %s", CHAGE_PATH,
	   usr_info.min_pass, usr_info.max_pass, usr_info.warn_pass, usr_info.user_die, usr_info.usrname);
    printf ("%s\n", cmd);
    if (system (cmd))
    {
        printf ("There was an error setting password expire values\n");
    };

    bzero (cmd, sizeof (cmd));
    sprintf (cmd, "%s %s", PASSWD_PATH, usr_info.usrname);
    if (system (cmd))
    {
        printf ("\nWarning: error setting password\n");
    }

    setuid (olduid);

    printf ("\nDone.\n");
}

