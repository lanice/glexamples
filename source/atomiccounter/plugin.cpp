
#include <gloperate/plugin/plugin_api.h>

#include <glexamples/glexamples-version.h>

#include "AtomicCounter.h"

GLOPERATE_PLUGIN_LIBRARY

    GLOPERATE_PAINTER_PLUGIN(AtomicCounter
    , "AtomicCounter"
    , "Atomic Counter"
    , GLEXAMPLES_AUTHOR_ORGANIZATION
    , "v1.0.0" )

GLOPERATE_PLUGIN_LIBRARY_END
