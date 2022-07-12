#include "object.h"


void Object::BuildOctree()
{

}

void Object::BuildDictionary()
{
    for(int i=0; i<m_vertices.size(); ++i)
    {
        m_dictPtToFace[i] = QVector<int>();
    }

    for(int i=0; i<m_faceids.size(); ++i)
    {
        uint3 face = m_faceids[i];

        m_dictPtToFace[face.x].append(i);
        m_dictPtToFace[face.y].append(i);
        m_dictPtToFace[face.z].append(i);
    }

}
