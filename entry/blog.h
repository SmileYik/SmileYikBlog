#ifndef BLOG_H
#define BLOG_H

#include <QString>
#include <QJsonObject>

class Blog
{
public:
    Blog();

    Blog(QString,
         QString,
         QString,
         QString,
         QString,
         QString,
         QString);

    Blog(QJsonObject& obj);

    QString getId();
    QString getTitle();
    QString getBase();
    QString getMarkdownBase();
    QString getMarkdownAlbumsBase();
    QString getAlbums();
    QString getNewestPost();
private:
    QString id;
    QString title;
    QString base;
    QString markdownBase;
    QString markdownAlbumsBase;
    QString albums;
    QString newestPost;
};

#endif // BLOG_H
