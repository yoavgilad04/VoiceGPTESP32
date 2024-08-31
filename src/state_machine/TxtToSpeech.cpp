#include <Arduino.h>
#include "TxtToSpeech.h"
#include <ArduinoJson.h>
#include "Audio.h"
#include <vector>

#define I2S_DOUT      25
#define I2S_BCLK      26
#define I2S_LRC       27

#include <Arduino.h>
#include <vector>

std::vector<String> splitStringWithPriority(String input, int max_length) {
    std::vector<String> result;
    String chunk = "";
    int length = input.length();
    int i = 0;

    while (i < length) {
        char c = input.charAt(i);
        chunk += c;

        // If we encounter a ',' or '.', prioritize splitting here
        if (c == ',' || c == '.') {
            if (chunk.length() <= max_length) {
                result.push_back(chunk);
            } else {
                // If chunk is longer than max_length, split at the last space
                int lastSpaceIndex = chunk.lastIndexOf(' ');
                if (lastSpaceIndex != -1 && lastSpaceIndex < max_length) {
                    result.push_back(chunk.substring(0, lastSpaceIndex));
                    chunk = chunk.substring(lastSpaceIndex + 1);  // Start new chunk after the space
                } else {
                    result.push_back(chunk.substring(0, max_length));
                    chunk = chunk.substring(max_length);  // Start new chunk after max_length
                }
            }
            chunk = "";
        }

        // Check if the next character would exceed max_length
        if (chunk.length() > max_length) {
            // Find the longest possible chunk without breaking words
            int lastSpaceIndex = chunk.lastIndexOf(' ');
            if (lastSpaceIndex != -1 && lastSpaceIndex < max_length) {
                result.push_back(chunk.substring(0, lastSpaceIndex));
                chunk = chunk.substring(lastSpaceIndex + 1);  // Continue from after the space
            } else {
                result.push_back(chunk.substring(0, max_length));  // No space found, cut off at max_length
                chunk = chunk.substring(max_length);  // Continue with the remaining string
            }
        }

        i++;
    }
    // Add any remaining part of the chunk
    if (chunk.length() > 0) {
        result.push_back(chunk);
    }

    return result;
}

TxtToSpeech::TxtToSpeech(String* gpt_answer)
{
    this->gpt_answer = gpt_answer;
    Serial.printf("In tts constructor gpt answer is : \n");
    Serial.println(*this->gpt_answer);
    this->audio = new Audio(false, 3, I2S_NUM_1);
}
void TxtToSpeech::enterState()
{
    audio->setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio->setVolume(21); // 0...21
}
bool TxtToSpeech::run()
{
    std::vector<String> split_string = splitStringWithPriority(*this->gpt_answer, 30);


    for (const auto& word : split_string) {

            Serial.println(word);
            audio->connecttospeech(word.c_str(), "en");

        while(audio->isRunning()){
            audio->loop();
        }
    }
    audio->loop();
    return true;
}


void TxtToSpeech::exitState()
{
    // Create our neural network
    delete audio;
    return;
}
  
String* TxtToSpeech::get_response()
{
    return nullptr;
}
