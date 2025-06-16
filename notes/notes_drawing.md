#### Points:

- square; if a pixel's centre is covered by the square region around the point: then colored.
- gl_PointSize in frag shader if GL_PROGRAM_POINT_SIZE is enabled else use glPointSize(GLfloat size) to set the size.

#### Lines:

- rasterisation rule: diamond exit rule.
- width set using glLineWidth(GLfloat width)
- when width > 1.0; it is replicated horizontally or vertically; essentially a rectangle is rasterised. If antialiasing is turned on; it is smooth.
- width < 1.0 is largely undefined - some drivers implement it by special visibility rules affecting the intensity of pixels.

#### Triangles:

- A sample is considered covered if it lies on the positive side of all the half spaces formed by lines between the vertices.
- no pixel is considered to be part of more than 1 triangle. Implementation takes care of shared edges.
- If a pixel is part of more than one; it cannot be left unlit either - it MUST be filled with something.

Triangle strip:
- First 3 vertices form the first triangle and the next vertex forms a triangle with the previous two vertices.

Triangle fan:
- Fist vertex forms a pivot; every pair of consecutive overlapping vertices after that forms a triangle with the pivot.


## Buffers:

glGenBuffers() only allots names; the actual buffer is created in memory only when the buffer object is bound to a buffer binding point the first tiime.

### Getting data in and out of buffers:

1. void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
usage tokens: {STATIC, DYNAMIC, STREAM} x {DRAW, READ, COPY}
STATIC: data will be modified once and used many times
DYNAMIC: upload repeatedly and use many times
STREAM: upload every frame; use once

DRAW: application writes the data; GPU reads it
READ: GPU will write to the buffer and application will read it
COPY: data written by and read by the GPU

2. void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
used with glBufferData after allocating enough size; the buffer is populated from offset bytes offset onwards
    
3. void glClearBufferData(GLenum target, GLenum internalformat, GLenum format, GLenum type, const void* data);
void glClearBufferSubData(GLenum target, GLenum internalformat, GLintptr offset, GLintptr size, 
GLenum format, GLenum type, const void * data);

4. void glCopyBufferSubData(GLenum readtarget, GLenum writetarget, GLintptr readoffset, GLintprr writeoffset, 
GLsizeiptr size);

glBufferData's implementation does not always reuse the same memory backing. 
If the buffer is busy (still in use by the GPU), it may: allocate a new memory region, point the buffer ID to the
new region and schedule the old one for deferred deletion. This is called buffer orphaning.
##### IMP: It avoids GPU/CPU synchronization stalls.
using glBufferData with a nullptr is also another strat for explicit orphaning if a subData() func is to follow.


### Reading the contents of a Buffer:
1. void glGetBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid * data);
error is thrown if the buffer object is currently mapped (see below) OR if offset + size is greater than the buffer
object's size.

2. All the above functions excluding clear's; COPY data from somewhere to somewhere - this is expensive
   void * glMapBuffer(GLenum target, GLenum access);
maps the client's address space to the entire data store of the buffer object. returns a pointer acc to the acess
policy - which allows the application to directly read from OpenGL owned memory, i.e GPU memory.
error in mapping returns NULL.
access tokens: GL_READ_ONLY, GL_WRITE_ONLY, GL_READ_WRITE.
once reading/writing is done; it must be unmapped using
   GLboolean glUnmapBuffer(GLenum target);
- returns true unless the data was corrupted - which should be handled by the application.

- mapping may involve moving data from one location to the other inside the GPU - this is expensive.
- glBufferData() and glBufferSubData() need to make a copy of your data in an internal buffer before returning.
- HOWEVER; mapping returns a pointer to memory owned by the GPU; so after unmapped is called - IF a copy is required
to be moved GPU-side; it can do it without any interference from the application.

To summarise:
mapping is async and overkill for small updates (like changing 1 vec3). zero-copy potential exists but is UNPREDICTABLE.
use this when updating buffers every frame OR updating LARGE data stores.

bufferSubData is sync and suitable for small updates. copying ALWAYS happens and is predictable.
use this for simple, small, occasional updates.

3. void* glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
specifies a range of the data store that is to be address mapped. For all the bit flags, refer pg 104

- possible to explicitly flush buffer ranges - useful to parallelise OpenGL's pipeline.


### Discarding Buffer Data

1. void glInvalidateBufferData(GLuint buffer);
   void glInvalidateBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr length);
- tells OpenGL that it can invalidate the data in the specified data store, i.e that it wont be accessed anymore.
- semantically, calling bufferData() with a nullptr does the same thing - except in the invalidate case; it doesnt
recreate the memory object - and is thus inexpensive and optimised.



## Vertex Specification:

- OpenGL normalises integers types to 32 bit floating point values before passing it into the vertex shader.
- If GL_NORMALISED is set to float; a simple type cast is applied; else special conversion rules are followed
  (check the OpenGL spec).
- This however, results in loss of precision; so integer style vertexAttribPointer funcs exist; must be passed
into ivecx/ uvecx.
- for double long precision. dvecx.
- size parameters can be 1, 2, 3, 4, and the special token GL_BGRA. Also, the type parameter may take one of the
special values GL_INT_2_10_10_10_REV or GL_UNSIGNED_INT_2_10_10_10_REV

### Static vertex attribute spec:
- used when a vertex attrib pointer is not enabled. glVertexAttrib*() functions are used for this. These implicitly
convert the values to 32 bit floating point numbers by a type cast. Similar to vertexAttribPointer func with
NORMALISED set to false
- use void glVertexAttrib4Nub(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
  && void glVertexAttrib4N{bsi ub us ui}v(GLuint index, const TYPE *v)
   to convert to [-1, 1] range.
- also exist for integers and double precision.
- if less components than in the vertex attrib is specified - then default values are used.
- if more; then additional components are discarded.

## OpenGL Drawing Commands:

1. void glDrawArrays(GLenum mode, GLint first, GLsizei count);

2. void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
indices - byte offset into the element array
type - must be one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT

3. void glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLint basevertex);
- adds baseVertex to each value in the element array.

4. void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);
- hint to OpenGL that all index values will lie in [start, end].

5. void glDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type,
const GLvoid *indices, GLint basevertex);
- hint to OpenGL that all index values will lie in [start, end] BEFORE baseVertex is added.

6. void glDrawArraysIndirect(GLenum mode, const GLvoid *indirect);
- params are taken from the struct stored in GL_DRAW_INDIRECT_BUFFER binding point. indirect is the byte offset
in the buffer.
- typedef struct DrawArraysIndirectCommand_t
{ GLuint count;
  GLuint primCount;
  GLuint first;
  GLuint baseInstance;
} DrawArraysIndirectCommand;
- fields are to be interpreted as if they were params to glDrawArraysInstanced() (see below).

7. void glDrawElementsIndirect(GLenum mode, GLenum type, const GLvoid * indirect);
- typedef struct DrawElementsIndirectCommand_t
  { GLuint count;
  GLuint primCount;
  GLuint firstIndex;
  GLuint baseVertex;
  GLuint baseInstance;
  } DrawElementsIndirectCommand;
- fields are to be interpreted as if they were params to glDrawElementsInstancedBaseVertex() (see below).

8. void glMultiDrawArrays(GLenum mode, const GLint* first, const GLint* count, GLsizei primcount);
- Draws multiple sets of geometric primitives with a single OpenGL function call. first and count are arrays of 
primcount parameters that would be valid for a call to glDrawArrays().

9. void glMultiDrawElements(GLenum mode, const GLint* count, GLenum type, const GLvoid* const* indices, 
GLsizei primcount);
void glMultiDrawElementsBaseVertex(GLenum mode, const GLint * count, GLenum type, const GLvoid * const * indices, 
GLsizei primcount, const GLint * baseVertex);

10. void glMultiDrawArraysIndirect(GLenum mode,const void* indirect, GLsizei drawcount, GLsizei stride);
void glMultiDrawElementsIndirect(GLenum mode, GLenum type, const void * indirect, GLsizei drawcount, GLsizei stride);
- each struct is separated by stride bytes.


## Restarting Primitives
controlled by calling glEnable() or glDisable() with the GL_PRIMITIVE_RESTART parameter.

1. void glPrimitiveRestartIndex(GLuint index);
- specifies the vertex array element index from which the new primitive must be rendered; ignoring the present
one.
- good value for restart index is usually 2^n - 1 where n is the number of bits in the type for the indices.


## Instanced Rendering

1. void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primCount);
- The built-in variable gl_InstanceID is incremented for each instance, and new values are presented to the 
vertex shader for each instanced vertex attribute.
- executed primCount number of times.

2. void glVertexAttribDivisor(GLuint index, GLuint divisor);
divisor = 0: Advance the attribute every vertex (default behavior).
divisor = N: Advance the attribute once every N instances.

3. void glDrawArraysInstancedBaseInstance(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount, 
GLuint baseInstance);
void glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices,
GLsizei instanceCount, GLuint baseInstance);
void glDrawElementsInstancedBaseVertexBaseInstance()
- adds baseInstance to each instance attribute index.
- attribute_fetch_index = gl_InstanceID + baseInstance;

