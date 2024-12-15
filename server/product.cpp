#include "product.h"

Product::Product():productId(0),productPrice(0),productNum(0),productBuyNum(0),productDiscount(0)
{

}

Product::Product(int productId, QString productName, int productPrice, int productNum, int productBuyNum, QString productImage, float productDiscount)
{
    this->productId = productId;
    this->productName = productName;
    this->productPrice = productPrice;
    this->productNum = productNum;
    this->productBuyNum = productBuyNum;
    this->productImage = productImage;
    this->productDiscount = productDiscount;
}

float Product::getProductDiscount() const
{
    return productDiscount;
}

void Product::setProductDiscount(float value)
{
    productDiscount = value;
}

QString Product::getProductImage() const
{
    return productImage;
}

void Product::setProductImage(const QString &value)
{
    productImage = value;
}

int Product::getProductBuyNum() const
{
    return productBuyNum;
}

void Product::setProductBuyNum(int value)
{
    productBuyNum = value;
}

int Product::getProductNum() const
{
    return productNum;
}

void Product::setProductNum(int value)
{
    productNum = value;
}

int Product::getProductPrice() const
{
    return productPrice;
}

void Product::setProductPrice(int value)
{
    productPrice = value;
}

QString Product::getProductName() const
{
    return productName;
}

void Product::setProductName(const QString &value)
{
    productName = value;
}

int Product::getProductId() const
{
    return productId;
}

void Product::setProductId(int value)
{
    productId = value;
}
