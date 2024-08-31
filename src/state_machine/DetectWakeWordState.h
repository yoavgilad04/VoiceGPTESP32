#ifndef _detect_wake_word_state_h_
#define _detect_wake_word_state_h_

#include "States.h"

class I2SSampler;
class NeuralNetwork;
class AudioProcessor;

class DetectWakeWordState : public State
{
private:
    NeuralNetwork *m_nn;
    AudioProcessor *m_audio_processor;
    float m_average_detect_time;
    int m_number_of_detections;
    int m_number_of_runs;

public:
    DetectWakeWordState(I2SSampler *sample_provider);
    void enterState();
    String* get_response();
    bool run();
    void exitState();
    I2SSampler *m_sample_provider;

};

#endif
