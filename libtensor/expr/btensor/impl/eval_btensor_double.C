#include <libtensor/core/tensor_transf_double.h>
#include <libtensor/expr/btensor/btensor.h>
#include <libtensor/expr/common/metaprog.h>
#include <libtensor/expr/dag/node_dot_product.h>
#include <libtensor/expr/dag/node_scalar.h>
#include <libtensor/expr/dag/node_trace.h>
#include <libtensor/expr/dag/node_transform.h>
#include <libtensor/expr/eval/eval_exception.h>
#include <libtensor/expr/eval/tensor_type_check.h>
#include "../eval_btensor.h"
#include "eval_btensor_double_autoselect.h"
#include "eval_btensor_double_contract.h"
#include "eval_btensor_double_dot_product.h"
#include "eval_btensor_double_scale.h"
#include "eval_btensor_double_trace.h"
#include "eval_tree_builder_btensor.h"
#include "node_interm.h"
#include "tensor_from_node.h"

namespace libtensor {
namespace expr {
using namespace eval_btensor_double;


namespace {

class eval_btensor_double_impl {
public:
    enum {
        Nmax = eval_btensor<double>::Nmax
    };

    typedef eval_tree_builder_btensor::eval_order_t eval_order_t;

private:
    expr_tree &m_tree;
    const eval_order_t &m_order;

public:
    eval_btensor_double_impl(expr_tree &tr, const eval_order_t &order) :
        m_tree(tr), m_order(order)
    { }

    /** \brief Processes the evaluation plan
     **/
    void evaluate();

private:
    void handle_assign(const expr_tree::node_id_t id);
    void handle_scale(const expr_tree::node_id_t id);

    void verify_scalar(const node &n);
    void verify_tensor(const node &n);

};


class eval_node {
public:
    static const char k_clazz[]; //!< Class name

private:
    const expr_tree &m_tree; //!< Expression tree
    expr_tree::node_id_t m_rhs; //!<  ID of rhs node

public:
    eval_node(const expr_tree &tr, expr_tree::node_id_t rhs) :
        m_tree(tr), m_rhs(rhs)
    { }

    void evaluate_scalar(expr_tree::node_id_t lhs);

    template<size_t N>
    void evaluate(expr_tree::node_id_t lhs, bool add);

};


const char eval_node::k_clazz[] = "eval_node";


void eval_node::evaluate_scalar(expr_tree::node_id_t lhs) {

    const node &n = m_tree.get_vertex(m_rhs);

    if(n.get_op().compare(node_dot_product::k_op_type) == 0) {
        eval_btensor_double::dot_product(m_tree, m_rhs).evaluate(lhs);
    } else if(n.get_op().compare(node_trace::k_op_type) == 0) {
        eval_btensor_double::trace(m_tree, m_rhs).evaluate(lhs);
    }
}


template<size_t N>
void eval_node::evaluate(expr_tree::node_id_t lhs, bool add) {

    tensor_transf<N, double> tr;
    expr_tree::node_id_t rhs = transf_from_node(m_tree, m_rhs, tr);
    const node &n = m_tree.get_vertex(rhs);

    eval_btensor_double::autoselect<N>(m_tree, rhs, tr).evaluate(lhs, add);
}


class eval_assign_tensor {
private:
    const expr_tree &m_tree;
    expr_tree::node_id_t m_lhs; //!< Left-hand side node (has to be ident or interm)
    expr_tree::node_id_t m_rhs;
    bool m_add;

public:
    eval_assign_tensor(const expr_tree &tr, expr_tree::node_id_t lhs,
        expr_tree::node_id_t rhs, bool add) :
        m_tree(tr), m_lhs(lhs), m_rhs(rhs), m_add(add)
    { }

    template<size_t N>
    void dispatch() {
        eval_node(m_tree, m_rhs).evaluate<N>(m_lhs, m_add);
    }

};


class eval_scale_tensor {
private:
    const expr_tree &m_tree;
    expr_tree::node_id_t m_lhs;
    expr_tree::node_id_t m_rhs;

public:
    eval_scale_tensor(const expr_tree &tr, expr_tree::node_id_t lhs,
        expr_tree::node_id_t rhs) :
        m_tree(tr), m_lhs(lhs), m_rhs(rhs)
    { }

    template<size_t N>
    void dispatch() {
        eval_btensor_double::scale<N>(m_tree, m_rhs).evaluate(m_lhs);
    }

};


void eval_btensor_double_impl::evaluate() {

    for (eval_order_t::const_iterator i = m_order.begin();
            i != m_order.end(); i++) {

        const node &n = m_tree.get_vertex(*i);
        if(n.check_type<node_assign>()) {
            handle_assign(*i);
        } else if(n.check_type<node_scale>()) {
            handle_scale(*i);
        } else {
            throw eval_exception(__FILE__, __LINE__, "libtensor::expr",
                "eval_btensor_double_impl", "evaluate()",
                "Unexpected node type.");
        }
    }
}


void eval_btensor_double_impl::handle_assign(expr_tree::node_id_t id) {

    const expr_tree::edge_list_t &out = m_tree.get_edges_out(id);
    const node_assign &n = m_tree.get_vertex(id).recast_as<node_assign>();

    if(out.size() != 2) {
        throw eval_exception(__FILE__, __LINE__, "libtensor::expr",
            "eval_btensor_double_impl", "handle_assign()",
            "Malformed expression (assignment must have two children).");
    }

    const node &lhs = m_tree.get_vertex(out[0]);

    if(lhs.get_n() > 0) {

        // Check l.h.s.
        verify_tensor(lhs);

        // Evaluate r.h.s. before performing the assignment
        eval_assign_tensor e(m_tree, out[0], out[1], n.is_add());
        dispatch_1<1, Nmax>::dispatch(e, lhs.get_n());

        // Put l.h.s. at position of assignment and erase subtree
        m_tree.graph::replace(id, lhs);
        for(size_t i = 0; i < out.size(); i++) m_tree.erase_subtree(out[i]);

    } else {

        // Check l.h.s
        verify_scalar(lhs);

        // Evaluate r.h.s. and assign
        eval_node(m_tree, out[1]).evaluate_scalar(out[0]);

    }
}


void eval_btensor_double_impl::handle_scale(expr_tree::node_id_t id) {

    const expr_tree::edge_list_t &out = m_tree.get_edges_out(id);
    const node_scale &n = m_tree.get_vertex(id).recast_as<node_scale>();

    if(out.size() != 2) {
        throw eval_exception(__FILE__, __LINE__, "libtensor::expr",
            "eval_btensor_double_impl", "handle_scale()",
            "Malformed expression (scaling must have two children).");
    }

    const node &lhs = m_tree.get_vertex(out[0]);

    verify_tensor(lhs);

    eval_scale_tensor e(m_tree, out[0], out[1]);
    dispatch_1<1, Nmax>::dispatch(e, lhs.get_n());
}


void eval_btensor_double_impl::verify_scalar(const node &t) {

    if(t.check_type<node_scalar_base>()) {
        const node_scalar_base &ti = t.recast_as<node_scalar_base>();
        if(ti.get_type() != typeid(double)) {
            throw not_implemented("libtensor::expr", "eval_btensor_double_impl",
                "verify_scalar()", __FILE__, __LINE__);
        }
        return;
    }

    throw eval_exception(__FILE__, __LINE__,
        "libtensor::expr", "eval_btensor_double_impl", "verify_scalar()",
        "Expect LHS to be a scalar.");
}


void eval_btensor_double_impl::verify_tensor(const node &t) {

    if(t.check_type<node_ident>()) {
        const node_ident &ti = t.recast_as<node_ident>();
        if(ti.get_type() != typeid(double)) {
            throw not_implemented("libtensor::expr", "eval_btensor_double_impl",
                "verify_tensor()", __FILE__, __LINE__);
        }
        return;
    }
    if(t.check_type<node_interm_base>()) {
        const node_interm_base &ti = t.recast_as<node_interm_base>();
        if(ti.get_t() != typeid(double)) {
            throw not_implemented("libtensor::expr", "eval_btensor_double_impl",
                "verify_tensor()", __FILE__, __LINE__);
        }
        return;
    }

    throw eval_exception(__FILE__, __LINE__,
        "libtensor::expr", "eval_btensor_double_impl", "verify_tensor()",
        "Expect LHS to be a tensor.");
}


} // unnamed namespace


eval_btensor<double>::~eval_btensor<double>() {

}


bool eval_btensor<double>::can_evaluate(const expr_tree &e) const {

    return tensor_type_check<Nmax, double, btensor_i>(e);
}


void eval_btensor<double>::evaluate(const expr_tree &tree) const {

    eval_tree_builder_btensor bld(tree);
    bld.build();

    eval_btensor_double_impl(bld.get_tree(), bld.get_order()).evaluate();
}


void eval_btensor<double>::use_libxm(bool usexm) {

    eval_btensor_double::use_libxm = usexm;
}


} // namespace expr
} // namespace libtensor
