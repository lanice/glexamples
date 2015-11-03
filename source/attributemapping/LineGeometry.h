
#pragma once


#include <globjects/base/Referenced.h>
#include <globjects/base/ref_ptr.h>
#include <globjects/VertexArray.h>


class DataSet;


/**
*  @brief
*    Geometry for lines
*/
class LineGeometry : public globjects::Referenced
{
public:
    /**
    *  @brief
    *    Constructor
    */
    LineGeometry();

    /**
    *  @brief
    *    Create geometry for lines
    *
    *  @param[in] dataset
    *    Data set
    */
    void setData(const DataSet & dataset);

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
    ~LineGeometry();


protected:
    globjects::ref_ptr<globjects::VertexArray> m_vao;   ///< Vertex array object
    unsigned int                               m_size;  ///< Number of elements
};
