#pragma once

#include <xng/math.hpp>
#include <xng/graphics/image.hpp>

namespace xng
{
	namespace graphics
	{
		class material :
			public res::resource
		{

		public:

			static const char * resource_type;

			material(const char * name,
				const res::resource_parameters & params,
				res::resource_loader_ptr loader,
				res::resource_manager * owner);

			const math::float3 & get_base_color(void) const;
			void set_base_color(const math::float3 & baseColor);

			image_ptr get_base_texture(void) const;
			void set_base_texture(const image_ptr & baseTexture);

			image_ptr get_normal_map(void) const;
			void set_normal_map(const image_ptr & normalMap);

			image_ptr get_specular_map(void) const;
			void set_specular_map(const image_ptr & specularMap);

			void set_default(void);

		protected:

			bool load_impl(const void * userdata) override;
			void unload_impl(void) override;
			size_t calculate_size_impl(void) override;

		private:

			math::float3 m_baseColor;

			float        m_alpha;
			float        m_metallic;
			float        m_roughness;
			float        m_subsurface;
			float        m_specular;
			float        m_fresnelZero;

			image_ptr    m_baseTexture;
			image_ptr    m_normalMap;
			image_ptr    m_specularMap;

		};

		typedef res::resource_ptr<material>           material_ptr;
		typedef res::basic_resource_manager<material> material_manager;
	}
}