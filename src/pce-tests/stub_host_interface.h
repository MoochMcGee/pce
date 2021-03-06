#pragma once
#include "pce/host_interface.h"

class StubHostInterface : public HostInterface
{
public:
  StubHostInterface();
  ~StubHostInterface();

  template<typename T, typename... Args>
  static T* CreateSystem(Args... args)
  {
    std::unique_ptr<T> system = std::make_unique<T>(args...);
    T* ret = system.get();
    SetSystem(std::move(system));
    return ret;
  }

  static void SetSystem(std::unique_ptr<System> system);
  static void ReleaseSystem();

  DisplayRenderer* GetDisplayRenderer() const override;
  Audio::Mixer* GetAudioMixer() const override;

protected:
  std::unique_ptr<DisplayRenderer> m_display_renderer;
  std::unique_ptr<Audio::Mixer> m_audio_mixer;
};
