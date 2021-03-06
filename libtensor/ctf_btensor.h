#ifndef LIBTENSOR_CTF_BTENSOR_H
#define LIBTENSOR_CTF_BTENSOR_H

#if defined(WITH_CTF)

#define WITH_CTF_BTENSOR
#include "ctf_dense_tensor/ctf.h"
#include "ctf_block_tensor/ctf_btod_collect.h"
#include "ctf_block_tensor/ctf_btod_distribute.h"
#include "expr/ctf_btensor/ctf_btensor.h"

#endif

#endif // LIBTENSOR_CTF_BTENSOR_H
