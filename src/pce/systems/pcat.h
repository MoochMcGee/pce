#pragma once
#include "pce/bitfield.h"
#include "pce/hw/cmos.h"
#include "pce/hw/fdc.h"
#include "pce/hw/hdc.h"
#include "pce/hw/i8237_dma.h"
#include "pce/hw/i8253_pit.h"
#include "pce/hw/i8259_pic.h"
#include "pce/hw/ps2.h"
#include "pce/systems/pcbase.h"

class ByteStream;

namespace Systems {

class PCAT : public PCBase
{
public:
  static const uint32 PHYSICAL_MEMORY_BITS = 24;
  static const PhysicalMemoryAddress BIOS_ROM_ADDRESS = 0xF0000;
  static const uint32 BIOS_ROM_SIZE = 65536;

  PCAT(HostInterface* host_interface, float cpu_frequency = 2000000.0f, uint32 memory_size = 1024 * 1024);
  ~PCAT();

  const char* GetSystemName() const override { return "IBM AT"; }
  InterruptController* GetInterruptController() const override { return m_interrupt_controller; }
  void Reset() override;

  auto GetFDDController() const { return m_fdd_controller; }
  auto GetHDDController() const { return m_hdd_controller; }
  auto GetKeyboardController() const { return m_keyboard_controller; }
  auto GetDMAController() const { return m_dma_controller; }
  auto GetTimer() const { return m_timer; }
  auto GetCMOS() const { return m_cmos; }

private:
  virtual bool LoadSystemState(BinaryReader& reader) override;
  virtual bool SaveSystemState(BinaryWriter& writer) override;

  void ConnectSystemIOPorts();
  void AddComponents();
  void SetCMOSVariables();

  void IOWriteSystemControlPortA(uint8 value);

  HW::PS2Controller* m_keyboard_controller = nullptr;
  HW::i8237_DMA* m_dma_controller = nullptr;
  HW::i8253_PIT* m_timer = nullptr;
  HW::i8259_PIC* m_interrupt_controller = nullptr;
  HW::CMOS* m_cmos = nullptr;

  HW::FDC* m_fdd_controller = nullptr;
  HW::HDC* m_hdd_controller = nullptr;

  union
  {
    uint8 raw = 0;

    BitField<uint8, bool, 0, 1> system_reset;
    BitField<uint8, bool, 1, 1> a20_gate;
    BitField<uint8, bool, 3, 1> cmos_lock;
    BitField<uint8, bool, 4, 1> watchdog_timeout;
    BitField<uint8, bool, 6, 2> activity_light;
  } m_system_control_port_a;
};

} // namespace Systems