//
// Created by mac_arvin on 2018/11/16.
//

#include "Slab.h"
#include <limits>

Slab::Slab()
: min(std::numeric_limits<float>::infinity())
, max(-std::numeric_limits<float>::infinity()) {
}

Slab::Slab(float min, float max) {
    this->min = min;
    this->max = max;
}

void Slab::insert(float v) {
    if (v < min ) min = v;
    if (v > max) max = v;
}

bool Slab::isValid() {
    return min <= max;
}

bool Slab::overlapped(const Slab &other) {
    // the tolerance is used to distinct the just touch case to the really overallap case
    // which is a BAD approach.
    const float tolerance = 1.0f;
    if (min >= other.max - tolerance) return false;
    if (max <= other.min + tolerance) return false;
    return true;
}
