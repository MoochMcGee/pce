#include "pce/cpu_x86/jitx64_code.h"

#if defined(Y_PLATFORM_WINDOWS)
#include "YBaseLib/Windows/WindowsHeaders.h"
#elif defined(Y_PLATFORM_LINUX) || defined(Y_PLATFORM_ANDROID)
#include <sys/mman.h>
#endif

namespace CPU_X86 {

JitX64Code::JitX64Code(size_t size)
{
#if defined(Y_PLATFORM_WINDOWS)
  m_code_ptr = VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#elif defined(Y_PLATFORM_LINUX) || defined(Y_PLATFORM_ANDROID)
  m_code_ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#else
  m_code_ptr = nullptr;
#endif
  m_free_code_ptr = m_code_ptr;
  m_code_size = size;
  m_code_used = 0;

  if (!m_code_ptr)
    Panic("Failed to allocate code space.");
}

JitX64Code::~JitX64Code()
{
#if defined(Y_PLATFORM_WINDOWS)
  VirtualFree(m_code_ptr, m_code_size, MEM_RELEASE);
#elif defined(Y_PLATFORM_LINUX) || defined(Y_PLATFORM_ANDROID)
  munmap(m_code_ptr, m_code_size);
#endif
}

void JitX64Code::CommitCode(size_t length)
{
  //     // Function alignment?
  //     size_t extra_bytes = ((length % 16) != 0) ? (16 - (length % 16)) : 0;
  //     for (size_t i = 0; i < extra_bytes; i++)
  //         reinterpret_cast<char*>(m_free_code_ptr)[i] = 0xCC;

  Assert(length <= (m_code_size - m_code_used));
  m_free_code_ptr = reinterpret_cast<char*>(m_free_code_ptr) + length;
  m_code_used += length;
}

void JitX64Code::Reset()
{
#if defined(Y_PLATFORM_WINDOWS)
  FlushInstructionCache(GetCurrentProcess(), m_code_ptr, m_code_size);
#elif defined(Y_PLATFORM_LINUX) || defined(Y_PLATFORM_ANDROID)
// TODO
#endif

  m_free_code_ptr = m_code_ptr;
  m_code_used = 0;
}

} // namespace CPU_X86
