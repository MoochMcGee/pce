#pragma once
#include "pce/cpu_x86/jitx64_backend.h"

namespace CPU_X86 {

class JitX64Code
{
public:
  JitX64Code(size_t size = 64 * 1024 * 1024);
  ~JitX64Code();

  void* GetFreeCodePointer() const { return m_free_code_ptr; }
  size_t GetFreeCodeSpace() const { return (m_code_size - m_code_used); }
  void CommitCode(size_t length);
  void Reset();

private:
  void* m_code_ptr;
  void* m_free_code_ptr;
  size_t m_code_size;
  size_t m_code_used;
};

} // namespace CPU_X86
