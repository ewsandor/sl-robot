/*
  sl_cr_circular_buffer.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#include "sl_cr_circular_buffer.hpp"

#include "sl_cr_utils.hpp"

template <typename T>
sl_cr_circular_buffer_c<T>::sl_cr_circular_buffer_c(sl_cr_circular_buffer_index_t constructor_size)
  : buffer_size(constructor_size)
{
  circular_buffer = (sl_cr_buffer_entry_s*) sl_cr_malloc(buffer_size*sizeof(sl_cr_buffer_entry_s));
  write_index = 0;
  read_index  = 0;
}
template <typename T>
sl_cr_circular_buffer_c<T>::~sl_cr_circular_buffer_c()
{
  sl_cr_free((void*)circular_buffer);
}

/* Push item into buffer (by copy), returns 'true' if successful.  
    Data will only be overwritten if force is set,  */
template <typename T>
bool sl_cr_circular_buffer_c<T>::push(const T* input, bool force)
{
  bool ret_val = false;
  sl_cr_buffer_entry_s *write_entry = &circular_buffer[write_index];

  if((SL_CR_BUFFER_ENTRY_AVAILABLE == write_entry->hdr.state) ||
     (true == force))
  {
    write_entry->data      = *input;
    write_entry->hdr.state = SL_CR_BUFFER_ENTRY_COMMITED;
    write_index = (write_index+1) % buffer_size;
  }

  return ret_val;
}
/* Returns pointer to next available entry or 'nullpointer' if no free entries.  Pointer remains writable and cannot be read until commited.  */
template <typename T>
T* sl_cr_circular_buffer_c<T>::allocate()
{
  T* ret_ptr = nullptr;
  sl_cr_buffer_entry_s *write_entry = &circular_buffer[write_index];

  if(SL_CR_BUFFER_ENTRY_AVAILABLE == write_entry->hdr.state)
  {
    ret_ptr = &write_entry->data;
    write_entry->hdr.state = SL_CR_BUFFER_ENTRY_ALLOCATED;
    write_index = (write_index+1) % buffer_size;
  }

  return ret_ptr;
}
/* Commits allocated entry for reading.  Returns 'true' if successful */
template <typename T>
bool sl_cr_circular_buffer_c<T>::commit(const T* commit_data)
{
  bool ret_val = false;
  sl_cr_circular_buffer_index_t i;

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

  return ret_val;
}

/* Returns 'true' if data is available */
template <typename T>
bool sl_cr_circular_buffer_c<T>::available()
{
  return (SL_CR_BUFFER_ENTRY_COMMITED == circular_buffer[read_index].hdr.state);
}
/* Reads and frees next buffer entry. Returns 0-memset data and nothing is freed if nothing to pop*/
template <typename T>
T sl_cr_circular_buffer_c<T>::pop()
{
  if(SL_CR_BUFFER_ENTRY_COMMITED == circular_buffer[read_index].hdr.state)
  {
    T ret_val = circular_buffer[read_index].data;
    circular_buffer[read_index].hdr.state = SL_CR_BUFFER_ENTRY_AVAILABLE;
    circular_buffer[read_index].data = {0};
    read_index = (read_index + 1) % buffer_size;

    return ret_val;
  }

  return {0};
}
/* Frees next buffer entry without returning data.  Does nothing if nothing to pop */
template <typename T>
void sl_cr_circular_buffer_c<T>::pop_void()
{
  if(SL_CR_BUFFER_ENTRY_COMMITED == circular_buffer[read_index].hdr.state)
  {
    circular_buffer[read_index].hdr.state = SL_CR_BUFFER_ENTRY_AVAILABLE;
    circular_buffer[read_index].data = {0};
    read_index = (read_index + 1) % buffer_size;
  }
}
/* Returns next buffer entry without freeing. Returns nullptr if no data is available.  Data remains valid until pop is called */
template <typename T>
const T* sl_cr_circular_buffer_c<T>::peek_ptr()
{
  const T* ret_ptr = nullptr;

  if(SL_CR_BUFFER_ENTRY_COMMITED == circular_buffer[read_index].hdr.state)
  {
    ret_ptr = &circular_buffer[read_index].data;
  }

  return ret_ptr;
}
/* Returns next buffer entry without freeing, returns 0-memset data if no data is available  */
template <typename T>
T sl_cr_circular_buffer_c<T>::peek()
{
  if(SL_CR_BUFFER_ENTRY_COMMITED == circular_buffer[read_index].hdr.state)
  {
    return circular_buffer[read_index].data;
  }

  return {0};
}
