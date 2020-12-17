#pragma once

#include "vertex_buffer.h"

#include <vector>

#include <GL/glew.h>

struct VertexAttribElem
{
	GLuint index;
	int size;
	GLenum type;
	GLboolean normalized;
	GLsizei stride;
	const void *startOffset;	
};

class VertexArray
{
	public:
		VertexArray();
		~VertexArray();

		void Bind();
		void Unbind();

		void AddVertexBuffer(VertexBuffer *vb);
		void AddVertexAttrib(
			GLuint index,
			int size,
			GLenum type,
			GLboolean normalized,
			GLsizei stride,
			const void *startOffset);

	private:
		unsigned int m_rendererId;
		std::vector<VertexBuffer*> m_vertexBuffers;
		std::vector<VertexAttribElem> m_vertexAttribs;
};
