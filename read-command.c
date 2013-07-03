// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include "alloc.h"

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

  command_stream_t stream = read_buffer (buffer, count);

  
  return stream;
}


int isEqual(char* a, char* b)
{
  return strcmp(a,b)?0:1;
}

int 
isConnToken(char* token)
{
  return (isEqual(token,"|") || isEqual(token,"&&"))
   || isEqual(token,"||");
}

int 
isWordToken(char* token)
{
  return (!isConnToken(token) && !isEqual(token,">") 
    && !isEqual(token,"<")  && !isEqual(token,"(") 
    && !isEqual(token,")"))&& !isEqual(token,";") 
    && !isEqual(token,"\n") ;   
}


enum command_state
{
  INIT_STATUS,
   SIMPLE_CMD_STATUS,
   IN_CMD_STATUS,
   IN_CMD_STATUS_FINISH,
   OUT_CMD_STATUS,
   OUT_CMD_STATUS_FINISH,
  SUBSHELL_CMD_FINISH,
};

command_t
switch_cmd_order(command_t cur_cmd, command_t *temp_cmd)
{
   command_t final_cmd = cur_cmd; 
  if(*temp_cmd == NULL)
    return NULL;    
  int priority[4] = {2,1,2,3};
 
  (*temp_cmd)->u.command[1] = cur_cmd;
  command_t cmd_left = (*temp_cmd)->u.command[0];
  final_cmd = *temp_cmd;

  if(cmd_left->type != SIMPLE_COMMAND && cmd_left->type != SUBSHELL_COMMAND)  
  {

    if(priority[(*temp_cmd)->type] > priority[cmd_left->type])
    {
      //switch order
      (*temp_cmd)->u.command[0] = cmd_left->u.command[1];
      cmd_left->u.command[1] = *temp_cmd;
      final_cmd = cmd_left;
    }
  }
  
  *temp_cmd = NULL;
  return final_cmd;
}

command_t
init_command(void)
{
  command_t cmd = checked_malloc(sizeof(struct command));
  cmd->type = SIMPLE_COMMAND;
  cmd->status = -1;
  cmd->input = 0;
  cmd->output = 0;
  return cmd;
}

command_t 
push_command_buffer(command_t cur_cmd, char* token){
  command_t tempcmd =  checked_malloc(sizeof(struct command));
  if(isEqual(token,";"))
    tempcmd->type = SEQUENCE_COMMAND;
  else if(isEqual(token,"&&"))
    tempcmd->type = AND_COMMAND;
  else if(isEqual(token,"||"))
    tempcmd->type = OR_COMMAND;
  else if(isEqual(token,"|"))
    tempcmd->type = PIPE_COMMAND;

  else
  tempcmd->type = -1;

  tempcmd->u.command[0]=cur_cmd;
  return tempcmd;
}

command_t
read_command_stream (command_stream_t s)
{
  size_t count = 0;
  int cur_line_num=0;
  char* cur_token = NULL;
  command_t cur_cmd = checked_malloc(sizeof(struct command));
  cur_cmd->type = SIMPLE_COMMAND;
  command_t temp_cmd = NULL;


  size_t cur_cmdWordIndex;
  size_t cur_cmdWordMax = 32;

  enum command_state state = INIT_STATUS; 

  while (s->index < s->wc)
  {
    printf ("count: %zd\n", s->index);
    cur_token = s->token[s->index];
    s->index++;

    printf("%s\n", cur_token);
    if(isEqual(cur_token, "\n"))
      cur_line_num++;

    switch(state)
    {

      case INIT_STATUS:

      if(isEqual(cur_token, "("))
      {
        state = SUBSHELL_CMD_FINISH;
      }
      else if (isEqual(cur_token, ")"))
      {
 
        fprintf (stderr, "%d: error", cur_line_num);
        exit(1);
      }
      else if(isEqual(cur_token, "\n"))
      {
        state = INIT_STATUS;
      }
      else if (isWordToken(cur_token))
      {
     //   printf("init status %s\n", cur_token);

        cur_cmdWordIndex = 0;
        cur_cmd->u.word = checked_malloc(cur_cmdWordMax);      
        cur_cmd->u.word[cur_cmdWordIndex] = cur_token;
        cur_cmd->u.word[++cur_cmdWordIndex] = NULL;
         
        state = SIMPLE_CMD_STATUS;
      }
      else
      {
        fprintf (stderr, "%d: error", cur_line_num);
        exit(1);
      }

      break;

      case SIMPLE_CMD_STATUS:

      if(isEqual(cur_token,"\n")||isEqual(cur_token,";"))
      {
        
          printf("2222\n");
          cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
          //state = INIT_STATUS;
          print_command(cur_cmd);
          return cur_cmd;
      }
      else if(isEqual(cur_token,"<"))
      {
        state = IN_CMD_STATUS;
      }
      else if(isEqual(cur_token, ">"))
      {
        state = OUT_CMD_STATUS;
      }
      else if(isWordToken(cur_token))
      {
        printf("simple_cmd_status %s\n", cur_token);
        state = SIMPLE_CMD_STATUS;
        if(cur_cmdWordIndex + 1>= cur_cmdWordMax)
        {
          cur_cmd->u.word = checked_grow_alloc(cur_cmd->u.word,&cur_cmdWordMax);
        }
        cur_cmd->u.word[cur_cmdWordIndex] = cur_token;
        cur_cmd->u.word[++cur_cmdWordIndex] = NULL;
      } 
      else if(isConnToken(cur_token))
      {
        cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
        temp_cmd = push_command_buffer(cur_cmd, cur_token);
        cur_cmd = init_command();
        state = INIT_STATUS;
      }
      else
      {
        fprintf (stderr, "%d: error", cur_line_num);
        exit(1);
      }
      break;

      case IN_CMD_STATUS:
      if(isWordToken(cur_token))
      {
        cur_cmd->input = cur_token;
        state = IN_CMD_STATUS_FINISH;  
      }
      else
      {
        fprintf (stderr, "%d: error", cur_line_num);
        exit(1);
      }
      break;

      case OUT_CMD_STATUS:
      if(isWordToken(cur_token))
      {
        cur_cmd->output = cur_token;
        state = OUT_CMD_STATUS_FINISH;
      }
      else
      {
        fprintf (stderr, "%d: error", cur_line_num);
        exit(1);
      }
      break;

      case IN_CMD_STATUS_FINISH:
      if(isEqual(cur_token,")"))
      {
        cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);

        return cur_cmd; 
      }
      else if(isEqual(cur_token,">"))
      {
        state = OUT_CMD_STATUS;
      }
      else if(isEqual(cur_token,"\n")||isEqual(cur_token,";"))
      {

          cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
          state = INIT_STATUS;
          return cur_cmd;
          
        
      }
      else if(isConnToken(cur_token))
      {
        cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
        temp_cmd = push_command_buffer(cur_cmd, cur_token);     
        cur_cmd = init_command();
        state = INIT_STATUS;
      }
      else
      {
        fprintf (stderr, "%d: error", cur_line_num);
        exit(1);
      }
      break;

      case OUT_CMD_STATUS_FINISH:              
      if(isEqual(cur_token,")"))
      {
        cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);

        return cur_cmd; 
      }
      else if(isEqual(cur_token,"\n")||isEqual(cur_token,";"))
      {

          cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
          state = INIT_STATUS;
          return cur_cmd;
  

      }
      else if(isConnToken(cur_token))
      {
        cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
        temp_cmd = push_command_buffer(cur_cmd, cur_token);                 
        cur_cmd = init_command();
        state = INIT_STATUS;
      }
      else
      {
        fprintf (stderr, "%d: error", cur_line_num);
        exit(1);
      }
        break;

      case SUBSHELL_CMD_FINISH:

      if(isEqual(cur_token,"\n"))
      {

          cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
          state = INIT_STATUS;
          return cur_cmd;
  
      }
      else if(isEqual(cur_token,"<"))
      {
        state = IN_CMD_STATUS;
      }
      else if(isEqual(cur_token, ">"))
      {
        state = OUT_CMD_STATUS;
      }
      else if(isConnToken(cur_token))
      {
        cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
        temp_cmd = push_command_buffer(cur_cmd, cur_token);
        cur_cmd = init_command();
        state = INIT_STATUS;
      }
      else if(s->index >= s->wc)
      {
        cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);

        return cur_cmd;    
      }                    

      else
      {
        fprintf (stderr, "%d: error", cur_line_num);
        exit(1);
      }

        break;
      default:
      fprintf (stderr, "%d: error", cur_line_num);
      exit(1);
    }


  }

  cur_line_num -= 1; // coz got additional \n from getc func

  if((state == IN_CMD_STATUS_FINISH || state == OUT_CMD_STATUS_FINISH ||
    state == SIMPLE_CMD_STATUS  || state == SUBSHELL_CMD_FINISH))
  {
    cur_cmd = switch_cmd_order(cur_cmd,&temp_cmd);
    return cur_cmd;
  }
  else if(state == INIT_STATUS)
  {
    if(temp_cmd!=NULL)
    { 
      fprintf (stderr, "%d: error", cur_line_num);
      exit(1);
    }
    else
      return 0; 
  }
  else
  {
    fprintf (stderr, "%d: error", cur_line_num);
    exit(1);
  } 
  return 0;

}

