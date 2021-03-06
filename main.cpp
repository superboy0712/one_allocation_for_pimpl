#include <iostream>
template <typename T> inline T *qGetPtrHelper(T *ptr) { return ptr; }
template <typename Ptr> inline auto qGetPtrHelper(const Ptr &ptr) -> decltype(ptr.operator->()) { return ptr.operator->(); }

#define Q_DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(qGetPtrHelper(d_ptr)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(qGetPtrHelper(d_ptr)); } \
    friend class Class##Private;

#define Q_DECLARE_PRIVATE_D(Dptr, Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(qGetPtrHelper(Dptr)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(qGetPtrHelper(Dptr)); } \
    friend class Class##Private;

#define Q_DECLARE_PUBLIC(Class)                                    \
    inline Class* q_func() { return static_cast<Class *>(q_ptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(q_ptr); } \
    friend class Class;

#define Q_D(Class) Class##Private * const d = d_func()
#define Q_Q(Class) Class * const q = q_func()

using namespace std;

template <typename T>
struct ScopedPtr {
    ScopedPtr() {}
    ScopedPtr(T *o) : p(o) {}
    ScopedPtr &operator= (T *other) {
        p = other;
        return *this;
    }
    T *data() const {return p;}
    T *operator->() const {
        return p;
    }

    T &operator*() const {
        return *p;
    }

    ~ScopedPtr() {
        if (p) delete p;
    }
private:
    T *p = nullptr;
};
class ObjectPrivate;
class Object {
public:
    void * operator new(size_t sz);
    void operator delete(void *p);
    Object();
    virtual ~Object();
protected:
    Object(ObjectPrivate& d);
    ObjectPrivate* d_ptr;
private:
    bool isHeapAllocated = false;
    Q_DECLARE_PRIVATE(Object)
};


#define printNew(sz, p) cout << __PRETTY_FUNCTION__ << "(" << sz << ") @" << p << endl;

#define printDelete(p, sz) cout << __PRETTY_FUNCTION__ << "(" << p << ") of size " << sz << endl;

#define printThis(p) cout << __PRETTY_FUNCTION__ << " @" << p << endl;

struct Test : public Object {
    Object m;
};

struct Base {
    Base() {printThis(this);}
    ~Base() {printThis(this);}
};

struct C {
    Base b;
    C() {printThis(this); throw std::exception();}
    ~C() {printThis(this);}
};

int main()
{
    cout << "Hello World!" << endl;
//    try {
//        auto a = new Object;
//        delete a;
//    } catch (...) {
//        std::cout << "what!" << endl;
//    }
//    Test t;
    try {
        Object o;

    } catch (std::exception &e) {
        std::cout << e.what() <<endl;
    }
//    try {
//        C b;

//    } catch (std::exception &e) {
//        std::cout << e.what() <<endl;
//    }
    return 0;
}

class ObjectPrivate {
public:
    ObjectPrivate() {
        printThis(this);
//        throw std::exception();
    }
    virtual ~ObjectPrivate();
    void * operator new(size_t sz);
    void* operator new(std::size_t sz, void* place);
    void operator delete(void *p, size_t sz);
    void operator delete(void *p, void *place);
    char a[100];
};

void *Object::operator new(size_t sz) {
    sz += sizeof(ObjectPrivate);
    auto ret = ::operator new(sz);
    printNew(sz, ret);
    reinterpret_cast<Object *>(ret)->isHeapAllocated = true;
//    cout << "sizeof(Object): " << sizeof(Object) << endl;
//    cout << "sizeof(ObjectPrivate): " << sizeof(ObjectPrivate) << endl;
//    cout << "sum size: " << sizeof(Object) + sizeof(ObjectPrivate) << endl;
    return ret;
}

void Object::operator delete(void *p) {
//    sz += sizeof(ObjectPrivate);
    printThis(p)
    ::operator delete(p);
}


//Object::Object() : Object(*(new ObjectPrivate)) {printThis(this);}

Object::Object() {
    printThis(this);
    // not consider padding and alighment yet
    if (isHeapAllocated) {
        d_ptr = reinterpret_cast<ObjectPrivate *>(reinterpret_cast<char *>(this) + sizeof (Object));
        new (d_ptr) ObjectPrivate;
    }
    else d_ptr = new ObjectPrivate;
//        Object(*(new ObjectPrivate));
}

Object::~Object() {
    printThis(this);
    if(d_ptr) {
        if(not isHeapAllocated) delete d_ptr;
        else d_ptr->~ObjectPrivate();
    }
    d_ptr = nullptr;

}

Object::Object(ObjectPrivate &d) : d_ptr(&d)
{
}

ObjectPrivate::~ObjectPrivate() {
    printThis(this);
}

void *ObjectPrivate::operator new(size_t sz)
{
    auto ret = ::operator new(sz);
    printNew(sz, ret);
    return ret;
}

void *ObjectPrivate::operator new(size_t sz, void *p)
{
    auto ret = ::operator new(sz, p);
    printNew(sz, ret);
    return ret;
}

void ObjectPrivate::operator delete(void *p, size_t sz)
{
    printDelete(p, sz);
    ::operator delete(p);
}

void ObjectPrivate::operator delete(void *p, void *place)
{
    printDelete(p, place);
}
