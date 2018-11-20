/****************************************************************************
 Copyright (c) 2014-2017 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __DRAW_NODE_3D_H__
#define __DRAW_NODE_3D_H__

#include "cocos2d.h"

NS_CC_BEGIN

static const int DEFAULT_3D_LINE_WIDTH = 2;

/**
 * Copy DrawNode for 3D geometry drawing.
 */
class DrawNode3D: public cocos2d::Node
{
public:

    struct CC_DLL V3F_C4B_V2F_V3F
    {
        /// vertices (3F)
        Vec3     vertices;            // 12 bytes

        /// colors (4B)
        Color4B      colors;              // 4 bytes

        // tex coords (2F)
        Vec2        texCoords;           // 8 bytes

        // normal
        Vec3        normal;         // 12 bytes
    };

    /** @struct V3F_C4B_V2F_V3F_Triangle
     * A Triangle of V3F_C4B_V2F_V3F.
    */
    struct CC_DLL V3F_C4B_V2F_V3F_Triangle
    {
        V3F_C4B_V2F_V3F a;
        V3F_C4B_V2F_V3F b;
        V3F_C4B_V2F_V3F c;
    };

    /** creates and initialize a DrawNode3D node */
    static cocos2d::DrawNode3D* create(GLfloat defaultLineWidth = DEFAULT_3D_LINE_WIDTH);
    
    /**
     * Draw 3D Line
     */
    void drawLine(const cocos2d::Vec3 &from, const cocos2d::Vec3 &to, const Color4F &color);

    /**
     * Draw 3D with light
     * @param verts
     * @param uvs
     * @param normal
     * @param fillColor
     * @param count
     */
    void drawPolygonWithLight(const cocos2d::Vec3 *verts, const cocos2d::Vec2 *uvs,
                              const cocos2d::Vec3 &normal, const cocos2d::Color4F &fillColor, int count);

    /**
     * set 3d texture
     */
    void set3DTextture(unsigned int texture) {
        this->_texture = texture;
    }
    
    /** Clear the geometry in the node's buffer. */
    void clear();
    
    /**
    * @js NA
    * @lua NA
    */
    const BlendFunc& getBlendFunc() const;
    
    /**
    * @code
    * When this function bound into js or lua,the parameter will be changed
    * In js: var setBlendFunc(var src, var dst)
    * @endcode
    * @lua NA
    */
    void setBlendFunc(const BlendFunc &blendFunc);

    virtual void onDraw(const cocos2d::Mat4& transform, uint32_t flags);
    virtual void onDrawGLLine(const cocos2d::Mat4& transform, uint32_t flags);
    
    // Overrides
    virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;

    void setLineWidth(GLfloat lineWidth);

    // Get CocosStudio guide lines width.
    GLfloat getLineWidth();

    void setLightDir(const Vec3 &lightDir);

CC_CONSTRUCTOR_ACCESS:
    DrawNode3D(GLfloat lineWidth = DEFAULT_3D_LINE_WIDTH);
    virtual ~DrawNode3D();
    virtual bool init() override;

protected:

    void ensureCapacity(int count);
    void ensureCapacityGLLine(int count);

    GLuint      _vao;
    GLuint      _vbo;
    GLuint      _vaoGLLine;
    GLuint      _vboGLLine;

    int         _bufferCapacity;
    GLsizei     _bufferCount;
    V3F_C4B_T2F *_buffer;

    int         _bufferCapacityGLLine;
    GLsizei     _bufferCountGLLine;
    V3F_C4B_T2F *_bufferGLLine;

    BlendFunc   _blendFunc;
    CustomCommand _customCommand;
    CustomCommand _customCommandGLLine;

    bool        _dirty;
    bool        _dirtyGLLine;

    GLfloat     _lineWidth;
    GLfloat     _defaultLineWidth;
    Vec3        _lightDir;

    unsigned int _texture;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(DrawNode3D);
};

NS_CC_END

#endif // __DRAW_NODE_3D_H__
