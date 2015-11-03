#include <gloperate/plugin/plugin_api.h>

#include "AttributeMapping.h"

#include <glexamples-version.h>

GLOPERATE_PLUGIN_LIBRARY

    GLOPERATE_PAINTER_PLUGIN(AttributeMapping
    , "AttributeMapping"
    , "Hardware-Accelerate Attribute Mapping Example"
    , GLEXAMPLES_AUTHOR_ORGANIZATION
    , "v1.0.0" )

GLOPERATE_PLUGIN_LIBRARY_END
