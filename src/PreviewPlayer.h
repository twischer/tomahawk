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
    
private:
    Ui::PreviewPlayer *ui;
    QTimeLine m_sliderTimeLine;

    void onPlaybackStarted( const Tomahawk::result_ptr& result );
    void onPlaybackLoading( const Tomahawk::result_ptr& result );

};

#endif // PREVIEWPLAYER_H
