//
//  MCGLSkysphere.c
//  Sapindus
//
//  Created by Sun YuLi on 2017/3/4.
//  Copyright © 2017年 oreisoft. All rights reserved.
//

#include "MCGLSkysphere.h"
#include "MCGLContext.h"
#include "MC3DShapeBase.h"

static const char* vsource = S(
//version is specified in MCGLContext
precision highp float;
precision mediump int;

//vertex attributes
layout (location=0) in vec3 position;
layout (location=1) in vec2 texcoord;

//uniforms
uniform mat4 sphViewMatrix;
uniform mat4 sphProjectionMatrix;

//output
out vec2 TexCoords;

void main()
{
   gl_Position = sphProjectionMatrix * sphViewMatrix * vec4(position, 1.0);
   TexCoords = texcoord;
}
);//vsource end

static const char* fsource = S(
//version is specified in MCGLContext
precision highp sampler2D;
precision highp float;
precision lowp int;
const float Epsilon = 0.0000001;

//input
in vec2 TexCoords;

//uniforms
uniform sampler2D sampler;

//output
out vec4 color;

void main()
{
    color = texture(sampler, TexCoords);
}
);//fsource end

oninit(MCGLSkysphere)
{
    if (init(MC3DNode)) {
        obj->Super.visible = true;
        
        var(vaoid) = 0;
        var(vboid) = 0;
        var(eboid) = 0;
        var(texid) = 0;
        
        var(tex) = null;
        var(ctx) = new(MCGLContext);
        
        var(sphViewMatrix) = MCMatrix4Identity;
        var(sphProjectionMatrix) = MCMatrix4Identity;
        var(sphCameraAngle) = M_PI * 0.55;
        var(sphCameraRatio) = 9.0/16.0;
        
        var(R_value) = 200.0;
        var(R_percent) = 1.0;
        var(tht) = 0.0;
        var(fai) = 0.0;
        
        int nr = 72;
        int nc = 144;
        
        var(vertices_size) = sizeof(GLfloat) * nr * nc * 5;
        var(indices_size)  = sizeof(GLuint) * nr * nc * 2;
        
        var(vertices) = (GLfloat*)malloc(obj->vertices_size);
        var(indices)  = (GLuint*)malloc(obj->indices_size);
        var(ic) = MCGenerateSkysphere(nr, nc, 100.0, var(vertices), var(indices));
        
        MCGLContext_setClearScreenColor((MCColorf){0.05, 0.25, 0.35, 1.0});
        MCGLContext_featureSwith(MCGLCullFace, true);
        MCGLContext_cullFace(MCGLBack);
        MCGLContext_setFrontCounterClockWise(true);//CW
        return obj;
    }else{
        return null;
    }
}

method(MCGLSkysphere, void, bye, voida)
{
    
    free(var(vertices));
    free(var(indices));
    
    GLuint texids[1] = {obj->texid};
    glDeleteTextures(1, texids);
    
    GLuint buffs[3] = {obj->vaoid, obj->vboid, obj->eboid};
    glDeleteBuffers(3, buffs);
    
    release(var(ctx));
    release(var(tex));
    
    MC3DNode_bye(sobj, 0);
}

method(MCGLSkysphere, MCGLSkysphere*, initWithBE2DTexture, BE2DTextureData* tex)
{
    retain(tex);
    var(tex) = tex;
    
    //Shader
    MCGLShader_initWithShaderCode(var(ctx)->shader, vsource, fsource,
                                   (const char* []){
                                       "position",
                                       "texcoord"
                                   }, 2,
                                   (MCGLUniformType []){
                                       MCGLUniformMat4,
                                       MCGLUniformMat4,
                                       MCGLUniformScalar
                                   },
                                   (const char* []){
                                       "sphViewMatrix",
                                       "sphProjectionMatrix",
                                       "sampler"
                                   }, 3);
    //Mesh & Texture
    MCUInt buffers[3];
    glGenVertexArrays(1, &var(vaoid));
    glGenBuffers(3, buffers);
    var(vboid) = buffers[0];
    var(eboid) = buffers[1];
    var(texid) = buffers[2];
    //VAO
    glBindVertexArray(var(vaoid));
    //VBO
    glBindBuffer(GL_ARRAY_BUFFER, var(vboid));
    glBufferData(GL_ARRAY_BUFFER, obj->vertices_size, obj->vertices, GL_STATIC_DRAW);
    //EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, var(eboid));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->indices_size, obj->indices, GL_STATIC_DRAW);
    //VAttributes
    MCVertexAttribute attr1 = (MCVertexAttribute){0, 3, GL_FLOAT, GL_FALSE, 20, MCBUFFER_OFFSET(0)};
    MCVertexAttributeLoad(&attr1);
    MCVertexAttribute attr2 = (MCVertexAttribute){1, 2, GL_FLOAT, GL_FALSE, 20, MCBUFFER_OFFSET(12)};
    MCVertexAttributeLoad(&attr2);
    //Texture
    MCGLContext_activeTextureUnit(0);
    MCGLContext_bind2DTexture(var(texid));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->width, tex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex->raw);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //Unbind
    glBindVertexArray(0);
    
    return obj;
}

method(MCGLSkysphere, MCGLSkysphere*, initWithFileName, const char* name)
{
    BE2DTextureData* data = BE2DTextureData_newWithFilename(name);
    if (data) {
        MCGLSkysphere* sph = MCGLSkysphere_initWithBE2DTexture(obj, data);
        release(data);
        return sph;
    }
    return null;
}

method(MCGLSkysphere, MCGLSkysphere*, initWithDefaultFile, voida)
{
    return MCGLSkysphere_initWithFileName(obj, "skysphtex.jpg");
}

function(MCMatrix4, sphViewMatrix, voida)
{
    as(MCGLSkysphere);
    MCMatrix4 m = MCMatrix4MakeLookAt(0, 0, 0,
                                      0, 0,-1,
                                      0, 1, 0);
    MCMatrix4 imat4 = MCMatrix4Invert(obj->Super.transform, null);
    
    return MCMatrix4Multiply(m, imat4);
}

function(MCMatrix4, sphProjectionMatrix, voida)
{
    as(MCGLSkysphere);
    return MCMatrix4MakePerspective(obj->sphCameraAngle,
                                    obj->sphCameraRatio,
                                    0.001,
                                    200.0);
}

//override
method(MCGLSkysphere, void, update, MCGLContext* ctx)
{
    if (obj && obj->Super.visible) {
        obj->sphViewMatrix = sphViewMatrix(obj, 0);
        obj->sphProjectionMatrix = sphProjectionMatrix(obj, 0);
        
        MCGLShader_activateShaderProgram(var(ctx)->shader, 0);
        
        MCGLUniformData data;
        data.mat4 = obj->sphProjectionMatrix;
        MCGLShader_updateUniform(var(ctx)->shader, "sphProjectionMatrix", data);
    }
}

method(MCGLSkysphere, void, draw, MCGLContext* ctx)
{
    if (obj && obj->Super.visible) {
        glDepthMask(GL_FALSE);
        MCGLShader_activateShaderProgram(var(ctx)->shader, 0);
        MCGLUniformData data;
        data.mat4 = obj->sphViewMatrix;
        MCGLShader_updateUniform(var(ctx)->shader, "sphViewMatrix", data);
        MCGLShader_setUniforms(var(ctx)->shader, 0);
        
        glBindVertexArray(obj->vaoid);
        MCGLContext_activeTextureUnit(0);
        glDrawElements(GL_TRIANGLE_STRIP, var(ic), GL_UNSIGNED_INT, MCBUFFER_OFFSET(0));
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
    }
}

method(MCGLSkysphere, void, setRotationMat3, float mat3[9])
{
    MC3DNode_rotateMat3(sobj, mat3, false);
}

method(MCGLSkysphere, void, setRotationMat4, float mat4[16])
{
    MC3DNode_rotateMat4(sobj, mat4, false);
}

method(MCGLSkysphere, void, transformSelfByEularAngle, double R, double fai, double tht)
{
    //MCMatrix4 lookat = MCMatrix4MakeLookAt(0, 0, 0, 0, 0,-1, 0, 1, 0);
    //double R = var(R_value) * var(R_percent);
    //sobj->transform = MCMatrix4MakeLookAtByEulerAngle_EyeUp(lookat, R, fai, tht, null, null);
}

onload(MCGLSkysphere)
{
    if (load(MC3DNode)) {
        binding(MCGLSkysphere, void, bye, voida);
        binding(MCGLSkysphere, MCGLSkysphere*, initWithBE2DTexture, BE2DTextureData* tex);
        binding(MCGLSkysphere, MCGLSkysphere*, initWithFileName, const char* name);
        binding(MCGLSkysphere, MCGLSkysphere*, initWithDefaultFile, voida);
        binding(MCGLSkysphere, void, setRotationMat3, float mat3[9]);
        binding(MCGLSkysphere, void, setRotationMat4, float mat4[16]);
        //override
        binding(MCGLSkysphere, void, update, MCGLContext* ctx);
        binding(MCGLSkysphere, void, draw, MCGLContext* ctx);
        
        return cla;
    }else{
        return null;
    }
}