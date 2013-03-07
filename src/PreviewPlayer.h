#ifndef PREVIEWPLAYER_H
#define PREVIEWPLAYER_H

#include <QDialog>

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
};

#endif // PREVIEWPLAYER_H
