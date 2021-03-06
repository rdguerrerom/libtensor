#include <libtensor/ctf_block_tensor/ctf_btod_dotprod.h>
#include <libtensor/expr/common/metaprog.h>
#include <libtensor/expr/dag/node_dot_product.h>
#include <libtensor/expr/dag/node_scalar.h>
#include "ctf_btensor_from_node.h"
#include "eval_ctf_btensor_double_dot_product.h"

namespace libtensor {
namespace expr {
namespace eval_ctf_btensor_double {

namespace {
using eval_btensor_double::dispatch_1;


class eval_dot_product_impl {
private:
    enum {
        Nmax = dot_product::Nmax
    };

private:
    struct dispatch_dot_product {
        eval_dot_product_impl &eval;
        expr_tree::node_id_t lhs;
        size_t na;

        dispatch_dot_product(
            eval_dot_product_impl &eval_,
            expr_tree::node_id_t lhs_,
            size_t na_) :
            eval(eval_), lhs(lhs_), na(na_)
        { }

        template<size_t NA> void dispatch();
    };

private:
    const expr_tree &m_tree; //!< Expression tree
    expr_tree::node_id_t m_id; //!< ID of copy node

public:
    eval_dot_product_impl(const expr_tree &tr, expr_tree::node_id_t id) :
        m_tree(tr), m_id(id)
    { }

    void evaluate(expr_tree::node_id_t lhs);

    template<size_t NA>
    void do_evaluate(expr_tree::node_id_t lhs);

};


void eval_dot_product_impl::evaluate(expr_tree::node_id_t lhs) {

    const expr_tree::edge_list_t &e = m_tree.get_edges_out(m_id);
    const node &n = m_tree.get_vertex(m_id);
    const node_dot_product &nd = n.recast_as<node_dot_product>();

    const node &arga = m_tree.get_vertex(e[0]);
    size_t na = arga.get_n();
    const node &argb = m_tree.get_vertex(e[1]);
    size_t nb = argb.get_n();

    if(na != nb) {
        throw "invalid order";
    }

    dispatch_dot_product dd(*this, lhs, na);
    dispatch_1<1, Nmax>::dispatch(dd, na);
}


template<size_t NA>
void eval_dot_product_impl::do_evaluate(expr_tree::node_id_t lhs) {

    const expr_tree::edge_list_t &e = m_tree.get_edges_out(m_id);
    const node &n = m_tree.get_vertex(m_id);
    const node_dot_product &nd = n.template recast_as<node_dot_product>();

    ctf_btensor_from_node<NA, double> bta(m_tree, e[0]);
    ctf_btensor_from_node<NA, double> btb(m_tree, e[1]);

    permutation<NA> perma(bta.get_transf().get_perm()),
        permb(btb.get_transf().get_perm());
    sequence<NA, size_t> seqa, seqb;
    for(size_t i = 0; i < NA; i++) {
        seqa[i] = nd.get_idx().at(i);
        seqb[i] = nd.get_idx().at(NA + i);
    }
    permutation_builder<NA> pb(seqa, seqb);
    permb.permute(pb.get_perm());

    double d = ctf_btod_dotprod<NA>(bta.get_btensor(), perma, btb.get_btensor(),
        permb).calculate();
    d *= bta.get_transf().get_scalar_tr().get_coeff();
    d *= btb.get_transf().get_scalar_tr().get_coeff();

    const node_scalar<double> &ns =
        m_tree.get_vertex(lhs).template recast_as< node_scalar<double> >();
    ns.get_scalar() = d;
}


template<size_t NA>
void eval_dot_product_impl::dispatch_dot_product::dispatch() {

    eval.template do_evaluate<NA>(lhs);
}


} // unnamed namespace


void dot_product::evaluate(node_id_t lhs) {

    eval_dot_product_impl(m_tree, m_id).evaluate(lhs);
}


} // namespace eval_ctf_btensor_double
} // namespace expr
} // namespace libtensor
