#include <Arduino.h>
#include "Application.h"
#include "state_machine/DetectWakeWordState.h"

// #include "IndicatorLight.h"
// #include "Speaker.h"
// #include "IntentProcessor.h"
#define RED_PIN 14
#define GREEN_PIN 12
#define BLUE_PIN 13


Application::Application(I2SSampler *sample_provider)
{
    m_sample_provider = sample_provider;
    m_current_state = new DetectWakeWordState(m_sample_provider);
    m_current_state->enterState();
}

// process the next batch of samples
void Application::run()
{   
    bool state_done = m_current_state->run();

    if (state_done)
    {
        m_current_state->exitState();
        // Application::set_next_state();
        // m_current_state->enterState();
    }
    vTaskDelay(10);
}


Application::~Application()
{
    // delete m_sample_provider;
    delete m_current_state;
}
