
#pragma once


#include <vector>
#include <string>

#include <globjects/base/Referenced.h>
#include <globjects/base/ref_ptr.h>
#include <globjects/Texture.h>

#include "attributemapping_api.h"


class DataSet;


/**
*  @brief
*    Storage that store/access attributes for each item (e.g., nodes or edges) on the GPU
*/
class ATTRIBUTEMAPPING_API AttributeStorage : public globjects::Referenced
{
public:
    /**
    *  @brief
    *    Constructor
    */
    AttributeStorage();

    /**
    *  @brief
    *    Create attribute storage for data set
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
    *    Get texture
    *
    *  @return
    *    Texture
    */
    globjects::Texture * texture();

    /**
    *  @brief
    *    Get texture
    *
    *  @return
    *    Texture
    */
    const globjects::Texture * texture() const;

    /**
    *  @brief
    *    Get available attributes
    *
    *  @return
    *    List of attributes
    */
    const std::vector<std::string> & attributes() const;

    /**
    *  @brief
    *    Get number of static and node attributes
    *
    *  @return
    *    Number of static and node attributes
    *
    *  @remarks
    *    Depending on this value, a shader program can decide whether to fetch
    *    a node or an edge/line attribute, based on the attribute index
    */
    int numNodeAttributes() const;


protected:
    /**
    *  @brief
    *    Destructor
    */
    ~AttributeStorage();


protected:
    globjects::ref_ptr<globjects::Texture> m_texture;           ///< Texture containing the attributes
    unsigned int                           m_size;              ///< Number of elements
    std::vector<std::string>               m_attrs;             ///< List of attributes
    int                                    m_numNodeAttributes; ///< Number of static and node attributes
};
