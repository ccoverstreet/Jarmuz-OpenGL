class IndexBuffer
{
	public:
		IndexBuffer(unsigned int *data, unsigned int count);
		~IndexBuffer();

		void Bind();
		void Unbind();

		inline unsigned int GetCount() { return m_count; };
	private:
		unsigned int m_rendererId;
		unsigned int m_count;
};
