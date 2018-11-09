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

    /** @struct V3F_C4B_T2F_Triangle
     * A Triangle of V3F_C4B_T2F.
    */
    struct CC_DLL V3F_C4B_T2F_Triangle
    {
        V3F_C4B_T2F a;
        V3F_C4B_T2F b;
        V3F_C4B_T2F c;
    };

    /** creates and initialize a DrawNode3D node */
    static cocos2d::DrawNode3D* create(GLfloat defaultLineWidth = DEFAULT_3D_LINE_WIDTH);
    
    /**
     * Draw 3D Line
     */
    void drawLine(const cocos2d::Vec3 &from, const cocos2d::Vec3 &to, const Color4F &color);

    /**
     * Draw 3D Polygon
     * @param verts
     * @param count
     * @param fillColor
     */
    void drawPolygon(const cocos2d::Vec3 *verts, int count, const cocos2d::Color4F &fillColor);
    
    /**
    * Draw 3D cube
    * @param point to a vertex array who has 8 element.
    *        vertices[0]:Left-top-front,
    *        vertices[1]:Left-bottom-front,
    *        vertices[2]:Right-bottom-front,
    *        vertices[3]:Right-top-front,
    *        vertices[4]:Right-top-back,
    *        vertices[5]:Right-bottom-back,
    *        vertices[6]:Left-bottom-back,
    *        vertices[7]:Left-top-back.
    * @param color
    */
    void drawCube(cocos2d::Vec3* vertices, const Color4F &color);
    
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

    virtual void onDrawGLLine(const cocos2d::Mat4 &transform, uint32_t flags);

    virtual void onDrawGLPoint(const cocos2d::Mat4 &transform, uint32_t flags);
    
    // Overrides
    virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;

    void setLineWidth(GLfloat lineWidth);

    // Get CocosStudio guide lines width.
    GLfloat getLineWidth();

CC_CONSTRUCTOR_ACCESS:
    DrawNode3D(GLfloat lineWidth = DEFAULT_3D_LINE_WIDTH);
    virtual ~DrawNode3D();
    virtual bool init() override;

protected:

    void ensureCapacity(int count);
    void ensureCapacityGLPoint(int count);
    void ensureCapacityGLLine(int count);

    GLuint      _vao;
    GLuint      _vbo;
    GLuint      _vaoGLPoint;
    GLuint      _vboGLPoint;
    GLuint      _vaoGLLine;
    GLuint      _vboGLLine;

    int         _bufferCapacity;
    GLsizei     _bufferCount;
    V3F_C4B_T2F *_buffer;

    int         _bufferCapacityGLPoint;
    GLsizei     _bufferCountGLPoint;
    V3F_C4B_T2F *_bufferGLPoint;

    int         _bufferCapacityGLLine;
    GLsizei     _bufferCountGLLine;
    V3F_C4B_T2F *_bufferGLLine;

    BlendFunc   _blendFunc;
    CustomCommand _customCommand;
    CustomCommand _customCommandGLPoint;
    CustomCommand _customCommandGLLine;

    bool        _dirty;
    bool        _dirtyGLPoint;
    bool        _dirtyGLLine;

    GLfloat     _lineWidth;
    GLfloat     _defaultLineWidth;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(DrawNode3D);
};

NS_CC_END

#endif // __DRAW_NODE_3D_H__
