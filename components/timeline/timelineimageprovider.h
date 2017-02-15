#pragma once

#include <QQuickImageProvider>

namespace QmlDesigner {
class TimelineImageProvider : public QQuickImageProvider
{
public:
    TimelineImageProvider();
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;
};
}
