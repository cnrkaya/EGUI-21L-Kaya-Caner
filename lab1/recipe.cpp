#include "recipe.h"


Recipes::Recipes(QString path){
    this->path = path;
}


QJsonObject Recipes::readJsonFromFile()
{

    QFile file(path);
    QString val;

    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug() << "File not exists.";
    }else{
         qDebug() << "File opened succesfully.";
    }
    val = file.readAll();
    file.close();

    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject  object = d.object();

    this->objects = object;

    return object;
}


QJsonObject Recipes::createRecipeObject(QStringList descriptions,QMap<QString,QJsonValue> *ingredients)
{
    QJsonObject recipe;
    QJsonArray description ;
    description = description.fromStringList(descriptions);
    recipe.insert("recipe",description);

    for(QString & key : ingredients->keys()){
        recipe.insert(key,ingredients->value(key));
    }

    return recipe;
}

void Recipes::insertJsonObject(QString recipeName,QStringList descriptions, QMap<QString,QJsonValue> *ingredients){

    QJsonObject object = createRecipeObject(descriptions,ingredients);

    QJsonDocument jsonDoc;
    this->objects.insert(recipeName,object);
    jsonDoc.setObject(this->objects);
    writeJsonDocToFile(jsonDoc);
}

void Recipes::writeJsonDocToFile(QJsonDocument jsonDoc){

    QFile file(this->path);
    if(!file.open(QIODevice::ReadWrite)) {
        qDebug() << "File open error";
    } else {
        qDebug() <<"File open!";
    }

    file.resize(0);

    file.write(jsonDoc.toJson());
    file.close();

    qDebug() << "Write to file";
}

QString Recipes::jsonValueArrayToString(QJsonValue value){
    QString str;
    int i = 0;
    while( value[i] != QJsonValue::Undefined){
        str.append(value[i].toString()+"\n");
        i++;
    }
    return str;
}

QStringList Recipes::jsonValueArrayToStringList(QJsonValue value){
    QStringList list;
    int i = 0;
    while( value[i] != QJsonValue::Undefined){
        list << value[i].toString();
        i++;
    }
    return list;
}

void Recipes::removeJsonObject(QString key){

    this->objects.remove(key);
    QJsonDocument jsonDoc;
    jsonDoc.setObject(this->objects);
    writeJsonDocToFile(jsonDoc);

}

