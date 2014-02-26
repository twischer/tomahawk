/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *   Copyright 2012-2013, Teo Mrnjavac <teo@kde.org>
 *
 *   Tomahawk is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Tomahawk is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Tomahawk. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FlexibleHeader.h"

#include <QBoxLayout>
#include <QFile>
#include <QLabel>
#include <QPixmap>
#include <QCheckBox>
#include <QPaintEvent>
#include <QPainter>
#include <QRadioButton>

#include "playlist/FlexibleView.h"
#include "ViewManager.h"
#include "utils/Closure.h"
#include "utils/TomahawkUtilsGui.h"
#include "utils/Logger.h"
#include "widgets/QueryLabel.h"
#include "Source.h"

using namespace Tomahawk;


FlexibleHeader::FlexibleHeader( FlexibleView* parent )
    : FilterHeader( parent )
    , m_parent( parent )
{
    QFile f( RESPATH "stylesheets/topbar-radiobuttons.css" );
    f.open( QFile::ReadOnly );
    QString css = QString::fromLatin1( f.readAll() );
    f.close();

    QHBoxLayout* outerModeLayout = new QHBoxLayout;
    m_verticalLayout->addLayout( outerModeLayout );
    outerModeLayout->addSpacing( 156 );
    outerModeLayout->addStretch();

    QWidget* modeWidget = new QWidget( this );
    QHBoxLayout* modeLayout = new QHBoxLayout;
    modeWidget->setLayout( modeLayout );
    modeWidget->setStyleSheet( css );

    m_radioNormal = new QRadioButton( modeWidget );
    m_radioDetailed = new QRadioButton( modeWidget );
    m_radioCloud = new QRadioButton( modeWidget );
    //for the CSS:
    m_radioNormal->setObjectName( "radioNormal" );
    m_radioCloud->setObjectName( "radioCloud" );

    m_radioNormal->setFocusPolicy( Qt::NoFocus );
    m_radioDetailed->setFocusPolicy( Qt::NoFocus );
    m_radioCloud->setFocusPolicy( Qt::NoFocus );

    modeLayout->addWidget( m_radioNormal );
    modeLayout->addWidget( m_radioDetailed );
    modeLayout->addWidget( m_radioCloud );

    modeWidget->setFixedSize( 87, 30 );

    m_radioNormal->setChecked( true );

    outerModeLayout->addWidget( modeWidget );
    outerModeLayout->addStretch();

    TomahawkUtils::unmarginLayout( outerModeLayout );
    TomahawkUtils::unmarginLayout( modeLayout );

    NewClosure( m_radioNormal,   SIGNAL( clicked() ), const_cast< FlexibleView* >( parent ), SLOT( setCurrentMode( FlexibleViewMode ) ), FlexibleView::Flat )->setAutoDelete( false );
    NewClosure( m_radioDetailed, SIGNAL( clicked() ), const_cast< FlexibleView* >( parent ), SLOT( setCurrentMode( FlexibleViewMode ) ), FlexibleView::Detailed )->setAutoDelete( false );
    NewClosure( m_radioCloud,    SIGNAL( clicked() ), const_cast< FlexibleView* >( parent ), SLOT( setCurrentMode( FlexibleViewMode ) ), FlexibleView::Grid )->setAutoDelete( false );
}


FlexibleHeader::~FlexibleHeader()
{
}


void
FlexibleHeader::changeEvent( QEvent* e )
{
    QWidget::changeEvent( e );
    switch ( e->type() )
    {
        case QEvent::LanguageChange:
//            ui->retranslateUi( this );
            break;

        default:
            break;
    }
}

