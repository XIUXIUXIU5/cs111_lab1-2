// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"

#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  int buffer_size = 512;
  int count = 0;
  char *buffer = (char*) checked_malloc (buffer_size);
  int c;
  do 
  {
    c = get_next_byte(get_next_byte_argument);
    char buffer[count++] = (char) c;

    if (count == buffer_size)
      buffer = checked_grow_alloc (buffer, &buffer_size);
  } while (c != EOF);
  printf("%c, %c, %c\n", buffer[0], buffer[count-1], buffer[count]);
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
