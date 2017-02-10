#include "timelinemodel.h"

#include <QAbstractItemModel>

namespace QmlDesigner {

  Keyframe::Keyframe(QObject *parent) : QObject(parent) {

  }

  Keyframe::Keyframe(const QString &property, const int &startTime, const int &duration, const QVariant &endValue, QObject *parent)
    : QObject(parent),
      m_property(property),
      m_startTime(startTime), 
      m_duration(duration),
      m_endValue(endValue)
  {  
  }

  int Keyframe::startTime() const {
    return m_startTime;
  }

  int Keyframe::duration() const {
    return m_duration;
  }

  TimelineItem::TimelineItem(const QString &name, const QString &iconPath, const int &depth)
    : m_name(name),
      m_iconPath(iconPath),
      m_depth(depth),
      m_properties(QList<QString>()),
      m_keyframes(QList<QObject*>())
  {
  }

  QString TimelineItem::name() const {
    return m_name;
  }

  QString TimelineItem::iconPath() const {
    return m_iconPath;
  }

  int TimelineItem::depth() const {
    return m_depth;
  }

  QList<QObject*> TimelineItem::keyframes() const {
    return m_keyframes;
  }

  QList<QString> TimelineItem::properties() const {
    return m_properties;
  }

  void TimelineItem::addKeyframe(Keyframe *keyframe) {
    m_keyframes.append(keyframe);
  }

  void TimelineItem::addProperty(QString property) {
    m_properties.append(property);
  }

  TimelineModel::TimelineModel(QObject *parent)
    : QAbstractListModel(parent)
  {  
  }
  
  void TimelineModel::addItem(const TimelineItem &item) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_items << item;
    endInsertRows();
  }

  int TimelineModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_items.count();
  }

  QVariant TimelineModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() >= m_items.count())
      return QVariant();
    const TimelineItem &item = m_items[index.row()];
    if (role == NameRole)
      return item.name();
    else if (role == IconPathRole)
      return item.iconPath();
    else if (role == DepthRole)
      return item.depth();
    else if (role == PropertyRole)
      return QVariant::fromValue(item.properties());
    else if (role == KeyframeRole)
      return QVariant::fromValue(item.keyframes());
    return QVariant();
  }

  QHash<int, QByteArray> TimelineModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[IconPathRole] = "iconPath";
    roles[DepthRole] = "deep";
    roles[PropertyRole] = "properties";
    roles[KeyframeRole] = "keyframes";
    return roles;
  }
}
