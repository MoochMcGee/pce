#pragma once
#include "pce/component.h"

class DebuggerInterface;

class CPUBase : public Component
{
  DECLARE_OBJECT_TYPE_INFO(CPUBase, Component);
  DECLARE_OBJECT_NO_FACTORY(CPUBase);
  DECLARE_OBJECT_PROPERTY_MAP(CPUBase);

public:
  CPUBase(float frequency, CPUBackendType backend_type);

  virtual ~CPUBase() = default;

  float GetFrequency() const { return m_frequency; }
  void SetFrequency(float frequency);

  SimulationTime GetCyclePeriod() const;

  // IRQs are level-triggered
  virtual void SetIRQState(bool state);

  // NMIs are edge-triggered
  virtual void SignalNMI();

  // Debugger interface
  virtual DebuggerInterface* GetDebuggerInterface();

  // Execution mode
  CPUBackendType GetCurrentBackend() const { return m_backend_type; }
  virtual bool SupportsBackend(CPUBackendType mode) = 0;
  virtual void SetBackend(CPUBackendType mode) = 0;

  // Execute cycles
  virtual void ExecuteCycles(CycleCount cycles) = 0;

  // Code cache flushing - for recompiler backends
  virtual void FlushCodeCache() = 0;

protected:
  SimulationTime m_cycle_period;
  float m_frequency;
  CPUBackendType m_backend_type;
};
