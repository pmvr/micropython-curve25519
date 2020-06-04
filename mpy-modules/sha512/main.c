 
// Include the header file to get access to the MicroPython API
#include "py/dynruntime.h"
#include "sha512.h"


// This is the function which will be called from Python, as factorial(x)
STATIC mp_obj_t digest(mp_obj_t data) {
    mp_buffer_info_t bufinfo_data;
    uint8_t out[64];
    
    mp_get_buffer_raise(data, &bufinfo_data, MP_BUFFER_READ);
    
    sha512_simple(bufinfo_data.buf, bufinfo_data.len, out);
    return mp_obj_new_bytes(out, 64);
}
// Define a Python reference to the function above
STATIC MP_DEFINE_CONST_FUN_OBJ_1(digest_obj, digest);

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    // Make the function available in the module's namespace
    mp_store_global(MP_QSTR_digest, MP_OBJ_FROM_PTR(&digest_obj));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}
