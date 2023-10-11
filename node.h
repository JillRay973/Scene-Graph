#pragma once
#include <QTreeWidgetItem>
#include <QString>
#include <vector>
#include "smartpointerhelp.h"
#include "drawable.h"
#include "scene/polygon.h"


class Node : public QTreeWidgetItem {
private:
    Polygon2D* geom;
    glm::vec3 rgbcolor;
    int type;
    QString nodeName;

     std::vector<uPtr<Node>> ptrchildren;

public:



    //in every class you have to define:
    //constructors

    Node();
    Node(QString nodeName);
    Node(const Node &node); //copy
    Node &addNewChild(uPtr<Node> node);
    Node(const QString &nodeName, Polygon2D *p, int type = 0);
    //getters for each public value
    Polygon2D* returnPolygon();
    const QString returnNameofString();
    const glm::vec3 returnColor();
     const std::vector<uPtr<Node>> &getChildren();
    //setters
    void set2DPolygon(Polygon2D *geom);
    void setColor(glm::vec3 rgbcolor);
    virtual void setValue(float v, int index) = 0;
      int getType();
    virtual float getValue(int index) = 0;
    //and then your primary functions
     virtual glm::mat3 transform() = 0;



    virtual ~Node();
};


class TranslateNode : public Node {
 private:
     float x;
     float y;
 public:
    TranslateNode(float x, float y, const QString &nodeName, Polygon2D *p);

     float returnXvalue();
     float returnYvalue();
      float getValue(int index);
     void translate(float x, float y);
     glm::mat3 transform();
    void setValue(float v, int index);
     virtual ~TranslateNode();
};

class RotateNode : public Node {
 private:
     float degree;

 public:

       RotateNode( float degree, const QString &nodeName, Polygon2D *p);

     void setValue(float v, int index);
     float getValue(int index);
     float returnDegree();
     glm::mat3 transform();
     void rotate(float degree);
     virtual ~RotateNode();


};

class ScaleNode : public Node {
 private:
     float scalex;
     float scaley;

 public:
       ScaleNode(float scalex, float scaley, const QString &nodeName, Polygon2D *p);

     float returnscaleX();
     float returnscaleY();
     void setValue(float v, int index);
     float getValue(int index);
     void scale(float scalex, float scaley);
     glm::mat3 transform();
     virtual ~ScaleNode();
};
