#pragma once

#include <cstdint>
#include <map>

#define XNG_POOL_INVALID ((pool_size_t) -1)

namespace xng
{
	namespace core
	{
		typedef uint32_t pool_size_t;

		class pool_manager
		{

		public:

			pool_manager(void);
			pool_manager(pool_size_t size);
			pool_manager(const pool_manager &) = default;
			pool_manager(pool_manager &&) = default;

			~pool_manager(void);

			pool_manager & operator= (const pool_manager & manager) = default;
			pool_manager & operator= (pool_manager && manager) = default;

			pool_size_t allocate(pool_size_t elements = 1u);
			void free(pool_size_t offset, pool_size_t elements = 1u);
			bool has_space(pool_size_t elements);

			void clear(void);			

		private:

			using pool_type = std::map<pool_size_t, pool_size_t>;
			using pool_iterator = pool_type::iterator;

			pool_iterator find_free_chunk(pool_size_t elements);
			void allocate_chunk(pool_iterator it, pool_size_t elements);
			void free_chunk(pool_size_t offset, pool_size_t elements);

			pool_type m_freeChunks;
			uint32_t  m_objectsCount;

		};
	}
}