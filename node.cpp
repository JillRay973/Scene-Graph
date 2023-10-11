#include "scene/node.h"
//node class
Node::Node() : QTreeWidgetItem(), nodeName(), geom(nullptr), rgbcolor(), ptrchildren() {
    this->QTreeWidgetItem::setText(0, this->nodeName);
}

Node::Node(const QString &name, Polygon2D *p, int type)
    : ptrchildren(std::vector<uPtr<Node>>()), geom(p), rgbcolor(glm::vec3()), nodeName(name), type(type)
{
    this->setText(0, name);
}


Node::Node(const Node &node)  : QTreeWidgetItem(), nodeName(node.nodeName), geom(node.geom), rgbcolor(node.rgbcolor) {
    for (const uPtr<Node> &ptrchild : node.ptrchildren){
        TranslateNode *translate = dynamic_cast<TranslateNode*>(ptrchild.get());
        RotateNode *rotate = dynamic_cast<RotateNode*>(ptrchild.get());
         ScaleNode *scale = dynamic_cast<ScaleNode*>(ptrchild.get());

        if((translate != nullptr)){
             ptrchildren.push_back(mkU<TranslateNode>(*translate));
         }

        if((rotate != nullptr)){
             ptrchildren.push_back(mkU<RotateNode>(*rotate));

         }
        if((scale != nullptr)){
             ptrchildren.push_back(mkU<ScaleNode>(*scale));

        }

    }
    this->QTreeWidgetItem::setText(0, this->nodeName);
}

Polygon2D* Node::returnPolygon() {
    return this->geom;
}

const QString Node::returnNameofString(){
    return this->nodeName;

}

const glm::vec3 Node::returnColor(){
    return this->rgbcolor;

}
const std::vector<uPtr<Node>> &Node::getChildren() {
    return this->ptrchildren;
}

void Node:: set2DPolygon(Polygon2D *geom){
    this->geom = geom;
}

void Node:: setColor(glm::vec3 rgbcolor){
    this->rgbcolor[0] = rgbcolor[0];
    this->rgbcolor[1] = rgbcolor[1];
    this->rgbcolor[2] = rgbcolor[2];
}

Node &Node::addNewChild(uPtr<Node> node){
    this->addChild(node.get());

    Node &ref = *node;
    this->ptrchildren.push_back(std::move(node));
    return ref;
}

Node:: ~Node(){

}
int Node::getType() {
    return this->type;
}

//Translate Node class

TranslateNode::TranslateNode(float x, float y, const QString &nodeName, Polygon2D *p)
    : x(x), y(y), Node(nodeName, p, 1)
{}

float TranslateNode::returnXvalue(){
    glm::mat3 translate = transform();
    return translate[2][0];

}

void TranslateNode::setValue(float v, int index) {
    if (index == 0) {
        this->x = v;
    } else if (index == 1) {
        this->y = v;
    }
}


float TranslateNode::returnYvalue(){
    glm::mat3 translate = transform();
    return translate[2][1];

}
float TranslateNode::getValue(int index) {
    if (index == 0) {
        return this-> x;
    } else if (index == 1) {
        return this->y;
    }
    return 0;
}

void TranslateNode::translate(float x, float y){
    this->x = x;
    this->y = y;

}

glm::mat3 TranslateNode::transform(){
     return glm::translate(glm::mat3(), glm::vec2(this->x, this->y));
}

TranslateNode::~TranslateNode(){

}

//Rotate Node


RotateNode::RotateNode( float degree, const QString &nodeName, Polygon2D *p)
    : Node(nodeName, p, 2), degree(degree)
{}


glm::mat3 RotateNode::transform(){
   return glm::rotate(glm::mat3(), glm::radians(this->degree));
}

void RotateNode::rotate(float degree){
    this->degree = degree;
}
void RotateNode::setValue(float v, int index) {
    if (index == 0) {
        this->degree = v;
    }
}
float RotateNode::getValue(int index) {
    if (index == 0) {
        return this->degree;
    }
    return 0;
}

RotateNode::~RotateNode(){

}

//Scale node class

ScaleNode::ScaleNode(float scalex, float scaley, const QString &nodeName, Polygon2D *p)
    :  scalex(scalex), scaley(scaley),Node(nodeName, p, 3)
{}

float ScaleNode::returnscaleX(){
    glm::mat3 scale = transform();
    return scale[0][0];
}

float ScaleNode::returnscaleY(){
    glm::mat3 scale = transform();
    return scale[1][1];
}

void ScaleNode::scale(float scalex, float scaley){
    this->scalex = scalex;
    this->scaley = scaley;
}

glm::mat3 ScaleNode::transform(){
     return glm::scale(glm::mat3(), glm::vec2(this->scalex, this->scaley));
}
void ScaleNode::setValue(float v, int index) {
    if (index == 0) {
        this->scalex = v;
    } else if (index == 1) {
        this->scaley = v;
    }
}

float ScaleNode::getValue(int index) {
    if (index == 0) {
        return this->scalex;
    } else if (index == 1) {
        return this->scaley;
    }
    return 0;
}
ScaleNode::~ScaleNode(){}
