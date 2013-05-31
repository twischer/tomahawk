#ifndef PREVIEWPLAYER_H
#define PREVIEWPLAYER_H

#include <QDialog>
#include <QTimeLine>

#include "Result.h"
#include "AudioControls.h"


namespace Ui {
class PreviewPlayer;
}

class PreviewPlayer : public QDialog
{
    Q_OBJECT
    
public:
    explicit PreviewPlayer(QWidget *parent = 0);
    ~PreviewPlayer();

private slots:
    void onPlaybackStarted( const Tomahawk::result_ptr& result );
    void onPlaybackLoading( const Tomahawk::result_ptr& result );

    void onPlaybackTimer( qint64 msElapsed );
    void onVolumeChanged( int volume );

private:
    qint64 m_lastTextSecondShown;
    Ui::PreviewPlayer *ui;
    QTimeLine m_sliderTimeLine;
    Tomahawk::result_ptr m_currentTrack;

    bool canGoNext();

};

#endif // PREVIEWPLAYER_H
