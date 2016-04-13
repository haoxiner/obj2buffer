//
//  main.cpp
//  ObjToBuffer
//
//  Created by hx on 16/3/27.
//  Copyright © 2016年 hx. All rights reserved.
//
#include "tinyobjloader.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <fstream>
#include <sstream>
using namespace std;
using namespace tinyobj;
struct vec2{
    float x,y;
    vec2():x(0),y(0){}
    vec2(float xx,float yy) : x(xx),y(yy){}
    vec2 operator-(const vec2 &v)const{
        return vec2(x-v.x,y-v.y);
    }
};
struct vec3{
    float x,y,z;
    vec3():x(0),y(0),z(0){}
    vec3(float xx,float yy,float zz) : x(xx),y(yy),z(zz){}
    vec3 operator-(const vec3 &v)const{
        return vec3(x-v.x,y-v.y,z-v.z);
    }
    vec3 operator*(const float f)const{
        return vec3(x*f,y*f,z*f);
    }
    vec3& operator+=(const vec3 &v){
        x += v.x;y+=v.y;z+=v.z;
        return *this;
    }
};
vec3 Cross(const vec3 &v1,const vec3 &v2)
{
    return vec3(v1.y*v2.z-v1.z*v2.y,v1.z*v2.x-v1.x*v2.z,v1.x*v2.y-v1.y*v2.x);
}
float Dot(const vec3 &v1,const vec3 &v2){
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}
struct Vertex
{
    float x,y,z;
    float u,v;
    float nx,ny,nz;
    vec3 tangent,bitangent;
    Vertex(float xx,float yy,float zz) : x(xx),y(yy),z(zz){}
    vec3 operator-(const Vertex &v)const{
        return vec3(x-v.x,y-v.y,z-v.z);
    }
    bool operator==(const Vertex &vertex)const
    {
        return
        (x==vertex.x)&&(y==vertex.y)&&(z==vertex.z)&&
        (u==vertex.u)&&(v==vertex.v)&&
        (nx==vertex.nx)&&(ny==vertex.ny)&&(nz==vertex.nz);
    }
    Vertex():x(100000.0f){}
};
int main(int argc, const char * argv[]) {
    std::string inputfile = argv[1];
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    
    std::string err;
    bool ret = tinyobj::LoadObj(shapes, materials, err, inputfile.c_str());
    
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }
    
    if (!ret) {
        return 0;
    }
    
    std::cout << "# of shapes    : " << shapes.size() << std::endl;
    std::cout << "# of materials : " << materials.size() << std::endl;

    
    cout << shapes.size() << endl;
    int count = 0;
    for (auto shape : shapes)
    {
        ++count;
        vector<Vertex> vertices;
        vertices.resize(shape.mesh.positions.size()/3);
        float minX = 0.0,minY = 0.0,minZ = 0.0,maxX = 0.0,maxY = 0.0,maxZ = 0.0;
        bool first = true;
        
        for (size_t i = 0; i<shape.mesh.positions.size()/3; ++i) {
            vertices[i].x = shape.mesh.positions[i*3+0];
            vertices[i].y = shape.mesh.positions[i*3+1];
            vertices[i].z = shape.mesh.positions[i*3+2];
            
            vertices[i].nx = shape.mesh.normals[i*3+0];
            vertices[i].ny = shape.mesh.normals[i*3+1];
            vertices[i].nz = shape.mesh.normals[i*3+2];

            vertices[i].u = shape.mesh.texcoords[i*2+0];
            vertices[i].v = shape.mesh.texcoords[i*2+1];
            
            
            Vertex &v = vertices[i];
            if (first) {
                minX = v.x;minY = v.y;minZ = v.z;
                maxX = v.x;maxY = v.y;maxZ = v.z;
                first = false;
            }else{
                if (v.x < minX) {
                    minX = v.x;
                }
                if (v.y < minY) {
                    minY = v.y;
                }
                if (v.z < minZ) {
                    minZ = v.z;
                }
                if (v.x > maxX) {
                    maxX = v.x;
                }
                if (v.y > maxY) {
                    maxY = v.y;
                }
                if (v.z > maxZ) {
                    maxZ = v.z;
                }
            }

        }
        ofstream check;
        check.open("check.txt",ios::trunc);

        for (auto f = 0; f<shape.mesh.indices.size()/3;++f){
            auto pv1 = shape.mesh.indices[f*3+0];
            auto pv2 = shape.mesh.indices[f*3+1];
            auto pv3 = shape.mesh.indices[f*3+2];
            auto &v1 = vertices[pv1];
            auto &v2 = vertices[pv2];
            auto &v3 = vertices[pv3];
            
            
            
            float x1 = v2.x - v1.x;
            float x2 = v3.x - v1.x;
            float y1 = v2.y - v1.y;
            float y2 = v3.y - v1.y;
            float z1 = v2.z - v1.z;
            float z2 = v3.z - v1.z;
            float s1 = v2.u - v1.u;
            float s2 = v3.u - v1.u;
            float t1 = v2.v - v1.v;
            float t2 = v3.v - v1.v;
            float r = 1.0f / (s1 * t2 - s2 * t1);
            vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                          (t2 * z1 - t1 * z2) * r);
            vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                          (s1 * z2 - s2 * z1) * r);

            v1.tangent += sdir;
            v1.bitangent += tdir;
            v2.tangent += sdir;
            v2.bitangent += tdir;
            v3.tangent += sdir;
            v3.bitangent += tdir;
        }
        
        for (auto a = 0; a < vertices.size(); a++)
        {
            const vec3 n = vec3(vertices[a].nx,vertices[a].ny,vertices[a].nz);
            const vec3 t = vertices[a].tangent;
            // Gram-Schmidt orthogonalize
            vertices[a].tangent = (t - n * Dot(n, t));
            // Calculate handedness
            //vertices[a].tangent.w = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
        }

        
        ofstream out;
        stringstream ss;
        ss << count;
        string outname = ss.str() + "body.pnuvti";
        out.open(outname, ios::trunc|ios::binary);
        
        int vertexBufferSize = static_cast<int>((vertices.size()*(8+3))*sizeof(float));
        int indexBufferSize = static_cast<int>(shape.mesh.indices.size()*sizeof(int));
        
        out.write(reinterpret_cast<char*>(&vertexBufferSize), sizeof(int));
        out.write(reinterpret_cast<char*>(&indexBufferSize), sizeof(int));
        
        out.write(reinterpret_cast<char*>(&minX),sizeof(float));
        out.write(reinterpret_cast<char*>(&minY),sizeof(float));
        out.write(reinterpret_cast<char*>(&minZ),sizeof(float));
        out.write(reinterpret_cast<char*>(&maxX),sizeof(float));
        out.write(reinterpret_cast<char*>(&maxY),sizeof(float));
        out.write(reinterpret_cast<char*>(&maxZ),sizeof(float));
        
        cout << minX << "," << minY << "," << minZ << endl;
        cout << maxX << "," << maxY << "," << maxZ << endl;
        cout << vertexBufferSize << endl;
        cout << indexBufferSize << endl;
        
        for (Vertex v : vertices){
            out.write(reinterpret_cast<char*>(&v.x), sizeof(v.x));
            out.write(reinterpret_cast<char*>(&v.y), sizeof(v.y));
            out.write(reinterpret_cast<char*>(&v.z), sizeof(v.z));
            
            out.write(reinterpret_cast<char*>(&v.nx), sizeof(v.nx));
            out.write(reinterpret_cast<char*>(&v.ny), sizeof(v.ny));
            out.write(reinterpret_cast<char*>(&v.nz), sizeof(v.nz));
            
            out.write(reinterpret_cast<char*>(&v.u), sizeof(v.u));
            out.write(reinterpret_cast<char*>(&v.v), sizeof(v.v));
            
            out.write(reinterpret_cast<char*>(&v.tangent.x), sizeof(float));
            out.write(reinterpret_cast<char*>(&v.tangent.y), sizeof(float));
            out.write(reinterpret_cast<char*>(&v.tangent.z), sizeof(float));
            
//            check << v.tangent.x << "," << v.tangent.y << "," << v.tangent.z << "===";
            
//            out.write(reinterpret_cast<char*>(&v.bitangent.x), sizeof(float));
//            out.write(reinterpret_cast<char*>(&v.bitangent.y), sizeof(float));
//            out.write(reinterpret_cast<char*>(&v.bitangent.z), sizeof(float));
            
//            check << v.bitangent.x << "," << v.bitangent.y << "," << v.bitangent.z << endl;
        }
        for (auto index : shape.mesh.indices)
        {
            out.write(reinterpret_cast<char*>(&index), sizeof(index));
        }
        check.close();
        out.close();
    }
    
    cout << "success" << endl;
    return 0;
}
