#pragma once

//Since thread_local not supported by iOS,
//use pthread to implement tls
#include <pthread.h>

namespace Brofiler
{
namespace Platform
{

    BF_FORCEINLINE bool TLSAlloc(unsigned long* index)
    {
        pthread_key_t key = -1;
        int result = pthread_key_create(&key, 0);
        if (result == 0){
            *index = key;
            return true;
        } else {
            return false;
        }
    }
    
    BF_FORCEINLINE void TLSFree(unsigned long index)
    {
        pthread_key_delete(index);
    }
    
    BF_FORCEINLINE bool TLSSet(unsigned long index, void* value)
    {
        return pthread_setspecific(index, value) == 0;
    }
    
    BF_FORCEINLINE void* TLSGet(unsigned long index)
    {
        return pthread_getspecific(index);
    }
    
    template<typename T>
    class TLSStorage
    {
        unsigned long m_key;
    public:
        TLSStorage() { TLSAlloc(&m_key); }
        ~TLSStorage() {
            free();
            TLSFree(m_key);
        }
        
        bool operator !()
        {
            return TLSGet(m_key) == NULL;
        }
        
        T* operator ->()
        {
            return static_cast<T*>(TLSGet(m_key));
        }
        
        inline T* get()
        {
            return static_cast<T*>(TLSGet(m_key));
        }
        
        T* alloc(int size)
        {
            T* p = get();
            if (p) {
                delete [] p;
            }
            p = new T[size];
            TLSSet(m_key, p);
            return p;
        }
        
        void free()
        {
            T* p = get();
            if (p)
            {
                delete [] p;
                TLSSet(m_key, NULL);
            }
        }
    };
}
}
