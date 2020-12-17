#include "vertex_array.h"

#include <GL/glew.h>

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_rendererId);
	glBindVertexArray(m_rendererId);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_rendererId);
}

void VertexArray::Bind()
{
	glBindVertexArray(m_rendererId);
}

void VertexArray::Unbind()
{
	glBindVertexArray(0);
}

void VertexArray::AddVertexBuffer(VertexBuffer *vb)
{
	m_vertexBuffers.push_back(vb);
}

void VertexArray::AddVertexAttrib(
			GLuint index,
			int size,
			GLenum type,
			GLboolean normalized,
			GLsizei stride,
			const void *startOffset)
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, type, normalized, stride, startOffset);
	m_vertexAttribs.push_back({index, size, type, normalized, stride, startOffset});
}
