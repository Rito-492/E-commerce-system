#ifndef PRODUCT_H
#define PRODUCT_H

#include <QDateTime>
#include <QJsonObject>
#include <QString>
#include <QVariantMap>

class Product
{
public:
    Product();
    Product(int productId, QString productName, int productPrice, int productNum, int productBuyNum, QString productImage, float productDiscount);

    static QJsonObject toJsonObject(const Product& product);
    // 静态成员函数：从QJsonObject转换为Product对象
    static Product fromJsonObject(const QJsonObject& json);

    float getProductDiscount() const;
    void setProductDiscount(float value);

    QString getProductImage() const;
    void setProductImage(const QString &value);

    int getProductBuyNum() const;
    void setProductBuyNum(int value);

    int getProductNum() const;
    void setProductNum(int value);

    int getProductPrice() const;
    void setProductPrice(int value);

    QString getProductName() const;
    void setProductName(const QString &value);

    int getProductId() const;
    void setProductId(int value);

private:
    int productId;
    QString productName;
    int productPrice;
    int productNum;
    int productBuyNum;
    QString productImage;
    float productDiscount;
};

#endif // PRODUCT_H
