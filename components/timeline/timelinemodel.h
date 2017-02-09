#pragma once

#include <QString>
#include <QAbstractItemModel>

#include "modelnode.h"

namespace QmlDesigner {
  class Keyframe: public QObject {
    Q_OBJECT

    Q_PROPERTY(int startTime READ startTime)
    Q_PROPERTY(int duration READ duration)

  public:
    explicit Keyframe(QObject *parent=0);
    Keyframe(const QString &property, const int &startTime, const int &duration, const int &startValue, const int &endValue,QObject *parent=0);

    int startTime() const ;
    int duration() const ;

  private:
    QString m_property;
    int m_startTime;
    int m_duration;
    int m_startValue;
    int m_endValue;
  };

  class TimelineItem
  {
  public: 
    TimelineItem(const QString &name, const QString &iconPath, const int &depth);

    QString name() const ;
    QString iconPath() const;
    int depth() const;

    QList<QString> properties() const ;
    QList<QObject*> keyframes() const ;

    void addProperty(QString property);
    void addKeyframe(Keyframe *keyframe);

  private:
    QString m_name;
    QString m_iconPath;
    int m_depth;
    QList<QString> m_properties;
    QList<QObject*> m_keyframes;

  };

  class TimelineModel : public QAbstractListModel
  {
    Q_OBJECT

  public:
    enum TimelineRoles {
      NameRole = Qt::UserRole +1,
      DepthRole,
      IconPathRole,
      PropertyRole,
      KeyframeRole,
    };

    TimelineModel(QObject *parent = 0);
    void addItem(const TimelineItem &item);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
  protected:
    QHash<int, QByteArray> roleNames() const;
  private:
    QList<TimelineItem> m_items;
  };
}
