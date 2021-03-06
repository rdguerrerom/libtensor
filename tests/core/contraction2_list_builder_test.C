#include <list>
#include <libtensor/core/contraction2_list_builder.h>
#include "../test_utils.h"

using namespace libtensor;

namespace {

struct loop_list_node {
private:
    size_t m_weight;
    size_t m_inca, m_incb, m_incc;
public:
    loop_list_node() : m_weight(0), m_inca(0), m_incb(0), m_incc(0) { }
    loop_list_node(size_t weight, size_t inca, size_t incb,
        size_t incc) : m_weight(weight), m_inca(inca), m_incb(incb),
        m_incc(incc) { }
};

class loop_list_adapter {
private:
    std::list<loop_list_node> &m_list;

public:
    loop_list_adapter(std::list<loop_list_node> &list) : m_list(list) { }
    void append(size_t weight, size_t inca, size_t incb, size_t incc) {
        m_list.push_back(loop_list_node(weight, inca, incb, incc));
    }
};

} // unnamed namespace


int test_1() {

    static const char testname[] = "contraction2_list_builder::test_1()";

    try {

    contraction2<1, 3, 1> contr;
    contr.contract(0, 3);

    index<2> ia1, ia2;
    ia2[0] = 5; ia2[1] = 9;
    index<4> ib1, ib2;
    ib2[0] = 9; ib2[1] = 9; ib2[2] = 9; ib2[3] = 5;
    index<4> ic1, ic2;
    ic2[0] = 9; ic2[1] = 9; ic2[2] = 9; ic2[3] = 9;
    dimensions<2> dima(index_range<2>(ia1, ia2));
    dimensions<4> dimb(index_range<4>(ib1, ib2));
    dimensions<4> dimc(index_range<4>(ic1, ic2));

    std::list<loop_list_node> lst;
    loop_list_adapter adapter(lst);
    contraction2_list_builder<1, 3, 1> bldr(contr);
    bldr.populate(adapter, dima, dimb, dimc);

    } catch(exception &e) {
        return fail_test(testname, __FILE__, __LINE__, e.what());
    }

    return 0;
}


int main() {

    return test_1();
}

