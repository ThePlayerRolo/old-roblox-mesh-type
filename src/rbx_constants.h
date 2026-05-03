#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"

using namespace godot;

enum RBX_SurfaceType {
    SMOOTH,
    GLUE,
    WELD,
    STUDS,
    INLET,
};

VARIANT_ENUM_CAST(RBX_SurfaceType)