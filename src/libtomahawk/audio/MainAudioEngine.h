#ifndef MAINAUDIOENGINE_H
#define MAINAUDIOENGINE_H


#include "DllMacro.h"

#include <QtCore/QObject>

#include "utils/Logger.h"

#include "AudioEngine.h"


class DLLEXPORT MainAudioEngine : public AudioEngine
{
Q_OBJECT

public:
    static MainAudioEngine* instance();
    explicit MainAudioEngine(const bool enableCrossfading);

private:
    static MainAudioEngine* s_instance;
};

#endif // MAINAUDIOENGINE_H
