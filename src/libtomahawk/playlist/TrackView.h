/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *   Copyright 2010-2011, Jeff Mitchell <jeff@tomahawk-player.org>
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

#ifndef TRACKVIEW_H
#define TRACKVIEW_H

#include <QtGui/QTreeView>
#include <QtGui/QSortFilterProxyModel>
#include <QtCore/QTimer>

#include "ContextMenu.h"
#include "PlaylistItemDelegate.h"
#include "ViewPage.h"

#include "DllMacro.h"

class QAction;
class AnimatedSpinner;
class ViewHeader;
class PlayableModel;
class PlayableProxyModel;
class OverlayWidget;

class DLLEXPORT TrackView : public QTreeView, public Tomahawk::ViewPage
{
Q_OBJECT

public:
    explicit TrackView( QWidget* parent = 0 );
    ~TrackView();

    virtual QString guid() const;
    virtual void setGuid( const QString& newguid );

    virtual void setPlaylistItemDelegate( PlaylistItemDelegate* delegate );
    virtual void setPlayableModel( PlayableModel* model );
    virtual void setModel( QAbstractItemModel* model );
    void setProxyModel( PlayableProxyModel* model );

    virtual PlayableModel* model() const { return m_model; }
    PlayableProxyModel* proxyModel() const { return m_proxyModel; }
    PlaylistItemDelegate* delegate() const { return m_delegate; }
    ViewHeader* header() const { return m_header; }
    OverlayWidget* overlay() const { return m_overlay; }
    Tomahawk::ContextMenu* contextMenu() const { return m_contextMenu; }
    AnimatedSpinner* loadingSpinner() const { return m_loadingSpinner; }

    void setEmptyTip( const QString& tip );

    virtual QWidget* widget() { return this; }
    virtual Tomahawk::playlistinterface_ptr playlistInterface() const;
    void setPlaylistInterface( const Tomahawk::playlistinterface_ptr& playlistInterface );

    virtual QString title() const;
    virtual QString description() const;
    virtual QPixmap pixmap() const;

    virtual bool showFilter() const { return true; }
    virtual bool setFilter( const QString& filter );
    virtual bool jumpToCurrentTrack();

    QModelIndex hoveredIndex() const { return m_hoveredIndex; }
    QModelIndex contextMenuIndex() const { return m_contextMenuIndex; }
    void setContextMenuIndex( const QModelIndex& idx ) { m_contextMenuIndex = idx; }

    bool updatesContextView() const { return m_updateContextView; }
    void setUpdatesContextView( bool b ) { m_updateContextView = b; }

    bool autoResize() const { return m_autoResize; }
    void setAutoResize( bool b );

    // Starts playing from the beginning if resolved, or waits until a track is playable
    void startPlayingFromStart();

public slots:
    virtual void onItemActivated( const QModelIndex& index );

    void deleteSelectedItems();

    void playItem();
    void onMenuTriggered( int action );

    void onViewChanged();
    void onScrollTimeout();

signals:
    void itemActivated( const QModelIndex& index );
    void modelChanged();

protected:
    virtual void resizeEvent( QResizeEvent* event );

    virtual void startDrag( Qt::DropActions supportedActions );
    virtual void dragEnterEvent( QDragEnterEvent* event );
    virtual void dragLeaveEvent( QDragLeaveEvent* /*event*/ ) { m_dragging = false; setDirtyRegion( m_dropRect ); }
    virtual void dragMoveEvent( QDragMoveEvent* event );
    virtual void dropEvent( QDropEvent* event );

    void wheelEvent( QWheelEvent* event );
    void mouseMoveEvent( QMouseEvent* event );
    void mousePressEvent( QMouseEvent* event );
    void leaveEvent( QEvent* event );
    void paintEvent( QPaintEvent* event );
    void keyPressEvent( QKeyEvent* event );

protected slots:
    virtual void currentChanged( const QModelIndex& current, const QModelIndex& previous );

private slots:
    void onItemResized( const QModelIndex& index );
    void onFilterChanged( const QString& filter );

    void onCustomContextMenu( const QPoint& pos );

    void autoPlayResolveFinished( const Tomahawk::query_ptr& query, int row );

    void verifySize();
    
private:
    void startAutoPlay( const QModelIndex& index );
    bool tryToPlayItem( const QModelIndex& index );
    void updateHoverIndex( const QPoint& pos );
    void setCustomContextMenuQueries( const QModelIndex& idx );

    QString m_guid;
    PlayableModel* m_model;
    PlayableProxyModel* m_proxyModel;
    PlaylistItemDelegate* m_delegate;
    ViewHeader* m_header;
    OverlayWidget* m_overlay;
    AnimatedSpinner* m_loadingSpinner;

    QString m_emptyTip;
    bool m_resizing;
    bool m_dragging;
    QRect m_dropRect;

    bool m_updateContextView;
    bool m_autoResize;

    QModelIndex m_hoveredIndex;
    QModelIndex m_contextMenuIndex;

    Tomahawk::query_ptr m_autoPlaying;
    Tomahawk::ContextMenu* m_contextMenu;

    QTimer m_timer;
};

#endif // TRACKVIEW_H
