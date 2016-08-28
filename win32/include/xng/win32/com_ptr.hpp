#pragma once

namespace xng
{
	namespace win32
	{
		template <typename ComObject>
		class com_ptr
		{
	
		public:
	
			XNG_INLINE com_ptr(void) : m_pointer(nullptr) { }
	
			XNG_INLINE com_ptr(ComObject * pointer) : m_pointer(pointer) { if (m_pointer) m_pointer->AddRef(); }
	
			XNG_INLINE com_ptr(const com_ptr<ComObject> & com) : com_ptr(com.get()) { }
			XNG_INLINE com_ptr(com_ptr<ComObject> && com) { m_pointer = com.m_pointer; com.m_pointer = nullptr; }
	
			XNG_INLINE ~com_ptr(void) { if (m_pointer) m_pointer->Release(); }
	
			XNG_INLINE ComObject * get(void) const { return m_pointer; }
																
			XNG_INLINE ComObject * const * get_address(void) const { return &m_pointer; }
			XNG_INLINE ComObject **        get_address(void)       { return &m_pointer; }
																
			XNG_INLINE void         reset(void) { if (m_pointer) { m_pointer->Release(); m_pointer = nullptr; } }
			XNG_INLINE ComObject ** reset_and_get_address(void) { reset(); return get_address(); }

			XNG_INLINE ComObject * release(void) { ComObject * ptr = m_pointer; m_pointer = nullptr; return ptr; }
	
			XNG_INLINE void swap(com_ptr<ComObject> & com) { ComObject * tmp = m_pointer; m_pointer = com.m_pointer; com.m_pointer = tmp; }
	
			XNG_INLINE operator bool() const { return m_pointer != nullptr; }
	
			XNG_INLINE ComObject * operator-> (void) const { return m_pointer;  }
	
			XNG_INLINE ComObject * const * operator&(void) const { return &m_pointer; }
			XNG_INLINE ComObject **        operator&(void) { return &m_pointer; }
	
			XNG_INLINE bool operator == (const com_ptr<ComObject> & com) { return m_pointer == com.m_pointer; }
			XNG_INLINE bool operator != (const com_ptr<ComObject> & com) { return m_pointer != com.m_pointer; }
	
			XNG_INLINE com_ptr & operator= (const com_ptr<ComObject> & com) { reset(); m_pointer = com.m_pointer; if (m_pointer) m_pointer->AddRef(); return *this; }
			XNG_INLINE com_ptr & operator= (com_ptr<ComObject> && com)      { reset(); swap(com); return *this; }
	
			template <typename Interface>
			XNG_INLINE bool as(com_ptr<Interface> & com) { com.reset(); return S_OK == m_pointer->QueryInterface(IID_PPV_ARGS(com.get_address())); }
	
		private:
	
			ComObject * m_pointer;
	
		};		
	}
}