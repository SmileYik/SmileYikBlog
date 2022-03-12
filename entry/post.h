#ifndef POST_H
#define POST_H

#include <QString>
#include <QVector>
#include <QSet>
#include <QJsonObject>

class Post
{
public:
    Post();
    Post(int, QJsonObject&);
    Post(const Post& post, int a);

    int weight;
    QString id;
    QString title;
    QString markdown;
    QString author;
    qint64 postTime;
    qint64 modifyTime;
    QString prev;
    QString album;

    const QSet<QString>& getTags();
    void setTags(QString tags);
    void addTag(QString tag);
    void removeTag(QString tag);
    const QVector<Post>& getItems();
    void addItem(Post& item);
    void removeItem(Post& item);
    void sortItems();
    void clearItem();
    void autoFinish(QString markdownBase);
    QString getTag();
    QJsonObject toJsonObject();
private:
    QSet<QString> tags;
    QVector<Post> items;

    static bool compareItems(Post& i1, Post& i2);
};

//"id": "item6",
//"title": "Item6Title",
//"markdown": "item2.md",
//"postTime": "01-06-05",
//"modifyTime": "01-07-05",
//"author": "SmileYik",
//"tag": "a,b,c",
//"prev": "markdown",
//"items": []

#endif // POST_H
