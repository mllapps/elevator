/**
 * @file syscalls.c
 * @author fl0mll
 * @date 2018/02/18
 *
 * This document contains proprietary information belonging to mllapps.com
 * Passing on and copying of this document, use and communication of its
 * contents is not permitted without prior written authorization.
 *
 * @brief System call function implementation
 */
#include  <errno.h>
#include  <sys/unistd.h> /* STDOUT_FILENO, STDERR_FILENO */

#ifdef STM32F103xB
#include <stm32f103xb.h>
#endif

/**
 * @brief Overwritten function to use printf
 *
 * @param file File handle
 * @param data String message to write
 * @param len String length
 *
 * @return The number of bytes written
 */
int _write(int file, char *ptr, int len)
{
  /* Implement your write code here, this is used by puts and printf for example */
  int i=0;
  for(i=0 ; i<len ; i++)
    ITM_SendChar((*ptr++));
  return len;
}
