#ifndef LIB_YOGURT_COMMON_RECORD_H_
#define LIB_YOGURT_WRAPPER_RECORD_H_

#include <milktea.h>

#include <yogurt/common.h>

namespace Yogurt {

class RecorderWrapper final {
  static constexpr char TAG[] = "Yogurt::RecorderWrapper";
  using raw_type = Yogurt_Recorder_t;
  using State = RecorderState;
 public:
  RecorderWrapper(raw_type *obj = nullptr) : obj_(obj) {}
  RecorderWrapper(RecorderWrapper &&another) : RecorderWrapper(another.release()) {}
  explicit RecorderWrapper(uint16_t division) : RecorderWrapper() {
    MilkTea_invoke_panic(Yogurt_Recorder_Create, &obj_, division);
  }
  ~RecorderWrapper() {
    auto obj = release();
    if (obj == nullptr) {
      return;
    }
    MilkTea_invoke_panic(Yogurt_Recorder_Destroy, obj);
  }
  State GetState() {
    Yogurt_Recorder_State_t state = Yogurt_Recorder_State_CLOSED;
    MilkTea_invoke_panic(Yogurt_Recorder_GetState, get(), &state);
    return FromRawType(state);
  }
  void Resume() {
    MilkTea_invoke_panic(Yogurt_Recorder_Resume, get());
  }
  void Pause() {
    MilkTea_invoke_panic(Yogurt_Recorder_Pause, get());
  }
  MilkPowder::MidiMutableWrapper Take() {
    MilkPowder_Midi_t *midi = nullptr;
    MilkTea_invoke_panic(Yogurt_Recorder_Take, get(), &midi);
    return midi;
  }
  SoyBean::FactoryWrapper CreateFactory(uint32_t tempo = 0) {
    SoyBean_Factory_t factory = {};
    MilkTea_invoke_panic(Yogurt_Recorder_Factory_Create, get(), tempo, &factory);
    return factory;
  }
  raw_type *get() const {
    return obj_;
  }
  raw_type *release() {
    return reset(nullptr);
  }
  raw_type *reset(raw_type *another) {
    std::swap(another, obj_);
    return another;
  }
 private:
  raw_type *obj_;
  MilkTea_NonCopy(RecorderWrapper)
  MilkTea_NonMoveAssign(RecorderWrapper)
};

} // namespace Yogurt

#endif // ifndef LIB_YOGURT_WRAPPER_RECORD_H_
