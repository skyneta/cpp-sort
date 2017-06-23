//
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//  Modified in 2016-2017 by Morwenn for inclusion into cpp-sort
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef CPPSORT_UTILITY_BEGIN_END_H_
#define CPPSORT_UTILITY_BEGIN_END_H_

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <type_traits>

namespace cppsort::utility
{
    namespace adl_begin_end_detail
    {
        struct begin_fn;
        struct end_fn;
        struct cbegin_fn;
        struct cend_fn;
        struct rbegin_fn;
        struct rend_fn;
        struct crbegin_fn;
        struct crend_fn;
    }

    using adl_begin_end_detail::begin_fn;
    using adl_begin_end_detail::end_fn;
    using adl_begin_end_detail::cbegin_fn;
    using adl_begin_end_detail::cend_fn;
    using adl_begin_end_detail::rbegin_fn;
    using adl_begin_end_detail::rend_fn;
    using adl_begin_end_detail::crbegin_fn;
    using adl_begin_end_detail::crend_fn;

    namespace adl_begin_end_detail
    {
        using std::begin;
        using std::end;

        // A reference-wrapped Range is an Range
        template<typename T>
        auto begin(std::reference_wrapper<T> ref)
            -> decltype(begin(ref.get()))
        {
            return begin(ref.get());
        }

        template<typename T>
        auto end(std::reference_wrapper<T> ref)
            -> decltype(end(ref.get()))
        {
            return end(ref.get());
        }

        template<typename T>
        auto rbegin(std::reference_wrapper<T> ref)
            -> decltype(rbegin(ref.get()))
        {
            return rbegin(ref.get());
        }

        template<typename T>
        auto rend(std::reference_wrapper<T> ref)
            -> decltype(rend(ref.get()))
        {
            return rend(ref.get());
        }

        struct begin_fn
        {
        private:
            template<typename Rng>
            static constexpr auto impl(Rng&& rng, long)
                noexcept(noexcept(begin(static_cast<Rng&&>(rng))))
                -> decltype(begin(static_cast<Rng&&>(rng)))
            {
                return begin(static_cast<Rng&&>(rng));
            }

            template<
                typename Rng,
                typename = std::enable_if_t<std::is_lvalue_reference<Rng>::value>
            >
            static constexpr auto impl(Rng&& rng, int)
                noexcept(noexcept(rng.begin()))
                -> decltype(rng.begin())
            {
                return rng.begin();
            }

        public:
            template<typename Rng>
            constexpr auto operator()(Rng&& rng) const
                noexcept(noexcept(begin_fn::impl(static_cast<Rng&&>(rng), 0)))
                -> std::decay_t<decltype(begin_fn::impl(static_cast<Rng&&>(rng), 0))>
            {
                return begin_fn::impl(static_cast<Rng&&>(rng), 0);
            }
        };

        struct end_fn
        {
        private:
            template<typename Rng>
            static constexpr auto impl(Rng&& rng, long)
                noexcept(noexcept(end(static_cast<Rng&&>(rng))))
                -> decltype(end(static_cast<Rng&&>(rng)))
            {
                return end(static_cast<Rng&&>(rng));
            }

            template<
                typename Rng,
                typename = std::enable_if_t<std::is_lvalue_reference<Rng>::value>
            >
            static constexpr auto impl(Rng&& rng, int)
                noexcept(noexcept(rng.end()))
                -> decltype(rng.end())
            {
                return rng.end();
            }

        public:
            template<typename Rng>
            constexpr auto operator()(Rng&& rng) const
                noexcept(noexcept(end_fn::impl(static_cast<Rng&&>(rng), 0)))
                -> std::decay_t<decltype(end_fn::impl(static_cast<Rng&&>(rng), 0))>
            {
                return end_fn::impl(static_cast<Rng&&>(rng), 0);
            }
        };

        struct rbegin_fn
        {
        private:
            template<typename Rng>
            static constexpr auto impl(Rng&& rng, long)
                noexcept(noexcept(rbegin(static_cast<Rng&&>(rng))))
                -> decltype(rbegin(static_cast<Rng&&>(rng)))
            {
                return rbegin(static_cast<Rng&&>(rng));
            }

            template<
                typename Rng,
                typename = std::enable_if_t<std::is_lvalue_reference<Rng>::value>
            >
            static constexpr auto impl(Rng&& rng, int)
                noexcept(noexcept(rng.rbegin()))
                -> decltype(rng.rbegin())
            {
                return rng.rbegin();
            }

        public:
            template<typename Rng>
            constexpr auto operator()(Rng&& rng) const
                noexcept(noexcept(rbegin_fn::impl(static_cast<Rng&&>(rng), 0)))
                -> std::decay_t<decltype(rbegin_fn::impl(static_cast<Rng&&>(rng), 0))>
            {
                return rbegin_fn::impl(static_cast<Rng&&>(rng), 0);
            }

            template<typename T, std::size_t N>
            constexpr auto operator()(T (&t)[N]) const noexcept
                -> std::reverse_iterator<T*>
            {
                return std::reverse_iterator<T*>(t + N);
            }

            template<typename T>
            constexpr auto operator()(std::initializer_list<T> il) const noexcept
                -> std::reverse_iterator<T const*>
            {
                return std::reverse_iterator<T const*>(il.end());
            }
        };

        struct rend_fn
        {
        private:
            template<typename Rng>
            static constexpr auto impl(Rng&& rng, long)
                noexcept(noexcept(rend(static_cast<Rng&&>(rng))))
                -> decltype(rend(static_cast<Rng&&>(rng)))
            {
                return rend(static_cast<Rng&&>(rng));
            }

            template<
                typename Rng,
                typename = std::enable_if_t<std::is_lvalue_reference<Rng>::value>
            >
            static constexpr auto impl(Rng&& rng, int)
                noexcept(noexcept(rng.rend()))
                -> decltype(rng.rend())
            {
                return rng.rend();
            }
        public:
            template<typename Rng>
            constexpr auto operator()(Rng&& rng) const
                noexcept(noexcept(rend_fn::impl(static_cast<Rng&&>(rng), 0)))
                -> std::decay_t<decltype(rend_fn::impl(static_cast<Rng&&>(rng), 0))>
            {
                return rend_fn::impl(static_cast<Rng&&>(rng), 0);
            }

            template<typename T, std::size_t N>
            constexpr auto operator()(T (&t)[N]) const noexcept
                -> std::reverse_iterator<T*>
            {
                return std::reverse_iterator<T*>(t);
            }
            template<typename T>
            constexpr auto operator()(std::initializer_list<T> il) const noexcept
                -> std::reverse_iterator<T const*>
            {
                return std::reverse_iterator<T const*>(il.begin());
            }
        };

        struct cbegin_fn
        {
            template<typename Rng>
            constexpr auto operator()(const Rng& rng) const
                noexcept(noexcept(begin_fn{}(rng)))
                -> decltype(begin_fn{}(rng))
            {
                return begin_fn{}(rng);
            }
        };

        struct cend_fn
        {
            template<typename Rng>
            constexpr auto operator()(const Rng& rng) const
                noexcept(noexcept(end_fn{}(rng)))
                -> decltype(end_fn{}(rng))
            {
                return end_fn{}(rng);
            }
        };

        struct crbegin_fn
        {
            template<typename Rng>
            constexpr auto operator()(const Rng& rng) const
                noexcept(noexcept(rbegin_fn{}(rng)))
                -> decltype(rbegin_fn{}(rng))
            {
                return rbegin_fn{}(rng);
            }
        };

        struct crend_fn
        {
            template<typename Rng>
            constexpr auto operator()(const Rng& rng) const
                noexcept(noexcept(rend_fn{}(rng)))
                -> decltype(rend_fn{}(rng))
            {
                return rend_fn{}(rng);
            }
        };
    }

    inline constexpr begin_fn begin{};
    inline constexpr end_fn end{};
    inline constexpr cbegin_fn cbegin{};
    inline constexpr cend_fn cend{};
    inline constexpr rbegin_fn rbegin{};
    inline constexpr rend_fn rend{};
    inline constexpr crbegin_fn crbegin{};
    inline constexpr crend_fn crend{};
}

#endif // CPPSORT_UTILITY_BEGIN_END_H_
