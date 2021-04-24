#include "blather.h"

// Print like printf but append "LOG: " to the front; do nothing of
// the environment variable BL_NOLOG is set. Prints to stderr.
void log_printf(char *fmt, ...){
  if(getenv("BL_NOLOG")==NULL){
    fprintf(stderr,"LOG: ");
    va_list myargs;                      // declare a va_list type variable 
    va_start(myargs, fmt);               // initialise the va_list variable with the ... after fmt 
    vfprintf(stderr,fmt,myargs);         // forward the '...' to vfprintf()
    va_end(myargs);                      // clean up the va_list
  }
}

// Print like printf but only if the environment variable BL_DEBUG is
// defined; prefixes messages with "DBG: ". Prints to stderrr.
void dbg_printf(char *fmt, ...){
  if(getenv("BL_DEBUG")!=NULL){
    fprintf(stderr,"DEBUG: ");
    va_list myargs;                      // declare a va_list type variable 
    va_start(myargs, fmt);               // initialise the va_list variable with the ... after fmt 
    vfprintf(stderr,fmt,myargs);         // forward the '...' to vfprintf()
    va_end(myargs);                      // clean up the va_list
  }
}

// If 'condition' is a truthy value fprint an error message and
// exit. If 'perr' is truthy, call perror() as well to show the cause
// of the error, usually when a system call is involved. The 'fmt'
// argument is a format string which will be used in a printf()
// invocation that acceptes additional arguments.
// 
// Example:
// 
// // System call involved, set 'perr' arg to 1
// int myfd = open(filename, O_RDONLY);
// checkfail(myfd==-1, 1, "Couldn't open file %s", filename);
//
// // User check only, set 'perr' arg to 0
// checkfail(i < max_index, 0, "Index out of bounds: %d vs max %d\n",i,max_index);
void check_fail(int condition, int perr, char *fmt, ...) {
  if(!condition){
    return;
  }
  if(perr){
    char msg[MAXLINE];                  // buffer for message
    va_list myargs;                     // declare a va_list type variable 
    va_start(myargs, fmt);              // initialise the va_list variable with the ... after fmt 
    vsnprintf(msg,MAXLINE,fmt,myargs);  // forward the '...' to vsnprintf()
    va_end(myargs);                     // clean up the va_list
    perror(msg);
  }
  else{
    va_list myargs;                     // declare a va_list type variable 
    va_start(myargs, fmt);              // initialise the va_list variable with the ... after fmt 
    vfprintf(stderr,fmt,myargs);        // forward the '...' to vfprintf()
    va_end(myargs);                     // clean up the va_list
  }
  exit(1);
}

// Sleep the running program for the given number of nanoseconds and
// seconds.
void pause_for(long nanos, int secs){
  struct timespec tm = {
    .tv_nsec = nanos,
    .tv_sec  = secs,
  };
  nanosleep(&tm,NULL);
}
