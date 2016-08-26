#pragma once

#include <iterator>

namespace xng
{
	namespace math
	{
		namespace detail
		{
			template <typename T, int N, int M>
			struct row_iterator :
				std::iterator<std::forward_iterator_tag, T>
			{

				row_iterator(T * data, int index = N * M) :
					m_data(data), m_index(index) {}

				row_iterator(const row_iterator &) = default;
				row_iterator & operator= (const row_iterator &) = default;

				row_iterator & operator++ ()
				{
					++m_index;
					return *this;
				}

				bool operator== (const row_iterator & it)
				{
					assert(it.m_data == m_data && "Can't compare iterators for different objects");
					return it.m_index == m_index;
				}

				T & operator* (void)
				{
					assert(m_index < N * M && "Attempting to dereference out of bounds pointer");
					return m_data[m_index];
				}

			private:

				T * m_data;
				int m_index;

			};

			template <typename T, int N, int M>
			struct column_iterator :
				std::iterator<std::forward_iterator_tag, T>
			{

				column_iterator(T * data, int index = N * M) :
					m_data(data), m_index(index), m_row(index % N) {}

				column_iterator(const column_iterator &) = default;
				column_iterator & operator= (const column_iterator &) = default;

				column_iterator & operator++ ()
				{
					m_index += N;
					int col = m_index / N;
					if (col == N)
					{
						m_index = (++m_row);
					}
					return *this;
				}

				bool operator== (const column_iterator & it)
				{
					assert(it.m_data == m_data && "Can't compare iterators for different objects");
					return it.m_index == m_index;
				}

				T & operator* (void)
				{
					assert(m_index < N * M && "Attempting to dereference out of bounds pointer");
					return m_data[m_index];
				}

			private:

				T * m_data;
				int m_index;
				int m_row;

			};
		}
	}
}