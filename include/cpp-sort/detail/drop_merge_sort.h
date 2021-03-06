/*
    The MIT license (MIT)

    A C++ reimplementation of a drop-merge sort, originally made by Emil Ernerfeldt:
    https://github.com/emilk/drop-merge-sort

    Modified in 2017-2018 by Morwenn for inclusion into cpp-sort

    There are two versions of this function.

    The first one is identical to the reference Rust implementation.
    It doesn't support uncopyable types.
    Here, it's used only for trivially copyable types.
    (just like the Rust version, which required the Copy trait).

    The second version is copy-free, so it supports noncopyable types like std::unique_ptr.
    It's also more efficient for types with expensive copying, like std::string.
    However, being copy-free involves some extra bookkeeping, so for trivial types
    it's a bit less efficient than the original function - which is why I use both versions.
*/
#ifndef CPPSORT_DETAIL_DROP_MERGE_SORT_H_
#define CPPSORT_DETAIL_DROP_MERGE_SORT_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>
#include <cpp-sort/utility/as_function.h>
#include <cpp-sort/utility/iter_move.h>
#include "iterator_traits.h"
#include "pdqsort.h"
#include "type_traits.h"

namespace cppsort
{
namespace detail
{
    constexpr static bool double_comparison = true;

    // move-only version
    template<typename BidirectionalIterator, typename Compare, typename Projection>
    auto drop_merge_sort(BidirectionalIterator begin, BidirectionalIterator end,
                         Compare compare, Projection projection)
        -> void
    {
        using utility::iter_move;

        auto size = std::distance(begin, end);
        if (size < 2) return;

        auto&& comp = utility::as_function(compare);
        auto&& proj = utility::as_function(projection);

        using difference_type = difference_type_t<BidirectionalIterator>;
        using rvalue_reference = remove_cvref_t<rvalue_reference_t<BidirectionalIterator>>;
        std::vector<rvalue_reference> dropped;

        difference_type num_dropped_in_row = 0;
        auto write = begin;
        auto read = begin;

        constexpr difference_type recency = 8;

        while (read != end) {
            if (begin != write && comp(proj(*read), proj(*std::prev(write)))) {

                if (double_comparison && num_dropped_in_row == 0 && write != std::next(begin) &&
                    not comp(proj(*read), proj(*std::prev(write, 2)))) {
                    dropped.push_back(iter_move(std::prev(write)));
                    *std::prev(write) = iter_move(read);
                    ++read;
                    continue;
                }

                if (num_dropped_in_row < recency) {
                    dropped.push_back(iter_move(read));
                    ++read;
                    ++num_dropped_in_row;
                } else {
                    for (difference_type i = 0 ; i < num_dropped_in_row ; ++i) {
                        --read;
                        if (not std::is_trivially_copyable<rvalue_reference>::value) {
                            // If the value is trivially copyable, then it shouldn't have
                            // been modified by the call to iter_move, and the original
                            // value is still fully where it should be
                            *read = std::move(*std::prev(dropped.end()));
                        }
                        dropped.pop_back();
                    }

                    --write;
                    dropped.push_back(iter_move(write));

                    num_dropped_in_row = 0;
                }
            } else {
                *write = iter_move(read);
                ++read;
                ++write;
                num_dropped_in_row = 0;
            }
        }

        // Sort the dropped elements
        pdqsort(std::begin(dropped), std::end(dropped), compare, projection);

        auto back = end;

        while (not dropped.empty()) {
            auto& last_dropped = dropped.back();

            while (begin != write && comp(proj(last_dropped), proj(*std::prev(write)))) {
                --back;
                --write;
                *back = iter_move(write);
            }
            --back;
            *back = std::move(last_dropped);
            dropped.pop_back();
        }
    }
}}

#endif // CPPSORT_DETAIL_DROP_MERGE_SORT_H_
