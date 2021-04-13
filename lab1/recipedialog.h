#ifndef RECIPEDIALOG_H
#define RECIPEDIALOG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <recipe.h>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr, QString mode = "NewRecipe", Recipes * recipes=nullptr);
    ~Dialog();


private slots:
    void on_addDescriptionButton_clicked();

    void on_editDescriptionButton_clicked();

    void on_deleteDescriptionButton_clicked();

    void on_addIngredientButton_clicked();

    void on_editIngredientButton_clicked();

    void on_deleteIngredientButton_clicked();

    void on_buttonBox_accepted();

    void onDataChanged(const QModelIndex);

private:
    Ui::Dialog *ui;
    QStringListModel * descriptionModel;
    QStandardItemModel * ingredientsModel;
    QString mode;
    Recipes * recipes;

    void setIngredientsTable(QJsonObject ingredienstObject);

};

#endif // RECIPEDIALOG_H
