#ifndef APRSPIXMAPITEM_H
#define APRSPIXMAPITEM_H


#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHoverEvent>
#include <QDebug>
#include <QPixmap>
#include <QString>
#include <QGraphicsScene>
#include <QPointF>

class AprsPixmapItem : public QGraphicsPixmapItem
{

public:
    explicit AprsPixmapItem(QPixmap &pixmap);
    void setMessage(QString &adressee, QString &via, QString &message);
    inline void setPosition(QPointF pos) { _pos=pos;}
private:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    QString _adressee;
    QString _via;
    QString _message;
    QGraphicsTextItem * _item_text;
    QPointF _pos;
    
};


#endif // APRSPIXMAPITEM_H
