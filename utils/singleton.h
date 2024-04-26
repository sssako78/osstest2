
#ifndef SINGLETON_H
#define SINGLETON_H


template < typename T >
class Singleton
{
protected:
    Singleton()
    {

    }
    virtual ~Singleton()
    {

    }

public:
    static T * GetInstance()
    {
        if (m_pInstance == nullptr)
        {
            static T* pInst = new T;
            m_pInstance = pInst;
        }
        return m_pInstance;
    };

    static void DestroyInstance()
    {
        if (m_pInstance)
        {
            delete m_pInstance;
            m_pInstance = nullptr;
        }
    };

private:
    static T * m_pInstance;
};
template <typename T> T * Singleton<T>::m_pInstance = 0;


#endif // ! defined __SINGLETON_H__


