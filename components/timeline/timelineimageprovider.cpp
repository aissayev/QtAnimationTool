#include "timelineimageprovider.h"

#include <utils/icon.h>

#include <coreplugin/icore.h>
#include <QDebug>

namespace QmlDesigner {
TimelineImageProvider::TimelineImageProvider()
    : QQuickImageProvider(Pixmap)
{

}

QPixmap TimelineImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    using namespace Utils;
    QPixmap result;
    if (id == "play") {
        result = Icon({{QLatin1String(":/timeline/icon/play.png"), Theme::IconsBaseColor}},Icon::Tint).pixmap();
    }
    if (id == "add-keyframe") {
        result = Icon({{QLatin1String(":/timeline/icon/add-keyframe.png"), Theme::IconsBaseColor}},Icon::Tint).pixmap();
    }
    else if (id == "to-end") {
        result = Icon({ {QLatin1String(":/timeline/icon/to-end.png"), Theme::IconsBaseColor}},Icon::Tint).pixmap();
    }
    else if (id == "to-start") {
        result = Icon({ {QLatin1String(":/timeline/icon/to-start.png"), Theme::IconsBaseColor}},Icon::Tint).pixmap();
    }
    else if (id == "step-forward") {
        result = Icon({ {QLatin1String(":/timeline/icon/step-f.png"), Theme::IconsBaseColor}},Icon::Tint).pixmap();
    }
    else if (id == "step-backwards") {
        result = Icon({ {QLatin1String(":/timeline/icon/step-b.png"), Theme::IconsBaseColor}},Icon::Tint).pixmap();
    }
    else if (id == "record") {
        result = Icon({ {QLatin1String(":/timeline/icon/record.png"), Theme::IconsBaseColor}},Icon::Tint).pixmap();
    }
    else if (id == "record-h") {
        result = Icon({ {QLatin1String(":/timeline/icon/record.png"), Theme::IconsErrorColor}},Icon::Tint).pixmap();
    }
    else if (id == "zoom") {
        result = Icon({ {QLatin1String(":/timeline/icon/zoom.png"), Theme::IconsBaseColor}},Icon::Tint).pixmap();
    }
    else
        qWarning() << Q_FUNC_INFO << "Image not found:" << id;
    return result;
}


}
