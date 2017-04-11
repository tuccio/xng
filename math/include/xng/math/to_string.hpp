#pragma once

#include "../math.hpp"

#include <string>
#include <sstream>
#include <istream>
#include <ostream>
#include <regex>

namespace xng
{
    namespace math
    {
        template <typename Char, typename T, int N, int M>
        struct default_matrix_formatter
        {

            void matrix_begin(std::basic_ostream<Char> & os, const matrix<T, N, M> & m) { os << "["; }
            void matrix_end(std::basic_ostream<Char> & os, const matrix<T, N, M> & m) { os << "]"; }

            void row_begin(std::basic_ostream<Char> & os, const matrix<T, N, M> & m, int i) {}
            void row_end(std::basic_ostream<Char> & os, const matrix<T, N, M> & m, int i) { if (i != N - 1) os << "; "; }

            void column_begin(std::basic_ostream<Char> & os, const matrix<T, N, M> & m, int j) {}
            void column_end(std::basic_ostream<Char> & os, const matrix<T, N, M> & m, int j) { if (j != M - 1) os << ", ";  }

        };

        template <typename Char>
        struct default_quaternion_formatter
        {

            void quaternion_begin(std::basic_ostream<Char> & os, const quaternion & q) { os << "["; }
            void quaternion_end(std::basic_ostream<Char> & os, const quaternion & q) { os << "]"; }

            void scalar_begin(std::basic_ostream<Char> & os, const quaternion & q) {}
            void scalar_end(std::basic_ostream<Char> & os, const quaternion & q) { os << "; "; }

            void vector_begin(std::basic_ostream<Char> & os, const quaternion & q) {}
            void vector_end(std::basic_ostream<Char> & os, const quaternion & q) {}

            void vector_separator(std::basic_ostream<Char> & os, const quaternion & q) {os << ", "; }

        };

        // Output Stream

        template <typename Char, typename T, int N, int M, template <typename Char, typename T, int N, int M> class Formatter = default_matrix_formatter>
        std::basic_ostream<Char> & operator<< (std::basic_ostream<Char> & os, const matrix<T, N, M> & m)
        {
            Formatter<Char, T, N, M> f;

            f.matrix_begin(os, m);

            for (int i = 0; i < N; i++)
            {

                f.row_begin(os, m, i);

                for (int j = 0; j < M; j++)
                {

                    f.column_begin(os, m, j);
                    os << m(i, j);
                    f.column_end(os, m, j);

                }

                f.row_end(os, m, i);

            }

            f.matrix_end(os, m);

            return os;
        }

        template <typename Char, template <typename Char> class Formatter = default_quaternion_formatter>
        std::basic_ostream<Char> & operator<< (std::basic_ostream<Char> & os, const quaternion & q)
        {
            Formatter<Char> f;

            f.quaternion_begin(os, q);

            f.scalar_begin(os, q);
            os << q.w;
            f.scalar_end(os, q);

            f.vector_begin(os, q);
            os << q.x;
            f.vector_separator(os, q);
            os << q.y;
            f.vector_separator(os, q);
            os << q.z;
            f.vector_end(os, q);

            f.quaternion_end(os, q);

            return os;
        }

        // Input Stream

        template <typename Char, typename T, int N, int M, template <typename Char, typename T, int N, int M> class Formatter = default_matrix_formatter>
        std::basic_istream<Char> & operator>> (std::basic_istream<Char> & is, matrix<T, N, M> & m)
        {
            // TODO
            return is;
        }

    }
}