 
// Include the header file to get access to the MicroPython API
#include "py/dynruntime.h"
#include "arithmetic.h"
#include "ec.h"


// This is the function which will be called from Python
STATIC mp_obj_t x25519(mp_obj_t k, mp_obj_t u) {
    mp_buffer_info_t bufinfo_k, bufinfo_u;
    
    mp_get_buffer_raise(k, &bufinfo_k, MP_BUFFER_READ);
    mp_get_buffer_raise(u, &bufinfo_u, MP_BUFFER_READ);
    
    uint32_t q[9];
    X25519(q, 0, bufinfo_k.buf, (uint32_t*)bufinfo_u.buf, 0, 0);

    return mp_obj_new_bytes((uint8_t *)q, 8*4);
}
// Define a Python reference to the function above
STATIC MP_DEFINE_CONST_FUN_OBJ_2(x25519_obj, x25519);


// This is the function which will be called from Python
STATIC mp_obj_t x25519_ed(mp_obj_t k, mp_obj_t u, mp_obj_t v) {
    mp_obj_t items[2];
    mp_buffer_info_t bufinfo_k, bufinfo_u, bufinfo_v;
    
    mp_get_buffer_raise(k, &bufinfo_k, MP_BUFFER_READ);
    mp_get_buffer_raise(u, &bufinfo_u, MP_BUFFER_READ);
    mp_get_buffer_raise(v, &bufinfo_v, MP_BUFFER_READ);
    
    uint32_t q[9], r[9];
    X25519(q, r, bufinfo_k.buf, (uint32_t*)bufinfo_u.buf, (uint32_t*)bufinfo_v.buf, 1);

    items[0] = mp_obj_new_bytes((uint8_t *)q, 8*4);
    items[1] = mp_obj_new_bytes((uint8_t *)r, 8*4);
    return mp_obj_new_tuple(2, items);
}
// Define a Python reference to the function above
STATIC MP_DEFINE_CONST_FUN_OBJ_3(x25519_ed_obj, x25519_ed);


// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    // Make the function available in the module's namespace
    mp_store_global(MP_QSTR_x25519, MP_OBJ_FROM_PTR(&x25519_obj));
    // Make the function available in the module's namespace
    mp_store_global(MP_QSTR_x25519_ed, MP_OBJ_FROM_PTR(&x25519_ed_obj));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}
