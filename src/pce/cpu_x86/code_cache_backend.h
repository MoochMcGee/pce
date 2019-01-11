#pragma once
#include "pce/bus.h"
#include "pce/cpu_x86/backend.h"
#include "pce/cpu_x86/cpu_x86.h"
#include "pce/cpu_x86/instruction.h"
#include <unordered_map>

namespace CPU_X86 {

class CodeCacheBackend : public Backend
{
public:
  CodeCacheBackend(CPU* cpu);
  ~CodeCacheBackend();

  virtual void Reset() override;
  virtual void OnControlRegisterLoaded(Reg32 reg, uint32 old_value, uint32 new_value) override;
  virtual void BranchTo(uint32 new_EIP) override;
  virtual void BranchFromException(uint32 new_EIP) override;

  virtual void FlushCodeCache() override;

protected:
  struct BlockKey
  {
    union
    {
      struct
      {
        u32 eip_physical_address;
        u32 cs_size : 1;
        u32 cs_granularity : 1;
        u32 ss_size : 1;
        u32 v8086_mode : 1;
        u32 pad : 28;
      };

      uint64 qword;
    };

    bool operator==(const BlockKey& key) const
    {
      // return (std::memcmp(this, &key, sizeof(key)) == 0);
      return (qword == key.qword);
    }

    bool operator!=(const BlockKey& key) const
    {
      // return (std::memcmp(this, &key, sizeof(key)) != 0);
      return (qword != key.qword);
    }
  };

  struct BlockKeyHash
  {
    size_t operator()(const BlockKey& key) const
    {
      return std::hash<uint64>()(key.qword);
      // return size_t(key.qword);
    }

    size_t operator()(const BlockKey& lhs, const BlockKey& rhs) const { return lhs.qword < rhs.qword; }
  };

  struct BlockBase
  {
    BlockBase(const BlockKey key_) : key(key_) {}

    std::vector<Instruction> instructions;
    std::vector<BlockBase*> link_predecessors;
    std::vector<BlockBase*> link_successors;
    CycleCount total_cycles = 0;
    Bus::CodeHashType code_hash;
    BlockKey key = {};
    u32 code_length = 0;
    u32 next_page_physical_address = 0;
    bool invalidated = false;
    bool linkable = false;
    bool crosses_page = false;
    bool destroy_pending = false;

    bool IsLinkable() const { return (linkable); }

    PhysicalMemoryAddress GetPhysicalPageAddress() const { return (key.eip_physical_address & CPU::PAGE_MASK); }
    PhysicalMemoryAddress GetNextPhysicalPageAddress() const { return next_page_physical_address; }

    // Returns true if a block crosses a virtual memory page.
    bool CrossesPage() const { return crosses_page; }
  };

  static bool IsExitBlockInstruction(const Instruction* instruction);
  static bool IsLinkableExitInstruction(const Instruction* instruction);
  static bool CanInstructionFault(const Instruction* instruction);
  static bool OperandIsESP(const Instruction* instruction, const Instruction::Operand& operand);

  /// Allocates storage for a block.
  virtual BlockBase* AllocateBlock(const BlockKey key) = 0;

  /// Uses the current state of the CPU to compile a block.
  virtual bool CompileBlock(BlockBase* block) = 0;

  /// Resets a block before recompiling it, which may be more efficient than completely destroying it.
  virtual void ResetBlock(BlockBase* block);

  /// Flushes a block. The destroying of the block may be deferred until later.
  virtual void FlushBlock(BlockBase* block, bool defer_destroy = false);

  /// Destroys a block, freeing all memory and code associated with it.
  virtual void DestroyBlock(BlockBase* block) = 0;

  /// Returns a code block ready for execution based on the current state, otherwise fall back to the interpreter.
  BlockBase* GetNextBlock();

  /// Compiles the base portion of a block (retrieves/decodes the instruction stream).
  bool CompileBlockBase(BlockBase* block);

  /// Inserts the block into the block map.
  void InsertBlock(BlockBase* block);

  /// Invalidates a single block of code, ensuring the code is re-hashed next execution.
  void InvalidateBlock(BlockBase* block);

  /// Invalidates any code blocks with a matching physical page.
  void InvalidateBlocksWithPhysicalPage(PhysicalMemoryAddress physical_page_address);

  /// Removes the physical page -> block mapping for block.
  void AddBlockPhysicalMappings(BlockBase* block);
  void RemoveBlockPhysicalMappings(BlockBase* block);

  /// Returns the hash of memory occupied by the block.
  Bus::CodeHashType GetBlockCodeHash(BlockBase* block);

  /// Returns the block key for the current execution state.
  /// Can raise general protection or page faults if the state is invalid.
  bool GetBlockKeyForCurrentState(BlockKey* key);

  /// Can the current block execute? This will re-validate the block if necessary.
  /// The block can also be flushed if recompilation failed, so ignore the pointer if false is returned.
  bool CanExecuteBlock(BlockBase* block);

  /// Link block from to to.
  void LinkBlockBase(BlockBase* from, BlockBase* to);

  /// Unlink all blocks which point to this block, and any that this block links to.
  void UnlinkBlockBase(BlockBase* block);

  /// Runs the interpreter until the emulated CPU branches.
  void InterpretUncachedBlock();

  CPU* m_cpu;
  System* m_system;
  Bus* m_bus;

  std::unordered_map<BlockKey, BlockBase*, BlockKeyHash> m_blocks;
  std::unordered_map<PhysicalMemoryAddress, std::vector<BlockBase*>> m_physical_page_blocks;
  bool m_branched = false;
};
} // namespace CPU_X86