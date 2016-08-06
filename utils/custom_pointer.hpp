#pragma once
#include <exception>

struct NotImplementedException : public std::logic_error {
};

class RC {
    private:
        int32_t count = 0;
    public:
        void AddRef() {
            count++;
        }
        
        int32_t Release() {
            if (--count < 0)
                throw std::logic_error("count < 0");
            return count;
        }
};

//JUST FOR SIMPLIFY DESTORY
template<typename T>
class custom_pointer {
    private:
        T *t = nullptr;
        RC *ref;
    public:
        custom_pointer() : t(Create()), ref(new RC) {
            ref->AddRef();
        }

        template<typename ...Values> custom_pointer(Values ...v) : t(Create(v...)), ref(new RC) {
            ref->AddRef();
        }

        custom_pointer(const custom_pointer<T> &pData) : t(pData.t), ref(pData.ref) {
            ref->AddRef();
        }

        custom_pointer(custom_pointer<T> &&pData) : t(pData.t), ref(pData.ref) {
            pData.ref = nullptr;
            pData.t = nullptr;
        }

        custom_pointer<T> & operator =(const custom_pointer<T> &&pData) {
            if (this != &pData) {
                if (ref && ref->Release() == 0) {
                    Destroy(t);
                    delete ref;
                }
                
                t = pData.t;
                ref = pData.ref;
                ref->AddRef();
            }
            return *this;
        }

        custom_pointer<T> & operator =(custom_pointer<T> &&pData) {
            if (this != &pData) {
                t = pData.t;
                ref = pData.ref;
                pData.ref = nullptr;
                pData.t = nullptr;
            }
            return *this;
        }

        operator bool() {
            return t != nullptr;
        }

        ~custom_pointer() {
            if (ref && ref->Release() == 0) {
                Destroy(t);
                delete ref;
            }
        }

        T &operator *() {
            return *t;
        }

        T *operator ->() {
            return t;
        }
        
        operator T *() {
            return t;
        }
        
        template<typename ...Values> T *Create(Values ...v) {
            throw NotImplementedException();
        }
        
        T *Create(decltype(nullptr) n) {
            return nullptr;
        }

        T *Create(T *t) {
            return t;
        }
        
        T *Create() {
            return new T;
        }

        void Destroy(T *t) {
            delete t;
        }
};

template<typename T>
custom_pointer<T> make_custom(T *pointer) {
    return custom_pointer<T>(pointer);
}

