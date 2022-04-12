/*
    Copyright (C) 2022  Iori Torres (shortanemoia@protonmail.com)
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <immintrin.h>
#include "Aligned.h"
#include "Concepts.h"

#if defined(__GNUC__) || defined(__clang__)
    #define forceinline __attribute__((always_inline))
#elif defined(_MSC_VER)
    #define forceinline __forceinline
#else
    #define forceinline
#endif

#ifdef __AVX2__
    #define HAS_AVX2 1
#else
    #define HAS_AVX2 0
#endif

#define MAKE_SHUFFLEF(a, b, c, d) _MM_SHUFFLE(d, c, b, a)
#define MAKE_SHUFFLED(a, b) _MM_SHUFFLE2(b, a)

namespace neo::math::simd
{
    template<typename T>
    concept SIMDType = PackContains<T, __m128, __m128d, __m256, __m256d, __m512, __m512d>;

    namespace detail
    {
        template<SIMDType T>
        struct simd_type_mapper_t
        {
        };

        template<>
        struct simd_type_mapper_t<__m128>
        {
            using type = float;
        };

        template<>
        struct simd_type_mapper_t<__m128d>
        {
            using type = double;
        };

        template<>
        struct simd_type_mapper_t<__m256>
        {
            using type = float;
        };

        template<>
        struct simd_type_mapper_t<__m256d>
        {
            using type = double;
        };

        template<>
        struct simd_type_mapper_t<__m512>
        {
            using type = float;
        };

        template<>
        struct simd_type_mapper_t<__m512d>
        {
            using type = double;
        };
    }

    template<SIMDType T>
    using UnitType = typename detail::simd_type_mapper_t<T>::type;

    namespace detail
    {
        template<SIMDType T>
        struct simd_half_width_t
        {
        };

        template<>
        struct simd_half_width_t<__m512>
        {
            using type = __m256;
        };

        template<>
        struct simd_half_width_t<__m512d>
        {
            using type = __m256d;
        };

        template<>
        struct simd_half_width_t<__m256>
        {
            using type = __m128;
        };

        template<>
        struct simd_half_width_t<__m256d>
        {
            using type = __m128d;
        };

        template<>
        struct simd_half_width_t<__m128>
        {
            using type = __m128;
        };

        template<>
        struct simd_half_width_t<__m128d>
        {
            using type = __m128d;
        };

    }

    template<SIMDType T>
    using HalfWidthType = typename detail::simd_half_width_t<T>::type;

    namespace detail
    {
        template<typename T>
        struct widest_simd_type_available_t
        {
            using type = Conditional<HAS_AVX2 == 1, Conditional<Same<T, float>, __m256, __m256d>, Conditional<Same<T, float>, __m256, __m256d>>;
        };
    }

    template<FloatingPoint T>
    using WidestSIMDTypeAvailable = typename detail::widest_simd_type_available_t<T>::type;

    template<typename T>
    void print_simd_register(T var)
    {
        UnitType<T> val[sizeof(T) / sizeof(UnitType<T>)];
        __builtin_memcpy(val, &var, sizeof(val));
        for (int i = 0; i < sizeof(T) / sizeof(UnitType<T>); i++)
            __builtin_printf("%f ", val[i]);
        __builtin_printf("\n");
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic zeroed_register()
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
            return _mm_setzero_ps();
        if constexpr (Same<TVectorIntrinsic, __m128d>)
            return _mm_setzero_pd();
        if constexpr (Same<TVectorIntrinsic, __m256>)
            return _mm256_setzero_ps();
        if constexpr (Same<TVectorIntrinsic, __m256d>)
            return _mm256_setzero_pd();
        if constexpr (Same<TVectorIntrinsic, __m512>)
            return _mm512_setzero_ps();
        if constexpr (Same<TVectorIntrinsic, __m512d>)
            return _mm512_setzero_pd();
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic add_packed(TVectorIntrinsic const& a, TVectorIntrinsic const& b)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_add_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_add_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_add_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_add_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_add_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_add_pd(a, b);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic sub_packed(TVectorIntrinsic const& a, TVectorIntrinsic const& b)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_sub_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_sub_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_sub_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_sub_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_sub_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_sub_pd(a, b);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic mul_packed(TVectorIntrinsic const& a, TVectorIntrinsic const& b)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_mul_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_mul_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_mul_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_mul_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_mul_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_mul_pd(a, b);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic div_packed(TVectorIntrinsic const& a, TVectorIntrinsic const& b)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_div_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_div_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_div_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_div_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_div_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_div_pd(a, b);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic sqrt_packed(TVectorIntrinsic const& a)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_sqrt_ps(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_sqrt_pd(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_sqrt_ps(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_sqrt_pd(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_sqrt_ps(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_sqrt_pd(a);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic fmadd_packed(TVectorIntrinsic const& a, TVectorIntrinsic const& b, TVectorIntrinsic const& c)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_fmadd_ps(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_fmadd_pd(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_fmadd_ps(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_fmadd_pd(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_fmadd_ps(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_fmadd_pd(a, b, c);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic fmadd_single(TVectorIntrinsic const& a, TVectorIntrinsic const& b, TVectorIntrinsic const& c)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_fmadd_ss(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_fmadd_sd(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_fmadd_ss(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_fmadd_sd(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_fmadd_ss(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_fmadd_sd(a, b, c);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic fmsub_packed(TVectorIntrinsic const& a, TVectorIntrinsic const& b, TVectorIntrinsic const& c)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_fmsub_ps(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_fmsub_pd(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_fmsub_ps(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_fmsub_pd(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_fmsub_ps(a, b, c);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_fmsub_pd(a, b, c);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic max_packed(TVectorIntrinsic const& a, TVectorIntrinsic const& b)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_max_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_max_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_max_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_max_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_max_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_max_pd(a, b);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic min_packed(TVectorIntrinsic const& a, TVectorIntrinsic const& b)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_min_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_min_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_min_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_min_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_min_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_min_pd(a, b);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic and_packed(TVectorIntrinsic const& a, TVectorIntrinsic const& b)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_and_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_and_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_and_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_and_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_and_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_and_ps(a, b);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic or_packed(TVectorIntrinsic const& a, TVectorIntrinsic const& b)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_or_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_or_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_or_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_or_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_or_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_or_ps(a, b);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic andnot_packed(TVectorIntrinsic const& a, TVectorIntrinsic const& b)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_andnot_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_andnot_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_andnot_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_andnot_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_andnot_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_andnot_ps(a, b);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic set1_packed(UnitType<TVectorIntrinsic> const& a)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_set1_ps(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_set1_pd(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_set1_ps(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_set1_pd(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_set1_ps(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_set1_pd(a);
        }
    }

    template<SIMDType... TVectorIntrinsic>
    requires(Same<FirstType<TVectorIntrinsic...>, TVectorIntrinsic>&&...) constexpr FirstType<TVectorIntrinsic...> set_packed(UnitType<TVectorIntrinsic>... values)
    {
        if constexpr (Same<FirstType<TVectorIntrinsic...>, __m128>)
        {
            static_assert(PackSize<TVectorIntrinsic...> == 4);
            return _mm_setr_ps(values...);
        }
        if constexpr (Same<FirstType<TVectorIntrinsic...>, __m128d>)
        {
            static_assert(PackSize<TVectorIntrinsic...> == 2);
            return _mm_setr_pd(values...);
        }
        if constexpr (Same<FirstType<TVectorIntrinsic...>, __m256>)
        {
            static_assert(PackSize<TVectorIntrinsic...> == 8);
            return _mm256_setr_ps(values...);
        }
        if constexpr (Same<FirstType<TVectorIntrinsic...>, __m256d>)
        {
            static_assert(PackSize<TVectorIntrinsic...> == 4);
            return _mm256_setr_pd(values...);
        }
        // TODO: Implement ReversePack<>
        /*
        if constexpr(Same<FirstType<TVectorIntrinsic...>, __m512>)
        {
            static_assert(PackSize<TVectorIntrinsic...> == 16);
            return _mm512_setr_ps(values...);
        }
        if constexpr(Same<FirstType<TVectorIntrinsic...>, __m512d>)
        {
            static_assert(PackSize<TVectorIntrinsic...> == 8);
            return _mm512_setr_pd(values...);
        }
         */
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic abs_packed(TVectorIntrinsic const& a)
    {
        TVectorIntrinsic mask = set1_packed<TVectorIntrinsic>(-0.);
        return andnot_packed(mask, a);
    };

    // For the list of possible comparisons see https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#text=_mm_cmp_ps&ig_expand=848
    template<u8 Comparison, SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic cmp_packed(TVectorIntrinsic const& a, TVectorIntrinsic const& b)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_cmp_ps(a, b, Comparison);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_cmp_pd(a, b, Comparison);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_cmp_ps(a, b, Comparison);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_cmp_pd(a, b, Comparison);
        }
    }

    // For documentation see https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=848,588,625,6146,590&text=BROADCAST
    // Mind the alignment!!
    template<SIMDType TVectorIntrinsic, typename TSource>
    constexpr TVectorIntrinsic broadcast(TSource a)
    {
        if constexpr (Same<TVectorIntrinsic, __m128> && Same<RemoveCV<TSource>, float*>)
        {
            return _mm_broadcast_ss(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m128> && Same<TSource, __m128>)
        {
            return _mm_broadcastss_ps(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d> && Same<TSource, __m128d>)
        {
            return _mm_broadcastsd_pd(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m256> && Same<RemoveCV<TSource>, float*>)
        {
            return _mm256_broadcast_ss(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m256> && Same<RemoveCV<TSource>, __m128*>)
        {
            return _mm256_broadcast_ps(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d> && Same<RemoveCV<TSource>, __m128d*>)
        {
            return _mm256_broadcast_pd(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d> && Same<RemoveCV<TSource>, double*>)
        {
            return _mm256_broadcast_sd(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d> && Same<TSource, __m128>)
        {
            return _mm256_broadcastss_ps(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d> && Same<TSource, __m128d>)
        {
            return _mm256_broadcastsd_pd(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m512> && Same<TSource, __m128>)
        {
            return _mm512_broadcastss_ps(a);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d> && Same<TSource, __m128d>)
        {
            return _mm512_broadcastsd_pd(a);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic load_packed(Aligned<UnitType<TVectorIntrinsic>*, alignof(TVectorIntrinsic)> from)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_load_ps(from);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_load_pd(from);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_load_ps(from);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_load_pd(from);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_load_ps(from);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_load_pd(from);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic load_packed_unaligned(UnitType<TVectorIntrinsic>* from)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_loadu_ps(from);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_loadu_pd(from);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_loadu_ps(from);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_loadu_pd(from);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_loadu_ps(from);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_loadu_pd(from);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr void store_packed(TVectorIntrinsic v, Aligned<UnitType<TVectorIntrinsic>*, alignof(TVectorIntrinsic)> to)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_store_ps(to, v);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_store_pd(to, v);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_store_ps(to, v);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_store_pd(to, v);
        }
        if constexpr (Same<TVectorIntrinsic, __m512>)
        {
            return _mm512_store_ps(to, v);
        }
        if constexpr (Same<TVectorIntrinsic, __m512d>)
        {
            return _mm512_store_pd(to, v);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr TVectorIntrinsic hadd_packed(TVectorIntrinsic const& a, TVectorIntrinsic const& b)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_hadd_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_hadd_pd(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_hadd_ps(a, b);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_hadd_pd(a, b);
        }
    }

    template<SIMDType TVectorIntrinsic>
    constexpr UnitType<TVectorIntrinsic> horizontal_sum(TVectorIntrinsic const& a)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            __m128 upper_half = _mm_shuffle_ps(a, a, MAKE_SHUFFLEF(2, 3, 0, 0));
            __m128 added_halfs = _mm_add_ps(a, upper_half);
            __m128 fully_added = _mm_add_ss(added_halfs, _mm_shuffle_ps(added_halfs, added_halfs, MAKE_SHUFFLEF(1, 0, 0, 0)));
            return _mm_cvtss_f32(fully_added);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            __m128d high = _mm_shuffle_pd(a, a, MAKE_SHUFFLED(1, 0));
            __m128d added = _mm_add_sd(a, high);
            return _mm_cvtsd_f64(added);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            __m128 lower_half = _mm256_extractf128_ps(a, 0);
            __m128 upper_half = _mm256_extractf128_ps(a, 1);
            __m128 added_halfs = _mm_add_ps(lower_half, upper_half);
            __m128 adjacents_added = _mm_add_ps(added_halfs, _mm_shuffle_ps(added_halfs, added_halfs, MAKE_SHUFFLEF(2, 3, 0, 0)));
            __m128 fully_added = _mm_add_ss(adjacents_added, _mm_shuffle_ps(adjacents_added, adjacents_added, MAKE_SHUFFLEF(1, 0, 0, 0)));
            return _mm_cvtss_f32(fully_added);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            __m128d lower_half = _mm256_extractf128_pd(a, 0);
            __m128d upper_half = _mm256_extractf128_pd(a, 1);
            __m128d added_halfs = _mm_add_pd(lower_half, upper_half);
            __m128d added = _mm_add_sd(added_halfs, _mm_shuffle_pd(added_halfs, added_halfs, MAKE_SHUFFLED(1, 0)));
            return _mm_cvtsd_f64(added);
        }
    }

    template<SIMDType TVectorIntrinsic, size_t Scale>
    constexpr TVectorIntrinsic gather_packed(UnitType<TVectorIntrinsic> const* base_address, Aligned<u32 const*, 16> indices)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            __m128i i = _mm_load_si128((__m128i const*)(u32 const*)indices);
            return _mm_i32gather_ps(base_address, i, Scale);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            __m128i i = _mm_load_si128((__m128i const*)(u32 const*)indices);
            return _mm_i32gather_pd(base_address, i, Scale);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            __m256i i = _mm256_load_si256((__m256i const*)(u32 const*)indices);
            return _mm256_i32gather_ps(base_address, i, Scale);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            __m128i i = _mm_load_si128((__m128i const*)(u32 const*)indices);
            return _mm256_i32gather_pd(base_address, i, Scale);
        }
    }

    template<SIMDType TFrom, SIMDType TTo>
    constexpr TTo cast_intrinsic(TFrom const& from)
    {
        if constexpr (Same<TFrom, __m128>)
        {
            if constexpr (Same<TTo, __m128d>)
                return _mm_castps_pd(from);
            if constexpr (Same<TTo, __m128i>)
                return _mm_castps_si128(from);
            if constexpr (Same<TTo, __m256>)
                return _mm256_castps128_ps256(from);
        }
        if constexpr (Same<TFrom, __m128d>)
        {
            if constexpr (Same<TTo, __m128d>)
                return _mm_castpd_pd(from);
            if constexpr (Same<TTo, __m128i>)
                return _mm_castpd_si128(from);
            if constexpr (Same<TTo, __m256>)
                return _mm256_castpd128_ps256(from);
        }
        if constexpr (Same<TFrom, __m256>)
        {
            if constexpr (Same<TTo, __m256d>)
                return _mm256_castps_pd(from);
            if constexpr (Same<TTo, __m128i>)
                return _mm256_castps_si256(from);
            if constexpr (Same<TTo, __m128>)
                return _mm256_castps256_ps128(from);
        }
        if constexpr (Same<TFrom, __m256d>)
        {
            if constexpr (Same<TTo, __m256d>)
                return _mm256_castpd_ps(from);
            if constexpr (Same<TTo, __m128i>)
                return _mm256_castpd_si256(from);
            if constexpr (Same<TTo, __m128>)
                return _mm256_castpd256_pd128(from);
        }
    }

    template<SIMDType TVectorIntrinsic, size_t Mask = MAKE_SHUFFLEF(1, 0, 0, 0)>
    constexpr TVectorIntrinsic blend_packed(TVectorIntrinsic const& front, TVectorIntrinsic const& back)
    {
        if constexpr (Same<TVectorIntrinsic, __m128>)
        {
            return _mm_blend_ps(front, back, Mask);
        }
        if constexpr (Same<TVectorIntrinsic, __m128d>)
        {
            return _mm_blend_pd(front, back, Mask);
        }
        if constexpr (Same<TVectorIntrinsic, __m256>)
        {
            return _mm256_blend_ps(front, back, Mask);
        }
        if constexpr (Same<TVectorIntrinsic, __m256d>)
        {
            return _mm256_blend_pd(front, back, Mask);
        }
    }
}
