// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include "alloc.h"

#include <error.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
int isword (char c)
{
  if (isalpha(c) || isdigit(c) ||
       c == '!' ||c == '%' ||c == '+' ||c == ',' || c == '-' ||
        c == '.' ||c == '/' ||c == ':' ||c == '@' ||c == '^' || c == '_')
    return 1;
  else
    return 0;
}

command_stream_t
read_buffer (char* buffer, size_t size)
{
  printf ("%zd\n", size);
  size_t word_count = 512;
  size_t count = 0;
  command_stream_t stream = (command_stream_t) checked_malloc (8);
  stream->token = (char**) checked_malloc (word_count);
  stream->wc = 0;
  while (count < size)
  {
    char c = tolower(*buffer);
    if (isspace(c) && c != '\n')
    {
      printf ("space, break\n");
      buffer++;
      count++;
    }
    else if (c == '\n'||c == ';' || 
        c == '(' ||c == ')' ||c == '<' ||c == '>')
    {
      char *t = (char*) checked_malloc (1);

      *t = c;
      *(stream->token) = t;
      stream->token++;
      if (++stream->wc == word_count)
        stream->token = checked_grow_alloc (stream->token, &word_count);
//      printf ("huiche:%cle", **(stream->token-1));
      buffer++;
      count++;
    }
    else if (c == '&')
    {
      if (*(++buffer) == '&')
      {
        char t[] = {'&','&'};
        *(stream->token) = t;
        stream->token++;
      if (++stream->wc == word_count)
        stream->token = checked_grow_alloc (stream->token, &word_count);
      buffer++;
      count += 2;
      }
      else
      {
        char t[] = {'&'};
        *(stream->token) = t;
      stream->token++;
      if (++stream->wc == word_count)
        stream->token = checked_grow_alloc (stream->token, &word_count);
      count++;
      }
    }
    else if (c == '|')
    {
      if (*(++buffer) == '|')
      {
        char t[] = {'|','|'};
        *(stream->token) = t;
        stream->token++;
      if (++stream->wc == word_count)
        stream->token = checked_grow_alloc (stream->token, &word_count);
      buffer++;
      count += 2;
      }
      else
      {
        char t[] = {'|'};
        *(stream->token) = t;
      stream->token++;
      if (++stream->wc == word_count)
        stream->token = checked_grow_alloc (stream->token, &word_count);
      count++;
      }
    }
    else if (isword(c))
    {
      size_t word_size = 8;
      size_t n = 0;
      char *t = (char*) checked_malloc (4);
      do
      {
        t[n++] = c;

        if (n == word_size)
          t = checked_grow_alloc (t, &word_size);
        buffer++;
        count++;
        c = tolower(*buffer);
        printf ("%c", *buffer);
      } while (isword(c));
      *(stream->token) = t;
      if (++stream->wc == word_count)
        stream->token = checked_grow_alloc (stream->token, &word_count);
      printf("to be break %zd\n", count);
    
    }
  }
  printf ("total world %zd", stream->wc);
  return stream;
}

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  size_t buffer_size = 512;
  size_t count = 0;
  char *buffer = (char*) checked_malloc (buffer_size);
  int c = get_next_byte(get_next_byte_argument);
  while (c != EOF)
  {
    buffer[count++] = c;
    c = get_next_byte(get_next_byte_argument);
    if (count == buffer_size)
      buffer = checked_grow_alloc (buffer, &buffer_size);
  }
/*  if (count == buffer_size)
      buffer = checked_grow_alloc (buffer, &buffer_size);
  printf("%c, %c, %c, %c\n", buffer[0], buffer[count - 2], 
        buffer[count-1], buffer[count]);*/

  command_stream_t stream = read_buffer (buffer, count);

  
  return stream;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
