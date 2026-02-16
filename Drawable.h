#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <array>
#include <map>
#define mat4x4 array<array<float,4>,4>
using namespace std;
GLuint texlen=0;
void out(mat4x4 m)
{
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
            cout<<m[i][j]<<" ";
        cout<<"\n";
    }
}

string getdir(string dir) {
    for(int i=dir.length()-1; i>=0; i--) {
        if(dir[i]=='/')
            return dir.substr(0,i+1);
    }
    return "";
}
void split(string s,string& l,string& r,char c)
{

    l=s;
    r="0";
    for(int i=0; i<s.length(); i++)
    {
        if(s[i]==c)
        {
            l=s.substr(0,i);
            r=s.substr(i+1,s.length()-i-1);
            break;
        }
    }

}
#include <math.h>
class point
{
public:
	float x,y,z;
	point(){}
	point(float gx,float gy,float gz) {
		x=gx;
		y=gy;
		z=gz;
	}
    float norm() {
        return sqrt(x*x+y*y+z*z);
    }
    point operator+(point b)
    {
        point res(x+b.x,y+b.y,z+b.z);
        return res;
    }
    point operator-(point b)
    {
        point res={x-b.x,y-b.y,z-b.z};
        return res;
    }
    point operator*(float b)
    {
        point res=*this;
        res.x*=b;
        res.y*=b;
        res.z*=b;
        return res;
    }
    point operator-() {
        return point(-x,-y,-z);
    }
};
class Material
{
public:
    Material(){}
    Material(string n){
    name=n;
    }
    string name;
    bool textured=0;
    GLuint tex=0;
    point color={1,1,1};
};
class Face
{
public:
    vector<int> pid,tpid;
    int mtl=0;
};
class Group {
public:
    Group(){}
    Group(string n){name=n;}
    string name;
    int first_face=-1,last_face;
    int first_point=-1,last_point;
};

mat4x4 getrotation(float x,point vec)
{
    float g=vec.norm();
    vec={vec.x/g,vec.y/g,vec.z/g};
    return {
        cos(x)+vec.x*vec.x*(1-cos(x)),vec.x*vec.y*(1-cos(x))-vec.z*sin(x),vec.x*vec.z*(1-cos(x))+vec.y*sin(x),0,
        vec.y*vec.x*(1-cos(x))+vec.z*sin(x),cos(x)+vec.y*vec.y*(1-cos(x)),vec.y*vec.z*(1-cos(x))-vec.x*sin(x),0,
        vec.z*vec.x*(1-cos(x))-vec.y*sin(x),vec.z*vec.y*(1-cos(x))+vec.x*sin(x),cos(x)+vec.z*vec.z*(1-cos(x)),0,
        0,0,0,1
    };
}
mat4x4 getshift(point sh) {
    return {
        1,0,0,sh.x,
        0,1,0,sh.y,
        0,0,1,sh.z,
        0,0,0,1
    };
}
bool once=1;
point mul(mat4x4 m,point p)
{
    if(once)
    {
        out(m);
        cout<<endl<<p.x<<" "<<p.y<<" "<<p.z<<endl;
    }
    point res;
    res.x=m[0][0]*p.x+m[0][1]*p.y+m[0][2]*p.z+m[0][3];
    res.y=m[1][0]*p.x+m[1][1]*p.y+m[1][2]*p.z+m[1][3];
    res.z=m[2][0]*p.x+m[2][1]*p.y+m[2][2]*p.z+m[2][3];
    float w=m[3][0]*p.x+m[3][1]*p.y+m[3][2]*p.z+m[3][3];
    res={res.x/w,res.y/w,res.z/w};
    p=res;
    if(once)cout<<endl<<p.x<<" " <<p.y<<" "<<p.z<<endl;
    once=0;
    return res;
}
class Rotation
{
public:
    Rotation(){}
    Rotation(int a,point b,point c,float d,float e,float f,bool g,float h,float i){
    groupid=a;
    x=b;
    y=c;
    speed=d;
    t[0]=e;
    t[1]=f;
    maximini=g;
    mini=h;
    maxi=i;
    }
    int groupid=0;
    point x,y;
    float speed=1;

    float t[2]={0,0};
    bool maximini=0;
    float mini,maxi;
    float total=0;
    bool sh[2]={0,0};
};

class Drawable
{
public:

	vector<point> p;
    vector<Material> material;
    vector<pair<float,float>> tp;
	vector<Face> face;
    vector<Group> group;
    vector<Rotation> rotation;
    bool textured=0;

    map<string,int> loadMaterials(string filename) {
        cout<<"loading materials form :"+filename<<endl;
        material.push_back(Material());//default material
        map<string,int> res;
        string got;
        ifstream in(filename);
        if(!in)
        {
            cout<<"file :\""+filename+"\" not found"<<endl;
            return res;
        }
        string line,get="";
        int g;
        vector<pair<int,int>> words;

        while(getline(in,line))
        {
            line+=" ";
            for(int i=0,j=0; i+j<=line.length();)
            {
                if(line[i+j]==' ')
                {
                    if(j==i)
                    {
                        i++;
                        j=0;
                    }
                    else
                    {
                        words.push_back({i,j});

                        i+=j+1;
                        j=0;
                    }
                }
                else
                {
                    j++;
                }

            }

            if(words.size()==0)continue;
            else if(line.substr(words[0].first,words[0].second)=="newmtl")
            {
                g=material.size();
                res[line.substr(words[1].first,words[1].second)]=g;
                material.push_back(Material{line.substr(words[1].first,words[1].second)});
            }
            else if(line.substr(words[0].first,words[0].second)=="Kd"||line.substr(words[0].first,words[0].second)=="kd")
            {
                material[g].color=point(atof(line.substr(words[1].first,words[1].second).data()),
                    atof(line.substr(words[2].first,words[2].second).data()),
                    atof(line.substr(words[3].first,words[3].second).data()));
            }


            words.clear();
        }
        cout<<"materials loaded successfully!\n ";

        return res;
    }
public:
    void loadFile(string fileName,point scale=point(1,1,1),point shift=point(0,0,0)) //only one file
    {
        int usedmtl=0;
        int v=0,f=0;
        p.clear();
        tp.clear();
        face.clear();
        map<string,int> getmat;
        string dir=getdir(fileName);
        ifstream in(fileName);
        if(!in)
        {
            cout<<"file :\""+fileName+"\" not found"<<endl;
            return;
        }
        string line,get="",l,r;
        int g;
        vector<pair<int,int>> words;

        while(getline(in,line))
        {
            line+=" ";
            for(int i=0,j=0; i+j<=line.length();)
            {
                if(line[i+j]==' ') {
                    if(j==0)
                    {
                        i++;
                        j=0;
                    }
                    else
                    {
                        words.push_back({i,j});

                        i+=j+1;
                        j=0;
                    }
                }
                else
                {
                    j++;
                }

            }
            if(words.size()==0)continue;//this is important
            if(line.substr(words[0].first,words[0].second)=="v")
            {
                v++;
                if(group[group.size()-1].first_point==-1)
                {
                    group[group.size()-1].first_point=group[group.size()-1].last_point=p.size();
                }
                else
                {
                    group[group.size()-1].last_point=p.size();
                }
                p.push_back(point(atof(line.substr(words[1].first,words[1].second).data()),
                    atof(line.substr(words[2].first,words[2].second).data()),
                    atof(line.substr(words[3].first,words[3].second).data())
                    ));
            }else
            if(line.substr(words[0].first,words[0].second)=="vt")
            {
                tp.push_back({atof(line.substr(words[1].first,words[1].second).data()),
                    atof(line.substr(words[2].first,words[2].second).data())
                    });

            }
            else if(line.substr(words[0].first,words[0].second)=="f")
            {
                f++;
                if(group[group.size()-1].first_face==-1)
                {
                    group[group.size()-1].first_face=face.size();
                    group[group.size()-1].last_face=face.size();
                }
                else
                {
                    group[group.size()-1].last_face=face.size();
                }
                g=face.size();
                face.push_back({});
                face[g].mtl=usedmtl;

                for(int i=1; i<words.size(); i++)
                {
                    split(line.substr(words[i].first,words[i].second),l,r,'/');
                    face[g].pid.push_back(atoi(l.data())-1);
                    face[g].tpid.push_back(atoi(r.data())-1);
                }
            }
            else if(line.substr(words[0].first,words[0].second)=="mtllib")
            {
                getmat=loadMaterials(dir+line.substr(words[1].first,words[1].second));
            }else if(line.substr(words[0].first,words[0].second)=="o")
            {
                cout<<"loading object:"<<line.substr(words[1].first,words[1].second) <<endl;
                group.push_back(Group{line.substr(words[1].first,words[1].second)});
            }
            else if(line.substr(words[0].first,words[0].second)=="usemtl")
            {
                usedmtl=getmat[line.substr(words[1].first,words[1].second)];
            }
            words.clear();
        }

        for(int i=0; i<tp.size(); i++)
        {
            if(tp[i].first<0)tp[i].first++;
            if(tp[i].second<0)tp[i].second++;
        }
        cout<<"total of "<<v<<"vertices and "<<f<<" faces\n";

    }
    void draw() {

        float tt=Timer::current_time();
        for(Rotation r : rotation)
        {
            if(r.t[0]!=-1)
            {

                //out(getshift(-r.x));
                //out(getrotation(r.speed*(r.t[1]-r.t[0]),r.y-r.x));
                //out(getshift(r.x));
                for(int i=group[r.groupid].first_point; i<=group[r.groupid].last_point; i++)
                {
                    p[i]=mul(getshift(-r.x),p[i]);
                    p[i]=mul(getrotation(r.speed*(r.t[1]-r.t[0]),r.y-r.x),p[i]);
                    p[i]=mul(getshift(r.x),p[i]);
                }
            }
        }
        for(int i=0; i<rotation.size(); i++)
        {

            if(rotation[i].t[0]==-1)
            {
                rotation[i].t[0]=rotation[i].t[1]=tt;
            }else
            rotation[i].total+=(rotation[i].t[1]-rotation[i].t[0])* rotation[i].speed;
            rotation[i].t[0]=rotation[i].t[1];
            rotation[i].t[1]=tt;
            if(rotation[i].maximini)
            {
                if(rotation[i].total>=rotation[i].maxi&&!rotation[i].sh[0])
                {
                    rotation[i].sh[0]=1;
                    rotation[i].speed=-rotation[i].speed;
                    rotation[i].sh[1]=0;
                }else
                if(rotation[i].total<=rotation[i].mini&&!rotation[i].sh[1])
                {
                    rotation[i].sh[1]=1;
                    rotation[i].speed=-rotation[i].speed;
                    rotation[i].sh[0]=0;
                }
            }
        }
        for(Group g : group)
        {
            //cout<<g.name<<endl;


                glPushMatrix();
            for(int i=g.first_face; i!=-1&&i<=g.last_face;i++) {
                if(material[face[i].mtl].textured)
                {
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D,material[face[i].mtl].tex);

                }
                else
                {
                    glColor3f(material[face[i].mtl].color.x,material[face[i].mtl].color.y,material[face[i].mtl].color.z);
                }
                glBegin(GL_POLYGON);
                for(int j=0; j<face[i].pid.size(); j++)
                {
                    if(material[face[i].mtl].textured)
                        glTexCoord2f(tp[face[i].tpid[j]].first,tp[face[i].tpid[j]].second);
                    glVertex3f(p[face[i].pid[j]].x,p[face[i].pid[j]].y,p[face[i].pid[j]].z);
                }

                glPopMatrix();
                glEnd();

                if(material[face[i].mtl].textured)
                {
                    glDisable(GL_TEXTURE_2D);
                }
            }
        }

    }
    void rotate(string who,string aroundwho,float speedpersecond=1,bool maximini=0,float mini=-1,float maxi=-1)
    {
        int g1=-1,g2=-1;
        for(int i=0; i<group.size(); i++)
        {
            if(group[i].name==who)g1=i;
            if(group[i].name==aroundwho)g2=i;
        }
        if(g1!=-1&&g2!=-1)
        {
            rotation.push_back(Rotation(g1,p[group[g2].first_point],p[group[g2].first_point+1],speedpersecond,-1,-1,maximini,mini,maxi));
        }
        else { cout<<"can't rotate "+who+" around "+aroundwho+" some objects were misnamed"; }

    }
};
