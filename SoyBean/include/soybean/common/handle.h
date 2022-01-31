#ifndef LIB_SOYBEAN_COMMON_HANDLE_H_
#define LIB_SOYBEAN_COMMON_HANDLE_H_

#include <soybean/core.h>

namespace SoyBean {

class BaseHandle {
  using raw_type = SoyBean_Handle_t;
  using interface_type = SoyBean_Handle_Interface_t;
 public:
  virtual raw_type ToRawType() && {
    return raw_type{
      .self_ = &std::move(*this).Move(),
      .interface_ = &Interface(),
    };
  }
  virtual ~BaseHandle() = default;
  virtual BaseHandle &Move() && = 0;
  virtual void Destroy() && = 0;
  virtual void NoteOff(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) = 0;
  virtual void ProgramChange(uint8_t channel, uint8_t program) = 0;
  virtual void ChannelPressure(uint8_t channel, uint8_t pressure) = 0;
  virtual void PitchBend(uint8_t channel, uint8_t low, uint8_t height) = 0;
 private:
  static MilkTea_decl(const interface_type &) Interface();
};

} // namespace SoyBean

#endif // ifndef LIB_SOYBEAN_COMMON_HANDLE_H_
