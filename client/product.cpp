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

// 静态成员函数，将 Product 对象转换为 QJsonObject
QJsonObject Product::toJsonObject(const Product& product) {
    QJsonObject json;
    json["product_id"] = product.productId;
    json["product_name"] = product.productName;
    json["product_price"] = product.productPrice;
    json["product_num"] = product.productNum;
    json["product_buy_num"] = product.productBuyNum;
    json["product_image"] = product.productImage;
    json["product_discount"] = product.productDiscount;
    return json;
}

// 静态成员函数：从QJsonObject转换为Product对象
Product Product::fromJsonObject(const QJsonObject& json) {
    Product product;
    product.productId = json["product_id"].toInt();
    product.productName = json["product_name"].toString();
    product.productPrice = json["product_price"].toInt();
    product.productNum = json["product_num"].toInt();
    product.productBuyNum = json["product_buy_num"].toInt();
    product.productImage = json["product_image"].toString();
    product.productDiscount = json["product_discount"].toDouble();
    return product;
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