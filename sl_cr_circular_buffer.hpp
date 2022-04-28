/*
  sl_cr_circular_buffer.hpp
  Sandor Laboratories Combat Robot Software
  Edward Sandor
  April 2022
*/

#ifndef __SL_CR_CIRCULAR_BUFFER_HPP__
#define __SL_CR_CIRCULAR_BUFFER_HPP__

#include "sl_cr_types.hpp"

typedef unsigned int sl_cr_circular_buffer_index_t;

template <typename T>
class sl_cr_circular_buffer_c
{
  private:
    typedef enum
    {
      SL_CR_BUFFER_ENTRY_AVAILABLE,
      SL_CR_BUFFER_ENTRY_ALLOCATED,
      SL_CR_BUFFER_ENTRY_COMMITED,

    } sl_cr_buffer_entry_state_e;

    typedef struct __attribute__((packed))
    {
      sl_cr_buffer_entry_state_e state:2;
      int pad:30;
    }
    sl_cr_buffer_entry_header_s;

    typedef struct __attribute__((packed))
    {
      sl_cr_buffer_entry_header_s hdr;
      T data;
    } sl_cr_buffer_entry_s;

    /* Config Data */
    const sl_cr_circular_buffer_index_t buffer_size;

    /* Buffer */
    sl_cr_buffer_entry_s* circular_buffer;

    /* Iterators */
    sl_cr_circular_buffer_index_t read_index;
    sl_cr_circular_buffer_index_t write_index;


  public:
    sl_cr_circular_buffer_c(sl_cr_circular_buffer_index_t size);
    ~sl_cr_circular_buffer_c();

    /* Push item into buffer (by copy), returns 'true' if successful.  
        Data will only be overwritten if force is set,  */
    bool     push(const T* input, bool force=false);
    /* Returns pointer to next available entry or 'nullpointer' if no free entries.  Pointer remains writable and cannot be read until commited.  */
    T*       allocate();
    /* Commits allocated entry for reading.  Returns 'true' if successful */
    bool     commit(const T*);

    /* Returns 'true' if data is available */
    bool     available();
    /* Reads and frees next buffer entry. Returns 0-memset data and nothing is freed if nothing to pop*/
    T        pop();
    /* Frees next buffer entry without returning data.  Does nothing if nothing to pop */
    void     pop_void();
    /* Returns next buffer entry without freeing. Returns nullptr if no data is available.  Data remains valid until pop is called */
    const T* peek_ptr();
    /* Returns next buffer entry without freeing, returns 0-memset data if no data is available  */
    T        peek();
};

template class sl_cr_circular_buffer_c<sl_cr_log_entry_s>;

#endif /* __SL_CR_CIRCULAR_BUFFER_HPP__ */