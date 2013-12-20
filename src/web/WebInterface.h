#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include "web/Api_v1.h"
#include "Pipeline.h"
#include "Artist.h"
#include "WebSearchQuery.h"

typedef QHash<QString, QString> QStringMap;


class WebInterface : public Api_v1
{
Q_OBJECT

public:
    WebInterface(QxtAbstractWebSessionManager* sm);


public slots:
    void index( QxtWebRequestEvent* event );
    void search( QxtWebRequestEvent* event );
    void add( QxtWebRequestEvent* event );
    void playlists( QxtWebRequestEvent* event );
    void playlist( QxtWebRequestEvent* event, QString guid );

private slots:
    void onSearchFinished( const QString query, const QList<Tomahawk::result_ptr> results, const QxtWebRequestEvent* event );

private:
    const QString getFileContent(const QString& filename) const;
    void sendMessagePage( QxtWebRequestEvent *event, const QString message );
    void sendMultiFilePage(const QxtWebRequestEvent* event, const QString& bodyFile, const QStringMap& bodyArgs,
                           const QString& entryFile, const QList<QStringMap>& entryArgs);
    void sendPage(const QxtWebRequestEvent* event, const QString& page);
    const QString getDecodedURLAttribute(const QUrl& url, const QString& key) const;

    const QString m_htmlHeader;
    const QString m_htmlQueue;
    const QString m_htmlSearch;
    const QString m_htmlResult;
};

#endif // WEBINTERFACE_H
