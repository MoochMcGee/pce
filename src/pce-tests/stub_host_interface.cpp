#include "stub_host_interface.h"
#include "common/audio.h"
#include "common/display.h"

static std::unique_ptr<StubHostInterface> s_host_interface;

StubHostInterface::StubHostInterface()
{
  m_display = NullDisplay::Create();
  m_audio_mixer = Audio::NullMixer::Create();
}

StubHostInterface::~StubHostInterface() {}

void StubHostInterface::SetSystem(std::unique_ptr<System> system)
{
  if (!s_host_interface)
    s_host_interface = std::make_unique<StubHostInterface>();

  if (s_host_interface->m_system)
    ReleaseSystem();

  s_host_interface->m_system = std::move(system);
  if (system)
    system->SetHostInterface(s_host_interface.get());
}

void StubHostInterface::ReleaseSystem()
{
  if (s_host_interface->m_system)
    s_host_interface->m_system->SetState(System::State::Stopped);

  s_host_interface->m_system.reset();
}

Display* StubHostInterface::GetDisplay() const
{
  return m_display.get();
}

Audio::Mixer* StubHostInterface::GetAudioMixer() const
{
  return m_audio_mixer.get();
}