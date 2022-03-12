#include "mainwindowcontroller.h"
#include "QFile"
#include "QIODevice"
#include "QJsonDocument"
#include "QJsonArray"
#include <QDir>

MainWindowController::MainWindowController()
{
    init();
}

void MainWindowController::init() {
    QFile settingFile("setting.json");
    if (!settingFile.open(QIODevice::ReadOnly)) {
        return;
    }
    QByteArray byteArray = settingFile.readAll();
    QJsonDocument document = QJsonDocument::fromJson(byteArray);
    QJsonObject setting = document.object();

    blogBase = setting.value("blogBase").toString();

    QJsonArray blogsArray = setting.value("blogs").toArray();
    for (auto begin = blogsArray.begin(), end = blogsArray.end(); begin != end; ++begin) {
        QJsonObject obj = begin->toObject();
        Blog blog(obj);
        blogs.push_back(blog);
        blogIds.push_back(blog.getId());
    }
}

const QVector<QString>& MainWindowController::getBlogIds() {
    return blogIds;
}

const QVector<QString>& MainWindowController::getAlbumIds() {
    return *albumIds;
}

const Blog* MainWindowController::getBlogById(QString id) {
    for (auto begin = blogs.begin(), end = blogs.end(); begin != end; ++begin) {
        if (begin->getId() == id) {
            return begin;
        }
    }
    return nullptr;
}

const Album* MainWindowController::getAlbumById(QString id) {
    for (auto begin = albums->begin(), end = albums->end(); begin != end; ++begin) {
        if (begin->id == id) {
            return &(*begin);
        }
    }
    return nullptr;
}

void MainWindowController::loadBlog(QString id) {
    if (currentBlog != nullptr && id == currentBlog->getId()) {
        return;
    }
    if (currentBlog != nullptr) {
        delete currentBlog;
    }
    if (albums != nullptr) {
        delete albums;
    }
    if (albumIds != nullptr) {
        delete albums;
    }
    if (currentAlbums != nullptr) {
        delete currentAlbums;
        currentAlbums = nullptr;
    }

    const Blog* blog = getBlogById(id);
    currentBlog = new Blog();
    *currentBlog = *blog;

    // load allAlbum;
    albums = new QVector<Album>();
    albumIds = new QVector<QString>();
    QString albumsPath = blogBase + currentBlog->getAlbums();
    QFile albumsFile(albumsPath);
    if (!albumsFile.open(QIODevice::ReadOnly)) {
        return;
    }
    QByteArray byteArray = albumsFile.readAll();
    QJsonDocument document = QJsonDocument::fromJson(byteArray);
    QJsonArray albumArray = document.array();
    for (auto begin = albumArray.begin(), end = albumArray.end(); begin != end; ++begin) {
        QJsonObject obj = begin->toObject();
        QString albunmPath = blogBase + currentBlog->getMarkdownAlbumsBase() + obj.value("id").toString() + ".json";
        qDebug() << "Load albunm: " << albunmPath;
        QJsonDocument* document = loadJsonFile(albunmPath);
        if (document == nullptr) {
            continue;
        }
        QJsonObject albunmObj = document->object();
        Album album(albunmObj);
        albums->push_back(albunmObj);
        albumIds->push_back(album.id);
    }
}

void MainWindowController::loadAlbum(QString id) {
    if (currentAlbums != nullptr && id == currentAlbums->id) {
        return;
    }
    if (currentAlbums != nullptr) {
        delete currentAlbums;
    }

    const Album* album = getAlbumById(id);
    currentAlbums = new Album();
    *currentAlbums = *album;
}

QJsonDocument* MainWindowController::loadJsonFile(QString path) {
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray array = file.readAll();
        QJsonDocument document = QJsonDocument::fromJson(array);
        QJsonDocument* doc = new QJsonDocument(document);
        return doc;
    }
    return nullptr;
}

Album* MainWindowController::getCurrentAlbum() {
    return currentAlbums;
}

void MainWindowController::reshapeAlbumItems(QVector<Post*> posts) {
    if (currentAlbums == nullptr) {
        return;
    }
    currentAlbums->reshapeItems(posts);

    for (int i = 0; i < albums->count(); ++i) {
        if (albums->value(i).id == currentAlbums->id) {
            albums->replace(i, *currentAlbums);
            break;
        }
    }
}


bool MainWindowController::addAlbum(Album album) {
    if (currentBlog == nullptr) {
        return false;
    }
    if (album.id == "" || album.author == "" || album.title == "") {
        return false;
    }
    if (getAlbumIds().contains(album.id)) {
        return false;
    }
    album.modifyTime = QDateTime::currentMSecsSinceEpoch();
    albums->push_back(album);
    albumIds->push_back(album.id);
    QFile path(blogBase + currentBlog->getMarkdownAlbumsBase() + album.id + ".md");
    if (path.open(QIODevice::WriteOnly)) {
        path.write(album.prev.toStdString().c_str());
        path.flush();
        path.close();
    }
    return true;
}

void MainWindowController::searchMarkdowns() {
    if (currentBlog == nullptr) {
        return;
    }
    markdowns.clear();
    QDir base(blogBase + currentBlog->getMarkdownBase());
    QVector<QString> filePaths;
    QVector<QDir> dirs;
    dirs.append(base);
    while (!dirs.empty()) {
        QDir dir = dirs.front();
        dirs.pop_front();
        QStringList mds = dir.entryList(QDir::Files);
        for (auto begin = mds.begin(), end = mds.end(); begin != end; ++begin) {
            filePaths.append(dir.path() + "/" + *begin);
        }

        QStringList subDirs = dir.entryList(QDir::Dirs);
        for (auto begin = subDirs.begin(), end = subDirs.end(); begin != end; ++begin) {
            if (*begin == "." || *begin == "..") {
                continue;
            }
            QDir subDir(dir.path() + "/" + *begin);
            dirs.append(subDir);
        }
    }
    for (auto begin = filePaths.begin(), end = filePaths.end(); begin != end; ++begin) {
        QString path = *begin;
        path = path.split(currentBlog->getMarkdownBase())[1];
        markdowns.push_back(path);
    }

    for (auto begin = albums->begin(), end = albums->end(); begin != end; ++begin) {
        const QMap<QString, Post> map = begin->getAllItems();
        for (auto b = map.begin(), e = map.end(); b != e; ++b) {
            markdowns.removeOne(b->markdown);
        }
    }

    qDebug() << markdowns;
}

const QVector<QString>& MainWindowController::getMarkdowns() {
    return markdowns;
}

void MainWindowController::saveAll() {
    QList<Post> allItems;
    QJsonArray allAlbums;
    for (auto begin = albums->begin(), end = albums->end(); begin != end; ++begin) {
        allAlbums.push_back(begin->toJsonObjectWithoutItems());
        QJsonObject aloneAlbumObject = begin->toJsonObject();
        QList<Post> temp = begin->getAllItems().values();
        for (Post post : temp) {
            post.album = begin->id;
            allItems.append(post);
        }
        QString filePath = blogBase + currentBlog->getMarkdownAlbumsBase() + begin->id + ".json";
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            continue;
        }
        QJsonDocument document(aloneAlbumObject);
        QByteArray array = document.toJson();
        file.write(array);
        file.flush();
        file.close();
    }
    QJsonDocument document(allAlbums);
    QString filePath = blogBase + currentBlog->getAlbums();
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    QByteArray array = document.toJson();
    file.write(array);
    file.flush();
    file.close();

    // refresh newest post
    std::sort(allItems.begin(), allItems.end(), compareItems);
    const int count = 6;
    QJsonArray newPosts;
    for (int i = 0; i < count && !allItems.empty(); ++i) {
        Post item = allItems.last();
        allItems.pop_back();
        QJsonObject obj;
        QJsonValue jId(item.id);
        QJsonValue jAlbum(item.album);
        QJsonValue jTitle(item.title);
        QJsonValue jPostTime(item.postTime);
        QJsonValue jModifyTime(item.modifyTime);
        QJsonValue jAuthor(item.author);
        QJsonValue jTag(item.getTag());
        obj["id"] = jId;
        obj["album"] = jAlbum;
        obj["title"] = jTitle;
        obj["postTime"] = jPostTime;
        obj["modifyTime"] = jModifyTime;
        obj["author"] = jAuthor;
        obj["tag"] = jTag;
        newPosts.append(obj);
    }
    QJsonDocument newestPostDoc(newPosts);
    QFile newestPostFile(blogBase + currentBlog->getNewestPost());
    if (newestPostFile.open(QIODevice::WriteOnly)) {
        newestPostFile.write(newestPostDoc.toJson());
        newestPostFile.flush();
        newestPostFile.close();
    }
}

bool MainWindowController::compareItems(Post& i1, Post& i2) {
    return i1.modifyTime > i2.modifyTime;
}


int MainWindowController::addPost(Post post) {
    if (currentAlbums == nullptr) {
        return -1;
    } else if (currentAlbums->getItem(post.id).id != "null") {
        return -2;
    } else if (post.id == "" || post.id.length() < 3) {
        return -3;
    } else if (post.title == "" || post.title.length() < 4) {
        return -4;
    } else if (post.author == "") {
        return -5;
    }
    post.autoFinish(blogBase + currentBlog->getMarkdownBase());
    currentAlbums->addItem(post);
    for (int i = 0; i < albums->count(); ++i) {
        if (albums->value(i).id == currentAlbums->id) {
            albums->replace(i, *currentAlbums);
            break;
        }
    }
    searchMarkdowns();
    return 0;
}

void MainWindowController::removeAlbum() {
    if (currentAlbums == nullptr) {
        return;
    }
    for (int i = 0; i < albums->count(); ++i) {
        const Album album = albums->at(i);
        if (album.id == currentAlbums->id) {
            albums->remove(i);
            albumIds->removeOne(currentAlbums->id);
            break;
        }
    }
    delete currentAlbums;
    currentAlbums = nullptr;
}

