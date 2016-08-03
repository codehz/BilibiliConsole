#pragma once

class NotImplementedException {
};

class RC {
    private:
        int32_t count;
    public:
        void AddRef() {
            count++;
        }
        
        int32_t Release() {
            return --count;
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

        custom_pointer(const custom_pointer<T>& pData) : t(pData.t), ref(pData.ref) {
            ref->AddRef();
        }
        
        custom_pointer<T> & operator =(const custom_pointer<T>& pData) {
            if (this != &pData) {
                if (ref->Release() == 0) {
                    Destroy(t);
                    delete ref;
                }
                
                t = pData.t;
                ref = pData.ref;
                ref->AddRef();
            }
            return *this;
        }

        operator bool() {
            return t != nullptr;
        }

        ~custom_pointer() {
            if (ref->Release() == 0) {
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
        
        static T *Create() {
            return new T;
        }

        static void Destroy(T *t) {
            delete t;
        }
};

