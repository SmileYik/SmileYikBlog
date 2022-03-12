#ifndef MAINWINDOWCONTROLLER_H
#define MAINWINDOWCONTROLLER_H

#include "entry/album.h"
#include "entry/blog.h"
#include "entry/post.h"
#include <QMap>

class MainWindowController
{
public:
    MainWindowController();
    const QVector<QString>& getAlbumIds();
    const QVector<QString>& getBlogIds();
    const Blog* getBlogById(QString id);
    const Album* getAlbumById(QString id);
    Album* getCurrentAlbum();
    void loadBlog(QString id);
    void loadAlbum(QString id);
    void reshapeAlbumItems(QVector<Post*> posts);
    bool addAlbum(Album album);
    int addPost(Post post);
    void searchMarkdowns();
    const QVector<QString>& getMarkdowns();
    void saveAll();
private:
    QString blogBase;
    QVector<Blog> blogs;
    QVector<QString> blogIds;
    QVector<QString> markdowns;

    Blog* currentBlog = nullptr;
    QVector<Album>* albums = nullptr;
    QVector<QString>* albumIds = nullptr;
    Album* currentAlbums = nullptr;

    void init();
    QJsonDocument* loadJsonFile(QString path);

};

#endif // MAINWINDOWCONTROLLER_H
