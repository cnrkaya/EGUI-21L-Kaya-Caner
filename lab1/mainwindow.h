#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QFileDialog>
#include <recipe.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:


    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_editRecipeButton_clicked();

    void on_addButton_clicked();

    void on_deleteButton_clicked();

    void on_cookButton_clicked();

    void on_removeCookingListBtn_clicked();

    void on_actionOpen_triggered();

    void on_actionCreate_triggered();

private:
    Ui::MainWindow *ui;
    Recipes * recipes = nullptr ;
    QMap<QString, float> ingredients_map;
    QStringList cookingList;
    QStandardItemModel * recipesModel;
    QStandardItemModel * needListmodel;
    QStringListModel * cookingListModel;

    void setRecipeTableView();

    void initializeRecipeTableView();

    void initializeNeedListView();

    void updateNeedTableView(QMap<QString,float> * ingredients_map,QStandardItemModel * needListmodel);

    void calculateNeedList(QString recipeToCook);


};
#endif // MAINWINDOW_H
