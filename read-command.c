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

int isLeftPrior(enum command_type type1, enum command_type type2){
  int priority[4] = {2,1,2,3};
  return (priority[type1]>priority[type2]);
}

int isEqual(char* a, char* b)
{
  return strcmp(a,b)?0:1;
}

int 
isConnToken(char* token)
{
  return (isEqual(token,"|") ||
    isEqual(token,"||") || 
    isEqual(token,"&&"));
}

int 
isWordToken(char* token)
{
  return (!isConnToken(token) 
    && !isEqual(token,"<") 
    && !isEqual(token,">") 
    && !isEqual(token,"\n") 
    && !isEqual(token,";") 
    && !isEqual(token,"(") 
    && !isEqual(token,")"));
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
complete_command(command_t curCmd, command_t *cmdBuffer){
  command_t cmd_finished = curCmd;  
  if(*cmdBuffer != NULL)
  {
    (*cmdBuffer)->u.command[1] = curCmd;
    command_t cmd_left = (*cmdBuffer)->u.command[0];
    if(cmd_left->type != SIMPLE_COMMAND && cmd_left->type != SUBSHELL_COMMAND)  
    {
      if(isLeftPrior((*cmdBuffer)->type, cmd_left->type))
      {
        //switch order
        (*cmdBuffer)->u.command[0] = cmd_left->u.command[1];
        cmd_left->u.command[1] = *cmdBuffer;
        cmd_finished = cmd_left;
      }
      else
      {
        cmd_finished = *cmdBuffer;
      }
    }
    else
    {
      cmd_finished = *cmdBuffer;
    }
  }
  free(cmdBuffer);
  return cmd_finished;
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
push_command_buffer(command_t curCmd, char* token){
  command_t tempcmd =  checked_malloc(sizeof(struct command));
  if(isEqual(token,";"))
    tempcmd->type = SEQUENCE_COMMAND;
  else if(isEqual(token,"||"))
    tempcmd->type = OR_COMMAND;
  else if(isEqual(token,"|"))
    tempcmd->type = PIPE_COMMAND;
  else if(isEqual(token,"&&"))
    tempcmd->type = AND_COMMAND;
  else
  tempcmd->type = -1;

  tempcmd->u.command[0]=curCmd;
  return tempcmd;
}

command_t parse_subshell(command_stream_t);
command_t 
parse_command_stream (command_stream_t s, int isSub)
{
    /* FIXME: Replace this with your implementation too.  */
  size_t count = 0;
  int cur_line_num=0;
  char* cur_token = NULL;
  command_t curCmd = checked_malloc(sizeof(struct command));
  curCmd->type = SIMPLE_COMMAND;
  command_t cmdBuffer = NULL;

  int haveCmd = 0;

  size_t curCmdWordIndex;
  size_t curCmdWordMax = 32;


  enum command_state state = INIT_STATUS; 

  while (count <= s->wc)
  {
    cur_token = s->token[count];
    count++;


    if(isEqual(cur_token, "\n"))
      cur_line_num++;

    switch(state)
    {

      case INIT_STATUS:
      if(isEqual(cur_token, "("))
      {
        curCmd = parse_subshell(s);
        print_command(curCmd);
        state = SUBSHELL_CMD_FINISH;
      }
      else if (isEqual(cur_token, ")"))
      {
        if(haveCmd!=0)
         {//inside subshell and already have command in it
            curCmd = complete_command(curCmd,&cmdBuffer);
           return curCmd;
         }
        // else if ; is the end symbol
        else 
         {
            fprintf (stderr, "%d: error", cur_line_num);
           exit(1);
          }
      }
      else if(isEqual(cur_token, "\n"))
      {
        state = INIT_STATUS;
      }
      else if (isWordToken(cur_token))
      {
        if(haveCmd!=0 && cmdBuffer == NULL)
        {
            curCmd = complete_command(curCmd,&cmdBuffer);
            cmdBuffer = push_command_buffer(curCmd,";");
            curCmd = init_command();
           //haveCmd = 0;
        }
        curCmdWordIndex = 0;
        curCmd->u.word = checked_malloc(curCmdWordMax);      
        curCmd->u.word[curCmdWordIndex] = cur_token;
        curCmd->u.word[++curCmdWordIndex] = NULL;//may useless
         
        state = SIMPLE_CMD_STATUS;
      }
      else
      {
        fprintf (stderr, "%d: error", cur_line_num);
        exit(1);
      }

      break;

      case SIMPLE_CMD_STATUS:

      if((isEqual(cur_token,")"))&&(isSub != 0))
      {
        curCmd = complete_command(curCmd,&cmdBuffer);
        return curCmd; 
      }
      else if(isEqual(cur_token,"\n")||isEqual(cur_token,";"))
      {
        if(isSub != 0)
        {
          curCmd = complete_command(curCmd,&cmdBuffer);
          state = INIT_STATUS;
          haveCmd = 1;
        }
        else
        {
          curCmd = complete_command(curCmd,&cmdBuffer);
          state = INIT_STATUS;
          return curCmd;
        }
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

        state = SIMPLE_CMD_STATUS;
        if(curCmdWordIndex + 1>= curCmdWordMax)
        {
          curCmd->u.word = checked_grow_alloc(curCmd->u.word,&curCmdWordMax);
        }
        curCmd->u.word[curCmdWordIndex] = cur_token;
        curCmd->u.word[++curCmdWordIndex] = NULL;
      } 
      else if(isConnToken(cur_token))
      {
        curCmd = complete_command(curCmd,&cmdBuffer);
        cmdBuffer = push_command_buffer(curCmd, cur_token);
        curCmd = init_command();
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
        curCmd->input = cur_token;
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
        curCmd->output = cur_token;
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
        curCmd = complete_command(curCmd,&cmdBuffer);

        return curCmd; 
      }
      else if(isEqual(cur_token,">"))
      {
        state = OUT_CMD_STATUS;
      }
      else if(isEqual(cur_token,"\n")||isEqual(cur_token,";"))
      {
        if(isSub == 0 )
        {
          curCmd = complete_command(curCmd,&cmdBuffer);
          return curCmd;
          state = INIT_STATUS;
        }
        else
        {
          curCmd = complete_command(curCmd,&cmdBuffer);
          state = INIT_STATUS;
          haveCmd = 1;
        }
      }
      else if(isConnToken(cur_token))
      {
        curCmd = complete_command(curCmd,&cmdBuffer);
        cmdBuffer = push_command_buffer(curCmd, cur_token);     
        curCmd = init_command();
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
        curCmd = complete_command(curCmd,&cmdBuffer);

        return curCmd; 
      }
      else if(isEqual(cur_token,"\n")||isEqual(cur_token,";"))
      {
        if(isSub == 0)
        {
          curCmd = complete_command(curCmd,&cmdBuffer);
          state = INIT_STATUS;
          return curCmd;
        }
        else // inside subshell;
        {
          curCmd = complete_command(curCmd,&cmdBuffer);
          state = INIT_STATUS;
          haveCmd = 1;
        }
      }
      else if(isConnToken(cur_token))
      {
        curCmd = complete_command(curCmd,&cmdBuffer);
        cmdBuffer = push_command_buffer(curCmd, cur_token);                 
        curCmd = init_command();
        state = INIT_STATUS;
      }
      else
      {
        fprintf (stderr, "%d: error", cur_line_num);
        exit(1);
      }
        break;

      case SUBSHELL_CMD_FINISH:
      if((isEqual(cur_token,")"))&&(isSub != 0))
      {                    
        curCmd = complete_command(curCmd,&cmdBuffer);
        state = INIT_STATUS;
        return curCmd;                         
      }
      else if(isEqual(cur_token,"\n"))
      {
        if(isSub != 0)
        {
          curCmd = complete_command(curCmd,&cmdBuffer);
          cmdBuffer = push_command_buffer(curCmd, ";");                 
          curCmd = init_command();
        }
        else
        {
          curCmd = complete_command(curCmd,&cmdBuffer);
          state = INIT_STATUS;
          return curCmd;
        }
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
        curCmd = complete_command(curCmd,&cmdBuffer);
        cmdBuffer = push_command_buffer(curCmd, cur_token);
        curCmd = init_command();
        state = INIT_STATUS;
      }
      else if(count >= s->wc)
      {
        curCmd = complete_command(curCmd,&cmdBuffer);

        return curCmd;    
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
  if(isSub==0)
  {
    if((state == IN_CMD_STATUS_FINISH ||
      state == OUT_CMD_STATUS_FINISH ||
      state == SIMPLE_CMD_STATUS  ||
      state == SUBSHELL_CMD_FINISH
      ))
    {
      curCmd = complete_command(curCmd,&cmdBuffer);
      return curCmd;
    }
    else if(state == INIT_STATUS)
    {
      if(cmdBuffer!=NULL)
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
  }
  else
  {
    fprintf (stderr, "%d: error", cur_line_num);
    exit(1);
  }
  return 0;
}

command_t 
parse_subshell(command_stream_t s)
{
    command_t subCmd = checked_malloc(sizeof(struct command));
    subCmd->type = SUBSHELL_COMMAND;
    subCmd->u.subshell_command = parse_command_stream(s, 1);
//   printf("in parse_subshell, print out subshell command\n");
//   print_command(subCmd->u.subshell_command);
    
    return subCmd;
}

command_t
read_command_stream (command_stream_t s)
{
  return parse_command_stream(s, 0);
}

