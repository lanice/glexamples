
#pragma once


#include <globjects/base/Referenced.h>
#include <globjects/base/ref_ptr.h>
#include <globjects/VertexArray.h>

#include "attributemapping_api.h"


class DataSet;


/**
*  @brief
*    Geometry for nodes
*/
class ATTRIBUTEMAPPING_API NodeGeometry : public globjects::Referenced
{
public:
    /**
    *  @brief
    *    Constructor
    */
    NodeGeometry();

    /**
    *  @brief
    *    Create geometry for nodes
    *
    *  @param[in] dataSet
    *    Data set
    */
    void setData(const DataSet & dataSet);

    /**
    *  @brief
    *    Get size
    *
    *  @return
    *    Number of elements
    */
    unsigned int size() const;

    /**
    *  @brief
    *    Get vertex array
    *
    *  @return
    *    Vertex array object
    */
    globjects::VertexArray * vao();

    /**
    *  @brief
    *    Get vertex array
    *
    *  @return
    *    Vertex array object
    */
    const globjects::VertexArray * vao() const;

    /**
    *  @brief
    *    Draw geometry
    */
    void draw();


protected:
    /**
    *  @brief
    *    Destructor
    */
    ~NodeGeometry();


protected:
    globjects::ref_ptr<globjects::VertexArray> m_vao;   ///< Vertex array object
    unsigned int                               m_size;  ///< Number of elements
};
