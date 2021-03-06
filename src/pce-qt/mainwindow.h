#pragma once

#include "pce-qt/ui_mainwindow.h"
#include "pce/cpu.h"
#include "pce/host_interface.h"
#include "pce/system.h"
#include "pce/systems/bochs.h"
#include <QtCore/QThread>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <memory>

class Display;
class DisplayWidget;
class DebuggerWindow;
class QtHostInterface;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  friend QtHostInterface;

public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

  QtHostInterface* GetHostInterface() const { return m_host_interface.get(); }

public Q_SLOTS:
  void onEnableDebuggerActionToggled(bool checked);
  void onResetActionTriggered();
  void onAboutActionTriggered();
  void onChangeFloppyATriggered();
  void onChangeFloppyBTriggered();

  void onDisplayWidgetKeyPressed(QKeyEvent* event);
  void onDisplayWidgetKeyReleased(QKeyEvent* event);

private Q_SLOTS:
  void onSystemInitialized();
  void onSystemDestroy();
  void onSimulationPaused();
  void onSimulationResumed();
  void onSimulationSpeedUpdate(float speed_percent, float vps);
  void onStatusMessage(QString message);
  void onDebuggerEnabled(bool enabled);

private:
  void connectSignals();
  void enableDebugger();
  void disableDebugger();
  void setUIState(bool started, bool running);

  std::unique_ptr<Ui::MainWindow> m_ui;
  DisplayWidget* m_display_widget = nullptr;
  QLabel* m_status_message = nullptr;
  QLabel* m_status_speed = nullptr;
  QLabel* m_status_fps = nullptr;

  std::unique_ptr<QtHostInterface> m_host_interface;

  DebuggerInterface* m_debugger_interface = nullptr;
  std::unique_ptr<DebuggerWindow> m_debugger_window;
};
