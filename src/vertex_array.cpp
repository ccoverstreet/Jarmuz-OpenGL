#include "vertex_array.h"

#include <GL/glew.h>

VertexArray::VertexArray() {
	glGenVertexArrays(1, &m_rendererId);
	glBindVertexArray(m_rendererId);
}

VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &m_rendererId);
}

void VertexArray::Bind() {
	glBindVertexArray(m_rendererId);
}

void VertexArray::Unbind() {
	glBindVertexArray(0);
}
