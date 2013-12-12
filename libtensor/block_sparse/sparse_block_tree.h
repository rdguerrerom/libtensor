#ifndef SPARSE_BLOCK_TREE_H
#define SPARSE_BLOCK_TREE_H

#include "sparse_block_tree_any_order.h"
#include "sparse_block_tree_iterator.h"

namespace libtensor {

namespace impl {
    
template<size_t N>
class sparse_block_tree : public sparse_block_tree_any_order
{
private:
    //Private constructor for implicit conversion to keep things wrapped nicely
    sparse_block_tree(const sparse_block_tree_any_order& rhs) : sparse_block_tree_any_order(rhs) {}
public:
    typedef sparse_block_tree_any_order::iterator iterator;

    //Constructor - just exposes the base class constructor
    sparse_block_tree(const std::vector< sequence<N,size_t> >& sig_blocks) : sparse_block_tree_any_order(sig_blocks) {};

    //Wraps base class method
    sparse_block_tree<N> permute(const runtime_permutation& perm) const { return sparse_block_tree_any_order::permute(perm); }

    sparse_block_tree<N-1> contract(size_t contract_idx) const { return sparse_block_tree_any_order::contract(contract_idx); }

    template<size_t M,size_t K>
    sparse_block_tree<N+M-K> fuse(const sparse_block_tree<M>& rhs,const sequence<K,size_t>& lhs_indices,const sequence<K,size_t>& rhs_indices) const;

    //Convenience wrapper for the most common case when we just want to fuse end to end
    template<size_t M>
    sparse_block_tree<N+M-1> fuse(const sparse_block_tree<M>& rhs) const { return sparse_block_tree_any_order::fuse(rhs); }


    //Friend for contract(), fuse()
    template<size_t M>
    friend class sparse_block_tree;
};

template<size_t N> template<size_t M,size_t K>
sparse_block_tree<N+M-K> sparse_block_tree<N>::fuse(const sparse_block_tree<M>& rhs,const sequence<K,size_t>& lhs_indices,const sequence<K,size_t>& rhs_indices) const
{
    std::vector<size_t> lhs_vec(&lhs_indices[0],&lhs_indices[0]+K);
    std::vector<size_t> rhs_vec(&rhs_indices[0],&rhs_indices[0]+K);
    return sparse_block_tree_any_order::fuse(rhs,lhs_vec,rhs_vec);
}

} // namespace impl

} // namespace libtensor

#endif /* SPARSE_BLOCK_TREE_H */
