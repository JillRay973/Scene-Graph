#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>

#include <scene/node.h>

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      prog_flat(this),
      m_geomGrid(this), m_geomSquare(this, {glm::vec3(0.5f, 0.5f, 1.f),
                                            glm::vec3(-0.5f, 0.5f, 1.f),
                                            glm::vec3(-0.5f, -0.5f, 1.f),
                                            glm::vec3(0.5f, -0.5f, 1.f)}),
      m_showGrid(true),
      circle(this, 75),
      mp_selectedNode(nullptr)

{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();

    glDeleteVertexArrays(1, &vao);
    m_geomSquare.destroy();
    m_geomGrid.destroy();
    circle.destroy();

}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the scene geometry
    m_geomGrid.create();
    m_geomSquare.create();
    circle.create();

    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);


    // TODO: Call your scene graph construction function here
    this->rootNode = sceneGraphConstruction();
    emit sig_sendRootNode(rootNode.get());
}

void MyGL::resizeGL(int w, int h)
{
    glm::mat3 viewMat = glm::scale(glm::mat3(), glm::vec2(0.2, 0.2)); // Screen is -5 to 5

    // Upload the view matrix to our shader (i.e. onto the graphics card)
    prog_flat.setViewMatrix(viewMat);

    printGLErrorLog();
}

// This function is called by Qt any time your GL window is supposed to update
// For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_showGrid)
    {
        prog_flat.setModelMatrix(glm::mat3());
        prog_flat.draw(*this, m_geomGrid);
    }

    //VVV CLEAR THIS CODE WHEN YOU IMPLEMENT SCENE GRAPH TRAVERSAL VVV///////////////////

    // Shapes will be drawn on top of one another, so the last object
    // drawn will appear in front of everything else
    /*prog_flat.setModelMatrix(glm::mat3());
    m_geomSquare.setColor(glm::vec3(0,1,0));
    prog_flat.draw(*this, m_geomSquare);

    m_geomSquare.setColor(glm::vec3(1,0,0));
    prog_flat.setModelMatrix(glm::translate(glm::mat3(), glm::vec2(-1.f, 0.f)) * glm::rotate(glm::mat3(), glm::radians(-30.f)));
    prog_flat.draw(*this, m_geomSquare);

    m_geomSquare.setColor(glm::vec3(0,0,1));
    prog_flat.setModelMatrix(glm::translate(glm::mat3(), glm::vec2(1.f, 0.f)) * glm::rotate(glm::mat3(), glm::radians(30.f)));
    prog_flat.draw(*this, m_geomSquare);*/

    //^^^ CLEAR THIS CODE WHEN YOU IMPLEMENT SCENE GRAPH TRAVERSAL ^^^/////////////////

    // Here is a good spot to call your scene graph traversal function.
    // Any time you want to draw an instance of geometry, call
    // prog_flat.draw(*this, yourNonPointerGeometry);
    traverse(this->rootNode, glm::mat3());

}

void MyGL::keyPressEvent(QKeyEvent *e)
{

    switch(e->key())
    {
    case(Qt::Key_Escape):
        QApplication::quit();
        break;

    case(Qt::Key_G):
        m_showGrid = !m_showGrid;
        break;
    }
}

void MyGL::slot_setSelectedNode(QTreeWidgetItem *i) {
    mp_selectedNode = static_cast<Node*>(i);

    if (mp_selectedNode->getType() == 1) {
        emit sig_translateNodeSeleted(mp_selectedNode->getValue(0), mp_selectedNode->getValue(1));
    } else if (mp_selectedNode->getType() == 2) {
        emit sig_rotateNodeSeleted(mp_selectedNode->getValue(0));
    } else if (mp_selectedNode->getType() == 3) {
        emit sig_scaleNodeSeleted(mp_selectedNode->getValue(0), mp_selectedNode->getValue(1));
    }
}

void MyGL::slot_setTX(double x) {
    // TODO update the currently selected Node's
    // X translation value IF AND ONLY IF
    // the currently selected node can be validly
    // dynamic_cast to a TranslateNode.
    // Remember that a failed dynamic_cast
    // will return a null pointer.
    mp_selectedNode->setValue((float)x, 0);
}

void MyGL::slot_setTY(double y){
      mp_selectedNode->setValue((float)y, 1);
}
void MyGL::slot_setRangle(double deg){
       mp_selectedNode->setValue((float)deg, 0);

}
void MyGL::slot_setSX(double x){
        mp_selectedNode->setValue((float)x, 0);

}
void MyGL::slot_setSY(double y){
  mp_selectedNode->setValue((float)y, 1);
}


void MyGL::slot_addTranslateNode() {
    // TODO invoke the currently selected Node's
    // addChild function on a newly-instantiated
    // TranslateNode.
    mp_selectedNode->Node::addNewChild(mkU<TranslateNode>(0, 0, "new_translate", nullptr));
}
void MyGL::slot_addRotate(){
        mp_selectedNode->Node::addNewChild(mkU<RotateNode>(0, "new_rotate", nullptr));
}
void MyGL::slot_addScale(){
        mp_selectedNode->Node::addNewChild(mkU<ScaleNode>(1, 1, "new_scale", nullptr));

}
void MyGL::slot_addSquare(){
        mp_selectedNode->set2DPolygon(&m_geomSquare);
}

uPtr<Node> MyGL::sceneGraphConstruction(){

        //body
     uPtr<Node> rootNode = mkU<TranslateNode>(0, 0, "root", nullptr);
        Node &body_translate = rootNode->addNewChild(mkU<TranslateNode>(0,0,"body-translate", nullptr));
     Node &body_rotate = body_translate.addNewChild(mkU<RotateNode>(0, "body_rotate", nullptr));
        Node &body_size = body_rotate.addNewChild(mkU<ScaleNode>(1,2, "body_size", &m_geomSquare));
     body_size.setColor(glm::vec3(100, 0, 100));


        //limb LEFT forearm
     Node &limb_trans = body_size.addNewChild(mkU<TranslateNode>(-0.8, 0.2,"limb_forearm_root", nullptr));
     Node &limb_rotate = limb_trans.addNewChild(mkU<RotateNode>(-120, "limb_forearm_rotate", nullptr));
     Node &limb_size = limb_rotate.addNewChild(mkU<ScaleNode>(0.6, 0.3, "limb_forearm_size", &m_geomSquare));
     limb_size.setColor(glm::vec3(100, 0, 100));

     //limb LEFT front wrist
     Node &leftwrist_trans = limb_size.addNewChild(mkU<TranslateNode>(0.8, 0, "leftwrist_root", nullptr));
     Node &leftwrist_rotate = leftwrist_trans.addNewChild(mkU<RotateNode>(180, "leftwrist_rotate", nullptr));
     Node &leftwrist_size = leftwrist_rotate.addNewChild(mkU<ScaleNode>(1, 1,"leftwrist_size", &m_geomSquare));
     leftwrist_size.setColor(glm::vec3(100, 0, 100));


      //limb RIGHT forearm

     Node &limbright_trans = body_size.addNewChild(mkU<TranslateNode>(0.8, 0.2, "limbright_forearm_root", nullptr));
     Node &limbright_rotate = limbright_trans.addNewChild(mkU<RotateNode>(120, "limbright_forearm_rotate", nullptr));
     Node &limbright_size = limbright_rotate.addNewChild(mkU<ScaleNode>(0.6, 0.3, "limbright_forearm_size", &m_geomSquare));
     limbright_size.setColor(glm::vec3(100, 0, 100));


   //limb RIGHT front wrist
     Node &rightwrist_trans = limbright_size.addNewChild(mkU<TranslateNode>(-0.8, 0, "rightwrist_root", nullptr));
     Node &rightwrist_rotate = rightwrist_trans.addNewChild(mkU<RotateNode>(180, "rightwrist_rotate", nullptr));
     Node &rightwrist_size = rightwrist_rotate.addNewChild(mkU<ScaleNode>(1, 1,"rightwrist_size", &m_geomSquare));
     rightwrist_size.setColor(glm::vec3(100, 0, 100));

   //Head

     Node &head_trans = body_size.addNewChild(mkU<TranslateNode>(0.0, 0.8, "Head_root translate", nullptr));
     Node &head_size = head_trans.addNewChild(mkU<ScaleNode>(0.8, 0.4, "Head_size", &circle));
     head_size.setColor(glm::vec3(100, 0, 100));

   //Leg Left upper
     Node &legleft_trans = body_size.addNewChild(mkU<TranslateNode>(-0.3, -0.9,"legleft_root", nullptr));
     Node &legleft_rotate = legleft_trans.addNewChild(mkU<RotateNode>(0, "legleft_rotate", nullptr));
     Node &legleft_size = legleft_rotate.addNewChild(mkU<ScaleNode>(0.4, 0.7, "legleft_size", &m_geomSquare));
     legleft_size.setColor(glm::vec3(100, 0, 100));

    //Leg Right upper
     Node &legright_trans = body_size.addNewChild(mkU<TranslateNode>(0.3, -0.9,"legright_root", nullptr));
     Node &legright_rotate = legright_trans.addNewChild(mkU<RotateNode>(0, "legright_rotate", nullptr));
     Node &legright_size = legright_rotate.addNewChild(mkU<ScaleNode>(0.4, 0.7, "legright_size", &m_geomSquare));
     legright_size.setColor(glm::vec3(100, 0, 100));

     //Leg Left calf
     Node &legleftcalf_trans = legleft_size.addNewChild(mkU<TranslateNode>(0.0, -1,"legleftcalf_root", nullptr));
     Node &legleftcalf_rotate = legleftcalf_trans.addNewChild(mkU<RotateNode>(0, "legleftcalf_rotate", nullptr));
     Node &legleftcalf_size = legleftcalf_rotate.addNewChild(mkU<ScaleNode>(1, 1, "legleftcalf_size", &m_geomSquare));
     legleftcalf_size.setColor(glm::vec3(100, 0, 100));

     //Leg Right calf
     Node &legrightcalf_trans = legright_size.addNewChild(mkU<TranslateNode>(0.0, -1,"legrightcalf_root", nullptr));
     Node &legrightcalf_rotate = legrightcalf_trans.addNewChild(mkU<RotateNode>(0, "legrightcalf_rotate", nullptr));
     Node &legrightcalf_size = legrightcalf_rotate.addNewChild(mkU<ScaleNode>(1, 1, "legrightcalf_size", &m_geomSquare));
     legrightcalf_size.setColor(glm::vec3(100, 0, 100));



     return rootNode;
}


void MyGL::traverse(const uPtr<Node> &node, glm::mat3 matrix) {
        matrix = matrix * node->transform();

        for (const uPtr<Node> &n : node->getChildren()) {
        traverse(n, matrix);
        }

        if (node->returnPolygon()) {
        node->returnPolygon()->setColor(node->returnColor());
        prog_flat.setModelMatrix(matrix);
        prog_flat.draw(*this, *node->returnPolygon());
        }
}
