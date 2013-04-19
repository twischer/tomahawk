#ifndef PREVIEWPLAYER_H
#define PREVIEWPLAYER_H

#include <QDialog>
#include <QTimeLine>

#include "Result.h"


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
//    void onPlaybackSeeked( qint64 msec );
//    void onPlaybackStopped();

    void onPlaybackTimer( qint64 msElapsed );
//    void onVolumeChanged( int volume );

private:
    qint64 m_lastTextSecondShown;
    Ui::PreviewPlayer *ui;
    QTimeLine m_sliderTimeLine;
    Tomahawk::result_ptr m_currentTrack;

};

#endif // PREVIEWPLAYER_H
