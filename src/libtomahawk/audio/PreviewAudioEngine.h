#ifndef PREVIEWENGINE_H
#define PREVIEWENGINE_H


#include "DllMacro.h"

#include <QtCore/QObject>

#include "utils/Logger.h"

#include "AudioEngine.h"


class DLLEXPORT PreviewAudioEngine : public AudioEngine
{
Q_OBJECT

public:
    static PreviewAudioEngine* instance();
    explicit PreviewAudioEngine();

private:
    static PreviewAudioEngine* s_instance;
};

#endif // PREVIEWENGINE_H
