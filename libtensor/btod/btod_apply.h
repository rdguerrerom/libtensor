#ifndef LIBTENSOR_BTOD_APPLY_H
#define LIBTENSOR_BTOD_APPLY_H

#include <cmath>
#include <map>
#include "../defs.h"
#include "../exception.h"
#include "../timings.h"
#include "../core/abs_index.h"
#include "../core/orbit.h"
#include "../core/orbit_list.h"
#include "../symmetry/so_copy.h"
#include "../symmetry/so_permute.h"
#include "../tod/tod_apply.h"
#include "../tod/tod_copy.h"
#include "../tod/tod_set.h"
#include "bad_block_index_space.h"
#include "additive_btod.h"

namespace libtensor {


/**	\brief Applies a functor, a permutation and a scaling coefficient
		to each element of the input tensor.
	\tparam N Tensor order.

	The operation scales and permutes the input %tensor and then applies
	the functor to each element.
	The functor class needs to have
	1. a proper copy constructor
	  \code
		  Functor(const Functor &f);
	  \endcode
	2. an implementation of
	  \code
		  double Functor::operator()(const double &a);
	  \endcode
	3. and an implementation of
	  \code
		  template<size_t N>
		  void Functor::update_sym(symmetry<N, double> &sym);
	  \endcode

	The latter function is meant to update the symmetry of the result tensor
	with regard to changes the functor will imposed on the %tensor. E.g. if
	the functor is unsymmetric, any permutational antisymmetry should be
	removed from the result symmetry. The parameter sym can be expected to
	be the symmetry of the result %tensor, as if the operation would be a
	simple copy.

	\ingroup libtensor_btod
 **/
template<size_t N, typename Functor, typename Alloc = libvmm::std_allocator<double> >
class btod_apply :
	public additive_btod<N>,
	public timings< btod_apply<N, Functor, Alloc> > {
public:
	static const char *k_clazz; //!< Class name

private:
	Functor m_fn; //!< Functor to apply to each element
	block_tensor_i<N, double> &m_bta; //!< Source block %tensor
	permutation<N> m_perm; //!< Permutation
	double m_c; //!< Scaling coefficient
	block_index_space<N> m_bis; //!< Block %index space of output
	dimensions<N> m_bidims; //!< Block %index dimensions
	symmetry<N, double> m_sym; //!< Symmetry of output
	assignment_schedule<N, double> m_sch;

public:
	//!	\name Construction and destruction
	//@{

	/**	\brief Initializes the element-wise operation
		\param bt Source block %tensor.
		\param f Functor instance.
		\param c Scaling coefficient.
	 **/
	btod_apply(block_tensor_i<N, double> &bta,
			const Functor &fn, double c = 1.0);

	/**	\brief Initializes the permuted element-wise operation
		\param bt Source block %tensor.
		\param f Functor instance.
		\param p Permutation.
		\param c Scaling coefficient.
	 **/
	btod_apply(block_tensor_i<N, double> &bta,
			const Functor &fn, const permutation<N> &p, double c = 1.0);

	/**	\brief Virtual destructor
	 **/
	virtual ~btod_apply() { }
	//@}

	//!	\name Implementation of
	//!		libtensor::direct_block_tensor_operation<N, double>
	//@{
	virtual const block_index_space<N> &get_bis() const {
		return m_bis;
	}

	virtual const symmetry<N, double> &get_symmetry() const {
		return m_sym;
	}

	using additive_btod<N>::perform;

	virtual void sync_on();
	virtual void sync_off();

	//@}

	//!	\name Implementation of libtensor::btod_additive<N>
	//@{
	virtual const assignment_schedule<N, double> &get_schedule() const {
		return m_sch;
	}
	//@}

protected:
	virtual void compute_block(tensor_i<N, double> &blk,
		const index<N> &ib);
	virtual void compute_block(tensor_i<N, double> &blk,
		const index<N> &ib, const transf<N, double> &tr, double c);

private:
	static block_index_space<N> mk_bis(const block_index_space<N> &bis,
		const permutation<N> &perm);
	void make_schedule();

private:
	btod_apply(const btod_apply<N, Functor>&);
	btod_apply<N, Functor> &operator=(const btod_apply<N, Functor>&);

};

template<size_t N, typename Functor, typename Alloc>
const char *btod_apply<N, Functor, Alloc>::k_clazz =
		"btod_apply<N, Functor, Alloc>";


template<size_t N, typename Functor, typename Alloc>
btod_apply<N, Functor, Alloc>::btod_apply(
		block_tensor_i<N, double> &bta, const Functor &fn, double c) :

	m_bta(bta), m_fn(fn), m_c(c), m_bis(m_bta.get_bis()),
	m_bidims(m_bis.get_block_index_dims()), m_sym(m_bis), m_sch(m_bidims) {

	block_tensor_ctrl<N, double> ctrla(m_bta);
	so_copy<N, double>(ctrla.req_const_symmetry()).perform(m_sym);
	m_fn.update_sym(m_sym);
	make_schedule();
}


template<size_t N, typename Functor, typename Alloc>
btod_apply<N, Functor, Alloc>::btod_apply(block_tensor_i<N, double> &bta,
		const Functor &fn, const permutation<N> &p, double c) :

		m_bta(bta), m_fn(fn), m_perm(p), m_c(c),
		m_bis(mk_bis(m_bta.get_bis(), m_perm)),
		m_bidims(m_bis.get_block_index_dims()), m_sym(m_bis), m_sch(m_bidims) {

	block_tensor_ctrl<N, double> ctrla(m_bta);
	so_permute<N, double>(ctrla.req_const_symmetry(), m_perm).perform(m_sym);
	m_fn.update_sym(m_sym);
	make_schedule();
}


template<size_t N, typename Functor, typename Alloc>
void btod_apply<N, Functor, Alloc>::sync_on() {

	block_tensor_ctrl<N, double> ctrla(m_bta);
	ctrla.req_sync_on();
}


template<size_t N, typename Functor, typename Alloc>
void btod_apply<N, Functor, Alloc>::sync_off() {

	block_tensor_ctrl<N, double> ctrla(m_bta);
	ctrla.req_sync_off();
}


template<size_t N, typename Functor, typename Alloc>
void btod_apply<N, Functor, Alloc>::compute_block(
		tensor_i<N, double> &blk, const index<N> &ib) {

	block_tensor_ctrl<N, double> ctrla(m_bta);
	dimensions<N> bidimsa = m_bta.get_bis().get_block_index_dims();

	permutation<N> pinv(m_perm, true);

	//	Corresponding index in A
	index<N> ia(ib);
	ia.permute(pinv);

	//	Find the canonical index in A
	orbit<N, double> oa(ctrla.req_const_symmetry(), ia);
	abs_index<N> acia(oa.get_abs_canonical_index(), bidimsa);

	//	Transformation for block from canonical A to B
	transf<N, double> tra(oa.get_transf(ia));
	tra.permute(m_perm);
	tra.scale(m_c);

	if(!ctrla.req_is_zero_block(acia.get_index())) {
		tensor_i<N, double> &blka = ctrla.req_block(acia.get_index());
		tod_apply<N, Functor>(blka, m_fn,
				tra.get_perm(), tra.get_coeff()).perform(blk);
		ctrla.ret_block(acia.get_index());
	} else {
		tod_set<N>(m_fn(0.0)).perform(blk);
	}
}


template<size_t N, typename Functor, typename Alloc>
void btod_apply<N, Functor, Alloc>::compute_block(tensor_i<N, double> &blk,
		const index<N> &ib, const transf<N, double> &tr, double c) {

	static const char *method =
			"compute_block(tensor_i<N, double> &, const index<N> &, "
			"const transf<N, double> &, double)";

	block_tensor_ctrl<N, double> ctrla(m_bta);
	dimensions<N> bidimsa = m_bta.get_bis().get_block_index_dims();

	permutation<N> pinv(m_perm, true);

	//	Corresponding index in A
	index<N> ia(ib);
	ia.permute(pinv);

	//	Find the canonical index in A
	orbit<N, double> oa(ctrla.req_const_symmetry(), ia);
	abs_index<N> acia(oa.get_abs_canonical_index(), bidimsa);

	//	Transformation for block from canonical A to B
	transf<N, double> tra(oa.get_transf(ia));
	tra.permute(m_perm);
	tra.scale(m_c);
	tra.transform(tr);

	if(! ctrla.req_is_zero_block(acia.get_index())) {
		tensor_i<N, double> &blka = ctrla.req_block(acia.get_index());
		tod_apply<N, Functor>(blka, m_fn,
				tra.get_perm(), tra.get_coeff()).perform(blk, c);
		ctrla.ret_block(acia.get_index());
	}
	else {
		double val = m_fn(0.0) * c * tra.get_coeff();
		if (val != 0.0) {
			tensor<N, double, Alloc> tblk(blk.get_dims());
			tod_set<N>(val).perform(tblk);
			tod_copy<N>(tblk).perform(blk, 1.0);
		}
	}
}


template<size_t N, typename Functor, typename Alloc>
void btod_apply<N, Functor, Alloc>::make_schedule() {

	block_tensor_ctrl<N, double> ctrla(m_bta);
	dimensions<N> bidimsa = m_bta.get_bis().get_block_index_dims();

	bool identity = m_perm.is_identity();

	orbit_list<N, double> ola(ctrla.req_const_symmetry());
	for(typename orbit_list<N, double>::iterator ioa = ola.begin();
			ioa != ola.end(); ioa++) {

		if(ctrla.req_is_zero_block(ola.get_index(ioa))) continue;

		if(!identity) {
			index<N> bib(ola.get_index(ioa)); bib.permute(m_perm);
			orbit<N, double> ob(m_sym, bib);
			m_sch.insert(ob.get_abs_canonical_index());
		} else {
			m_sch.insert(ola.get_abs_index(ioa));
		}
	}
}


template<size_t N, typename Functor, typename Alloc>
block_index_space<N>
btod_apply<N, Functor, Alloc>::mk_bis(const block_index_space<N> &bis,
		const permutation<N> &perm) {

	block_index_space<N> bis1(bis);
	bis1.permute(perm);
	return bis1;
}



} // namespace libtensor

#endif // LIBTENSOR_BTOD_APPLY_H