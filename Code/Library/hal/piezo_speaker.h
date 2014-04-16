/*
 * piezo_speaker.h
 *
 * Created: 27/03/2014 18:20:04
 *  Author: schill
 */ 


#ifndef PIEZO_SPEAKER_H_
#define PIEZO_SPEAKER_H_

#define PIEZO_HIGH_PIN AVR32_PIN_PA12
#define PIEZO_LOW_PIN AVR32_PIN_PA15

void init_piezo_speaker();
void init_piezo_speaker_binary();

// instantaneous output voltage sent to the speaker - to make sounds this needs to be called repeatedly.
void set_value(int analog_value);
void set_value_binary(int binary_value);

void beep(int duration_ms, int frequency);


#endif /* PIEZO_SPEAKER_H_ */