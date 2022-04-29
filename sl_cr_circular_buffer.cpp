/*
  sl_cr_circular_buffer.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#include "sl_cr_circular_buffer.hpp"

using namespace sandor_laboratories::combat_robot;

#define MUTEX_LOCK   if(mutex){mutex_lock(mutex);}
#define MUTEX_UNLOCK if(mutex){mutex_unlock(mutex);}

template <typename T>
circular_buffer_c<T>::circular_buffer_c(circular_buffer_index_t constructor_size, bool mutexed)
  : buffer_size(constructor_size)
{
  circular_buffer = (buffer_entry_s*) heap_malloc(buffer_size*sizeof(buffer_entry_s));
  write_index = 0;
  read_index  = 0;

  if(mutexed)
  {
    mutex = (mutex_handle_t*) heap_malloc(sizeof(mutex_handle_t));
    mutex_init(mutex);
  }
  else
  {
    mutex = nullptr;
  }
}
template <typename T>
circular_buffer_c<T>::~circular_buffer_c()
{
  heap_free((void*)circular_buffer);
  if(mutex)
  {
    heap_free(mutex);
  }
}

/* Push item into buffer (by copy), returns 'true' if successful.  
    Data will only be overwritten if force is set,  */
template <typename T>
bool circular_buffer_c<T>::push(const T* input, bool force)
{
  bool ret_val = false;

  MUTEX_LOCK  
  buffer_entry_s *write_entry = &circular_buffer[write_index];

  if((SL_CR_BUFFER_ENTRY_AVAILABLE == write_entry->hdr.state) ||
     (true == force))
  {
    write_entry->data      = *input;
    write_entry->hdr.state = SL_CR_BUFFER_ENTRY_COMMITED;
    write_index = (write_index+1) % buffer_size;
  }
  MUTEX_UNLOCK  

  return ret_val;
}
/* Returns pointer to next available entry or 'nullpointer' if no free entries.  Pointer remains writable and cannot be read until commited.  */
template <typename T>
T* circular_buffer_c<T>::allocate()
{
  T* ret_ptr = nullptr;

  MUTEX_LOCK  
  buffer_entry_s *write_entry = &circular_buffer[write_index];

  if(SL_CR_BUFFER_ENTRY_AVAILABLE == write_entry->hdr.state)
  {
    ret_ptr = &write_entry->data;
    write_entry->hdr.state = SL_CR_BUFFER_ENTRY_ALLOCATED;
    write_index = (write_index+1) % buffer_size;
  }
  MUTEX_UNLOCK  

  return ret_ptr;
}
/* Commits allocated entry for reading.  Returns 'true' if successful */
template <typename T>
bool circular_buffer_c<T>::commit(const T* commit_data)
{
  bool ret_val = false;
  circular_buffer_index_t i;

  MUTEX_LOCK  
  for(i = 0; i < buffer_size; i++)
  {
    if((&circular_buffer[i].data == commit_data) &&
       (SL_CR_BUFFER_ENTRY_ALLOCATED == circular_buffer[i].hdr.state))
    {
      circular_buffer[i].hdr.state = SL_CR_BUFFER_ENTRY_COMMITED;
      ret_val = true;
      break;
    }
  }
  MUTEX_UNLOCK

  return ret_val;
}

/* Returns 'true' if data is available */
template <typename T>
bool circular_buffer_c<T>::available()
{
  MUTEX_LOCK  
  bool ret_val = (SL_CR_BUFFER_ENTRY_COMMITED == circular_buffer[read_index].hdr.state);
  MUTEX_UNLOCK  

  return ret_val;
}
/* Reads and frees next buffer entry. Returns 0-memset data and nothing is freed if nothing to pop*/
template <typename T>
T circular_buffer_c<T>::pop()
{
  T ret_val = {0};

  MUTEX_LOCK  
  if(SL_CR_BUFFER_ENTRY_COMMITED == circular_buffer[read_index].hdr.state)
  {
    ret_val = circular_buffer[read_index].data;
    memset(&circular_buffer[read_index], 0, sizeof(circular_buffer[read_index]));
    read_index = (read_index + 1) % buffer_size;
  }
  MUTEX_UNLOCK  

  return ret_val;
}
/* Frees next buffer entry without returning data.  Does nothing if nothing to pop */
template <typename T>
void circular_buffer_c<T>::pop_void()
{
  MUTEX_LOCK  
  if(SL_CR_BUFFER_ENTRY_COMMITED == circular_buffer[read_index].hdr.state)
  {
    memset(&circular_buffer[read_index], 0, sizeof(circular_buffer[read_index]));
    read_index = (read_index + 1) % buffer_size;
  }
  MUTEX_UNLOCK  
}
/* Returns next buffer entry without freeing. Returns nullptr if no data is available.  Data remains valid until pop is called */
template <typename T>
const T* circular_buffer_c<T>::peek_ptr()
{
  const T* ret_ptr = nullptr;

  MUTEX_LOCK  
  if(SL_CR_BUFFER_ENTRY_COMMITED == circular_buffer[read_index].hdr.state)
  {
    ret_ptr = &circular_buffer[read_index].data;
  }
  MUTEX_UNLOCK  

  return ret_ptr;
}
/* Returns next buffer entry without freeing, returns 0-memset data if no data is available  */
template <typename T>
T circular_buffer_c<T>::peek()
{
  T ret_val = {0};

  MUTEX_LOCK  
  if(SL_CR_BUFFER_ENTRY_COMMITED == circular_buffer[read_index].hdr.state)
  {
    ret_val = circular_buffer[read_index].data;
  }
  MUTEX_UNLOCK

  return ret_val;
}
