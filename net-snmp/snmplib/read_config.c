/*
 * read_config.c
 */

#include <config.h>

#include <stdio.h>
#include <ctype.h>
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if TIME_WITH_SYS_TIME
# ifdef WIN32
#  include <sys/timeb.h>
# else
#  include <sys/time.h>
# endif
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#if HAVE_WINSOCK_H
#include <winsock.h>
#endif
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if HAVE_NETDB_H
#include <netdb.h>
#endif
#include <errno.h>

#if HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

#include "asn1.h"
#include "mib.h"
#include "parse.h"
#include "system.h"
#include "snmp_api.h"
#include "snmp_debug.h"
#include "snmp_logging.h"
#include "snmp_impl.h"
#include "default_store.h"
#include "callback.h"

#include "read_config.h"
#include "tools.h"

static int config_errors;

struct config_files *config_files = NULL;

struct config_line *
register_premib_handler(const char *type,
			const char *token,
			void (*parser) (const char *, char *),
			void (*releaser) (void),
			const char *help)
{
  struct config_line *ltmp;
  ltmp = register_config_handler(type, token, parser, releaser, help);
  if (ltmp != NULL)
    ltmp->config_time = PREMIB_CONFIG;
  return (ltmp);
}

struct config_line *
register_app_premib_handler(const char *token,
			void (*parser) (const char *, char *),
			void (*releaser) (void),
			const char *help)
{
  return(register_premib_handler( NULL, token, parser, releaser, help ));
}

/*******************************************************************-o-******
 * register_config_handler
 *
 * Parameters:
 *	*type
 *	*token
 *	*parser
 *	*releaser
 *      
 * Returns:
 *	Pointer to a new config line entry  -OR-  NULL on error.
 */
struct config_line *
register_config_handler(const char *type_param,
			const char *token,
			void (*parser) (const char *, char *),
			void (*releaser) (void),
			const char *help)
{
  struct config_files **ctmp = &config_files;
  struct config_line **ltmp;
  const char *type = type_param;

  if ( type == NULL )
    type = ds_get_string(DS_LIBRARY_ID, DS_LIB_APPTYPE);

  /* 
   * Find type in current list  -OR-  create a new file type.
   */
  while (*ctmp != NULL && strcmp((*ctmp)->fileHeader, type)) {
    ctmp = &((*ctmp)->next);
  }

  if (*ctmp == NULL) {
    *ctmp = (struct config_files *)
      calloc(1,sizeof(struct config_files));
    if ( !*ctmp ) {
      return NULL;
    }

    (*ctmp)->fileHeader	 = strdup(type);
  }

  /* 
   * Find parser type in current list  -OR-  create a new
   * line parser entry.
   */
  ltmp = &((*ctmp)->start);

  while (*ltmp != NULL && strcmp((*ltmp)->config_token, token)) {
    ltmp = &((*ltmp)->next);
  }

  if (*ltmp == NULL) {
    *ltmp = (struct config_line *)
      calloc(1,sizeof(struct config_line));
    if ( !*ltmp ) {
      return NULL;
    }

    (*ltmp)->config_time	 = NORMAL_CONFIG;
    (*ltmp)->config_token	 = strdup(token);
    if (help != NULL)
      (*ltmp)->help = strdup(help);
  }

  /* 
   * Add/Replace the parse/free functions for the given line type
   * in the given file type.
   */
  (*ltmp)->parse_line = parser;
  (*ltmp)->free_func  = releaser;

  return (*ltmp);

}  /* end register_config_handler() */

struct config_line *
register_app_config_handler(const char *token,
			void (*parser) (const char *, char *),
			void (*releaser) (void),
			const char *help)
{
  return(register_config_handler( NULL, token, parser, releaser, help ));
}

void
unregister_config_handler(const char *type_param, 
			  const char *token)
{
  struct config_files **ctmp = &config_files;
  struct config_line **ltmp, *ltmp2;
  const char *type = type_param;

  if ( type == NULL )
    type = ds_get_string(DS_LIBRARY_ID, DS_LIB_APPTYPE);

  /* find type in current list */
  while (*ctmp != NULL && strcmp((*ctmp)->fileHeader,type)) {
    ctmp = &((*ctmp)->next);
  }

  if (*ctmp == NULL) {
    /* Not found, return. */
    return;
  }
  
  ltmp = &((*ctmp)->start);
  if (*ltmp == NULL) {
    /* Not found, return. */
    return;
  }
  if (strcmp((*ltmp)->config_token,token) == 0) {
    /* found it at the top of the list */
    ltmp2 = (*ltmp)->next;
    free((*ltmp)->config_token);
    SNMP_FREE((*ltmp)->help);
    free(*ltmp);
    (*ctmp)->start = ltmp2;
    return;
  }
  while ((*ltmp)->next != NULL && strcmp((*ltmp)->next->config_token,token)) {
    ltmp = &((*ltmp)->next);
  }
  if ((*ltmp)->next != NULL) {
    free((*ltmp)->next->config_token);
    SNMP_FREE((*ltmp)->next->help);
    ltmp2 = (*ltmp)->next->next;
    free((*ltmp)->next);
    (*ltmp)->next = ltmp2;
  }
}

void
unregister_app_config_handler(const char *token)
{
  unregister_config_handler( NULL, token );
}

void 
unregister_all_config_handlers()
{
  struct config_files *ctmp, *save;
  struct config_line *ltmp;

  free_config();

  /* Keep using config_files until there are no more! */
  for (ctmp = config_files; ctmp;) {
      for (ltmp = ctmp->start; ltmp; ltmp = ctmp->start) {
          unregister_config_handler (ctmp->fileHeader, ltmp->config_token);
      }
      free (ctmp->fileHeader);
      save = ctmp->next;
      free (ctmp);
      ctmp = save;
      config_files = save;
  }
}

#ifdef TESTING
void print_config_handlers (void)
{
  struct config_files *ctmp = config_files;
  struct config_line *ltmp;

  for(;ctmp != NULL; ctmp = ctmp->next) {
    DEBUGMSGTL(("read_config", "read_conf: %s\n", ctmp->fileHeader));
    for(ltmp = ctmp->start; ltmp != NULL; ltmp = ltmp->next)
      DEBUGMSGTL(("read_config", "                   %s\n", ltmp->config_token));
  }
}
#endif

int linecount;
const char *curfilename;

struct config_line *
read_config_get_handlers(const char *type) {
    struct config_files *ctmp = config_files;
    for(;ctmp != NULL && strcmp(ctmp->fileHeader,type); ctmp = ctmp->next);
    if (ctmp)
        return ctmp->start;
    return NULL;
}

void read_config_with_type(const char *filename, 
			   const char *type)
{
  struct config_line *ctmp = read_config_get_handlers(type);
  if (ctmp)
    read_config(filename, ctmp, EITHER_CONFIG);
  else
    DEBUGMSGTL(("read_config", "read_config: I have no registrations for type:%s,file:%s\n",
           type, filename));
}

/*******************************************************************-o-******
 * read_config
 *
 * Parameters:
 *	*filename
 *	*line_handler
 *	 when
 *
 * Read <filename> and process each line in accordance with the list of
 * <line_handler> functions.
 *
 *
 * For each line in <filename>, search the list of <line_handler>'s 
 * for an entry that matches the first token on the line.  This comparison is
 * case insensitive.
 *
 * For each match, check that <when> is the designated time for the
 * <line_handler> function to be executed before processing the line.
 */
void read_config(const char *filename,
		 struct config_line *line_handler,
		 int when)
{

  FILE *ifile;
  char line[STRINGMAX], token[STRINGMAX], tmpbuf[STRINGMAX];
  char *cptr;
  int i, done;
  struct config_line *lptr;

  linecount = 0;
  curfilename = filename;
  
  if ((ifile = fopen(filename, "r")) == NULL) {
#ifdef ENOENT
    if (errno == ENOENT) {
      DEBUGMSGTL(("read_config", "%s: %s\n", filename, strerror(errno)));
    } else
#endif /* ENOENT */
#ifdef EACCES
    if (errno == EACCES) {
      DEBUGMSGTL(("read_config", "%s: %s\n", filename, strerror(errno)));
    } else
#endif /* EACCES */
#if defined(ENOENT) || defined(EACCES)
    {
      snmp_log_perror(filename);
    }
#else /* defined(ENOENT) || defined(EACCES) */
    snmp_log_perror(filename);
#endif /* ENOENT */
    return;
  } else {
    DEBUGMSGTL(("read_config", "Reading configuration %s\n", filename));
  }

  while (fgets(line, sizeof(line), ifile) != NULL) 
    {
      lptr = line_handler;
      linecount++;
      cptr = line;
      i = strlen(line)-1;
      if (line[i] == '\n')
        line[i] = 0;
      /* check blank line or # comment */
      if ((cptr = skip_white(cptr)))
	{
          cptr = copy_word(cptr,token);
          if (token[0] == '[') {
              token[strlen(token)-1] = '\0';
              lptr = read_config_get_handlers(&token[1]);
              if (lptr == NULL) {
                  sprintf(tmpbuf,"No handlers regestered for type %s.",
                          &token[1]);
                  config_perror(tmpbuf);
                  continue;
              }
              DEBUGMSGTL(("read_config","Switching to new context: %s%s\n",
                          ((cptr)?"(this line only) ":""),&token[1]));
              if (cptr == NULL) {
                  /* change context permanently */
                  line_handler = lptr;
                  continue;
              } else {
                  /* the rest of this line only applies. */
                  cptr = copy_word(cptr,token);
              }
          } else {
              lptr = line_handler;
          }
          if (cptr == NULL) {
            sprintf(tmpbuf,"Blank line following %s token.", token);
            config_perror(tmpbuf);
          } else {
              
            for(done=0; lptr != NULL && !done;
                lptr = lptr->next) {
              if (!strcasecmp(token,lptr->config_token)) {
                if (when == EITHER_CONFIG || lptr->config_time == when) {
                    DEBUGMSGTL(("read_config", "%s:%d Parsing: %s\n",
                                filename, linecount, line));
                    (*(lptr->parse_line))(token,cptr);
                }
                done = 1;
              }
            }
            if (!done && when != PREMIB_CONFIG &&
                !ds_get_boolean(DS_LIBRARY_ID, DS_LIB_NO_TOKEN_WARNINGS)) {
              sprintf(tmpbuf,"Unknown token: %s.", token);
              config_pwarn(tmpbuf);
            }
          }
	}
    }
  fclose(ifile);
  return;

}  /* end read_config() */



void
free_config (void)
{
  struct config_files *ctmp = config_files;
  struct config_line *ltmp;

  for(;ctmp != NULL; ctmp = ctmp->next)
    for(ltmp = ctmp->start; ltmp != NULL; ltmp = ltmp->next)
      if (ltmp->free_func)
        (*(ltmp->free_func))();
}

void
read_configs (void)
{

  char *optional_config = ds_get_string(DS_LIBRARY_ID, DS_LIB_OPTIONALCONFIG);
  char *type = ds_get_string(DS_LIBRARY_ID, DS_LIB_APPTYPE);

  DEBUGMSGTL(("read_config","reading normal configuration tokens\n"));
  
  if (!ds_get_boolean(DS_LIBRARY_ID, DS_LIB_DONT_READ_CONFIGS))
    read_config_files(NORMAL_CONFIG);

 /* do this even when the normal above wasn't done */
  if (optional_config && type) {
      struct stat statbuf;
      if (stat(optional_config, &statbuf)) {
          DEBUGMSGTL(("read_config","Optional File \"%s\" does not exist.\n",
                      optional_config));
          snmp_log_perror(optional_config);
      } else {
          DEBUGMSGTL(("read_config","Reading optional config file: \"%s\"\n",
                      optional_config));
          read_config_with_type(optional_config, type);
      }
  }

  snmp_call_callbacks(SNMP_CALLBACK_LIBRARY, SNMP_CALLBACK_POST_READ_CONFIG,
                      NULL);
}

void
read_premib_configs (void)
{
  DEBUGMSGTL(("read_config","reading premib configuration tokens\n"));

  if (!ds_get_boolean(DS_LIBRARY_ID, DS_LIB_DONT_READ_CONFIGS))
    read_config_files(PREMIB_CONFIG);

  snmp_call_callbacks(SNMP_CALLBACK_LIBRARY,
                      SNMP_CALLBACK_POST_PREMIB_READ_CONFIG,
                      NULL);
}

/*******************************************************************-o-******
 * set_configuration_directory
 *
 * Parameters:
 *      char *dir - value of the directory
 * Sets the configuration directory. Multiple directories can be
 * specified, but need to be seperated by 'ENV_SEPARATOR_CHAR'.
 */
void set_configuration_directory(const char *dir)
{
    ds_set_string(DS_LIBRARY_ID, DS_LIB_CONFIGURATION_DIR, strdup(dir));
}

/*******************************************************************-o-******
 * get_configuration_directory
 *
 * Parameters: -
 * Retrieve the configuration directory or directories.
 * (For backwards compatibility that is:
 *       SNMPCONFPATH, SNMPSHAREPATH, SNMPLIBPATH, HOME/.snmp
 * First check whether the value is set.
 * If not set give it the default value.
 * Return the value.
 * We always retrieve it new, since we have to do it anyway if it is just set.
 */
const char *get_configuration_directory()
{
char defaultPath[SPRINT_MAX_LEN];
char *homepath;

    if (NULL == ds_get_string(DS_LIBRARY_ID, DS_LIB_CONFIGURATION_DIR)) {
        homepath=getenv("HOME");
        sprintf(defaultPath,"%s%c%s%c%s%s%s%s",
              SNMPCONFPATH, ENV_SEPARATOR_CHAR,
              SNMPSHAREPATH, ENV_SEPARATOR_CHAR, SNMPLIBPATH,
              ((homepath == NULL) ? "" : ENV_SEPARATOR),
              ((homepath == NULL) ? "" : homepath),
              ((homepath == NULL) ? "" : "/.snmp"));
        set_configuration_directory(defaultPath);
    }
    return(ds_get_string(DS_LIBRARY_ID, DS_LIB_CONFIGURATION_DIR));
}

/*******************************************************************-o-******
 * set_persistent_directory
 *
 * Parameters:
 *      char *dir - value of the directory
 * Sets the configuration directory. 
 * No multiple directories may be specified.
 * (However, this is not checked)
 */
void set_persistent_directory(const char *dir)
{
    ds_set_string(DS_LIBRARY_ID, DS_LIB_PERSISTENT_DIR, strdup(dir));
}

/*******************************************************************-o-******
 * get_persistent_directory
 *
 * Parameters: -
 * Function will retrieve the persisten directory value.
 * First check whether the value is set.
 * If not set give it the default value.
 * Return the value. 
 * We always retrieve it new, since we have to do it anyway if it is just set.
 */
const char *get_persistent_directory()
{
    if (NULL == ds_get_string(DS_LIBRARY_ID, DS_LIB_PERSISTENT_DIR)) {
        set_persistent_directory(PERSISTENT_DIRECTORY);
    }
    return(ds_get_string(DS_LIBRARY_ID, DS_LIB_PERSISTENT_DIR));
}


/*******************************************************************-o-******
 * read_config_files
 *
 * Parameters:
 *	when	== PREMIB_CONFIG, NORMAL_CONFIG  -or-  EITHER_CONFIG
 *
 *
 * Traverse the list of config file types, performing the following actions
 * for each --
 *
 * First, build a search path for config files.  If the contents of 
 * environment variable SNMPCONFPATH are NULL, then use the following
 * path list (where the last entry exists only if HOME is non-null):
 *
 *	SNMPSHAREPATH:SNMPLIBPATH:${HOME}/.snmp
 *
 * Then, In each of these directories, read config files by the name of:
 *
 *	<dir>/<fileHeader>.conf		-AND-
 *	<dir>/<fileHeader>.local.conf
 *
 * where <fileHeader> is taken from the config file type structure.
 *
 *
 * PREMIB_CONFIG causes free_config() to be invoked prior to any other action.
 *
 *
 * EXITs if any 'config_errors' are logged while parsing config file lines.
 */
void
read_config_files (int when)
{
  int i, j;
  char configfile[300];
  char *envconfpath;
  const char *confpath, *perspath;
  char *cptr1, *cptr2;
  char defaultPath[SPRINT_MAX_LEN];

  struct config_files *ctmp = config_files;
  struct config_line *ltmp;
  struct stat statbuf;

  config_errors = 0;
  
  if (when == PREMIB_CONFIG)
    free_config();

  confpath = get_configuration_directory();
  perspath = get_persistent_directory();

  /* read all config file types */
  for(;ctmp != NULL; ctmp = ctmp->next) {

    ltmp = ctmp->start;

    /* read the config files */
    if ((envconfpath = getenv("SNMPCONFPATH")) == NULL) {
      sprintf(defaultPath,"%s%s%s",
              ((confpath == NULL) ? "" : confpath),
              ((perspath == NULL) ? "" : ENV_SEPARATOR),
              ((perspath == NULL) ? "" : perspath));
      envconfpath = defaultPath;
    }
    envconfpath = strdup(envconfpath);  /* prevent actually writing in env */
    DEBUGMSGTL(("read_config","config path used:%s\n", envconfpath));
    cptr1 = cptr2 = envconfpath;
    i = 1;
    while (i && *cptr2 != 0) {
      while(*cptr1 != 0 && *cptr1 != ENV_SEPARATOR_CHAR)
        cptr1++;
      if (*cptr1 == 0)
        i = 0;
      else
        *cptr1 = 0;
      /*
       * for proper persistent storage retrival, we need to read old backup
       * copies of the previous storage files.  If the application in
       * question has died without the proper call to snmp_clean_persistent,
       * then we read all the configuration files we can, starting with
       * the oldest first.
       */
      if (strncmp(cptr2, perspath,
                  strlen(perspath)) == 0 ||
          (getenv("SNMP_PERSISTENT_FILE") != NULL &&
           strncmp(cptr2, getenv("SNMP_PERSISTENT_FILE"),
                   strlen(getenv("SNMP_PERSISTENT_FILE"))) == 0)) {
        /* limit this to the known storage directory only */
        for(j=0; j <= MAX_PERSISTENT_BACKUPS; j++) {
          sprintf(configfile,"%s/%s.%d.conf",cptr2, ctmp->fileHeader, j);
          if (stat(configfile, &statbuf) != 0) {
            /* file not there, continue */
            break;
          } else {
            /* backup exists, read it */
            DEBUGMSGTL(("read_config_files","old config file found: %s, parsing\n", configfile));
            read_config (configfile, ltmp, when);
          }
        }
      }
      sprintf(configfile,"%s/%s.conf",cptr2, ctmp->fileHeader);
      read_config (configfile, ltmp, when);
      sprintf(configfile,"%s/%s.local.conf",cptr2, ctmp->fileHeader);
      read_config (configfile, ltmp, when);
      cptr2 = ++cptr1;
    }
    free(envconfpath);
  }
  
  if (config_errors) {
    snmp_log(LOG_ERR, "ucd-snmp: %d error(s) in config file(s)\n", config_errors);
/*    exit(1); */
  }
}

void read_config_print_usage(const char *lead)
{
  struct config_files *ctmp = config_files;
  struct config_line *ltmp;

  if (lead == NULL)
    lead = "";

  for(ctmp = config_files; ctmp != NULL; ctmp = ctmp->next) {
    snmp_log(LOG_INFO, "%sIn %s.conf and %s.local.conf:\n", lead, ctmp->fileHeader,
            ctmp->fileHeader);
    for(ltmp = ctmp->start; ltmp != NULL; ltmp = ltmp->next) {
        DEBUGIF("read_config_usage") {
            if (ltmp->config_time == PREMIB_CONFIG)
                DEBUGMSG(("read_config_usage", "*"));
            else
                DEBUGMSG(("read_config_usage", " "));
        }
        if (ltmp->help) {
            snmp_log(LOG_INFO, "%s%s%-15s %s\n", lead, lead,
                     ltmp->config_token, ltmp->help);
        } else {
            DEBUGIF("read_config_usage") {
                snmp_log(LOG_INFO, "%s%s%-15s [NO HELP]\n", lead, lead,
                         ltmp->config_token);
            }
        }
    }
  }
}

/*******************************************************************-o-******
 * read_config_store
 *
 * Parameters:
 *	*type
 *	*line
 *      
 * 
 * Append line to a file named either ENV(SNMP_PERSISTENT_FILE) or
 *   "<PERSISTENT_DIRECTORY>/<type>.conf".
 * Add a trailing newline to the stored file if necessary.
 *
 * Intended for use by applications to store permenant configuration 
 * information generated by sets or persistent counters.
 *
 */
void
read_config_store(const char *type, const char *line)
{
#ifdef PERSISTENT_DIRECTORY
  char file[512], *filep;
  FILE *fout;
#ifdef PERSISTENT_MASK
  mode_t oldmask;
#endif

  /* store configuration directives in the following order of preference:
     1. ENV variable SNMP_PERSISTENT_FILE
     2. configured <PERSISTENT_DIRECTORY>/<type>.conf
  */
  if ((filep = getenv("SNMP_PERSISTENT_FILE")) == NULL) {
    sprintf(file,"%s/%s.conf",get_persistent_directory(),type);
    filep = file;
  }
  
#ifdef PERSISTENT_MASK
  oldmask = umask(PERSISTENT_MASK);
#endif
  if (mkdirhier(filep, AGENT_DIRECTORY_MODE, 1)) {
      snmp_log(LOG_ERR, "Failed to create the persistent directory for %s\n",
               file);
  }
  if ((fout = fopen(filep, "a")) != NULL) {
    fprintf(fout, "%s", line);
    if (line[strlen(line)] != '\n')
      fprintf(fout,"\n");
    DEBUGMSGTL(("read_config","storing: %s\n",line));
    fclose(fout);
  } else {
    DEBUGMSGTL(("read_config","open failure"));
  }
#ifdef PERSISTENT_MASK
  umask(oldmask);
#endif

#endif
}  /* end read_config_store() */

void
read_app_config_store(const char *line)
{
  read_config_store(ds_get_string(DS_LIBRARY_ID, DS_LIB_APPTYPE), line);
}




/*******************************************************************-o-******
 * snmp_save_persistent
 *
 * Parameters:
 *	*type
 *      
 *
 * Save the file "<PERSISTENT_DIRECTORY>/<type>.conf" into a backup copy
 * called "<PERSISTENT_DIRECTORY>/<type>.%d.conf", which %d is an
 * incrementing number on each call, but less than MAX_PERSISTENT_BACKUPS.
 *
 * Should be called just before all persistent information is supposed to be
 * written to move aside the existing persistent cache.
 * snmp_clean_persistent should then be called afterward all data has been
 * saved to remove these backup files.
 *
 * Note: on an rename error, the files are removed rather than saved.
 *
 */
void
snmp_save_persistent(const char *type)
{
  char file[512], fileold[SPRINT_MAX_LEN];
  struct stat statbuf;
  int j;

  DEBUGMSGTL(("snmp_save_persistent","saving %s files...\n", type));
  sprintf(file,"%s/%s.conf", get_persistent_directory(), type);
  if (stat(file, &statbuf) == 0) {
    for(j=0; j <= MAX_PERSISTENT_BACKUPS; j++) {
      sprintf(fileold,"%s/%s.%d.conf", get_persistent_directory(), type, j);
      if (stat(fileold, &statbuf) != 0) {
        DEBUGMSGTL(("snmp_save_persistent"," saving old config file: %s -> %s.\n", file, fileold));
        if (rename(file, fileold)) {
          unlink(file);/* moving it failed, try nuking it, as leaving
                          it around is very bad. */
        }
        break;
      }
    }
  }
  /* save a warning header to the top of the new file */
  sprintf(fileold, "#\n# net-snmp (or ucd-snmp) persistent data file.\n#\n# DO NOT STORE CONFIGURATION ENTRIES HERE.\n# Please save normal configuration tokens for %s in SNMPCONFPATH/%s.conf.\n# Only \"createUser\" tokens should be placed here by %s administrators.\n#\n", type, type, type);
  read_config_store(type, fileold);
}


/*******************************************************************-o-******
 * snmp_clean_persistent
 *
 * Parameters:
 *	*type
 *      
 *
 * Unlink all backup files called "<PERSISTENT_DIRECTORY>/<type>.%d.conf".
 *
 * Should be called just after we successfull dumped the last of the
 * persistent data, to remove the backup copies of previous storage dumps.
 *
 * XXX  Worth overwriting with random bytes first?  This would
 *	ensure that the data is destroyed, even a buffer containing the
 *	data persists in memory or swap.  Only important if secrets
 *	will be stored here.
 */
void
snmp_clean_persistent(const char *type)
{
  char file[512];
  struct stat statbuf;
  int j;

  DEBUGMSGTL(("snmp_clean_persistent","cleaning %s files...\n", type));
  sprintf(file,"%s/%s.conf",get_persistent_directory(),type);
  if (stat(file, &statbuf) == 0) {
    for(j=0; j <= MAX_PERSISTENT_BACKUPS; j++) {
      sprintf(file,"%s/%s.%d.conf", get_persistent_directory(), type, j);
      if (stat(file, &statbuf) == 0) {
        DEBUGMSGTL(("snmp_clean_persistent"," removing old config file: %s\n", file));
        unlink(file);
      }
    }
  }
}
  



/* config_perror: prints a warning string associated with a file and
   line number of a .conf file and increments the error count. */
void config_perror(const char *string)
{
  snmp_log(LOG_ERR, "%s: line %d: Error: %s\n", curfilename, linecount, string);
  config_errors++;
}

void config_pwarn(const char *string)
{
  snmp_log(LOG_WARNING, "%s: line %d: Warning: %s\n", curfilename, linecount, string);
}

/* skip all white spaces and return 1 if found something either end of
   line or a comment character */
char *skip_white(char *ptr)
{
  if (ptr == NULL) return (NULL);
  while (*ptr != 0 && isspace(*ptr)) ptr++;
  if (*ptr == 0 || *ptr == '#') return (NULL);
  return (ptr);
}

char *skip_not_white(char *ptr)
{
  if (ptr == NULL) return (NULL);
  while (*ptr != 0 && !isspace(*ptr)) ptr++;
  if (*ptr == 0 || *ptr == '#') return (NULL);
  return (ptr);
}

char *skip_token(char *ptr)
{
  ptr = skip_white(ptr);
  ptr = skip_not_white(ptr);
  ptr = skip_white(ptr);
  return (ptr);
}

/* copy_word
   copies the next 'token' from 'from' into 'to'.
   currently a token is anything seperate by white space
   or within quotes (double or single) (i.e. "the red rose" 
   is one token, \"the red rose\" is three tokens)
   a '\' character will allow a quote character to be treated
   as a regular character 
   It returns a pointer to first non-white space after the end of the token
   being copied or to 0 if we reach the end.*/

char *copy_word(char *from, char *to)
{
  char quote;
  if ( (*from == '\"') || (*from =='\'') ){
    quote = *(from++);
    while ( (*from != quote) && (*from != 0) ) {
      if ((*from == '\\') && (*(from+1) != 0)) {
	*to++ = *(from+1);
	from = from +2;
      }
      else  *to++ = *from++;
    }
    if (*from == 0) {
      DEBUGMSGTL(("read_config_copy_word",
                  "no end quote found in config string\n"));
    } else from++;
  }
  else {
    while (*from != 0 && !isspace(*from)) {
      if ((*from == '\\') && (*(from+1) != 0)) {
	*to++ = *(from+1);
	from = from +2;
      }
      else  *to++ = *from++;
    }
  }
  *to = 0;
  from = skip_white(from);
  return(from);
}  /* copy_word */

/* read_config_save_octet_string(): saves an octet string as a length
   followed by a string of hex */
char *read_config_save_octet_string(char *saveto, u_char *str, size_t len) {
  int i;
  u_char *cp;

  /* is everything easily printable */
  for(i=0, cp=str; i < (int)len && cp &&
          (isalpha(*cp) || isdigit(*cp) || *cp == ' '); cp++, i++);

  if (len != 0 && i == (int)len) {
      sprintf(saveto, "\"%s\"", str);
      saveto += strlen(saveto);
  } else {
      if (str != NULL) {
          sprintf(saveto, "0x");
          saveto += 2;
          for(i = 0; i < (int)len; i++) {
              sprintf(saveto,"%02x", str[i]);
              saveto = saveto + 2;
          }
      } else {
          sprintf(saveto,"\"\"");
          saveto += 2;
      }
  }
  return saveto;
}

/* read_config_read_octet_string(): reads an octet string that was
   saved by the read_config_save_octet_string() function */
char *read_config_read_octet_string(char *readfrom, u_char **str, size_t *len) {
  u_char *cptr=NULL;
  char *cptr1;
  u_int tmp;
  int i;

  if (readfrom == NULL || str == NULL)
    return NULL;
  
  if (strncasecmp(readfrom,"0x",2) == 0) {
    /* A hex string submitted. How long? */
    readfrom += 2;
    cptr1 = skip_not_white(readfrom);
    if (cptr1)
      *len = (cptr1 - readfrom);
    else
      *len = strlen(readfrom);

    if (*len % 2) {
      DEBUGMSGTL(("read_config_read_octet_string","invalid hex string: wrong length"));
      return NULL;
    }
    *len = *len / 2;

    /* malloc data space if needed (+1 for good measure) */
    if (*str == NULL) {
      if ((cptr = (u_char *)malloc(*len + 1)) == NULL) {
        return NULL;
      }
      *str = cptr;
    } else {
      cptr = *str;
    }

    /* copy validated data */
    for(i = 0; i < (int)*len; i++) {
      if (1 == sscanf(readfrom,"%2x",&tmp))
        *cptr++ = (u_char) tmp;
      else {
        /* we may lose memory, but don't know caller's buffer XX free(cptr); */
        return (NULL);
      }
      readfrom += 2;
    }
    *cptr++ = '\0';
    readfrom = skip_white(readfrom);
  } else {
    /* Normal string */

    /* malloc string space if needed (including NULL terminator) */
    if (*str == NULL) {
      char buf[SNMP_MAXBUF];
      readfrom = copy_word(readfrom, buf);

      *len = strlen(buf);
      if (*len > 0 && ((cptr = (u_char *) malloc(*len + 1)) == NULL))
        return NULL;
      *str = cptr;
      if (cptr)
        memcpy(cptr, buf, (*len+1));
    } else {
      readfrom = copy_word(readfrom, (char *)*str);
    }
  }

  return readfrom;
}


/* read_config_save_objid(): saves an objid as a numerical string */
char *read_config_save_objid(char *saveto, oid *objid, size_t len) {
  int i;
  
  if (len == 0) {
    strcat(saveto, "NULL");
    saveto += strlen(saveto);
    return saveto;
  }

  /* in case len=0, this makes it easier to read it back in */
  for(i=0; i < (int)len; i++) {
    sprintf(saveto,".%ld", objid[i]);
    saveto += strlen(saveto);
  }
  return saveto;
}

/* read_config_read_objid(): reads an objid from a format saved by the above */
char *read_config_read_objid(char *readfrom, oid **objid, size_t *len) {

  if (objid == NULL || readfrom == NULL)
    return NULL;

  if (*objid == NULL) {
      *len = 0;
      if ((*objid = (oid*)malloc(MAX_OID_LEN * sizeof(oid))) == NULL)
        return NULL;
      *len = MAX_OID_LEN;
  }

  if (strncmp(readfrom,"NULL",4) == 0) {
      /* null length oid */
      *len = 0;
  } else {
      /* qualify the string for read_objid */
      char buf[SPRINT_MAX_LEN];
      copy_word(readfrom, buf);

      if (!read_objid(buf, *objid, len)) {
        DEBUGMSGTL(("read_config_read_objid","Invalid OID"));
        *len = 0;
        return NULL;
      }
  }
    
  readfrom = skip_token(readfrom);
  return readfrom;
}

/* read_config_read_data():
   reads data of a given type from a token(s) on a configuration line.

   Returns: character pointer to the next token in the configuration line.
            NULL if none left.
            NULL if an unknown type.
*/
char *read_config_read_data(int type, char *readfrom, void *dataptr, size_t *len) {
  int *intp;
  char **charpp;
  oid  **oidpp;

  if (dataptr && readfrom)
  switch(type) {
    case ASN_INTEGER:
      intp = (int *) dataptr;
      *intp = atoi(readfrom);
      readfrom = skip_token(readfrom);
      return readfrom;
      
    case ASN_OCTET_STR:
      charpp = (char **) dataptr;
      return read_config_read_octet_string(readfrom, (u_char **) charpp, len);

    case ASN_OBJECT_ID:
      oidpp = (oid **) dataptr;
      return read_config_read_objid(readfrom, oidpp, len);

    default:
      DEBUGMSGTL(("read_config_read_data","Fail: Unknown type: %d", type));
      return NULL;
  }
  return NULL;
}

/* read_config_read_data():
   reads data of a given type from a token(s) on a configuration line.

   Returns: character pointer to the next token in the configuration line.
            NULL if none left.
            NULL if an unknown type.
*/
char *read_config_store_data(int type, char *storeto, void *dataptr, size_t *len) {
  int *intp;
  u_char **charpp;
  oid  **oidpp;

  if (dataptr && storeto)
  switch(type) {
    case ASN_INTEGER:
      intp = (int *) dataptr;
      sprintf(storeto," %d", *intp);
      return (storeto + strlen(storeto));
      
    case ASN_OCTET_STR:
      *storeto++ = ' ';
      charpp = (u_char **) dataptr;
      return read_config_save_octet_string(storeto, *charpp, *len);

    case ASN_OBJECT_ID:
      *storeto++ = ' ';
      oidpp = (oid **) dataptr;
      return read_config_save_objid(storeto, *oidpp, *len);

    default:
      DEBUGMSGTL(("read_config_store_data","Fail: Unknown type: %d", type));
      return NULL;
  }
  return NULL;
}
