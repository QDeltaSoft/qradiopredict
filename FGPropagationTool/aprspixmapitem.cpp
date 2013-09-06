#include "aprspixmapitem.h"


AprsPixmapItem::AprsPixmapItem(QPixmap &pixmap) :
    QGraphicsPixmapItem(pixmap)
{
    this->setAcceptHoverEvents(true);
}

void AprsPixmapItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    event->setAccepted(true);
    QGraphicsScene *scene = this->scene();
    QGraphicsTextItem * message = new QGraphicsTextItem;
    message->setPos(_pos - QPoint(0,16));
    message->setHtml("<div style=\"background:white;color:blue;font-height:15px;\"><b>"+
                      _callsign+"</b></div><div style=\"background:white;color:black;\"><br/>"+_message+"<br/><b>Via:</b>"+_via+"</span>");
    scene->addItem(message);
    _item_text = message;

}

void AprsPixmapItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    event->setAccepted(true);
    this->scene()->removeItem(_item_text);
}

void AprsPixmapItem::setMessage(QString &callsign, QString &via, QString &message)
{
    _callsign = callsign;
    _via = via;
    _message = message;

}
