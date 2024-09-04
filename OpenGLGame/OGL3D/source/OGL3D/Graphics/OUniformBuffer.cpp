#include <OGL3D/Graphics/OUniformBuffer.h>
#include <glad/glad.h>


OUniformBuffer::OUniformBuffer(const OUniformBufferDesc& desc)
{
	glGenBuffers(1, &m_id);
	glBindBuffer(GL_UNIFORM_BUFFER, m_id);
	glBufferData(GL_UNIFORM_BUFFER, desc.size, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	m_size = desc.size;

}

OUniformBuffer::~OUniformBuffer()
{
	glDeleteBuffers(1, &m_id);
}

void OUniformBuffer::setData(void* data)
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_id);      
	glBufferSubData(GL_UNIFORM_BUFFER, 0, m_size, data);//not the same as glbuffer data this allows the already created buffer to be updated with new data
	glBindBuffer(GL_UNIFORM_BUFFER, 0);      //0 is for the slot of the buffer
	  
}

ui32 OUniformBuffer::getId()
{
	return m_id;
}

