#include "post.h"
#include <QJsonArray>
#include <QFile>
#include <QIODevice>
#include <QFileInfo>

Post::Post()
{

}

Post::Post(const Post& post, int a) : weight(post.weight),
                         id(post.id), title(post.title),
                         markdown(post.markdown), author(post.author),
                         postTime(post.postTime), modifyTime(post.modifyTime),
                         prev(post.prev), tags(post.tags){

}

Post::Post(int weight, QJsonObject& obj) {
    this->weight = weight;
    id = obj.value("id").toString();
    title = obj.value("title").toString();
    markdown = obj.value("markdown").toString();
    author = obj.value("author").toString();
    postTime = obj.value("postTime").toDouble();
    modifyTime = obj.value("modifyTime").toDouble();
    prev = obj.value("prev").toString();
    QString tagStr = obj.value("tag").toString();
    QStringList list = tagStr.split(",");
    for (auto begin = list.begin(), end = list.end(); begin != end; ++begin) {
        tags.insert(*begin);
    }

    // load items;
    QJsonArray itemArray = obj.value("items").toArray();
    int index = 0;
    for (auto begin = itemArray.begin(), end = itemArray.end(); begin != end; ++begin, ++index) {
        QJsonObject obj = begin->toObject();
        Post post(index, obj);
        addItem(post);
    }
}

void Post::setTags(QString tagStr) {
    QStringList list = tagStr.split(",");
    for (auto begin = list.begin(), end = list.end(); begin != end; ++begin) {
        tags.insert(*begin);
    }
}

const QSet<QString>& Post::getTags() {
    return tags;
}

void Post::addTag(QString tag) {
    tags.insert(tag);
}

void Post::removeTag(QString tag) {
    tags.remove(tag);
}

const QVector<Post>& Post::getItems() {
    return items;
}

void Post::addItem(Post& item) {
    if (this->id == item.id) {
        return;
    }
    for (auto begin = items.cbegin(),
              end   = items.cend();
         begin != end; ++begin) {
        if (begin->id == item.id) {
            return;
        }
    }
    items.push_back(item);
}

void Post::removeItem(Post& item) {
    for (auto begin = items.begin(),
              end   = items.end();
         begin != end; ++begin) {
        if (begin->id == item.id) {
            begin->removeItem(*begin);
            return;
        }
    }
}
bool Post::compareItems(Post& i1, Post& i2) {
    return i2.weight > i1.weight;
}


void Post::sortItems() {
    std::sort(items.begin(), items.end(), compareItems);
    for (auto begin = items.begin(),
              end   = items.end();
         begin != end; ++begin) {
        begin->sortItems();
    }
}

void Post::clearItem() {
    this->items.clear();
}

void Post::autoFinish(QString markdownBase) {
    QFile file(markdownBase + markdown);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QTextStream stream(&file);
    QString line = nullptr;
    const int maxLength = 5;
    int length = 0;
    prev = "";
    while (!(line = stream.readLine()).isNull() && length < maxLength) {
        prev += line;
        ++length;
    }

    QFileInfo info(file);
    postTime = info.birthTime().currentMSecsSinceEpoch();
    modifyTime = info.lastModified().currentMSecsSinceEpoch();
}

QJsonObject Post::toJsonObject() {
    QJsonObject obj;
    QJsonValue jId(id);
    QJsonValue jTitle(title);
    QJsonValue jMarkdown(markdown);
    QJsonValue jPostTime(postTime);
    QJsonValue jModifyTime(modifyTime);
    QJsonValue jAuthor(author);
    QJsonValue jPrev(prev);
    QJsonValue jTag(getTag());
    obj["id"] = jId;
    obj["title"] = jTitle;
    obj["markdown"] = jMarkdown;
    obj["postTime"] = jPostTime;
    obj["modifyTime"] = jModifyTime;
    obj["author"] = jAuthor;
    obj["prev"] = jPrev;
    obj["tag"] = jTag;
    QJsonArray itemArray;
    for (auto begin = items.begin(), end = items.end(); begin != end; ++begin) {
        QJsonValue value(begin->toJsonObject());
        itemArray.push_back(value);
    }
    QJsonValue allItems(itemArray);
    obj["items"] = allItems;
    return obj;
}

QString Post::getTag() {
    QString tag = "";
    for (auto begin = tags.begin(), end = tags.end(); begin != end; ++begin) {
        tag += *begin + ",";
    }
    return tag;
}
