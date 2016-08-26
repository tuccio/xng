#include <xng/core.hpp>
#include <xng/core/pool_manager.hpp>

using namespace xng::core;

pool_manager::pool_manager(void) : m_objectsCount(0) { }

pool_manager::pool_manager(pool_size_t size) :
	m_freeChunks({ { 0, size } }),
	m_objectsCount(size) {}

pool_manager::~pool_manager(void)
{
	clear();
}

pool_size_t pool_manager::allocate(pool_size_t elements)
{
	pool_iterator it = find_free_chunk(elements);

	if (it == m_freeChunks.end())
	{
		XNG_LOG("pool_manager", "Not enough memory available for the requested allocation.")
		return XNG_POOL_INVALID;
	}

	pool_size_t allocatedOffset = it->first + it->second - elements;

	allocate_chunk(it, elements);

	return allocatedOffset;
}

void pool_manager::free(pool_size_t offset, pool_size_t elements)
{
	free_chunk(offset, elements);
}

bool pool_manager::has_space(pool_size_t elements)
{
	return find_free_chunk(elements) != m_freeChunks.end();
}

void pool_manager::clear(void)
{
	m_objectsCount = 0;
	m_freeChunks.clear();
}

pool_manager::pool_iterator pool_manager::find_free_chunk(pool_size_t elements)
{
	auto end = m_freeChunks.end();

	for (auto it = m_freeChunks.begin(); it != end; it++)
	{
		if (it->second >= elements)
		{
			return it;
		}
	}

	return end;
}

void pool_manager::allocate_chunk(pool_manager::pool_iterator it, pool_size_t elements)
{
	it->second -= elements;
}

void pool_manager::free_chunk(pool_size_t offset, pool_size_t elements)
{
	if (offset > m_objectsCount)
	{
		return;
	}
	
	auto lb = m_freeChunks.lower_bound(offset);
	auto ub = m_freeChunks.upper_bound(offset);

	// We can merge with the lower bound if we're freeing the element(s) right after it
	bool mergeLower = lb != m_freeChunks.end() && ((lb->first + lb->second) == offset);

	// We can merge with the upper bound if we're freeing the element(s) righte before it
	bool mergeUpper = ub != m_freeChunks.end() && (ub->first == (offset + elements));

	if (mergeLower)
	{

		if (mergeUpper)
		{
			// Merge both into the lb
			lb->second += elements + ub->second;
			m_freeChunks.erase(ub);
		}
		else
		{
			// Merge with the lb only
			lb->second += elements;
		}
		
	}
	else if (mergeUpper)
	{
		// Merge with the ub only
		m_freeChunks.emplace(offset, elements + ub->second);
		m_freeChunks.erase(ub);
	}
	else
	{
		m_freeChunks.emplace(offset, elements);
	}
}