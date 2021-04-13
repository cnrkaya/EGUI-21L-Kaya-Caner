#ifndef RECIPE_H
#define RECIPE_H
#include <QtCore>

//TODO save path here
//maybe define a RecipeClass

class Recipes{

public:
    explicit Recipes(QString path);
    QJsonObject objects;

    void removeJsonObject(QString key);
    void insertJsonObject(QString recipeName,QStringList descriptions, QMap<QString,QJsonValue> *ingredients);
    QJsonObject readJsonFromFile();

    static QString jsonValueArrayToString(QJsonValue value);
    static QStringList jsonValueArrayToStringList(QJsonValue value);

private:
    QString path;

    QJsonObject createRecipeObject(QStringList descriptions,QMap<QString,QJsonValue> *ingredients);

    void writeJsonDocToFile(QJsonDocument jsonDoc);


};




#endif // RECIPE_H
