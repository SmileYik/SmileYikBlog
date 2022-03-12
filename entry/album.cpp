#include "album.h"
#include <QJsonArray>

//"id": "album1",
//"title": "Album1",
//"modifyTime": "01-06-05",
//"author": "SmileYik",
//"items": [

Album::Album()
{

}

Album::Album(QJsonObject obj)
{
    id = obj.value("id").toString();
    title = obj.value("title").toString();
    modifyTime = obj.value("modifyTime").toString().toLongLong();
    author = obj.value("author").toString();
    QJsonArray itemArray = obj.value("items").toArray();
    int weight = 0;
    for (auto begin = itemArray.begin(), end = itemArray.end(); begin != end; ++begin, ++weight) {
        QJsonObject obj = begin->toObject();
        Post post(weight, obj);
        addItem(post);
    }
    bakePostMap();
}

const QVector<Post>& Album::getItems() {
    return items;
}

const Post Album::getItem(QString id) {
    Post post;
    post.id = "null";
    return postMap.value(id, post);
}

void Album::addItem(Post& item) {
    for (auto start = items.cbegin(), end = items.cend(); start != end; ++start) {
        if (start->id == item.id) {
            return;
        }
    }
    postMap.insert(item.id, item);
    items.push_back(item);
}

void Album::removeItem(Post &item) {
    for (auto start = items.begin(), end = items.end(); start != end; ++start) {
        if (start->id == item.id) {
            start->removeItem(*start);
            return;
        }
    }
}

bool Album::compareItems(Post &i1, Post &i2) {
    return i1.weight > i2.weight;
}

void Album::sortItems() {
    std::sort(items.begin(), items.end(), compareItems);
    for (auto start = items.begin(), end = items.end(); start != end; ++start) {
        start->sortItems();
    }
}

void Album::reshapeItems(QVector<Post*> ids) {
    QVector<Post> posts;
    for (auto begin = ids.begin(), end = ids.end(); begin != end; ++begin) {
        Post* postId = *begin;
        Post post(postMap[postId->id], 1);
        post.weight = postId->weight;
        reshapeItems(*postId, post);
        posts.push_back(post);
    }
    items = posts;
    bakePostMap();
}

void Album::reshapeItems(Post &ids, Post &parent) {
    const QVector<Post> posts = ids.getItems();
    for (auto begin = posts.begin(), end = posts.end(); begin != end; ++begin) {
        Post postId = *begin;
        Post post(postMap[postId.id], 1);
        post.weight = postId.weight;
        reshapeItems(postId, post);
        parent.addItem(post);
    }
}


void Album::bakePostMap() {
    QVector<Post> stack;
    const QVector<Post> posts = getItems();
    for (auto begin = posts.begin(), end = posts.end(); begin != end; ++begin) {
        stack.push_back(*begin);
    }
    while (!stack.empty()) {
        Post post = stack.front();
        stack.pop_front();
        postMap.insert(post.id, post);
        const QVector<Post> sub = post.getItems();
        for (auto begin = sub.begin(), end = sub.end(); begin != end; ++begin) {
            stack.push_back(*begin);
        }
    }
}

const QMap<QString, Post>& Album::getAllItems() {
    return postMap;
}

QJsonObject Album::toJsonObject() {
    QJsonObject obj;
    QJsonValue jId(id);
    QJsonValue jTitle(title);
    QJsonValue jModifyTime(modifyTime);
    QJsonValue jAuthor(author);
    QJsonArray itemArray;
    sortItems();
    for (auto begin = items.begin(), end = items.end(); begin != end; ++begin) {
        QJsonValue value(begin->toJsonObject());
        itemArray.push_back(value);
    }
    QJsonValue allItem(itemArray);

    obj["id"] = jId;
    obj["title"] = jTitle;
    obj["modifyTime"] = jModifyTime;
    obj["author"] = jAuthor;
    obj["items"] = allItem;
    return obj;
}

QJsonObject Album::toJsonObjectWithoutItems() {
    QJsonObject obj;
    QJsonValue jId(id);
    QJsonValue jTitle(title);
    QJsonValue jModifyTime(modifyTime);
    QJsonValue jAuthor(author);
    obj["id"] = jId;
    obj["title"] = jTitle;
    obj["modifyTime"] = jModifyTime;
    obj["author"] = jAuthor;
    return obj;
}
