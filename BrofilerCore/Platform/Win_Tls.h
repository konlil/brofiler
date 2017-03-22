#pragma once

namespace Brofiler
{
namespace Platform
{
    BF_FORCEINLINE bool TLSAlloc(unsigned long* index)
    {
        unsigned long i = TlsAlloc();
        if ( i == TLS_OUT_OF_INDEXES )
            return false;
        else{
            *index = i;
            return true;
        }
    }
    
    BF_FORCEINLINE void TLSFree(unsigned long index)
    {
        TlsFree(index);
    }
    
    BF_FORCEINLINE bool TLSSet(unsigned long index, void* value)
    {
        return TlsGetValue(index);
    }
    
    BF_FORCEINLINE void* TLSGet(unsigned long index)
    {
        return TlsSetValue(index, value) != 0;
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
