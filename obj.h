
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <map>  
#include<limits>  
#include <stdio.h>
#include <stdlib.h>
using namespace std;

class OBJ_Vector4d
{
public:
	double x;
	double y;
	double z;
	double n;
	OBJ_Vector4d() : x(0), y(0), z(0),n(0)
	{
	}
	~OBJ_Vector4d()
	{
	}
	OBJ_Vector4d(const OBJ_Vector4d &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		n = v.n;
	}
    OBJ_Vector4d(glm::vec3 v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		n = 0;
	}

	OBJ_Vector4d operator+(const OBJ_Vector4d &v)
	{
		OBJ_Vector4d t;
		t.x = x + v.x;
		t.y = y + v.y;
		t.z = z + v.z;
		t.n = n + v.n;
		return t;
	}
    OBJ_Vector4d operator*(double v)
	{		
        OBJ_Vector4d t;
        t.x =x*v;
		t.y =y*v;
        t.z =z*v;
        t.n =n*v;
		return t;
	}
	OBJ_Vector4d(double x1, double y1, double z1, double n1)
	{
		x = x1;
		y = y1;
		z = z1;
		n = n1;
	}
	void set(double x1, double y1, double z1, double n1)
	{
		x = x1;
		y = y1;
		z = z1;
		n = n1;
	}
};
class Gldata{
public:
    vector<float*> vertices;
	vector<int> vertices_byte;
	vector<int> vertices_size;
    int m_all_size;

};
class Obj
{
public:
	Obj()
	{
		face_size = 3;
		m_v.clear();
		m_f.clear();

        m_pos.set(1,1,1,1);
	}
	~Obj()
	{

        for(int i = 0; i <  m_v_vertices.size(); i++)
        {
            delete[] m_v_vertices[i];
        }

	}
public:
	vector<float*> m_v_vertices;
	vector<int>  m_v_vertices_byte;
	vector<int>  m_v_vertices_size;

	vector<unsigned int>  m_VAO;
	vector<unsigned int>  m_VBO;
    vector<unsigned int> m_texture;

    vector<string> m_texture_name_string;

    vector<OBJ_Vector4d> m_v;
    vector<OBJ_Vector4d> m_vt;
	vector<OBJ_Vector4d> m_f;
    vector<OBJ_Vector4d> m_ft;

    vector<vector<OBJ_Vector4d>> m_v_muti;
    vector<vector<OBJ_Vector4d>> m_vt_muti;
	vector<vector<OBJ_Vector4d>> m_f_muti;
    vector<vector<OBJ_Vector4d>> m_ft_muti;

    map<string, string> Material_map_Kd;  
    map<string, unsigned int> Material_map_texture;  
    OBJ_Vector4d m_pos;
    
    virtual void sky_move(OBJ_Vector4d m)
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
	int face_size;
    void read_mtl(string name)
    {
        fstream f(name, ios::in);
        if (!f.is_open())
        {
            printf("read model error!\n");
            return;
        }
        string content;
        string Material__name;
        while (getline(f, content))
        {

            if(content.find("newmtl ") != string::npos)
            {
                vector<string> v_s;
                split(content, " ", v_s);
                Material__name = v_s[1].c_str();
            }
            if(content.find("map_Kd ") != string::npos)
            {
                vector<string> v_s;
                split(content, " ", v_s);
                Material_map_Kd.insert(pair<string, string>(Material__name, v_s[1].c_str()));  
            }
        }
        f.close();
    }
    void read_obj(string name)
    {
        fstream f(name, ios::in);
        if (!f.is_open())
        {
            printf("read model error!\n");
            return;
        }
        string content;
        int child_model = 0;
        vector<OBJ_Vector4d> temp_m_v;
        vector<OBJ_Vector4d> temp_m_vt;
	    vector<OBJ_Vector4d> temp_m_f;
        vector<OBJ_Vector4d> temp_m_ft;

        while (getline(f, content))
        {
            OBJ_Vector4d m_temp;
            if (content.find("v ") != string::npos)
            {
                vector<string> v_s;
                split(content, " ", v_s);
                double x = atof(v_s[1].c_str());
                double y = atof(v_s[2].c_str());
                double z = atof(v_s[3].c_str());
                m_temp.set(x,y,z,0);
                m_v.push_back(m_temp);
                temp_m_v.push_back(m_temp);
            }
            if (content.find("usemtl ") != string::npos)
            {
                vector<string> v_s;
                split(content, " ", v_s);
                map<string, string>::iterator iter = Material_map_Kd.find(v_s[1]);  
                m_texture_name_string.push_back(iter->first);
            }
            if (content.find("vt ") != string::npos)
            {
                vector<string> v_s;
                split(content, " ", v_s);
                double x = atof(v_s[1].c_str());
                double y = 1-atof(v_s[2].c_str());
                double z = atof(v_s[3].c_str());
                m_temp.set(x,y,z,0);
                m_vt.push_back(m_temp);
                temp_m_vt.push_back(m_temp);
            }
            if (content.find("f ") != string::npos)
            {
                vector<string> v_s;
                split(content, " ", v_s);
                OBJ_Vector4d m_temp;
                vector<string> v_1;
                split(v_s[1], "/", v_1);
                vector<string> v_2;
                split(v_s[2], "/", v_2);
                vector<string> v_3;
                split(v_s[3], "/", v_3);
                vector<string> v_4;
               
                double x = atof(v_1[0].c_str());
                double y = atof(v_2[0].c_str());
                double z = atof(v_3[0].c_str());
                m_temp.set(x,y,z,0);
                m_f.push_back(m_temp);
                temp_m_f.push_back(m_temp);
                x = atof(v_1[1].c_str());
                y = atof(v_2[1].c_str());
                z = atof(v_3[1].c_str());
                m_temp.set(x,y,z,0);
                m_ft.push_back(m_temp);
                temp_m_ft.push_back(m_temp);
            }
            if (content.find("usemtl ") != string::npos)
            {
                m_v_muti.push_back(temp_m_v);
                m_vt_muti.push_back(temp_m_vt);
                m_f_muti.push_back(temp_m_f);
                m_ft_muti.push_back(temp_m_ft);

                temp_m_v.clear();
                temp_m_vt.clear();
                temp_m_f.clear();
                temp_m_ft.clear();

            }
        }
        f.close();
        m_f_muti.erase(m_f_muti.begin());
        m_ft_muti.erase(m_ft_muti.begin());

        m_f_muti.push_back(temp_m_f);
        m_ft_muti.push_back(temp_m_ft);
        gen_opengl_data();
    }
    void gen_obj()
    {
        ofstream out;
        out.open("out.obj",ios::binary);
        if(!out.is_open())
        {
            printf("write error\n");
            exit(0);
        }
        string tips = "# point:"+to_string(m_v.size())+"\n";
        out.write(tips.c_str(),tips.size());
        tips =  "# face:"+to_string(m_f.size())+"\n";
        out.write(tips.c_str(),tips.size());
        for(int i = 0; i < m_v.size(); i++)
        {
            float x = m_v[i].x;
            float y = m_v[i].y;
            float z = m_v[i].z;
            string str = "v " +to_string(x) + " " + to_string(y) + " " + to_string(z) + "\n";
            out.write(str.c_str(),str.size());
        }
        for(int i = 0; i < m_f.size(); i++)
        {
            float x = m_f[i].x;
            float y = m_f[i].y;
            float z = m_f[i].z;
            string str = "f " +to_string(x) + " " + to_string(y) + " " + to_string(z) + "\n";
            out.write(str.c_str(),str.size());
        }
        out.close();
    }
    void show_data()
    {
        printf("mutl v-t:%d,%d\n", m_v_muti.size(), m_vt_muti.size());
        printf("mutl f-fv:%d,%d\n", m_f_muti.size(), m_ft_muti.size());
        printf("v-f:%d,%d\n", m_v.size(), m_f.size());
        printf("b-s:%d,%d\n", m_v_vertices_byte.size(), m_v_vertices_size.size());
        for(int i = 0; i < m_f_muti.size(); i++)
        {
            printf("f %d\n",m_f_muti[i].size());
        }
        for(int i = 0; i < m_v_vertices_byte.size(); i++)
        {
            printf("gl point size %d %d \n",m_v_vertices_byte[i],m_v_vertices_size[i]);
        }
        for(int i = 0; i <  m_f_muti.size(); i++)
        {
            //printf("text\n");
            for(int j = 0; j < m_f_muti[i].size()*27; j++)
            {
                if((j % 9) == 6 || (j % 9) == 7 || (j % 9) == 8)
                {
                    //printf("%f ",m_v_vertices[i][j]);
                }
                
            }
            //printf("\n");
        }
        for(int i = 0; i <  m_f_muti.size(); i++)
        {
            for(int j = 0; j <  m_f_muti[i].size(); j++)
            {
                int id1t = int(m_ft_muti[i][j].x)-1;
                int id2t = int(m_ft_muti[i][j].y)-1;
                int id3t = int(m_ft_muti[i][j].z)-1;

                //printf("%f %f %f\n",m_vt[id1t].x,m_vt[id1t].y,m_vt[id1t].z);
            }
        }
        map<string, string>::iterator iter;  
        for(iter = Material_map_Kd.begin(); iter != Material_map_Kd.end(); iter++)  
        {
            printf("mtl:%s %s\n",iter->first.c_str(),iter->second.c_str()); 
        }

        map<string, unsigned int>::iterator iter2;  
        for(iter2 = Material_map_texture.begin(); iter2 != Material_map_texture.end(); iter2++)  
        {
            printf("obj newmtl:%s %d\n",iter2->first,iter2->second); 
        }
    }

    void split(const string &str, const string &splits, vector<string> &res)
    {
        res.clear();
        if (str == "")
            return;

        string content = str + splits;
        int pos = content.find(splits);
        int step = splits.size();
        while ((content.find(splits) != content.npos))
        {
            int pos = content.find(splits);
            string sub = content.substr(0, pos);
            // same split
            if (sub == "")
            {
                content = content.substr(pos + step, content.size());
                continue;
            }
            res.push_back(sub);
            content = content.substr(pos + step, content.size());
        }
    }
    void Cal_Normal_3D(const OBJ_Vector4d& v1, const OBJ_Vector4d& v2, const OBJ_Vector4d& v3, OBJ_Vector4d &vn)
    {
        double na = (v2.y - v1.y)*(v3.z - v1.z) - (v2.z - v1.z)*(v3.y - v1.y);
        double nb = (v2.z - v1.z)*(v3.x - v1.x) - (v2.x - v1.x)*(v3.z - v1.z);
        double nc = (v2.x - v1.x)*(v3.y - v1.y) - (v2.y - v1.y)*(v3.x - v1.x);
        vn.set(na, nb, nc,0);
    }
    void gen_opengl_data()
    {	
        for(int i = 0; i <  m_f_muti.size(); i++)
        {
            float* vertices = new float[m_f_muti[i].size()*27];
            m_v_vertices.push_back(vertices);
            for(int j = 0; j <  m_f_muti[i].size(); j++)
            {
                int id1 = int(m_f_muti[i][j].x)-1;
                int id2 = int(m_f_muti[i][j].y)-1;
                int id3 = int(m_f_muti[i][j].z)-1;

                int id1t = int(m_ft_muti[i][j].x)-1;
                int id2t = int(m_ft_muti[i][j].y)-1;
                int id3t = int(m_ft_muti[i][j].z)-1;

                OBJ_Vector4d normal;
                Cal_Normal_3D(m_v[id1],m_v[id2],m_v[id3],normal);
                vertices[j*27+0] = m_v[id1].x;
                vertices[j*27+1] = m_v[id1].y;
                vertices[j*27+2] = m_v[id1].z;
                vertices[j*27+3] = normal.x;
                vertices[j*27+4] = normal.y;
                vertices[j*27+5] = normal.z;
                vertices[j*27+6] = m_vt[id1t].x;
                vertices[j*27+7] = m_vt[id1t].y;
                vertices[j*27+8] = m_vt[id1t].z;

                vertices[j*27+9] = m_v[id2].x;
                vertices[j*27+10] = m_v[id2].y;
                vertices[j*27+11] = m_v[id2].z;
                vertices[j*27+12] = normal.x;
                vertices[i*27+13] = normal.y;
                vertices[j*27+14] = normal.z;
                vertices[j*27+15] = m_vt[id2t].x;
                vertices[j*27+16] = m_vt[id2t].y;
                vertices[j*27+17] = m_vt[id2t].z;

                vertices[j*27+18] = m_v[id3].x;
                vertices[j*27+19] = m_v[id3].y;
                vertices[j*27+20] = m_v[id3].z;
                vertices[j*27+21] = normal.x;
                vertices[j*27+22] = normal.y;
                vertices[j*27+23] = normal.z;
                vertices[j*27+24] = m_vt[id3t].x;
                vertices[j*27+25] = m_vt[id3t].y;
                vertices[j*27+26] = m_vt[id3t].z;
            }
            int vertices_byte = m_f_muti[i].size()*27*sizeof(float);
            int vertices_size = m_f_muti[i].size()*3;
            m_v_vertices_byte.push_back(vertices_byte);
            m_v_vertices_size.push_back(vertices_size);
        } 
    }
    
};
