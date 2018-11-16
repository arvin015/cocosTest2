//
// Created by mac_arvin on 2018/11/16.
//

#ifndef PROJ_ANDROID_STUDIO_SLAB_H
#define PROJ_ANDROID_STUDIO_SLAB_H


class Slab {
public:
    Slab();
    Slab(float min, float max);

    void insert(float v);
    bool isValid();

    bool overlapped(const Slab &other);

    float min, max;
};


#endif //PROJ_ANDROID_STUDIO_SLAB_H
