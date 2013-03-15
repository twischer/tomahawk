#include "PreviewAudioEngine.h"

PreviewAudioEngine* PreviewAudioEngine::s_instance = 0;


PreviewAudioEngine*
PreviewAudioEngine::instance()
{
    return s_instance;
}

// TODO PreviewEngine implements only the needed methods and AudioEngine inherts from PreviewEngine

PreviewAudioEngine::PreviewAudioEngine()
    : AudioEngine()
{
    s_instance = this;
    tDebug() << "Init PreviewEngine";
}

