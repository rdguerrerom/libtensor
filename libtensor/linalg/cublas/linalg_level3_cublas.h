#ifndef LIBTENSOR_LINALG_LEVEL3_CUBLAS_H
#define LIBTENSOR_LINALG_LEVEL3_CUBLAS_H

#include <libtensor/timings.h>

namespace libtensor {


/** \brief Level-3 linear algebra operations (cuBLAS)

    \ingroup libtensor_linalg
 **/
class linalg_level3_cublas : public timings<linalg_level3_cublas> {
public:
    static const char *k_clazz; //!< Class name

public:
    static void mul2_ij_ip_jp_x(
        cublasHandle_t h,
        size_t ni, size_t nj, size_t np,
        const double *a, size_t sia,
        const double *b, size_t sjb,
        double *c, size_t sic,
        double d);

    static void mul2_ij_ip_pj_x(
        cublasHandle_t h,
        size_t ni, size_t nj, size_t np,
        const double *a, size_t sia,
        const double *b, size_t spb,
        double *c, size_t sic,
        double d);

    static void mul2_ij_pi_jp_x(
        cublasHandle_t h,
        size_t ni, size_t nj, size_t np,
        const double *a, size_t spa,
        const double *b, size_t sjb,
        double *c, size_t sic,
        double d);

    static void mul2_ij_pi_pj_x(
        cublasHandle_t h,
        size_t ni, size_t nj, size_t np,
        const double *a, size_t spa,
        const double *b, size_t spb,
        double *c, size_t sic,
        double d);

};


} // namespace libtensor

#endif // LIBTENSOR_LINALG_LEVEL3_CUBLAS_H