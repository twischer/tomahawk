#include "PreviewPlayer.h"
#include "ui_PreviewPlayer.h"

PreviewPlayer::PreviewPlayer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreviewPlayer)
{
    ui->setupUi(this);
}

PreviewPlayer::~PreviewPlayer()
{
    delete ui;
}
