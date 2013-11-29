#ifndef TOMAHAWK_WEBSEARCHQUERY_H
#define TOMAHAWK_WEBSEARCHQUERY_H

#include <QtCore/QObject>
#include <QxtWebRedirectEvent>
#include "Query.h"

namespace Tomahawk {

class WebSearchQuery : public QObject
{
    Q_OBJECT

public:
    explicit WebSearchQuery(const QString& query, const QxtWebRequestEvent* event);

signals:
    void searchFinished( const QString query, const QList<Tomahawk::result_ptr> results, const QxtWebRequestEvent* event );


private:
    ~WebSearchQuery(void);

    const QString m_query;
    const QxtWebRequestEvent* m_event;

private slots:
    void onSearchFinished( const QList<Tomahawk::result_ptr> results);

};

typedef QSharedPointer<WebSearchQuery> webQuery_ptr;

} // namespace Tomahawk

#endif // TOMAHAWK_WEBSEARCHQUERY_H
