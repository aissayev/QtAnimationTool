#include "timelinenavigatorentity.h"
#include "itemlibraryinfo.h"
#include "modelnode.h"
#include "nodemetainfo.h"
#include "metainfo.h"
#include "model.h"

namespace QmlDesigner {

TimelineNavigatorEntity::TimelineNavigatorEntity(QObject *parent)
    : QObject(parent) {}

TimelineNavigatorEntity::TimelineNavigatorEntity(QString name, QString iconPath, int depth, QObject *parent)
    : QObject(parent),
      m_name(name),
      m_iconPath(iconPath),
      m_depth(depth) {}

QString TimelineNavigatorEntity::name(){
    return m_name;
}

void TimelineNavigatorEntity::setName(QString name) {
    if (name != m_name) {
        m_name = name;
        emit nameChanged();
    }
}

QString TimelineNavigatorEntity::iconPath(){
    return m_iconPath;
}

void TimelineNavigatorEntity::setIconPath(QString iconPath){
    if (iconPath != m_iconPath) {
        m_iconPath = iconPath;
        emit iconPathChanged();
    }
}

int TimelineNavigatorEntity::depth() {
    return m_depth;
}

void TimelineNavigatorEntity::setDepth(int depth) {
    if (depth != m_depth) {
        m_depth = depth;
        emit depthChanged();
    }
}


QString getTypeIcon(const ModelNode &modelNode) {
    if (modelNode.isValid()) {
        // if node has no own icon, search for it in the itemlibrary
        const ItemLibraryInfo *libraryInfo = modelNode.model()->metaInfo().itemLibraryInfo();
        QList <ItemLibraryEntry> itemLibraryEntryList = libraryInfo->entriesForType(
            modelNode.type(), modelNode.majorVersion(), modelNode.minorVersion());
        if (!itemLibraryEntryList.isEmpty())
            return QStringLiteral("../../common/") + *(itemLibraryEntryList.first().libraryEntryIconPath().replace(":/",""));
        else if (modelNode.metaInfo().isValid())
            return QStringLiteral("../../common/ItemLibrary/images/item-default-icon.png");
    }

    return QStringLiteral("../../common/ItemLibrary/images/item-invalid-icon.png");
}

}
