#include <gloperate/plugin/plugin_api.h>

#include "AttributeMappingPainter.h"

#include <glexamples-version.h>

GLOPERATE_PLUGIN_LIBRARY

    GLOPERATE_PAINTER_PLUGIN(AttributeMappingPainter
    , "AttributeMapping"
    , "Hardware-Accelerate Attribute Mapping Example"
    , GLEXAMPLES_AUTHOR_ORGANIZATION
    , "v1.0.0" )

GLOPERATE_PLUGIN_LIBRARY_END
