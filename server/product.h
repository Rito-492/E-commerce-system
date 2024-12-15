#ifndef PRODUCT_H
#define PRODUCT_H

#include <QDateTime>
#include <QString>

class Product
{
public:
    Product();
    Product(int productId, QString productName, int productPrice, int productNum, int productBuyNum, QString productImage, float productDiscount);
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
