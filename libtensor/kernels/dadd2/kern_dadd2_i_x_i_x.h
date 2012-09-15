#ifndef LIBTENSOR_KERN_DADD2_I_X_I_X_H
#define LIBTENSOR_KERN_DADD2_I_X_I_X_H

#include "../kern_dadd2.h"

namespace libtensor {


/** \brief Kernel for \f$ c_i = c_i + (a + b_i) d \f$
    \tparam LA Linear algebra.

     \ingroup libtensor_kernels
 **/
template<typename LA>
class kern_dadd2_i_x_i_x : public kernel_base<2, 1> {
public:
    static const char *k_clazz; //!< Kernel name

private:
    double m_ka, m_kb;
    double m_d;
    size_t m_ni;
    size_t m_sib, m_sic;

public:
    virtual ~kern_dadd2_i_x_i_x() { }

    virtual const char *get_name() const {
        return k_clazz;
    }

    virtual void run(const loop_registers<2, 1> &r);

    static kernel_base<2, 1> *match(const kern_dadd2<LA> &z,
        list_t &in, list_t &out);

};


} // namespace libtensor

#endif // LIBTENSOR_KERN_DADD2_I_X_I_X_H
