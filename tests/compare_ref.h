#ifndef LIBTENSOR_COMPARE_REF_H
#define LIBTENSOR_COMPARE_REF_H

#include <libtest.h>
#include <libtensor.h>

namespace libtensor {

template<size_t N>
class compare_ref {
public:
	static void compare(const char *test, tensor_i<N, double> &t,
		tensor_i<N, double> &t_ref, double thresh)
		throw(libtest::test_exception);
};

template<size_t N>
void compare_ref<N>::compare(const char *test, tensor_i<N, double> &t,
	tensor_i<N, double> &t_ref, double thresh)
	throw(libtest::test_exception) {

	tod_compare<N> cmp(t, t_ref, thresh);
	if(!cmp.compare()) {
		std::ostringstream ss1, ss2;
		ss2 << "In " << test << ": ";
		ss2 << "Result does not match reference at element "
			<< cmp.get_diff_index() << ": "
			<< cmp.get_diff_elem_1() << " (act) vs. "
			<< cmp.get_diff_elem_2() << " (ref), "
			<< cmp.get_diff_elem_1() - cmp.get_diff_elem_2()
			<< " (diff)";
		throw libtest::test_exception("compare_ref::compare()",
			__FILE__, __LINE__, ss2.str().c_str());
	}
}

} // namespace libtensor

#endif // LIBTENSOR_COMPARE_REF_H
