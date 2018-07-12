#pragma once
#include "pce/bitfield.h"
#include "pce/cpu_x86/cpu.h"
#include "pce/hw/cmos.h"
#include "pce/hw/fdc.h"
#include "pce/hw/hdc.h"
#include "pce/hw/i8042_ps2.h"
#include "pce/hw/i8237_dma.h"
#include "pce/hw/i8253_pit.h"
#include "pce/hw/i8259_pic.h"
#include "pce/hw/pcspeaker.h"
#include "pce/systems/isapc.h"

class ByteStream;

namespace Systems {

class Bochs : public ISAPC
{
public:
  static const uint32 PHYSICAL_MEMORY_BITS = 32;
  static const PhysicalMemoryAddress BIOS_ROM_ADDRESS = 0xFFFF0000;
  static const PhysicalMemoryAddress BIOS_ROM_MIRROR_ADDRESS = 0xF0000;
  static const uint32 BIOS_ROM_SIZE = 65536;

  Bochs(HostInterface* host_interface, CPU_X86::Model model = CPU_X86::MODEL_486, float cpu_frequency = 8000000.0f,
        uint32 memory_size = 16 * 1024 * 1024);
  ~Bochs();

  const char* GetSystemName() const override { return "Bochs PC"; }
  InterruptController* GetInterruptController() const override { return m_interrupt_controller; }
  bool Initialize() override;
  void Reset() override;

  auto GetFDDController() const { return m_fdd_controller; }
  auto GetHDDController() const { return m_hdd_controller; }
  auto GetKeyboardController() const { return m_keyboard_controller; }
  auto GetDMAController() const { return m_dma_controller; }
  auto GetTimer() const { return m_timer; }
  auto GetCMOS() const { return m_cmos; }

private:
  bool LoadSystemState(BinaryReader& reader) override;
  bool SaveSystemState(BinaryWriter& writer) override;

  void ConnectSystemIOPorts();
  void AddComponents();
  void SetCMOSVariables();

  void IOReadSystemControlPortA(uint8* value);
  void IOWriteSystemControlPortA(uint8 value);
  void IOReadSystemControlPortB(uint8* value);
  void IOWriteSystemControlPortB(uint8 value);
  void UpdateKeyboardControllerOutputPort();

  std::string m_bios_file_path;

  HW::i8042_PS2* m_keyboard_controller = nullptr;
  HW::i8237_DMA* m_dma_controller = nullptr;
  HW::i8253_PIT* m_timer = nullptr;
  HW::i8259_PIC* m_interrupt_controller = nullptr;

  HW::CMOS* m_cmos = nullptr;

  HW::PCSpeaker* m_speaker = nullptr;

  HW::FDC* m_fdd_controller = nullptr;
  HW::HDC* m_hdd_controller = nullptr;

  bool m_refresh_bit = false;
};

} // namespace Systems