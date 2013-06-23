#include "MainAudioEngine.h"

MainAudioEngine* MainAudioEngine::s_instance = 0;


MainAudioEngine*
MainAudioEngine::instance()
{
    return s_instance;
}

MainAudioEngine::MainAudioEngine()
    : AudioEngine()
{
    s_instance = this;
    tDebug() << "Init MainAudioEngine";
}

