#include "smileyikblogtool.h"
#include "ui_smileyikblogtool.h"
#include "entry/album.h"
#include <QVector>
#include "entry/post.h"
#include <QMessageBox>

SmileYikBlogTool::SmileYikBlogTool(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SmileYikBlogTool)
{
    ui->setupUi(this);

    const QVector<QString> blogIds = controller.getBlogIds();
    for (auto begin = blogIds.cbegin(), end = blogIds.cend(); begin != end; ++begin) {
        ui->blogListWeight->addItem(*begin);
    }
}

SmileYikBlogTool::~SmileYikBlogTool()
{
    delete ui;
}


void SmileYikBlogTool::on_blogListWeight_clicked(const QModelIndex &index)
{
    if (index.isValid()) {
        const QString id = index.data().toString();
        ui->label->setText(id);
        controller.loadBlog(id);
        ui->albumListWeight->clear();
        const QVector<QString> albumIds = controller.getAlbumIds();
        for (auto begin = albumIds.cbegin(), end = albumIds.cend(); begin != end; ++begin) {
            ui->albumListWeight->addItem(*begin);
        }
        on_postRefreshButton_clicked();
    }
}



void SmileYikBlogTool::on_albumListWeight_clicked(const QModelIndex &index)
{
    if (index.isValid()) {
        const QString id = index.data().toString();
        controller.loadAlbum(id);
        Album* album = controller.getCurrentAlbum();
        ui->itemTree->clear();
        const QVector<Post>& posts = album->Album::getItems();
        for (auto begin = posts.begin(), end = posts.end(); begin != end; ++begin) {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            ui->itemTree->addTopLevelItem(item);
            Post* p = new Post();
            *p = *begin;
            searchPost(item, p);
            ui->itemTree->expandAll();
        }
    }
}

void SmileYikBlogTool::searchPost(QTreeWidgetItem *item, Post* post) {
    item->setText(0, post->id);
    item->setText(1, post->title);
    const QVector<Post>& posts = post->getItems();
    for (auto begin = posts.begin(), end = posts.end(); begin != end; ++begin) {
        QTreeWidgetItem* child = new QTreeWidgetItem();
        item->addChild(child);
        Post* p = new Post();
        *p = *begin;
        searchPost(child, p);
    }
}

void SmileYikBlogTool::on_itemUp_clicked()
{
    QList<QTreeWidgetItem*> items = ui->itemTree->selectedItems();
    if (items.empty()) {
        return;
    }
    QTreeWidgetItem* item = items.first();
    if (item == nullptr) {
        return;
    }
    QTreeWidgetItem* parent = item->parent();
    if (parent == nullptr) {
        // this is top
        int index = ui->itemTree->indexOfTopLevelItem(item);
        if (index > 0) {
            item = ui->itemTree->takeTopLevelItem(index);
            ui->itemTree->insertTopLevelItem(index - 1, item);
        }
        ui->itemTree->setCurrentItem(item);
        ui->itemTree->scrollToItem(item);
        ui->itemTree->expandAll();
        return;
    }
    int index = parent->indexOfChild(item);
    qDebug() << index;
    if (index == 0) {
        // up to parent
        QTreeWidgetItem* grandparent = parent->parent();
        if (grandparent == nullptr) {
            // grandparent is tree
            item = parent->takeChild(index);
            index = ui->itemTree->indexOfTopLevelItem(parent);
            qDebug() << index;
            ui->itemTree->insertTopLevelItem(index, item);
        } else {
            item = parent->takeChild(index);
            index = grandparent->indexOfChild(parent);
            grandparent->insertChild(index, item);
        }
        ui->itemTree->setCurrentItem(item);
        ui->itemTree->scrollToItem(item);
        ui->itemTree->expandAll();
        return;
    }
    item = parent->takeChild(index);
    parent->insertChild(index - 1, item);
    ui->itemTree->setCurrentItem(item);
    ui->itemTree->scrollToItem(item);
    ui->itemTree->expandAll();
}


void SmileYikBlogTool::on_itemDown_clicked()
{
    QList<QTreeWidgetItem*> items = ui->itemTree->selectedItems();
    if (items.empty()) {
        return;
    }
    QTreeWidgetItem* item = items.first();
    if (item == nullptr) {
        return;
    }
    QTreeWidgetItem* parent = item->parent();
    if (parent == nullptr) {
        // top of tree
        int index = ui->itemTree->indexOfTopLevelItem(item);
        if (index == ui->itemTree->topLevelItemCount() - 1) {
            // it is alreay at bottom
            return;
        }
        index += 1;
        QTreeWidgetItem* next = ui->itemTree->topLevelItem(index);
        if (next == nullptr) {
            return;
        }
        item = ui->itemTree->takeTopLevelItem(index - 1);
        next->insertChild(0, item);
    } else {
        int index = parent->indexOfChild(item);
        if (index == parent->childCount() - 1) {
            QTreeWidgetItem* grandparent = parent->parent();
            if (grandparent == nullptr) {
                int parentIndex = ui->itemTree->indexOfTopLevelItem(parent);
                item = parent->takeChild(index);
                ui->itemTree->insertTopLevelItem(parentIndex + 1, item);
            } else {
                int parentIndex = grandparent->indexOfChild(parent);
                item = parent->takeChild(index);
                grandparent->insertChild(parentIndex + 1, item);
            }
        } else {
            index += 1;
            QTreeWidgetItem* next = parent->child(index);
            if (next == nullptr) {

            } else {
                item = parent->takeChild(index - 1);
                next->insertChild(0, item);
            }
        }
    }
    ui->itemTree->expandAll();
    ui->itemTree->setCurrentItem(item);
    ui->itemTree->scrollToItem(item);
}


void SmileYikBlogTool::on_itemDelete_clicked()
{
    QList<QTreeWidgetItem*> items = ui->itemTree->selectedItems();
    if (items.empty()) {
        return;
    }
    QTreeWidgetItem* item = items.first();
    if (item == nullptr) {
        return;
    }

    QTreeWidgetItem* parent = item->parent();
    int index = 0;
    if (parent == nullptr) {
        index = ui->itemTree->indexOfTopLevelItem(item);
        ui->itemTree->takeTopLevelItem(index);
    } else {
        index = parent->indexOfChild(item);
        parent->takeChild(index);
    }
}

void SmileYikBlogTool::on_itemSave_clicked()
{
    QVector<Post*> posts;
    int count = ui->itemTree->topLevelItemCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem* item = ui->itemTree->topLevelItem(i);
        Post* post = new Post();
        post->weight = i;
        post->id = item->text(0);
        qDebug() << post->id;
        posts.push_back(post);
        savePost(item, post);
    }
    controller.reshapeAlbumItems(posts);
}

void SmileYikBlogTool::savePost(QTreeWidgetItem* items, Post* posts) {
    int count = items->childCount();
    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem* item = items->child(i);
        Post* post = new Post();
        post->weight = i;
        post->id = item->text(0);
        qDebug() << post->id << " " << post->weight;
        savePost(item, post);
        posts->addItem(*post);
    }
}

void SmileYikBlogTool::on_albumAddButon_clicked()
{
    Album album;
    album.id = ui->albumIdEdit->text();
    album.title = ui->albumTitleEdit->text();
    album.author = ui->albumAuthorEdit->text();
    album.prev = ui->albumPrevEdit->toPlainText();
    if (!controller.addAlbum(album)) {
        QMessageBox::warning(NULL, "warning", "加入失败!");
        return;
    }
    ui->albumIdEdit->clear();
    ui->albumTitleEdit->clear();
    ui->albumAuthorEdit->clear();
    ui->albumPrevEdit->clear();
    ui->albumListWeight->clear();
    const QVector<QString> albumIds = controller.getAlbumIds();
    for (auto begin = albumIds.cbegin(), end = albumIds.cend(); begin != end; ++begin) {
        ui->albumListWeight->addItem(*begin);
    }
}


void SmileYikBlogTool::on_postRefreshButton_clicked()
{
    controller.searchMarkdowns();
    const QVector<QString> markdowns = controller.getMarkdowns();
    ui->postListWidget->clear();
    for (auto begin = markdowns.begin(), end = markdowns.end(); begin != end; ++begin) {
        ui->postListWidget->addItem(*begin);
    }
}


void SmileYikBlogTool::on_action_triggered()
{
    controller.saveAll();
}


void SmileYikBlogTool::on_postAddButton_clicked()
{
    QList<QListWidgetItem*> items = ui->postListWidget->selectedItems();
    if (items.empty()) {
        QMessageBox::warning(NULL, "添加失败", "没有选择一个markdown文件!");
        return;
    }
    QListWidgetItem* item = items.first();
    if (item == nullptr) {
        QMessageBox::warning(NULL, "添加失败", "没有选择一个markdown文件!");
        return;
    }
    Post post;
    post.markdown = item->text();
    post.id = ui->postIdEdit->text();
    post.title = ui->postTitleEdit->text();
    post.author = ui->postAuthorEdit->text();
    post.setTags(ui->postTagEdit->text());
    int code = controller.addPost(post);
    switch (code) {
    case -1:
        QMessageBox::warning(NULL, "添加失败", "没有选择一个文章集.");
        break;
    case -2:
        QMessageBox::warning(NULL, "添加失败", "id已经存在.");
        break;
    case -3:
        QMessageBox::warning(NULL, "添加失败", "id无效.");
        break;
    case -4:
        QMessageBox::warning(NULL, "添加失败", "标题无效.");
        break;
    case -5:
        QMessageBox::warning(NULL, "添加失败", "作者无效.");
        break;
    }
    ui->postIdEdit->clear();
    ui->postTitleEdit->clear();
    ui->postAuthorEdit->clear();
    ui->postTagEdit->clear();
    on_postRefreshButton_clicked();
    on_albumListWeight_clicked(ui->albumListWeight->currentIndex());
}
