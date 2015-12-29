
#include <gloperate/plugin/plugin_api.h>

#include <glexamples/glexamples-version.h>

#include "OpenGLExample.h"


GLOPERATE_PLUGIN_LIBRARY

    GLOPERATE_PAINTER_PLUGIN(OpenGLExample
    , "OpenGLExample"
    , "An OpenGL Example"
    , GLEXAMPLES_AUTHOR_ORGANIZATION
    , "v1.0.0" )

GLOPERATE_PLUGIN_LIBRARY_END
