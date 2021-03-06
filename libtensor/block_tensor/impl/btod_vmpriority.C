#include <libtensor/gen_block_tensor/impl/gen_bto_vmpriority_impl.h>
#include "../btod_vmpriority.h"

namespace libtensor {


template class gen_bto_vmpriority<1, btod_traits>;
template class gen_bto_vmpriority<2, btod_traits>;
template class gen_bto_vmpriority<3, btod_traits>;
template class gen_bto_vmpriority<4, btod_traits>;
template class gen_bto_vmpriority<5, btod_traits>;
template class gen_bto_vmpriority<6, btod_traits>;
template class gen_bto_vmpriority<7, btod_traits>;
template class gen_bto_vmpriority<8, btod_traits>;


template class btod_vmpriority<1>;
template class btod_vmpriority<2>;
template class btod_vmpriority<3>;
template class btod_vmpriority<4>;
template class btod_vmpriority<5>;
template class btod_vmpriority<6>;
template class btod_vmpriority<7>;
template class btod_vmpriority<8>;


} // namespace libtensor
