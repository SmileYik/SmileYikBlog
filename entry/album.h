#ifndef ALBUM_H
#define ALBUM_H

#include "post.h"
#include <QVector>
#include <QString>
#include <QJsonObject>
#include <QMap>

class Album
{
public:
    Album();
    Album(QJsonObject obj);

    QString id;
    QString title;
    QString author;
    QString prev;
    qint64 modifyTime;

    const Post getItem(QString id);
    const QVector<Post>& getItems();
    const QMap<QString, Post>& getAllItems();
    void addItem(Post& item);
    void removeItem(Post& item);
    void sortItems();
    void reshapeItems(QVector<Post*> ids);
    QJsonObject toJsonObject();
    QJsonObject toJsonObjectWithoutItems();
private:
    QVector<Post> items;
    QMap<QString, Post> postMap;
    void reshapeItems(Post &ids, Post &parent);
    void bakePostMap();
    static bool compareItems(Post& i1, Post& i2);
};

#endif // ALBUM_H

//"id": "album1",
//"title": "Album1",
//"modifyTime": "01-06-05",
//"author": "SmileYik",
//"items": [
