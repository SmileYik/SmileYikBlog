#ifndef SMILEYIKBLOGTOOL_H
#define SMILEYIKBLOGTOOL_H

#include <QMainWindow>
#include "controller/mainwindowcontroller.h"
#include <QTreeWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class SmileYikBlogTool; }
QT_END_NAMESPACE

class SmileYikBlogTool : public QMainWindow
{
    Q_OBJECT

public:
    SmileYikBlogTool(QWidget *parent = nullptr);
    ~SmileYikBlogTool();

private slots:
    void on_blogListWeight_clicked(const QModelIndex &index);

    void on_albumListWeight_clicked(const QModelIndex &index);

    void on_itemUp_clicked();

    void on_itemDown_clicked();

    void on_itemSave_clicked();

    void on_albumAddButon_clicked();

    void on_postRefreshButton_clicked();

    void on_action_triggered();

    void on_postAddButton_clicked();

    void on_itemDelete_clicked();

    void on_albumDeleteButon_clicked();

private:
    Ui::SmileYikBlogTool *ui;
    MainWindowController controller;

    void searchPost(QTreeWidgetItem* item, Post* post);
    void savePost(QTreeWidgetItem* items, Post* post);
};
#endif // SMILEYIKBLOGTOOL_H
