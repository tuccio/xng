#include <boost/preprocessor/variadic.hpp>
#include <boost/preprocessor/variadic/to_list.hpp>
#include <boost/preprocessor/list/for_each.hpp>

#include <deque>

#define XNG_LOOSEOCTREE_TEMPLATE_DECLARATION template <typename Object, typename BoundingVolume, typename BoundingVolumeFunctor, typename Comparator>
#define XNG_LOOSEOCTREE_TYPE                 loose_octree<Object, BoundingVolume, BoundingVolumeFunctor, Comparator>

#define XNG_LOOSEOCTREE_MAKE_CHILD_MASK(Child)                 (1 << (Child))
#define XNG_LOOSEOCTREE_MAKE_CHILDREN_MASK_LOOP(r, data, elem) | XNG_LOOSEOCTREE_MAKE_CHILD_MASK(elem)
#define XNG_LOOSEOCTREE_MAKE_CHILDREN_MASK(...)                (0 BOOST_PP_LIST_FOR_EACH(XNG_LOOSEOCTREE_MAKE_CHILDREN_MASK_LOOP, 0, BOOST_PP_VARIADIC_TO_LIST(__VA_ARGS__)))

#define XNG_LOOSEOCTREE_ROOT_INDEX (1)

namespace xng
{
	namespace math
	{

			XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			XNG_LOOSEOCTREE_TYPE::loose_octree(void)
		{

		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			XNG_LOOSEOCTREE_TYPE::loose_octree(const vec128 & center,
				float halfextent,
				unsigned int maxdepth,
				BoundingVolumeFunctor functor,
				Comparator comparator)
		{
			create_octree(center, halfextent, maxdepth, functor, comparator);
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			XNG_LOOSEOCTREE_TYPE::~loose_octree(void)
		{
			clear();
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			void XNG_LOOSEOCTREE_TYPE::clear(void)
		{
			m_nodes.clear();
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			bool XNG_LOOSEOCTREE_TYPE::insert(const Object & object)
		{
			return insert(object, m_functor(object));
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			bool XNG_LOOSEOCTREE_TYPE::insert(const Object & object, const BoundingVolume & volume)
		{
			aabb aabb = bounding_aabb(volume);

			if (!contains(get_octant_aabb(XNG_LOOSEOCTREE_ROOT_INDEX), aabb))
			{
				return false;
			}

			morton_code octant = calculate_fitting_octant(aabb);

			node_hashmap_iterator it = create_octant(octant);

			it->second.objects.push_back(object);
			increase_occupancy(it);

			return true;
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			bool XNG_LOOSEOCTREE_TYPE::remove(const Object & object)
		{
			return remove(object, m_functor(object));
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			bool XNG_LOOSEOCTREE_TYPE::remove(const Object & object, const BoundingVolume & volume)
		{
			aabb aabb = bounding_aabb(volume);

			morton_code octant = calculate_fitting_octant(aabb);

			auto it = m_nodes.find(octant);

			if (it != m_nodes.end())
			{
				node & node = it->second;

				for (auto listIt = node.objects.begin(); listIt != node.objects.end(); listIt++)
				{
					if (m_comparator(*listIt, object))
					{
						node.objects.erase(listIt);
						decrease_occupancy(it);
						return true;
					}
				}
			}

			return false;
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			template <typename QueryType, typename Visitor>
		void XNG_LOOSEOCTREE_TYPE::query(const QueryType & query, Visitor visitor)
		{
			aabb rootAABB = aabb::from_center_half_extents(m_center, m_halfextent * 2.f);
			traverse(XNG_LOOSEOCTREE_ROOT_INDEX, rootAABB, query, visitor);
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			bool XNG_LOOSEOCTREE_TYPE::ray_pick(const ray & ray, Object & result, float & t)
		{
			aabb rootAABB = aabb::from_center_half_extents(m_center, m_halfextent * 2.f);

			Object * object = nullptr;
			t = std::numeric_limits<float>::infinity();

			ray_pick(XNG_LOOSEOCTREE_ROOT_INDEX, rootAABB, ray, t, object);

			if (object != nullptr)
			{
				result = *object;
				return true;
			}
			else
			{
				return false;
			}
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			void XNG_LOOSEOCTREE_TYPE::create_octree(const vec128 & center,
				float halfextent,
				unsigned int maxdepth,
				BoundingVolumeFunctor functor,
				Comparator comparator)
		{
			m_center = center;
			m_halfextent = vec128_set(halfextent, halfextent, halfextent, halfextent);
			m_maxdepth = maxdepth;
			m_functor = functor;
			m_comparator = comparator;

			m_nodes.emplace(XNG_LOOSEOCTREE_ROOT_INDEX, node());
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			template <typename Visitor>
		void XNG_LOOSEOCTREE_TYPE::traverse(Visitor visitor)
		{
			dfs_visit(XNG_LOOSEOCTREE_ROOT_INDEX,
				[this, visitor](node_hashmap_iterator it)
			{
				aabb aabb = get_octant_aabb(it->first);
				visitor(aabb, it->second.objects.begin(), it->second.objects.end());
			});
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			aabb XNG_LOOSEOCTREE_TYPE::get_aabb(void) const
		{
			return aabb::from_center_half_extents(m_center, m_halfextent);
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			typename XNG_LOOSEOCTREE_TYPE::node_hashmap_iterator
			XNG_LOOSEOCTREE_TYPE::create_octant(morton_code octantLocation)
		{
			auto nodeInsertResult = m_nodes.emplace(octantLocation, node());

			node_hashmap_iterator octant = nodeInsertResult.first;

			// If the node was created, initialize it and continue in the recursion
			if (nodeInsertResult.second)
			{

				morton_code childCode = (octantLocation & 7);

				morton_code parentLocation = octantLocation >> 3;
				node_hashmap_iterator parent = create_octant(parentLocation);

				// Add the XNG_NEW node to the parent
				parent->second.childrenMask |= XNG_LOOSEOCTREE_MAKE_CHILD_MASK(childCode);

			}

			return octant;
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			void XNG_LOOSEOCTREE_TYPE::shrink(void)
		{
			dfs_visit(XNG_LOOSEOCTREE_ROOT_INDEX,
				[this](node_hashmap_iterator it)
			{

				if (it->second.occupancy == 0 && it->first != XNG_LOOSEOCTREE_ROOT_INDEX)
				{
					morton_code parentCode = it->first >> 3;
					auto parentIt = m_nodes.find(parentCode);
					parentIt->second.childrenMask ^= XNG_LOOSEOCTREE_MAKE_CHILDREN_MASK((it->first & 7));
					m_nodes.erase(it);
				}

			});
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			morton_code XNG_LOOSEOCTREE_TYPE::calculate_fitting_octant(const aabb & aabb) const
		{
			vec128 center = aabb.get_center();
			vec128 hExtents = aabb.get_half_extents();

			float maxExtent = std::max(std::max(vec128_get_x(hExtents), vec128_get_y(hExtents)), vec128_get_z(hExtents));

			vec128 normalizedCentroid = (center - m_center) + m_halfextent;
			normalizedCentroid = normalizedCentroid / (2.f * m_halfextent);

			// Given an object of size wx, we want a depth d such that wx <= wd, where wd = w0 / 2^d
			// Solving we have d <= log2 w0/wx. So d = floor(log2(w0/wx)) is the max depth an
			// object can fit in.

			unsigned int fitDepth = (unsigned int)std::floor(std::log2(vec128_get_x(m_halfextent) / maxExtent));
			unsigned int depth = min(fitDepth, m_maxdepth);

			vec128_f32 t = vec128_floor(normalizedCentroid * (1 << m_maxdepth));

			morton_unpacked octreeCenterCoordinates = {
				static_cast<uint32_t>(t.f32[0]),
				static_cast<uint32_t>(t.f32[1]),
				static_cast<uint32_t>(t.f32[2])
			};

			morton_code centerLocation = morton_encode3(octreeCenterCoordinates);

			// Build the octant location code with the sentinel value

			morton_code sentinel = 1ULL << (3L * m_maxdepth);
			morton_code centerLocationSentinel = centerLocation | sentinel;
			morton_code location = centerLocationSentinel >> (3 * (m_maxdepth - depth));

			return location;
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			unsigned int XNG_LOOSEOCTREE_TYPE::get_octant_depth(morton_code location) const
		{
			// Find the most significant one (the sentinel value) and
			// return the number of bit triplets in the code as the depth value

			uint32_t mso = most_significant_one(location);
			return mso / 3;
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			aabb XNG_LOOSEOCTREE_TYPE::get_octant_aabb(morton_code location) const
		{
			auto it = m_nodes.find(location);

			if (it != m_nodes.end())
			{
				unsigned int depth = get_octant_depth(location);

				// The half extents of the regular octant
				vec128 hd = m_halfextent * (1.f / (1 << depth));
				vec128 hdloose = hd * 2.f;

				uint3 octantCoords = get_octant_coords(location);

				vec128 ijk = vec128_set(octantCoords.x, octantCoords.y, octantCoords.z, 0.f);

				ijk = ijk * 2.f;
				ijk = ijk + vec128_one();

				vec128 minPoint = m_center - m_halfextent;
				vec128 center = ijk * hd + minPoint;

				return aabb::from_center_half_extents(center, hdloose);
			}

			return aabb::from_center_half_extents(vec128_zero(), vec128_zero());
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			uint3 XNG_LOOSEOCTREE_TYPE::get_octant_coords(morton_code location) const
		{
			uint32_t mso = most_significant_one(location);

			// Remove the sentinel
			morton_code code = (1ULL << mso) ^ location;
			return reinterpret_cast<const uint3&>(morton_decode3(code));
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			template <typename QueryType, typename Visitor>
		void XNG_LOOSEOCTREE_TYPE::traverse(morton_code currentLocation,
			const aabb & current,
			const QueryType & query,
			Visitor visitor)
		{
			auto it = m_nodes.find(currentLocation);

			if (it != m_nodes.end() &&
				it->second.occupancy > 0 &&
				intersect(current, query))
			{
				for (Object & o : it->second.objects)
				{
					if (intersect(query, m_functor(o)))
					{
						visitor(o);
					}
				}

				auto currentCenter = current.get_center();
				auto currentHalfExtents = current.get_half_extents();

				auto nextHalfExtents = currentHalfExtents * .5f;
				auto shiftSize = currentHalfExtents * .25f;

				for (int child = 0; child < 8; child++)
				{
					morton_code nextLevelLocation = currentLocation << 3;

					if (it->second.childrenMask & XNG_LOOSEOCTREE_MAKE_CHILD_MASK(child))
					{
						morton_code childCode = nextLevelLocation | child;

						// Set the direction of shift setting the signbit to 0/1 according to the child
						// location (thus moving in the positive/negative direction as needed) ...

						vec128 centerShift = vec128_set(child & 1 ? 0.f : -0.f,
							child & 2 ? 0.f : -0.f,
							child & 4 ? 0.f : -0.f,
							0.f);

						// ... then set mantissa/exponent to the shift size

						centerShift = vec128_or(centerShift, shiftSize);

						aabb childAABB = aabb::from_center_half_extents(currentCenter + centerShift, nextHalfExtents);

						traverse(childCode, childAABB, query, visitor);
					}
				}
			}
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			void XNG_LOOSEOCTREE_TYPE::ray_pick(morton_code currentLocation,
				const aabb & current,
				const ray  & ray,
				float & minDistance,
				Object * & object)
		{
			if (intersect(ray, current))
			{
				auto it = m_nodes.find(currentLocation);

				for (Object & o : it->second.objects)
				{

					float t;
					if (intersect(ray, m_functor(o), t) && t < minDistance)
					{
						minDistance = t;
						object = &o;
					}
				}

				float3 currentCenter      = current.center;
				float3 currentHalfExtents = current.halfExtents;

				float3 nextHalfExtents = currentHalfExtents * .5f;
				float3 shiftSize       = currentHalfExtents * .25f;

				for (int child = 0; child < 8; child++)
				{
					morton_code nextLevelLocation = currentLocation << 3;

					if (it->second.childrenMask & XNG_LOOSEOCTREE_MAKE_CHILD_MASK(child))
					{
						morton_code childCode = nextLevelLocation | child;

						// Set the direction of shift setting the signbit to 0/1 according to the child
						// location (thus moving in the positive/negative direction as needed)

						float3 centerShift = {
							child & 1 ? shiftSize : -shiftSize,
							child & 2 ? shiftSize : -shiftSize,
							child & 4 ? shiftSize : -shiftSize,
						};

						aabb childAABB = aabb::from_center_half_extents(currentCenter + centerShift, nextHalfExtents);

						ray_pick(childCode, childAABB, ray, minDistance, object);
					}
				}
			}
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			template <typename Visitor>
		void XNG_LOOSEOCTREE_TYPE::dfs_visit(morton_code code, Visitor visitor)
		{
			auto it = m_nodes.find(code);

			morton_code nextLevelLocation = code << 3;

			for (int child = 0; child < 8; child++)
			{
				if (it->second.childrenMask & XNG_LOOSEOCTREE_MAKE_CHILD_MASK(child))
				{
					dfs_visit(nextLevelLocation | child, visitor);
				}
			}

			visitor(it);
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			void XNG_LOOSEOCTREE_TYPE::increase_occupancy(node_hashmap_iterator octant)
		{
			octant->second.occupancy++;

			if (octant->first != XNG_LOOSEOCTREE_ROOT_INDEX)
			{
				auto parentIt = m_nodes.find(octant->first >> 3);
				increase_occupancy(parentIt);
			}
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			void XNG_LOOSEOCTREE_TYPE::decrease_occupancy(node_hashmap_iterator octant)
		{
			octant->second.occupancy--;

			if (octant->first != XNG_LOOSEOCTREE_ROOT_INDEX)
			{
				auto parentIt = m_nodes.find(octant->first >> 3);
				decrease_occupancy(parentIt);
			}
		}

		XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
			size_t XNG_LOOSEOCTREE_TYPE::size(void) const
		{
			auto it = m_nodes.find(XNG_LOOSEOCTREE_ROOT_INDEX);
			return it->second.occupancy;
		}

	}

}

#undef XNG_LOOSEOCTREE_TEMPLATE_DECLARATION
#undef XNG_LOOSEOCTREE_TYPE