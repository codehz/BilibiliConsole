#pragma once

//JUST FOR SIMPLIFY DESTORY
template<typename T>
class custom_pointer {
    private:
        T * t = nullptr;
    public:
        custom_pointer() : t(Create()) {}
        template<typename ...Values> custom_pointer(Values ...v) : t(Create(v...)) {}
        custom_pointer(const custom_pointer<T>& ref) : t(ref.t) {}
        
        custom_pointer<T> & operator =(const custom_pointer<T>& ref) {
            if (ref.t != t) {
                if (t)
                    Destroy(t);
                t = ref.t;
            }
            return *this;
        }
        
        template<typename F>
        custom_pointer<T> & operator =(F *f) {
            t = Create(f);
            return *this;
        }

        operator bool() {
            return t != nullptr;
        }

        ~custom_pointer() {
            if (t)
                Destroy(t);
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
            throw;
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

