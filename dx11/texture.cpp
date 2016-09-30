#include <xng/dx11/texture.hpp>
#include <xng/dx11/dxgi_formats.hpp>

using namespace xng::dx11;
using namespace xng::res;
using namespace xng::graphics;

const char * texture::resource_type = "dx11texture";

texture::texture(const char * name, const resource_parameters & params, resource_loader_ptr loader, resource_manager * owner) :
	resource(name, params, loader, owner)
{
	m_width  = 0;
	m_height = 0;
	m_format = DXGI_FORMAT_UNKNOWN;
}

bool texture::create(
	ID3D11Device * device,
	ID3D11DeviceContext * context,
	image_ptr image,
	D3D11_USAGE usage,
	UINT mipmaps,
	bool generateMipmaps)
{
	clear();

	if (image && image->load())
	{
		m_format = get_dxgi_format(image->get_format());

		D3D11_SUBRESOURCE_DATA data = {};

		data.pSysMem     = image->data();
		data.SysMemPitch = image->get_scan_width();

		if (!generateMipmaps)
		{
			return !XNG_HR_FAILED(device->CreateTexture2D(
				&CD3D11_TEXTURE2D_DESC(m_format, image->get_width(), image->get_height(), 1, mipmaps, D3D11_BIND_SHADER_RESOURCE, usage),
				&data,
				m_texture.reset_and_get_address())) &&
				!XNG_HR_FAILED(device->CreateShaderResourceView(m_texture.get(), nullptr, m_srv.reset_and_get_address()));
		}
		else if(!XNG_HR_FAILED(device->CreateTexture2D(
			&CD3D11_TEXTURE2D_DESC(m_format,
				image->get_width(), image->get_height(),
				1, mipmaps,
				D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
				usage, 0, 1, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS),
			nullptr,
			m_texture.reset_and_get_address())))
		{
			context->UpdateSubresource(m_texture.get(), 0, nullptr, data.pSysMem, data.SysMemPitch, 0);

			if (!XNG_HR_FAILED(device->CreateShaderResourceView(m_texture.get(), nullptr, m_srv.reset_and_get_address())))
			{
				context->GenerateMips(m_srv.get());
				return true;
			}
		}
		
	}

	return false;
}

void texture::clear(void)
{
	m_texture.reset();
	m_srv.reset();
}

bool texture::load_impl(const void * userdata)
{
	image_ptr img;
	
	if (!get_dependency() || !(img = dynamic_cast<image*>(get_dependency())))
	{
		img = resource_factory::get_singleton()->create<image>(get_name());
	}

	if (img)
	{
		const resource_parameters & params = get_parameters();

		auto optMips    = params.get_optional<uint32_t>("mipmaps");
		auto optGenMips = params.get_optional<bool>("generate_mipmaps");

		bool        genMips = optGenMips ? *optGenMips : false;
		UINT        mips    = optMips ? *optMips : (genMips ? 0 : 1);
		D3D11_USAGE usage   = genMips ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;

		const load_data * data = static_cast<const load_data*>(userdata);

		return create(data->device, data->context, img, usage, mips, genMips);
	}

	return false;
}

void texture::unload_impl(void)
{
	clear();
}

size_t texture::calculate_size_impl(void)
{
	// TODO: Consider mipmaps
	return get_dxgi_format_byte_size(m_format) * m_width * m_height;
}