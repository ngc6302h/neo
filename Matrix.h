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
#include "Concepts.h"
#include "SIMD.h"

namespace neo::math
{
    // matrix elements are stored by row
    template<FloatingPoint T, size_t NRows, size_t NCols, simd::SIMDType TSIMDIntrinsic = simd::WidestSIMDTypeAvailable<T>>
    class alignas(32) matrix
    {
    public:
        static constexpr size_t Rows = NRows;
        static constexpr size_t Columns = NCols;

        constexpr T* data()
        {
            return m_storage;
        }

        constexpr T const* data() const
        {
            return m_storage;
        }

        constexpr void fill(T value)
        {
            for (size_t i = 0; i < NCols * NRows; i++)
                m_storage[i] = value;
        }

        constexpr T& at(size_t row, size_t col)
        {
            return m_storage[row * Columns + col];
        }

        constexpr T at(size_t row, size_t col) const
        {
            return m_storage[row * Columns + col];
        }

        constexpr matrix<T, NCols, NRows, TSIMDIntrinsic> transposed() const
        {
            matrix<T, NCols, NRows, TSIMDIntrinsic> result;

            for (size_t i = 0; i < NRows; ++i)
            {
                for (size_t j = 0; j < NCols; ++j)
                {
                    result.at(i, j) = at(j, i);
                }
            }
            return result;
        }

        constexpr matrix& operator+=(matrix const& right)
        {
            constexpr size_t items_per_stride = (sizeof(TSIMDIntrinsic) / sizeof(T));
            constexpr size_t strides = (NCols * NRows) / items_per_stride;
            constexpr size_t leftover = (NCols * NRows) % items_per_stride;
            for (size_t i = 0; i < strides; i++)
            {
                auto v1 = load_packed<TSIMDIntrinsic>(m_storage + i * items_per_stride);
                auto v2 = load_packed<TSIMDIntrinsic>(right.data() + i * items_per_stride);
                store_packed<TSIMDIntrinsic>(add_packed(v1, v2), m_storage + i * items_per_stride);
            }
            if constexpr (leftover != 0)
            {
                if constexpr (leftover == 4)
                {
                    auto v1 = load_packed<__m128>(m_storage + strides * items_per_stride);
                    auto v2 = load_packed<__m128>(right.data() + strides * items_per_stride);
                    store_packed<__m128>(add_packed(v1, v2), m_storage + strides * items_per_stride);
                }
                else
                {
                    for (size_t j = 0; j < leftover; j++)
                        m_storage[strides * items_per_stride + j] += right.m_storage[strides * items_per_stride + j];
                }
            }
            return *this;
        }

        constexpr matrix& operator-=(matrix const& right)
        {
            constexpr size_t items_per_stride = (sizeof(TSIMDIntrinsic) / sizeof(T));
            constexpr size_t strides = (NCols * NRows) / items_per_stride;
            constexpr size_t leftover = (NCols * NRows) % items_per_stride;
            for (size_t i = 0; i < strides; i++)
            {
                auto v1 = load_packed<TSIMDIntrinsic>(m_storage + i * items_per_stride);
                auto v2 = load_packed<TSIMDIntrinsic>(right.data() + i * items_per_stride);
                store_packed<TSIMDIntrinsic>(sub_packed(v1, v2), m_storage + i * items_per_stride);
            }
            if constexpr (leftover != 0)
            {
                if constexpr (leftover == 4)
                {
                    auto v1 = load_packed<__m128>(m_storage + strides * items_per_stride);
                    auto v2 = load_packed<__m128>(right.data() + strides * items_per_stride);
                    store_packed<__m128>(sub_packed(v1, v2), m_storage + strides * items_per_stride);
                }
                else
                {
                    for (size_t j = 0; j < leftover; j++)
                        m_storage[strides * items_per_stride + j] -= right.m_storage[strides * items_per_stride + j];
                }
            }
            return *this;
        }

        constexpr matrix& operator*=(T right)
        {
            constexpr size_t items_per_stride = (sizeof(TSIMDIntrinsic) / sizeof(T));
            constexpr size_t strides = (NCols * NRows) / items_per_stride;
            constexpr size_t leftover = (NCols * NRows) % items_per_stride;

            auto s = set1_packed<TSIMDIntrinsic>(right);
            for (size_t i = 0; i < strides; i++)
            {
                auto v1 = load_packed<TSIMDIntrinsic>(m_storage + i * items_per_stride);
                store_packed<TSIMDIntrinsic>(mul_packed(v1, s), m_storage + i * items_per_stride);
            }
            if constexpr (leftover != 0)
            {
                if constexpr (leftover == 4)
                {
                    auto v1 = load_packed<__m128>(m_storage + strides * items_per_stride);
                    store_packed<__m128>(mul_packed(v1, s), m_storage + strides * items_per_stride);
                }
                else
                {
                    for (size_t j = 0; j < leftover; j++)
                        m_storage[strides * items_per_stride + j] *= right.m_storage[strides * items_per_stride + j];
                }
            }
            return *this;
        }

        template<size_t OtherNRows, size_t OtherNCols>
        requires(NCols == OtherNRows) constexpr matrix<T, NRows, OtherNCols, TSIMDIntrinsic> operator*(matrix<T, OtherNRows, OtherNCols, TSIMDIntrinsic> const& right)
        {
            using TLocalSIMDIntrinsic = Conditional<sizeof(TSIMDIntrinsic) / sizeof(T) <= NCols, TSIMDIntrinsic, simd::HalfWidthType<TSIMDIntrinsic>>;
            constexpr size_t items_per_stride = (sizeof(TLocalSIMDIntrinsic) / sizeof(T));
            constexpr size_t strides = NCols / items_per_stride;
            constexpr size_t leftover = NCols % items_per_stride;

            matrix<T, NRows, OtherNCols, TSIMDIntrinsic> result;
            matrix<T, OtherNCols, OtherNRows, TSIMDIntrinsic> right_transposed;
            constexpr bool use_transposed = NCols > 31;
            if constexpr (use_transposed)
            {
                right_transposed = right.transposed();
            }

            alignas(16) u32 indices[sizeof(TLocalSIMDIntrinsic) / sizeof(T)];
            for (size_t i = 0; i < items_per_stride; i++)
                indices[i] = OtherNCols * i;

            for (size_t r = 0; r < NRows; ++r)
            {
                for (size_t c = 0; c < OtherNCols; ++c)
                {
                    T item_result;
                    TLocalSIMDIntrinsic acc;
                    T* row_base = m_storage + NCols * r;
                    T const* col_base = right.m_storage + c;
                    if constexpr (NRows >= sizeof(TLocalSIMDIntrinsic) / sizeof(T) || leftover >= sizeof(simd::HalfWidthType<TLocalSIMDIntrinsic>) / sizeof(T))
                        acc = zeroed_register<TLocalSIMDIntrinsic>();
                    if constexpr (NRows >= sizeof(TLocalSIMDIntrinsic) / sizeof(T))
                    {
                        for (size_t i = 0; i < strides; i++)
                        {
                            if constexpr (NCols * sizeof(T) % alignof(TLocalSIMDIntrinsic) == 0)
                            {
                                auto row_segment = load_packed<TLocalSIMDIntrinsic>(row_base + items_per_stride * i);
                                TLocalSIMDIntrinsic col_segment;
                                if constexpr (!use_transposed)
                                    col_segment = gather_packed<TLocalSIMDIntrinsic, sizeof(T)>(col_base + items_per_stride * i * OtherNCols, indices);
                                else
                                    col_segment = load_packed<TLocalSIMDIntrinsic>(right_transposed.m_storage + right_transposed.Columns * c + items_per_stride * i);
                                acc = fmadd_packed<TLocalSIMDIntrinsic>(row_segment, col_segment, acc);
                            }
                            else
                            {
                                auto row_segment = load_packed_unaligned<TLocalSIMDIntrinsic>(row_base + items_per_stride * i);
                                TLocalSIMDIntrinsic col_segment;
                                if constexpr (!use_transposed)
                                    col_segment = gather_packed<TLocalSIMDIntrinsic, sizeof(T)>(col_base + items_per_stride * i * OtherNCols, indices);
                                else
                                    col_segment = load_packed_unaligned<TLocalSIMDIntrinsic>(right_transposed.m_storage + right_transposed.Columns * c + items_per_stride * i);
                                acc = fmadd_packed<TLocalSIMDIntrinsic>(row_segment, col_segment, acc);
                            }
                        }
                    }
                    if constexpr (leftover != 0)
                    {
                        if constexpr (leftover >= sizeof(simd::HalfWidthType<TLocalSIMDIntrinsic>) / sizeof(T) && !IsSame<simd::HalfWidthType<TLocalSIMDIntrinsic>, TLocalSIMDIntrinsic>)
                        {
                            auto row_segment = load_packed_unaligned<simd::HalfWidthType<TLocalSIMDIntrinsic>>(row_base + items_per_stride * strides);
                            simd::HalfWidthType<TLocalSIMDIntrinsic> col_segment;
                            if constexpr (!use_transposed)
                                col_segment = gather_packed<simd::HalfWidthType<TLocalSIMDIntrinsic>, sizeof(T)>(col_base + items_per_stride * strides * OtherNCols, indices);
                            else
                                col_segment = load_packed_unaligned<simd::HalfWidthType<TLocalSIMDIntrinsic>>(right_transposed.m_storage + right_transposed.Columns * c + items_per_stride * strides);
                            auto narrowed_acc = cast_intrinsic<TLocalSIMDIntrinsic, simd::HalfWidthType<TLocalSIMDIntrinsic>>(acc);
                            auto dot = fmadd_packed(row_segment, col_segment, narrowed_acc);
                            auto dot_extended = cast_intrinsic<simd::HalfWidthType<TLocalSIMDIntrinsic>, TLocalSIMDIntrinsic>(dot);
                            acc = blend_packed<TLocalSIMDIntrinsic, 0b11'11'00'00>(dot_extended, acc);

                            auto sum = horizontal_sum(acc);
                            for (size_t i = 0; i < leftover - sizeof(simd::HalfWidthType<TLocalSIMDIntrinsic>) / sizeof(T); ++i)
                            {
                                auto x = this->at(r, items_per_stride * strides + i + sizeof(simd::HalfWidthType<TLocalSIMDIntrinsic>) / sizeof(T)); //*(m_storage+NCols * r + items_per_stride*strides+i);
                                simd::UnitType<TLocalSIMDIntrinsic> y;
                                if constexpr (!use_transposed)
                                    y = right.at(items_per_stride * strides + i + sizeof(simd::HalfWidthType<TLocalSIMDIntrinsic>) / sizeof(T), c);
                                else
                                    y = right_transposed.at(r, items_per_stride * strides + i + sizeof(simd::HalfWidthType<TLocalSIMDIntrinsic>) / sizeof(T));
                                sum += x * y;
                            }
                            item_result = sum;
                        }
                        else
                        {
                            simd::UnitType<TLocalSIMDIntrinsic> sum {};
                            if constexpr (strides > 0)
                                sum = horizontal_sum(acc);
                            for (size_t i = 0; i < leftover; ++i)
                            {
                                auto x = this->at(r, items_per_stride * strides + i);
                                auto y = right.at(items_per_stride * strides + i, c);
                                sum += x * y;
                            }
                            item_result = sum;
                        }
                    }
                    else
                    {
                        item_result = horizontal_sum(acc);
                    }
                    result.at(r, c) = item_result;
                }
            }
            return result;
        }

    private:
        [[maybe_unused]] u8 __zero_padding[((NRows * NCols) | 31) + 1 - (NRows * NCols)];

    public:
        T m_storage[NRows * NCols];
    };

    template<FloatingPoint T, size_t Size, simd::SIMDType TSIMDIntrinsic>
    class matrix<T, Size, 1, TSIMDIntrinsic>
    {
    public:
        constexpr matrix() = default;

    private:
        template<size_t Index, typename TFirst, typename... TRest>
        constexpr void variadic_constructor_helper(TFirst first, TRest... rest)
        {
            if constexpr (sizeof...(TRest) == 1)
            {
                m_storage[Index] = first;
            }
            else
            {
                m_storage[Index] = first;
                set_values<Index + 1>(rest...);
            }
        }

    public:
        template<typename... U>
        requires(Same<T, U>&&...) constexpr matrix(U... values)
        {
            variadic_constructor_helper<0>(values...);
        }

        constexpr void fill(T value)
        {
            for (size_t i = 0; i < Size; ++i)
                m_storage[i] = value;
        }

    public:
        T m_storage[Size];
    };

    template<FloatingPoint T, size_t Size>
    class matrix<T, 1, Size>
    {
    };

    template<FloatingPoint T>
    class matrix<T, 4, 4>
    {
    };
}

template<FloatingPoint T, size_t Rows, size_t Cols, typename TIntrinsic = neo::math::simd::WidestSIMDTypeAvailable<T>>
using matrix = neo::math::matrix<T, Rows, Cols, TIntrinsic>;
template<FloatingPoint T, size_t Size>
using row_vector = neo::math::matrix<T, Size, 1>;
template<FloatingPoint T, size_t Size>
using column_vector = neo::math::matrix<T, 1, Size>;

template<FloatingPoint T>
using m2x2 = neo::math::matrix<T, 2, 2>;
template<FloatingPoint T>
using m2x3 = neo::math::matrix<T, 3, 3>;
template<FloatingPoint T>
using m3x3 = neo::math::matrix<T, 3, 3>;
template<FloatingPoint T>
using m3x4 = neo::math::matrix<T, 3, 4>;
template<FloatingPoint T>
using m4x4 = neo::math::matrix<T, 4, 4>;

using m2x2f = neo::math::matrix<float, 2, 2>;
using m2x3f = neo::math::matrix<float, 3, 3>;
using m3x3f = neo::math::matrix<float, 3, 3>;
using m3x4f = neo::math::matrix<float, 3, 4>;
using m4x4f = neo::math::matrix<float, 4, 4>;

using m2x2d = neo::math::matrix<double, 2, 2>;
using m2x3d = neo::math::matrix<double, 3, 3>;
using m3x3d = neo::math::matrix<double, 3, 3>;
using m3x4d = neo::math::matrix<double, 3, 4>;
using m4x4d = neo::math::matrix<double, 4, 4>;

using float3 = column_vector<float, 3>;
using float4 = column_vector<float, 4>;
using double3 = column_vector<double, 4>;
using double4 = column_vector<double, 4>;
