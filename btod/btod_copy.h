#ifndef LIBTENSOR_BTOD_COPY_H
#define LIBTENSOR_BTOD_COPY_H

#include <map>
#include "defs.h"
#include "exception.h"
#include "btod/btod_additive.h"
#include "tod/tod_copy.h"

namespace libtensor {

/**	\brief Makes a copy of a block %tensor, applying a permutation or
		a scaling coefficient
	\tparam N Tensor order.

	\ingroup libtensor_btod
 **/
template<size_t N>
class btod_copy : public btod_additive<N> {
public:
	static const char *k_clazz; //!< Class name

private:
	block_tensor_i<N, double> &m_bt; //!< Source block %tensor
	permutation<N> m_perm; //!< Permutation
	double m_c; //!< Scaling coefficient
	block_index_space<N> m_bis; //!< Block %index space of output
	dimensions<N> m_bidims; //!< Block %index dimensions
	symmetry<N, double> m_symmetry; //!< Symmetry of output

public:
	//!	\name Construction and destruction
	//@{

	/**	\brief Initializes the copy operation
		\param bt Source block %tensor.
		\param c Scaling coefficient.
	 **/
	btod_copy(block_tensor_i<N, double> &bt, double c = 1.0);

	/**	\brief Initializes the permuted copy operation
		\param bt Source block %tensor.
		\param p Permutation.
		\param c Scaling coefficient.
	 **/
	btod_copy(block_tensor_i<N, double> &bt, const permutation<N> &p,
		double c = 1.0);

	/**	\brief Virtual destructor
	 **/
	virtual ~btod_copy();
	//@}

	//!	\name Implementation of
	//!		libtensor::direct_block_tensor_operation<N, double>
	//@{
	virtual const block_index_space<N> &get_bis() const;
	virtual const symmetry<N, double> &get_symmetry() const;
	virtual void perform(block_tensor_i<N, double> &bt) throw(exception);
	//@}

	//!	\name Implementation of libtensor::btod_additive<N>
	//@{
	virtual void perform(block_tensor_i<N, double> &bt, double c)
		throw(exception);
	//@}

};


template<size_t N>
const char *btod_copy<N>::k_clazz = "btod_copy<N>";


template<size_t N>
btod_copy<N>::btod_copy(block_tensor_i<N, double> &bt, double c)
: m_bt(bt), m_c(c), m_bis(bt.get_bis()), m_bidims(m_bis.get_block_index_dims()),
	m_symmetry(m_bidims) {

}


template<size_t N>
btod_copy<N>::btod_copy(
	block_tensor_i<N, double> &bt, const permutation<N> &p, double c)
: m_bt(bt), m_perm(p), m_c(c), m_bis(bt.get_bis()),
	m_bidims(m_bis.get_block_index_dims()), m_symmetry(m_bidims) {

	m_bis.permute(m_perm);
	m_bidims.permute(m_perm);
	m_symmetry.permute(m_perm);
}


template<size_t N>
btod_copy<N>::~btod_copy() {

}


template<size_t N>
inline const block_index_space<N> &btod_copy<N>::get_bis() const {

	return m_bis;
}


template<size_t N>
inline const symmetry<N, double> &btod_copy<N>::get_symmetry() const {

	return m_symmetry;
}


template<size_t N>
void btod_copy<N>::perform(block_tensor_i<N, double> &bt) throw(exception) {

	static const char *method = "perform(block_tensor_i<N, double>&)";

	if(!m_bis.equals(bt.get_bis())) {
		throw bad_parameter("libtensor", k_clazz, method,
			__FILE__, __LINE__,
			"Incorrect block index space of the output tensor.");
	}

	block_tensor_ctrl<N, double> src_ctrl(m_bt), dst_ctrl(bt);
	dimensions<N> bidims = m_bis.get_block_index_dims();

	dst_ctrl.req_zero_all_blocks();
	dst_ctrl.req_sym_clear_elements();
	size_t n_sym_elem = m_symmetry.get_num_elements();
	for(size_t ielem = 0; ielem < n_sym_elem; ielem++) {
		dst_ctrl.req_sym_add_element(m_symmetry.get_element(ielem));
	}

	size_t norbits = src_ctrl.req_sym_num_orbits();
	for(size_t iorbit = 0; iorbit < norbits; iorbit++) {

		orbit<N, double> orb = src_ctrl.req_sym_orbit(iorbit);
		index<N> src_blk_idx;
		bidims.abs_index(orb.get_abs_index(), src_blk_idx);
		if(src_ctrl.req_is_zero_block(src_blk_idx)) continue;
		index<N> dst_blk_idx(src_blk_idx);
		dst_blk_idx.permute(m_perm);

		tensor_i<N, double> &src_blk = src_ctrl.req_block(src_blk_idx);
		tensor_i<N, double> &dst_blk = dst_ctrl.req_block(dst_blk_idx);
		tod_copy<N> cp(src_blk, m_perm, m_c);
		cp.perform(dst_blk);
		src_ctrl.ret_block(src_blk_idx);
		dst_ctrl.ret_block(dst_blk_idx);

	}

}


template<size_t N>
void btod_copy<N>::perform(block_tensor_i<N, double> &bt, double c)
	throw(exception) {
	block_tensor_ctrl<N, double> ctrl_src(m_bt), ctrl_dst(bt);
	index<N> i0;
	tensor_i<N, double> &tsrc(ctrl_src.req_block(i0));
	tensor_i<N, double> &tdst(ctrl_dst.req_block(i0));
	tod_copy<N> op(tsrc, m_perm, m_c);
	op.perform(tdst, c);
}


} // namespace libtensor

#endif // LITENSOR_BTOD_COPY_H
