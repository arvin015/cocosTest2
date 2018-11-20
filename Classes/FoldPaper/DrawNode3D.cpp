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

#include "DrawNode3D.h"

NS_CC_BEGIN

const std::string MY_SHADER = "MY_SHADER_NAME";

static Vec2 v2fzero(0.0f,0.0f);

static inline Tex2F __t(const Vec2 &v)
{
   return *(Tex2F*)&v;
}

DrawNode3D::DrawNode3D(GLfloat lineWidth)
: _vao(0)
, _vbo(0)
, _vaoGLLine(0)
, _vboGLLine(0)
, _bufferCapacity(0)
, _bufferCount(0)
, _buffer(nullptr)
, _bufferCapacityGLLine(0)
, _bufferCountGLLine(0)
, _bufferGLLine(nullptr)
, _dirty(false)
, _dirtyGLLine(false)
, _lineWidth(lineWidth)
, _defaultLineWidth(lineWidth)
, _texture(0)
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
}

DrawNode3D::~DrawNode3D()
{
    free(_buffer);
    _buffer = nullptr;
    free(_bufferGLLine);
    _bufferGLLine = nullptr;

    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_vboGLLine);
    _vbo = 0;
    _vboGLLine = 0;

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
        glDeleteVertexArrays(1, &_vao);
        glDeleteVertexArrays(1, &_vaoGLLine);
        _vao = _vaoGLLine = 0;
    }

    _texture = 0;
}

DrawNode3D* DrawNode3D::create(GLfloat defaultLineWidth)
{
    DrawNode3D* ret = new (std::nothrow) DrawNode3D(defaultLineWidth);
    if (ret && ret->init())
    {
        ret->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(ret);
    }
    
    return ret;
}

void DrawNode3D::ensureCapacity(int count)
{
    CCASSERT(count>=0, "capacity must be >= 0");
    
    if(_bufferCount + count > _bufferCapacity)
    {
		_bufferCapacity += MAX(_bufferCapacity, count);
		_buffer = (V3F_C4B_T2F*)realloc(_buffer, _bufferCapacity*sizeof(V3F_C4B_T2F));
	}
}

void DrawNode3D::ensureCapacityGLLine(int count)
{
    CCASSERT(count>=0, "capacity must be >= 0");

    if(_bufferCountGLLine + count > _bufferCapacityGLLine)
    {
        _bufferCapacityGLLine += MAX(_bufferCapacityGLLine, count);
        _bufferGLLine = (V3F_C4B_T2F*)realloc(_bufferGLLine, _bufferCapacityGLLine*sizeof(V3F_C4B_T2F));
    }
}

bool DrawNode3D::init()
{
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

    auto glProgram = GLProgram::createWithFilenames("MyVertexShader.vert", "MyFragmentShader.frag");
    GLProgramCache::getInstance()->addGLProgram(glProgram, MY_SHADER);
    setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(MY_SHADER));

    ensureCapacity(512);
    ensureCapacityGLLine(256);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        int vertex = getGLProgram()->getAttribLocation("vertex");
        int uv = getGLProgram()->getAttribLocation("uv");
        int color = getGLProgram()->getAttribLocation("color");
        int normal = getGLProgram()->getAttribLocation("normal");

        glGenVertexArrays(1, &_vao);
        GL::bindVAO(_vao);
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F)* _bufferCapacity, _buffer, GL_STREAM_DRAW);
        // vertex
        glEnableVertexAttribArray(vertex);
        glVertexAttribPointer(vertex, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_V2F_V3F), (GLvoid *)offsetof(V3F_C4B_V2F_V3F, vertices));
        // color
        glEnableVertexAttribArray(color);
        glVertexAttribPointer(color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_V2F_V3F), (GLvoid *)offsetof(V3F_C4B_V2F_V3F, colors));
        // texcoord
        glEnableVertexAttribArray(uv);
        glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_V2F_V3F), (GLvoid *)offsetof(V3F_C4B_V2F_V3F, texCoords));
        // normal
        glEnableVertexAttribArray(normal);
        glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_V2F_V3F), (GLvoid *)offsetof(V3F_C4B_V2F_V3F, normal));

        glGenVertexArrays(1, &_vaoGLLine);
        GL::bindVAO(_vaoGLLine);
        glGenBuffers(1, &_vboGLLine);
        glBindBuffer(GL_ARRAY_BUFFER, _vboGLLine);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F)*_bufferCapacityGLLine, _bufferGLLine, GL_STREAM_DRAW);
        // vertex
        glEnableVertexAttribArray(vertex);
        glVertexAttribPointer(vertex, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, vertices));
        // color
        glEnableVertexAttribArray(color);
        glVertexAttribPointer(color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, colors));
        // texcoord
        glEnableVertexAttribArray(uv);
        glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, texCoords));
    }
    else
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F)* _bufferCapacity, _buffer, GL_STREAM_DRAW);

        glGenBuffers(1, &_vboGLLine);
        glBindBuffer(GL_ARRAY_BUFFER, _vboGLLine);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F)*_bufferCapacityGLLine, _bufferGLLine, GL_STREAM_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    CHECK_GL_ERROR_DEBUG();

    _dirty = true;
    _dirtyGLLine = true;
    
#if CC_ENABLE_CACHE_TEXTURE_DATA
    // Need to listen the event only when not use batchnode, because it will use VBO
    auto listener = EventListenerCustom::create(EVENT_COME_TO_FOREGROUND, [this](EventCustom* event){
    /** listen the event that coming to foreground on Android */
        this->init();
    });

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif

    return true;
}

void DrawNode3D::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    if(_bufferCount)
    {
        _customCommand.init(_globalZOrder, transform, flags);
        _customCommand.func = CC_CALLBACK_0(DrawNode3D::onDraw, this, transform, flags);
        renderer->addCommand(&_customCommand);
    }

    if(_bufferCountGLLine)
    {
        _customCommandGLLine.init(_globalZOrder, transform, flags);
        _customCommandGLLine.func = CC_CALLBACK_0(DrawNode3D::onDrawGLLine, this, transform, flags);
        renderer->addCommand(&_customCommandGLLine);
    }
}

void DrawNode3D::onDraw(const Mat4 &transform, uint32_t flags)
{
    auto glProgram = GLProgramCache::getInstance()->getGLProgram(MY_SHADER);
    glProgram->use();
    glProgram->setUniformsForBuiltins(transform);
    //设置Uniform数据
    glProgram->setUniformLocationWith3f(glProgram->getUniformLocation("lightDir"), _lightDir.x, _lightDir.y, _lightDir.z);
    glProgram->setUniformLocationWith1i(glProgram->getUniformLocation("diffuse"), 0);
    GL::blendFunc(_blendFunc.src, _blendFunc.dst);

    if (_dirty)
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_V2F_V3F)*_bufferCapacity, _buffer, GL_STREAM_DRAW);

        _dirty = false;
    }
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(_vao);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        // vertex
        int vertex = glProgram->getAttribLocation("vertex");
        glEnableVertexAttribArray(vertex);
        glVertexAttribPointer(vertex, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_V2F_V3F), (GLvoid *)offsetof(V3F_C4B_V2F_V3F, vertices));
        // texcoord
        int uv = glProgram->getAttribLocation("uv");
        glEnableVertexAttribArray(uv);
        glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_V2F_V3F), (GLvoid *)offsetof(V3F_C4B_V2F_V3F, texCoords));
        // normal
        int normal = glProgram->getAttribLocation("normal");
        glEnableVertexAttribArray(normal);
        glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_V2F_V3F), (GLvoid *)offsetof(V3F_C4B_V2F_V3F, normal));
        // color
        int color = glProgram->getAttribLocation("color");
        glEnableVertexAttribArray(color);
        glVertexAttribPointer(color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_V2F_V3F), (GLvoid *)offsetof(V3F_C4B_V2F_V3F, colors));
    }

    GL::bindTexture2D(_texture);

    glDrawArrays(GL_TRIANGLES, 0, _bufferCount);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
    }

    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, _bufferCount);
    CHECK_GL_ERROR_DEBUG();
}

void DrawNode3D::onDrawGLLine(const Mat4 &transform, uint32_t /*flags*/)
{
    auto glProgram = GLProgramCache::getInstance()->getGLProgram(MY_SHADER);
    glProgram->use();
    glProgram->setUniformsForBuiltins(transform);

    GL::blendFunc(_blendFunc.src, _blendFunc.dst);

    if (_dirtyGLLine)
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vboGLLine);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F)*_bufferCapacityGLLine, _bufferGLLine, GL_STREAM_DRAW);
        _dirtyGLLine = false;
    }
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(_vaoGLLine);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vboGLLine);

        int vertex = getGLProgram()->getAttribLocation("vertex");
        int uv = getGLProgram()->getAttribLocation("uv");
        int color = getGLProgram()->getAttribLocation("color");

        // vertex
        glEnableVertexAttribArray(vertex);
        glVertexAttribPointer(vertex, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, vertices));
        // color
        glEnableVertexAttribArray(color);
        glVertexAttribPointer(color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, colors));
        // texcoord
        glEnableVertexAttribArray(uv);
        glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid *)offsetof(V3F_C4B_T2F, texCoords));
    }

    glLineWidth(_lineWidth);
    glDrawArrays(GL_LINES, 0, _bufferCountGLLine);

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        GL::bindVAO(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,_bufferCountGLLine);

    CHECK_GL_ERROR_DEBUG();
}

void DrawNode3D::drawLine(const Vec3 &from, const Vec3 &to, const Color4F &color)
{
    unsigned int vertex_count = 2;
    ensureCapacityGLLine(vertex_count);

    V3F_C4B_T2F a = {from, Color4B(color), __t(v2fzero)};
    V3F_C4B_T2F b = {to, Color4B(color), __t(v2fzero)};

    V3F_C4B_T2F *lines = (V3F_C4B_T2F *)(_bufferGLLine + _bufferCountGLLine);
    lines[0] = a;
    lines[1] = b;

    _bufferCountGLLine += vertex_count;
    _dirtyGLLine = true;
}

void DrawNode3D::drawPolygonWithLight(const Vec3 *verts, const Vec2 *uvs,
                              const Vec3 &normal, const Color4F &fillColor, int count) {
    auto triangle_count = count - 2;
    auto vertex_count = 3*triangle_count;
    ensureCapacity(vertex_count);

    V3F_C4B_V2F_V3F_Triangle *triangles = (V3F_C4B_V2F_V3F_Triangle *)(_buffer + _bufferCount);
    V3F_C4B_V2F_V3F_Triangle *cursor = triangles;

    for (int i = 0; i < count-2; i++)
    {
        V3F_C4B_V2F_V3F_Triangle tmp = {
                {verts[0], Color4B(fillColor), uvs[0], normal},
                {verts[i+1], Color4B(fillColor), uvs[i+1], normal},
                {verts[i+2], Color4B(fillColor), uvs[i+2], normal},
        };

        *cursor++ = tmp;
    }

    _bufferCount += vertex_count;
    _dirty = true;
}

void DrawNode3D::clear()
{
    _bufferCount = 0;
    _dirty = true;
    _bufferCountGLLine = 0;
    _dirtyGLLine = true;
    _lineWidth = _defaultLineWidth;
}

const BlendFunc& DrawNode3D::getBlendFunc() const
{
    return _blendFunc;
}

void DrawNode3D::setBlendFunc(const BlendFunc &blendFunc)
{
    _blendFunc = blendFunc;
}

void DrawNode3D::setLineWidth(GLfloat lineWidth)
{
    _lineWidth = lineWidth;
}

GLfloat DrawNode3D::getLineWidth()
{
    return this->_lineWidth;
}

void DrawNode3D::setLightDir(const Vec3 &lightDir)
{
    _lightDir = lightDir;
}

NS_CC_END
