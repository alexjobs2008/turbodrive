#ifndef I_SINGLETON_FACTORY_H
#define I_SINGLETON_FACTORY_H

namespace Drive
{

template<typename T>
class ISingletonFactory
{
public:
	virtual T& instance() const = 0;
};

}

#endif // I_SINGLETON_FACTORY_H
