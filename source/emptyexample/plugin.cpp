
#include <gloperate/plugin/plugin_api.h>

#include <glexamples/glexamples-version.h>

#include "EmptyExample.h"


GLOPERATE_PLUGIN_LIBRARY

    GLOPERATE_PAINTER_PLUGIN(EmptyExample
    , "EmptyExample"
    , "Copy to implement a new example"
    , GLEXAMPLES_AUTHOR_ORGANIZATION
    , "v1.0.0" )

GLOPERATE_PLUGIN_LIBRARY_END
