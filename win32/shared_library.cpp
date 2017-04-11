#include <xng/os/shared_library.hpp>

using namespace xng::os;

const path::value_type * shared_library::extension = L".dll";

shared_library::shared_library(void) :
    m_hModule(NULL) {}

shared_library::shared_library(shared_library && rhs) :
    m_hModule(rhs.m_hModule)
{
    rhs.m_hModule = NULL;
}

shared_library::~shared_library(void)
{
    unload();
}

bool shared_library::load(const path & p)
{
    m_hModule = LoadLibraryW(p.c_str());
    return m_hModule != NULL;
}

void shared_library::unload(void)
{
    FreeLibrary(m_hModule);
    m_hModule = NULL;
}

bool shared_library::is_loaded(void) const
{
    return m_hModule != NULL;
}

shared_library::native_symbol_handle shared_library::get_symbol_address(const char * name)
{
    static HMODULE hProcess = GetModuleHandle(NULL);
    return GetProcAddress(m_hModule ? m_hModule : hProcess, name);
}

shared_library::operator bool() const
{
    return m_hModule != NULL;
}

void shared_library::swap(shared_library & rhs)
{
    native_handle tmp = m_hModule;
    m_hModule = rhs.m_hModule;
    rhs.m_hModule = tmp;
}

shared_library::native_handle shared_library::release(void)
{
    native_handle ret = m_hModule;
    m_hModule = NULL;
    return ret;
}