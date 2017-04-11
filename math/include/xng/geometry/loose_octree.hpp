#pragma once

#include <xng/geometry.hpp>
#include <xng/math.hpp>

#include <functional>
#include <list>
#include <unordered_map>

#include "morton.hpp"

#define XNG_LOOSEOCTREE_DEFAULT_MAXDEPTH 8

enum xng_loose_octree_children
{
    XNG_LOOSEOCTREE_FRONT_BOTTOM_RIGHT = 0, // 000
    XNG_LOOSEOCTREE_FRONT_BOTTOM_LEFT  = 1, // 001
    XNG_LOOSEOCTREE_FRONT_TOP_RIGHT    = 2, // 010
    XNG_LOOSEOCTREE_FRONT_TOP_LEFT     = 3, // 011
    XNG_LOOSEOCTREE_BACK_BOTTOM_RIGHT  = 4, // 100
    XNG_LOOSEOCTREE_BACK_BOTTOM_LEFT   = 5, // 101
    XNG_LOOSEOCTREE_BACK_TOP_RIGHT     = 6, // 110
    XNG_LOOSEOCTREE_BACK_TOP_LEFT      = 7, // 111
};

namespace xng
{
    namespace math
    {
        namespace detail
        {
            template <typename Object>
            struct loose_octree_node
            {
                std::list<Object> objects;
                uint8_t           childrenMask;
                uint32_t          occupancy;

                loose_octree_node(void) : childrenMask(0), occupancy(0) { }
            };
        }

        /*
        *
        * Loose octree with looseness factor k = 2, represented as a linear octree
        * with 64 bit Morton code for octant location key (hence max depth is 21)
        *
        * Insertion and removal are O(1)
        *
        */

        template <typename Object,
            typename BoundingVolume,
            typename BoundingVolumeFunctor,
            typename Comparator = std::equal_to<Object>>
            class loose_octree
        {

        public:

            typedef std::list<Object> objects_list;
            typedef typename objects_list::iterator objects_list_iterator;

            loose_octree(void);

            loose_octree(const vec128 & center,
                float halfextent,
                unsigned int maxdepth = XNG_LOOSEOCTREE_DEFAULT_MAXDEPTH,
                BoundingVolumeFunctor functor = BoundingVolumeFunctor(),
                Comparator comparator = Comparator());

            loose_octree(const loose_octree & octree) = default;
            loose_octree(loose_octree && octree) = default;

            ~loose_octree(void);

            void clear(void);

            loose_octree & operator= (const loose_octree &) = default;
            loose_octree & operator= (loose_octree &&) = default;

            bool insert(const Object & object);
            bool insert(const Object & object, const BoundingVolume & volume);

            bool remove(const Object & object);
            bool remove(const Object & object, const BoundingVolume & volume);

            template <typename QueryType, typename Visitor>
            void query(const QueryType & query, Visitor visitor);

            bool ray_pick(const ray & ray, Object & result, float & t);

            template <typename Visitor>
            void traverse(Visitor visitor);

            aabb get_aabb(void) const;

            size_t size(void) const;
            XNG_INLINE bool empty(void) const { return size() == 0; }

            void shrink(void);

        private:

            typedef detail::loose_octree_node<Object>     node;

            typedef std::unordered_map<morton_code, node> node_hashmap;
            typedef typename node_hashmap::iterator       node_hashmap_iterator;

            float3 m_center;
            float3 m_halfextent;

            unsigned int m_maxdepth;

            BoundingVolumeFunctor m_functor;
            Comparator            m_comparator;

            node_hashmap m_nodes;

            /* Private methods */

            void create_octree(const vec128 & center,
                float halfextent,
                unsigned int maxdepth,
                BoundingVolumeFunctor functor,
                Comparator comparator);

            typename node_hashmap_iterator create_octant(morton_code octantLocation);

            morton_code calculate_fitting_octant(const aabb & aabb) const;

            unsigned int get_octant_depth(morton_code octant) const;
            aabb         get_octant_aabb(morton_code octant) const;
            uint3        get_octant_coords(morton_code octant) const;

            template <typename QueryType, typename Visitor>
            void traverse(morton_code currentLocation,
                const aabb & current,
                const QueryType & query,
                Visitor visitor);

            XNG_INLINE void ray_pick(morton_code currentLocation,
                const aabb & current,
                const ray  & ray,
                float & minDistance,
                Object * & object);

            template <typename Visitor>
            void dfs_visit(morton_code octant, Visitor visitor);

            void increase_occupancy(node_hashmap_iterator octant);
            void decrease_occupancy(node_hashmap_iterator octant);

        };
    }
}

#include "loose_octree.inl"