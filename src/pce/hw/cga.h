#pragma once

#include "common/bitfield.h"
#include "common/clock.h"
#include "pce/component.h"
#include "pce/system.h"

class Display;

namespace HW {

class CGA : public Component
{
  DECLARE_OBJECT_TYPE_INFO(CGA, Component);
  DECLARE_GENERIC_COMPONENT_FACTORY(CGA);
  DECLARE_OBJECT_PROPERTY_MAP(CGA);

public:
  static constexpr uint32 SERIALIZATION_ID = MakeSerializationID('C', 'G', 'A');
  static constexpr uint32 VRAM_SIZE = 16384;
  static constexpr uint32 PIXEL_CLOCK = 14318181;
  static constexpr uint32 NUM_CRTC_REGISTERS = 18;
  static constexpr uint32 CHARACTER_WIDTH = 8;
  static constexpr uint32 CHARACTER_HEIGHT = 8;
  static constexpr uint32 ADDRESS_COUNTER_MASK = 0x3FFF;
  static constexpr uint32 ADDRESS_COUNTER_VRAM_MASK_TEXT = 0x1FFF;
  static constexpr uint32 ADDRESS_COUNTER_VRAM_MASK_GRAPHICS = 0x0FFF;
  static constexpr uint32 CHARACTER_ROW_COUNTER_MASK = 0x1F;
  static constexpr uint32 VERTICAL_COUNTER_MASK = 0x7F;
  static constexpr uint32 CRTC_ADDRESS_SHIFT = 1;
  static constexpr uint32 VSYNC_PULSE_WIDTH = 16;
  static constexpr uint8 BLINK_INTERVAL = 8;

public:
  CGA(const String& identifier, const ObjectTypeInfo* type_info = &s_type_info);
  ~CGA();

  const uint8* GetVRAM() const { return m_vram; }
  uint8* GetVRAM() { return m_vram; }

  bool Initialize(System* system, Bus* bus) override;
  void Reset() override;
  bool LoadState(BinaryReader& reader) override;
  bool SaveState(BinaryWriter& writer) override;

private:
  uint32 GetBorderColor() const;
  uint32 GetCursorAddress() const;
  uint32 InCursorBox() const;
  void ConnectIOPorts(Bus* bus);
  void RenderLineEvent(CycleCount cycles);
  void BeginFrame();
  void RenderLineText();
  void RenderLineGraphics();
  void RenderLineBorder();
  void FlushFrame();

  std::unique_ptr<Display> m_display;
  uint8 m_vram[VRAM_SIZE];

  // 03D8h: Mode control register
  union
  {
    uint8 raw = 0;
    BitField<uint8, bool, 0, 1> high_resolution;
    BitField<uint8, bool, 1, 1> graphics_mode;
    BitField<uint8, bool, 2, 1> monochrome;
    BitField<uint8, bool, 3, 1> enable_video_output;
    BitField<uint8, bool, 4, 1> high_resolution_graphics;
    BitField<uint8, bool, 5, 1> enable_blink;
  } m_mode_control_register;
  void ModeControlRegisterWrite(uint8 value);

  // 03D9h: Colour control register
  union
  {
    uint8 raw = 0;
    BitField<uint8, bool, 5, 1> palette_select;
    BitField<uint8, bool, 4, 1> foreground_intensity;
    BitField<uint8, uint8, 0, 4> background_color;
  } m_color_control_register;
  void ColorControlRegisterWrite(uint8 value);

  // 03DAh: Status register
  union StatusRegister
  {
    uint8 raw;
    BitField<uint8, bool, 0, 1> safe_vram_access;
    BitField<uint8, bool, 1, 1> light_pen_trigger_set;
    BitField<uint8, bool, 2, 1> light_pen_switch_status;
    BitField<uint8, bool, 3, 1> vblank;
  };
  void StatusRegisterRead(uint8* value);

  // CRTC registers
  union
  {
    struct
    {
      uint8 horizontal_total;            // characters
      uint8 horizontal_displayed;        // characters
      uint8 horizontal_sync_position;    // characters
      uint8 horizontal_sync_pulse_width; // characters
      uint8 vertical_total;              // character rows
      uint8 vertical_total_adjust;       // scanlines
      uint8 vertical_displayed;          // character rows
      uint8 vertical_sync_position;      // character rows
      uint8 interlace_mode;
      uint8 maximum_scan_lines; // scan lines
      uint8 cursor_start;       // scan lines
      uint8 cursor_end;         // scan lines
      uint8 start_address_high; // Big-endian
      uint8 start_address_low;
      uint8 cursor_location_high; // Big-endian
      uint8 cursor_location_low;
      uint8 light_pen_high;
      uint8 light_pen_low;
    };
    uint8 index[NUM_CRTC_REGISTERS] = {};
  } m_crtc_registers;

  // 03D0/2/4: CRT (6845) index register
  uint8 m_crtc_index_register = 0;

  // 03D1/3/5: CRT data register
  void CRTDataRegisterRead(uint8* value);
  void CRTDataRegisterWrite(uint8 value);

  // Timing
  struct Timing
  {
    double horizontal_frequency;

    uint32 horizontal_left_border_pixels;
    uint32 horizontal_right_border_pixels;
    SimulationTime horizontal_display_start_time;
    SimulationTime horizontal_display_end_time;

    uint32 vertical_display_end;
    uint32 vertical_sync_start;
    uint32 vertical_sync_end;

    bool operator==(const Timing& rhs) const;
  };
  Timing m_timing = {};

  void RecalculateEventTiming();

  Clock m_clock;
  uint32 m_address_counter = 0;
  uint32 m_character_row_counter = 0;
  uint32 m_current_row = 0;
  uint32 m_remaining_adjust_lines = 0;
  TimingEvent::Pointer m_line_event;

  // Currently-rendering frame.
  std::vector<uint32> m_current_frame;
  uint32 m_current_frame_width = 0;
  uint32 m_current_frame_line = 0;
  uint32 m_current_frame_offset = 0;

  // Blink bit. XOR with the character value.
  uint8 m_blink_frame_counter = BLINK_INTERVAL;
  uint8 m_cursor_frame_counter = BLINK_INTERVAL;
  uint8 m_blink_state = 0;
  uint8 m_cursor_state = 0;
};
} // namespace HW