#ifndef I_FACTORY_H
#define I_FACTORY_H

namespace Drive
{

template<typename T>
class IFactory
{
public:
	virtual T create() const = 0;
};

}

#endif // I_FACTORY_H
