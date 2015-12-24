
#include <gloperate/plugin/plugin_api.h>

#include <glexamples/glexamples-version.h>

#include "screendoor/ScreenDoor.h"
#include "stochastic/StochasticTransparency.h"


GLOPERATE_PLUGIN_LIBRARY

    GLOPERATE_PAINTER_PLUGIN(ScreenDoor
    , "ScreenDoor"
    , "Screen-Door Transparency"
    , GLEXAMPLES_AUTHOR_ORGANIZATION
    , "v1.0.0" )

    GLOPERATE_PAINTER_PLUGIN(StochasticTransparency
    , "StochasticTransparency"
    , "Stochastic Transparency"
    , GLEXAMPLES_AUTHOR_ORGANIZATION
    , "v1.0.0" )

GLOPERATE_PLUGIN_LIBRARY_END
