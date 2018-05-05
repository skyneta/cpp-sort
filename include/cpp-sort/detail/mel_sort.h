/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Morwenn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef CPPSORT_DETAIL_MEL_SORT_H_
#define CPPSORT_DETAIL_MEL_SORT_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <algorithm>
#include <iterator>
#include <list>
#include <vector>
#include <cpp-sort/utility/as_function.h>
#include <cpp-sort/utility/iter_move.h>
#include "iterator_traits.h"
#include "merge_move.h"
#include "projection_compare.h"
#include "type_traits.h"

namespace cppsort
{
namespace detail
{
    template<typename ForwardIterator, typename Compare, typename Projection>
    auto mel_sort(ForwardIterator first, ForwardIterator last,
                  Compare compare, Projection projection)
        -> void
    {
        auto&& comp = utility::as_function(compare);
        auto&& proj = utility::as_function(projection);

        // Encroaching lists
        using rvalue_reference = remove_cvref_t<rvalue_reference_t<ForwardIterator>>;
        std::vector<std::list<rvalue_reference>> lists;

        ////////////////////////////////////////////////////////////
        // Create encroaching lists

        for (auto it = first ; it != last ; ++it) {
            auto&& value = proj(*it);

            // Binary search for an encroaching list where
            // value <= list.first or value >= list.second

            // Whether the found value is smaller than the head
            // of the found encroaching list or greater than its
            // tail
            bool value_is_smaller = true;

            auto size = lists.size();
            auto enc_it = std::begin(lists);
            while (size > 0) {
                auto tmp_it = enc_it;
                std::advance(tmp_it, size / 2);
                if (not comp(proj(tmp_it->front()), value)) {
                    size /= 2;
                    value_is_smaller = true;
                } else if (not comp(value, proj(tmp_it->back()))) {
                    size /= 2;
                    value_is_smaller = false;
                } else {
                    enc_it = ++tmp_it;
                    size -= size / 2 + 1;
                }
            }

            using utility::iter_move;
            if (enc_it != std::end(lists)) {
                // Add the new element to the head or tail or the most
                // suitable encroaching list if any has been found
                if (value_is_smaller) {
                    enc_it->push_front(iter_move(it));
                } else {
                    enc_it->push_back(iter_move(it));
                }
            } else {
                // Create a new encroaching list if the element
                // didn't fit in any of the existing ones
                lists.emplace_back();
                lists.back().push_back(iter_move(it));
            }
        }

        ////////////////////////////////////////////////////////////
        // Merge encroaching lists

        auto proj_comp = make_projection_compare(compare, projection);

        while (lists.size() > 2) {
            if (lists.size() % 2 != 0) {
                auto last_it = std::prev(std::end(lists));
                auto last_1_it = std::prev(last_it);
                last_1_it->merge(*last_it, proj_comp);
                lists.pop_back();
            }

            auto first_it = std::begin(lists);
            auto half_it = first_it + lists.size() / 2;
            while (half_it != std::end(lists)) {
                first_it->merge(*half_it, proj_comp);
                ++first_it;
                ++half_it;
            }

            lists.erase(std::begin(lists) + lists.size() / 2, std::end(lists));
        }

        // Merge lists back into the original collection

        if (lists.size() == 2) {
            merge_move(
                std::begin(lists.front()), std::end(lists.front()),
                std::begin(lists.back()), std::end(lists.back()),
                first, compare, projection, projection
            );
        } else if (lists.size() == 1) {
            std::move(std::begin(lists.front()), std::end(lists.front()), first);
        }
    }
}}


#endif // CPPSORT_DETAIL_MEL_SORT_H_
