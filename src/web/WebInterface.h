#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include "web/Api_v1.h"
#include "Pipeline.h"
#include "Artist.h"
#include "WebSearchQuery.h"


class WebInterface : public Api_v1
{
Q_OBJECT

public:
    WebInterface(QxtAbstractWebSessionManager* sm);


public slots:
    virtual void index( QxtWebRequestEvent* event );
    virtual void search( QxtWebRequestEvent* event );

private slots:
    void onSearchFinished( const QString query, const QList<Tomahawk::result_ptr> results, const QxtWebRequestEvent* event );

private:
    const QString m_htmlHeader;
    const QString m_htmlStates;
    const QString m_htmlSearch;
    const QString m_htmlResult;
};

#endif // WEBINTERFACE_H
