#ifndef PRODUCT_H
#define PRODUCT_H

#include <QDateTime>
#include <QJsonArray>
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

    static QJsonObject toJsonObjectArray(const QList<Product>& orders);

    static QList<Product> fromJsonObjectArray(const QJsonObject& jsonObj);

    static bool cmpBySales(const Product &a, const Product &b) {
        // 基于销量比较
        return a.productBuyNum > b.productBuyNum;
    }

    static bool cmpByPrice(const Product &a, const Product &b) {
        // 基于价格比较
        return a.productPrice > b.productPrice;
    }

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
