#pragma once

#include <QObject>
#include <QString>
#include "modelnode.h"

namespace QmlDesigner {

class TimelineNavigatorEntity : public QObject {

    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString iconPath READ iconPath WRITE setIconPath NOTIFY iconPathChanged)
    Q_PROPERTY(int depth READ depth WRITE setDepth NOTIFY depthChanged)

public:
    explicit TimelineNavigatorEntity(QObject *parent=0);
    TimelineNavigatorEntity(QString name, QString iconPath, int depth, QObject *parent=0);

    QString name();
    void setName(QString name);

    QString iconPath();
    void setIconPath(QString iconPath);

    int depth();
    void setDepth(int depth);

signals:
    void nameChanged();
    void iconPathChanged();
    void depthChanged();

private:
    QString m_name;
    QString m_iconPath;
    int m_depth;
};

QString getTypeIcon(const ModelNode &modelNode);

}
