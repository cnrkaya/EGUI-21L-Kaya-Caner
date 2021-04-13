#include "recipedialog.h"
#include "ui_dialog.h"
#include "recipe.h"


Dialog::Dialog(QWidget *parent, QString mode, Recipes * recipes) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->mode = mode;

    this->recipes = recipes;

    descriptionModel = new QStringListModel();
    ingredientsModel = new QStandardItemModel(0,3,this);
    ingredientsModel->setHeaderData(0,Qt::Horizontal,QStringLiteral("Ingredient"));
    ingredientsModel->setHeaderData(1,Qt::Horizontal,QStringLiteral("Amount"));
    ingredientsModel->setHeaderData(2,Qt::Horizontal,QStringLiteral("Unit"));

    QStringList descriptionList;
    if(mode == "NewRecipe"){
        this->setWindowTitle("Add New Recipe");

    }else{
        this->setWindowTitle("Edit the Recipe");

        //get recipe will be edited
        ui->recipeNameLineEdit->setText(mode);

        // get description
        QJsonValue description = recipes->objects.value(mode)["recipe"];
        QStringList descriptionList = Recipes::jsonValueArrayToStringList(description);
        descriptionModel->setStringList(descriptionList);

        //get ingredients
        QJsonObject ingredienstObject = recipes->objects.value(mode).toObject();
        setIngredientsTable(ingredienstObject);

    }


    ui->descriptionsListView->setModel(descriptionModel);
    ui->ingredientsTableView->setModel(ingredientsModel);
    ui->ingredientsTableView->horizontalHeader()->setStretchLastSection(true);
    //QObject::connect(ingredientsModel &QAbstractItemModel::dataChanged(QModelIndex index ),this, myFunct());
    connect(ingredientsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this,  SLOT(onDataChanged(QModelIndex)));

}

void Dialog::onDataChanged(const QModelIndex index){

    if(index.column() == 1){
        QVariant data = ingredientsModel->data(index);
        bool res;
        float num = data.toFloat(&res);
        if(res == false){
           qDebug() << "Amount must be float ";
           ingredientsModel->setData(index,0.0);
        }

    }

}


Dialog::~Dialog()
{
    delete ui;
}

void Dialog::setIngredientsTable(QJsonObject ingredienstObject){

    for(QString & key : ingredienstObject.keys())
    {
        if(key != "recipe")
        {
            QStringList amount_and_unit = ingredienstObject.value(key).toString().split(" ");
            qDebug() << amount_and_unit;
            int row = ingredientsModel->rowCount();
            ingredientsModel->insertRow(row);
            //column1   ingredient
            QModelIndex index = ingredientsModel->index(row,0,QModelIndex());
            ingredientsModel->setData(index,key);
            //column2     amount
            index = ingredientsModel->index(row,1,QModelIndex());
            ingredientsModel->setData(index,amount_and_unit[0]);
            //column3     unit
            index = ingredientsModel->index(row,2,QModelIndex());
            amount_and_unit.removeFirst();
            ingredientsModel->setData(index,amount_and_unit.join(" "));
        }
    }
}

void Dialog::on_addDescriptionButton_clicked()
{
    int row  =descriptionModel->rowCount();
    descriptionModel->insertRows(row,1);

    QModelIndex index = descriptionModel->index(row);
    ui->descriptionsListView->setCurrentIndex(index);
    ui->descriptionsListView->edit(index);
}

void Dialog::on_editDescriptionButton_clicked()
{
    QModelIndex index = ui->descriptionsListView->currentIndex();
    ui->descriptionsListView->edit(index);
}

void Dialog::on_deleteDescriptionButton_clicked()
{
    int row = ui->descriptionsListView->currentIndex().row();
    descriptionModel->removeRows(row,1,QModelIndex());
}

void Dialog::on_addIngredientButton_clicked()
{
    int row  =ingredientsModel->rowCount();
    ingredientsModel->insertRows(row,1);

    QModelIndex index = ingredientsModel->index(row,0);
    ui->ingredientsTableView->setCurrentIndex(index);
    ui->ingredientsTableView->edit(index);
}

void Dialog::on_editIngredientButton_clicked()
{
    QModelIndex index = ui->ingredientsTableView->currentIndex();
    ui->ingredientsTableView->edit(index);
}

void Dialog::on_deleteIngredientButton_clicked()
{
    int row = ui->ingredientsTableView->currentIndex().row();
    ingredientsModel->removeRows(row,1,QModelIndex());
}

void Dialog::on_buttonBox_accepted()
{

    QString recipeName = ui->recipeNameLineEdit->text();
    QStringList descriptions = descriptionModel->stringList();
    qDebug() << descriptions;

    QMap<QString,QJsonValue> map;
    for(int row = 0; row < ingredientsModel->rowCount(); row++ ){

        QModelIndex index = ingredientsModel->index(row,0,QModelIndex());
        QString key = ingredientsModel->data(index).toString();
        index = ingredientsModel->index(row,1,QModelIndex());
        QString amount = ingredientsModel->data(index).toString();
        index = ingredientsModel->index(row,2,QModelIndex());
        QString unit = ingredientsModel->data(index).toString();
        QJsonValue value =  amount + " " + unit;
        map.insert(key,value);

    }

    if(this->mode != "NewRecipe" && mode != recipeName){
        //if edited recipeName is changed then delete old recipe from json file
        recipes->removeJsonObject(mode);
        qDebug() << "Json object deleted";
    }


    recipes->insertJsonObject(recipeName,descriptions,&map);

}
