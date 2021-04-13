#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "recipedialog.h"
#include "recipe.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->setWindowTitle("Recipe Planner");

    initializeRecipeTableView();

    initializeNeedListView();

    //initializeCookingList
    cookingListModel =  new QStringListModel(this);
    ui->cookingListView->setModel(cookingListModel);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeRecipeTableView(){

    //initialize model
    recipesModel  = new QStandardItemModel(0,2,this);
    recipesModel->setHeaderData(0,Qt::Horizontal,QStringLiteral("Name"));
    recipesModel->setHeaderData(1,Qt::Horizontal,QStringLiteral("Description"));

    //initializeRecipeTableView
    QHeaderView *verticalHeader = ui->recipiesTableView->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(verticalHeader->defaultSectionSize()*4);

    ui->recipiesTableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->recipiesTableView->horizontalHeader()->setStretchLastSection(true);
    ui->recipiesTableView->setModel(recipesModel);
}

void MainWindow::initializeNeedListView(){

    needListmodel  = new QStandardItemModel(0,3,this);
    needListmodel->setHeaderData(0,Qt::Horizontal,QStringLiteral("Ingredient"));
    needListmodel->setHeaderData(1,Qt::Horizontal,QStringLiteral("Amount"));
    needListmodel->setHeaderData(2,Qt::Horizontal,QStringLiteral("Unit"));

    ui->needsTableView->horizontalHeader()->setStretchLastSection(true);
    ui->needsTableView->setModel(needListmodel);

}

void MainWindow::setRecipeTableView(){

    QStringList recipe_names = recipes->objects.keys();
    recipesModel->setRowCount(recipe_names.size());
    for( int row= 0; row < recipe_names.size() ; row++)
    {
        for( int col=0; col < 2; col ++)
        {
            QModelIndex index = recipesModel->index(row,col,QModelIndex());
            if(col == 0){
                recipesModel->setData(index,recipe_names[row]);
            }else{
                // get description array
                QJsonValue description =  recipes->objects.value(recipe_names[row])["recipe"];
                QString desc_string = Recipes::jsonValueArrayToString(description);
                recipesModel->setData(index,desc_string);
            }
        }
    }
}



void MainWindow::updateNeedTableView(QMap<QString,float> * ingredients_map,QStandardItemModel * needListmodel  ){

    //allocate new rows
    int newRowsCount = ingredients_map->size() - needListmodel->rowCount();
    if(newRowsCount < 0 )
        needListmodel->removeRows(0,-newRowsCount);
    else
        needListmodel->insertRows(0,newRowsCount);

    //update rows
    int row = 0;
    for(QString & key : ingredients_map->keys()){

        float value = ingredients_map->value(key);

        //column1   ingredient
        QModelIndex index = needListmodel->index(row,0,QModelIndex());
        needListmodel->setData(index,key.split("_")[0]);

        //column2     amount
        index = needListmodel->index(row,1,QModelIndex());
        needListmodel->setData(index,value);

        //column3     unit
        index = needListmodel->index(row,2,QModelIndex());
        needListmodel->setData(index,key.split("_")[1]);
        row ++;
    }


}

void MainWindow::on_editRecipeButton_clicked()
{

    QModelIndexList indexes = ui->recipiesTableView->selectionModel()->selectedRows();
    if(indexes.size()>1){
        qDebug() << "Multi index selection is not allowed on edit mode";
        return;
    }

    if(indexes.size()<=0){
       qDebug() << "Choose an item";
       return;
    }

    //open recipe dialog with edit mode
    QString recipeToEdit = recipesModel->data(indexes[0]).toString();
    Dialog *dlg = new Dialog(this,recipeToEdit,recipes);
    int result = dlg->exec();

    if(result==QDialog::Accepted)
        setRecipeTableView();
}

void MainWindow::on_addButton_clicked()
{
    if(recipes == nullptr)
        return;

    //open recipe dialog with NewRecipe mode
    Dialog *dlg = new Dialog(this,"NewRecipe",recipes);
    int result = dlg->exec();
    if(result==QDialog::Accepted)
        setRecipeTableView();

}
void MainWindow::on_deleteButton_clicked()
{
    //delete selected rows
    QModelIndexList indexes = ui->recipiesTableView->selectionModel()->selectedRows();
    for (int i = 0; i < indexes.count(); ++i)
    {
        QModelIndex index = indexes.at(i);
        QString recipteToDelete = recipesModel->data(index).toString();
        recipes->removeJsonObject(recipteToDelete);
        recipesModel->removeRows(index.row(),1);

    }
}


void  MainWindow::calculateNeedList(QString recipeToCook){

    QStringList ingredients = recipes->objects.value(recipeToCook).toObject().keys();
    for(int i = 0; i< ingredients.size(); i++){
        if(ingredients[i] != "recipe" ){
            QStringList amount_and_unit = recipes->objects.value(recipeToCook)[ingredients[i]].toString().split(" ");

            float value = amount_and_unit[0].toFloat();
            amount_and_unit.removeFirst();
            QString key = ingredients[i] ;
            key += "_" + amount_and_unit.join(" ");

            if(ingredients_map.contains(key)){
                //add amount of units
                ingredients_map.insert(key,ingredients_map.value(key) + value);


            }else{
                ingredients_map.insert(key,value);

            }
        }
    }
    updateNeedTableView(&ingredients_map, needListmodel);
    ui->needsTableView->sortByColumn(0,Qt::AscendingOrder);
}


void MainWindow::on_cookButton_clicked()
{


    // Get all selections
    QModelIndexList indexes = ui->recipiesTableView->selectionModel()->selectedRows();
    if(indexes.size()<=0)
        return;

    QStringList recipe_names = recipes->objects.keys();
    for (int i = 0; i < indexes.count(); i++)
    {
        QModelIndex index = indexes.at(i);

        cookingList.append(recipesModel->data(index).toString());


        QString recipeToCook = recipe_names[index.row()];


        calculateNeedList(recipeToCook);

    }

    cookingListModel->setStringList(cookingList);

}

void MainWindow::on_removeCookingListBtn_clicked()
{
    QModelIndex index= ui->cookingListView->selectionModel()->currentIndex();
    QString recipteToDelete  = cookingListModel->data(index).toString();
    qDebug() << recipteToDelete;
    cookingListModel->removeRows(index.row(),1);
    cookingList.removeAt(index.row());
    qDebug() << cookingList;


    QStringList ingredients = recipes->objects.value(recipteToDelete).toObject().keys();  //hashmapin keyi ingredients
    for(int i = 0; i< ingredients.size(); i++){
        if(ingredients[i] != "recipe" ){
            QStringList amount_and_unit = recipes->objects.value(recipteToDelete)[ingredients[i]].toString().split(" ");

            float value = amount_and_unit[0].toFloat();
            QString key = ingredients[i];
            amount_and_unit.removeFirst();
            key += "_" +amount_and_unit.join(" ");
            if(ingredients_map.contains(key)){
                float currentAmount = ingredients_map.value(key) -value;
                if(currentAmount <= 0)
                    ingredients_map.remove(key);
                else
                    ingredients_map.insert(key,currentAmount);
            }
        }
    }
    qDebug()<< ingredients_map;
    updateNeedTableView(&ingredients_map, needListmodel);

}



void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Browse File",qApp->applicationDirPath(),"*.json");
    this->recipes = new Recipes(fileName);
    this->recipes->readJsonFromFile();
    setRecipeTableView();

}

void MainWindow::on_actionCreate_triggered()
{
     QString fileName = QFileDialog::getSaveFileName(this,"Browse File",qApp->applicationDirPath(),"*.json");
     this->recipes = new Recipes(fileName);
     this->recipes->readJsonFromFile();
     setRecipeTableView();

}
