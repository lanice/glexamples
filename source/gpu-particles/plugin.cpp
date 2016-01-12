#include <gloperate/plugin/plugin_api.h>

#include "gpu-particles.h"

#include <glexamples-version.h>

GLOPERATE_PLUGIN_LIBRARY

    GLOPERATE_PAINTER_PLUGIN(GpuParticles
    , "GpuParticles"
    , "GPU Particles"
    , GLEXAMPLES_AUTHOR_ORGANIZATION
    , "v1.0.0" )

GLOPERATE_PLUGIN_LIBRARY_END
