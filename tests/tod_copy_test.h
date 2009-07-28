#ifndef LIBTENSOR_TOD_COPY_TEST_H
#define LIBTENSOR_TOD_COPY_TEST_H

#include <cmath>
#include <cstdlib>
#include <libtest.h>
#include <libtensor.h>

namespace libtensor {

/**	\brief Tests the libtensor::tod_copy class

	\ingroup libtensor_tests
**/
class tod_copy_test : public libtest::unit_test {
public:
	virtual void perform() throw(libtest::test_exception);

private:
	/**	\brief Tests plain copying of a %tensor
	 **/
	template<size_t N>
	void test_plain(const dimensions<N> &dims)
		throw(libtest::test_exception);

	/**	\brief Tests plain copying of a %tensor (additive version)
	 **/
	template<size_t N>
	void test_plain_additive(const dimensions<N> &dims, double d)
		throw(libtest::test_exception);

	/**	\brief Tests scaled copying of a %tensor
	 **/
	template<size_t N>
	void test_scaled(const dimensions<N> &dims, double c)
		throw(libtest::test_exception);

	/**	\brief Tests scaled copying of a %tensor (additive version)
	 **/
	template<size_t N>
	void test_scaled_additive(const dimensions<N> &dims, double c, double d)
		throw(libtest::test_exception);

	/**	\brief Tests permuted copying of a %tensor
	 **/
	template<size_t N>
	void test_perm(const dimensions<N> &dims,
		const permutation<N> &perm) throw(libtest::test_exception);

	/**	\brief Tests permuted copying of a %tensor (additive version)
	 **/
	template<size_t N>
	void test_perm_additive(const dimensions<N> &dims,
		const permutation<N> &perm, double d)
		throw(libtest::test_exception);

	/**	\brief Tests permuted and scaled copying of a %tensor
	 **/
	template<size_t N>
	void test_perm_scaled(const dimensions<N> &dims,
		const permutation<N> &perm, double c)
		throw(libtest::test_exception);

	/**	\brief Tests permuted and scaled copying of a %tensor
			(additive version)
	 **/
	template<size_t N>
	void test_perm_scaled_additive(const dimensions<N> &dims,
		const permutation<N> &perm, double c, double d)
		throw(libtest::test_exception);

	/**	\brief Tests if an exception is throws when the tensors have
			different dimensions
	 **/
	void test_exc() throw(libtest::test_exception);

	template<size_t N>
	void compare_ref(const char *test, tensor_i<N, double> &t,
		tensor_i<N, double> &t_ref, double thresh)
		throw(libtest::test_exception);
};

} // namespace libtensor

#endif // LIBTENSOR_TOD_COPY_TEST_H
