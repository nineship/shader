#pragma once
#include "obj.h"
#include "ball.h"
class Model{
public:
    Model()
    {
        m_pos.set(1,1,1,1);

        m_model_name.push_back("models/tian.obj");
        m_mtl_name.push_back("models/tian.mtl");

        m_model_name.push_back("models/hu.obj");
        m_mtl_name.push_back("models/hu.mtl");

        m_model_name.push_back("models/monitor02.obj");
        m_mtl_name.push_back("models/monitor02.mtl");

        m_model_name.push_back("models/arm.obj");
        m_mtl_name.push_back("models/arm.mtl");

        
    }
    ~Model()
    {
        for(int i = 0; i < m_obj.size(); i++)
        {
            delete m_obj[i];
        }
    }
    vector<string> m_model_name;
    vector<string> m_mtl_name;
public:
    vector<Obj*> m_obj;
    OBJ_Vector4d m_pos;
    void read_model()
    {
        Obj * obj = new Obj;
        obj->read_mtl("box.mtl");
        obj->read_obj("box.obj");
        m_obj.push_back(obj);
        for(int i = 0; i < m_model_name.size(); i++)
        {
            obj = new Obj;
            obj->read_mtl(m_mtl_name[i].c_str());
            obj->read_obj(m_model_name[i].c_str());
            m_obj.push_back(obj);
        }
        obj = new Ball;
        obj->read_mtl("models/ball.mtl");
        obj->read_obj("models/ball.obj");
        m_obj.push_back(obj);
    }
    void sky_move(OBJ_Vector4d m)
    {
        m_pos.y += m.y;
        if(m_pos.y <= -100)
        {
            m_pos.y = 0;
        }
    }
    void move(OBJ_Vector4d m)
    {
         m_pos = m;
    }
    void show()
    {
        for(int i = 0; i < m_obj.size(); i++)
        {
            m_obj[i]->show_data();
        }
    }
};