#ifndef LIB_SOYBEAN_FACTORY_H_
#define LIB_SOYBEAN_FACTORY_H_

#ifdef __cplusplus
#include <soybean/core.h>
namespace SoyBean {

class BaseSoyBean {
 public:
  virtual ~BaseSoyBean() = default;
  virtual void NoteOff(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void NoteOn(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void AfterTouch(uint8_t channel, uint8_t note, uint8_t pressure) = 0;
  virtual void ControlChange(uint8_t channel, uint8_t control, uint8_t argument) = 0;
  virtual void ProgramChange(uint8_t channel, uint8_t program) = 0;
  virtual void ChannelPressure(uint8_t channel, uint8_t pressure) = 0;
  virtual void PitchBend(uint8_t channel, uint8_t low, uint8_t height) = 0;
};

} // namespace SoyBean
#endif // ifdef __cplusplus

#endif // ifndef LIB_SOYBEAN_FACTORY_H_
