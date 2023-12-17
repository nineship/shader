#pragma once
#include "obj.h"

class Ball:public Obj
{
public:
    Ball()
    {
        frame = 0;
        m_direction.set(0.1,0,0,0);
    }
    ~Ball()
    {

    }
    double m_init_speed;
    OBJ_Vector4d m_direction;
    int frame;
    void init_sprit(glm::vec3 pos,double Yaw,double Pitch)
    {
        frame = 0;
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front = glm::normalize(front);
        m_pos = pos+front*glm::vec3(3);
        m_direction.set(front.x,front.y,front.z,0);
    }
    void sprit()
    {
        frame += 1;
        if(frame >= 1000)
        {
            return;
        }
        m_pos = m_pos + m_direction*20;
    }
};