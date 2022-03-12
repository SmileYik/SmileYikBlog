#include <QString>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QIODevice>
#include <QTextStream>
#include <iostream>

#include "blog.h"

Blog::Blog() {

}

Blog::Blog(QString _id,
           QString _title,
           QString _base,
           QString _markdownBase,
           QString _markdwonAlbumsBase,
           QString _albums,
           QString _newestPost) :
    id(_id),
    title(_title),
    base(_base),
    markdownBase(_markdownBase),
    markdownAlbumsBase(_markdwonAlbumsBase),
    albums(_albums),
    newestPost(_newestPost){

}

Blog::Blog(QJsonObject& obj) {
    this->id = obj.take("id").toString();
    this->title = obj.take("title").toString();
    this->base = obj.take("base").toString();
    this->markdownBase = obj.take("markdownBase").toString();
    this->markdownAlbumsBase = obj.take("markdownAlbumsBase").toString();
    this->albums = obj.take("albums").toString();
    this->newestPost = obj.take("newestPost").toString();
}

QString Blog::getId() {
    return id;
}

QString Blog::getTitle() {
    return title;
}

QString Blog::getBase() {
    return base;
}

QString Blog::getMarkdownBase() {
    return getBase() + markdownBase;
}

QString Blog::getMarkdownAlbumsBase() {
    return getBase() + markdownAlbumsBase;
}

QString Blog::getAlbums() {
    return getBase() + albums;
}

QString Blog::getNewestPost() {
    return getBase() + newestPost;
}

