// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include "alloc.h"

#include <error.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define STACK_MAX 100
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
 // printf ("%s\n", buffer);
  printf ("%zd\n", size);
  size_t word_count = 512;
  size_t count = 0;
  command_stream_t stream = (command_stream_t) checked_malloc (sizeof(struct command_stream));
  stream->token = (char**) checked_malloc (word_count);
  stream->wc = 0;
  while (count < size)
  {
    char c = *buffer;
    if (c == '#')
    {
      do
      {
        buffer++;
        count++;
        c = *buffer;  
      } while (c!='\n');
    }
    if (isspace(c) && c != '\n')
    {
 //     printf ("space, break\n");
      buffer++;
      count++;
    }
    else if (c == '\n'||c == ';' ||
        c == '(' ||c == ')' ||c == '<' ||c == '>')
    {
      char *t = (char*) checked_malloc (1);

      *t = c;
      stream->token[stream->wc] = t;
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
        char *t = (char*) checked_malloc (2);
        t[0] = '&';
        t[1] = '&';
        stream->token[stream->wc] = t;
      if (++stream->wc == word_count)
        stream->token = checked_grow_alloc (stream->token, &word_count);
      buffer++;
      count += 2;
      }
      else
      {
        char *t = (char*) checked_malloc (1);
        t[0] = '&';
        stream->token[stream->wc] = t;
      if (++stream->wc == word_count)
        stream->token = checked_grow_alloc (stream->token, &word_count);
      count++;
      }
    }
    else if (c == '|')
    {
      if (*(++buffer) == '|')
      {
        char *t = (char*) checked_malloc (2);
        t[0] = '|';
        t[1] = '|';
        stream->token[stream->wc] = t;
      if (++stream->wc == word_count)
        stream->token = checked_grow_alloc (stream->token, &word_count);
      buffer++;
      count += 2;
      }
      else
      {
        char *t = (char*) checked_malloc (1);
        t[0] = '|';
        stream->token[stream->wc] = t;
      if (++stream->wc == word_count)
        stream->token = checked_grow_alloc (stream->token, &word_count);
      count++;
      }
    }
    else if (isword(c))
    {
      size_t word_size = 8;
      size_t n = 0;
      char *t = (char*) checked_malloc (8);
      do
      {
        t[n++] = c;

        if (n == word_size)
          t = checked_grow_alloc (t, &word_size);
        buffer++;
        count++;
        c = *buffer;
  
      } while (isword(c));
      stream->token[stream->wc] = t;
      if (++stream->wc == word_count)
        stream->token = checked_grow_alloc (stream->token, &word_count);
 //     printf("be to break %zd\n", count);
    
    }
  }
  printf ("total words %zd\n", stream->wc);
// size_t i=0;
//     for(;i < stream->wc;i++)
//     printf ("%s\n", stream->token[i]);

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

typedef struct{
 int top;
 char* data[STACK_MAX];
} stack;

void init (stack *st)
{
 st->top=0;
}

void push (stack *s, char* cdata)
{
 if (s->top==STACK_MAX) {
  fprintf(stderr,"stack is full");
  exit(1);
 }

 s->data[s->top]=cdata;
  s->top++;
}

char* peek (stack *s)
{
  return s->data[s->top];
}

char* pop (stack *s) 
{
 if (s->top==0) 
 {
  fprintf(stderr,"empty stack");
  exit(-1);
 }
 else
  s->top--;
 return s->data[s->top];
}

int isEqual(char* a, char* b)
{
  int result = strcmp(a,b)?0:1;
  return result;
}

int isLeftPop (char* a, char* b)
{
  if (isEqual (a, "<") || isEqual (a, ">"))
    return 1;
  else if (isEqual (b, "<") || isEqual (b, ">"))
    return 0;
  else
    return 1;
}

char* createSingleChar (char x)
{
  char* t = (char*) checked_malloc (1);
        t[0] = x;
  return t;
}
command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  size_t count = 0;
  char* cur_token = NULL;
 // char* pre_token = NULL;
  stack word_stack, op_stack, isCmd_stack;
  init (&word_stack); init (&op_stack); init (&isCmd_stack);
  char status = 'o';
    size_t word_count = 0;
  while (count <= s->wc)
  {
    printf ("count:%zd, wc:%zd\n", count, s->wc);
    cur_token = *(s->token);
    if (isword(*cur_token))
    {
      printf("%c\n", *cur_token);
      if (status == 'o' )
      {
        push (&word_stack, cur_token);
        
        push (&isCmd_stack, createSingleChar('n'));
        status = 'w';
        count++;
      }
      else if (status == 'w')
      {
        if (*peek(&isCmd_stack)=='n')
        {
          command_t cmd = (command_t) checked_malloc (24);
          cmd-> type = SIMPLE_COMMAND;
          cmd-> input = NULL;
          cmd-> output = NULL;
          cmd->u.word = (char**) checked_malloc(4); //to be implement if more than 4 words
          *(cmd->u.word) = pop(&word_stack);
          *(++cmd->u.word) = cur_token;
          push (&word_stack, (char*) cmd);
          pop(&isCmd_stack);
          push(&isCmd_stack, createSingleChar('y'));
        }
        else if (*peek(&isCmd_stack) == 'y')
        {
          command_t cmd_dup = (command_t) checked_malloc (24);
          cmd_dup = (command_t)pop(&word_stack);
          *(++cmd_dup->u.word) = cur_token;
          push(&word_stack, (char*) cmd_dup);
        }
      }

    }
     else
      return NULL; 
  }
  command_t cmd = (command_t) checked_malloc (24);
          cmd = (command_t)pop(&word_stack);
      return cmd;
}
