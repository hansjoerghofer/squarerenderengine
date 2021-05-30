#pragma once

#include "Common/Macros.h"
#include "API/SharedResource.h"

class IUniformBlockData
{
public:
	virtual void link(IUniformBlockResourceUPtr resource) = 0;

	virtual bool linked() const = 0;

	virtual int bindingPoint() const = 0;

	virtual size_t dataSize() const = 0;
};

template<typename T>
class UniformBlockData : public IUniformBlockData
{
public:
	explicit UniformBlockData(int bindingPoint);

	virtual void link(IUniformBlockResourceUPtr resource) override;

	virtual bool linked() const override;

	virtual int bindingPoint() const override;

	virtual size_t dataSize() const override;

	bool update(const T& dataBlock);

private:
	IUniformBlockResourceUPtr m_linkedResource;

	int m_bindingPoint;

	std::unique_ptr<T> m_dataBlock;
};

template<typename T>
inline UniformBlockData<T>::UniformBlockData(int bindingPoint)
	: m_dataBlock(new T())
	, m_bindingPoint(bindingPoint)
{
}

template<typename T>
inline void UniformBlockData<T>::link(IUniformBlockResourceUPtr resource)
{
	m_linkedResource = std::move(resource);
}

template<typename T>
inline bool UniformBlockData<T>::linked() const
{
	return m_linkedResource && m_linkedResource->isValid();
}

template<typename T>
inline bool UniformBlockData<T>::update(const T& dataBlock)
{
	if (*m_dataBlock != dataBlock && m_linkedResource)
	{
		*m_dataBlock = dataBlock;

		return m_linkedResource->update(
			reinterpret_cast<const char*>(m_dataBlock.get()));
	}

	return false;
}

template<typename T>
inline size_t UniformBlockData<T>::dataSize() const
{
	return sizeof(T);
}

template<typename T>
inline int UniformBlockData<T>::bindingPoint() const
{
	if (m_linkedResource)
	{
		return m_linkedResource->bindingPoint();
	}
	else
	{
		return m_bindingPoint;
	}
}
