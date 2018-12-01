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
    void operator delete(void *p, size_t sz);
    Object();
    virtual ~Object();
protected:
    Object(ObjectPrivate& d);
    ScopedPtr<ObjectPrivate> d_ptr;
private:
    Q_DECLARE_PRIVATE(Object)
};


#define printNew(sz, p) cout << __PRETTY_FUNCTION__ << "(" << sz << ") @" << p << endl;

#define printDelete(p, sz) cout << __PRETTY_FUNCTION__ << "(" << p << ") of size " << sz << endl;

#define printThis(p) cout << __PRETTY_FUNCTION__ << " @" << p << endl;
int main()
{
    cout << "Hello World!" << endl;
    auto a = new Object;
    delete a;
    return 0;
}

class ObjectPrivate {
public:
    ObjectPrivate() {
        printThis(this);
    }
    virtual ~ObjectPrivate() {
        printThis(this);
    }
    void * operator new(size_t sz);
    void operator delete(void *p, size_t sz);
    char a[100];
};

void *Object::operator new(size_t sz) {
    sz += sizeof(ObjectPrivate);
    auto ret = ::operator new(sz);
    printNew(sz, ret);
    return ret;
}

void Object::operator delete(void *p, size_t sz) {
    sz += sizeof(ObjectPrivate);
    printDelete(p, sz)
    ::operator delete(p);
}


Object::Object() : Object(*(new ObjectPrivate)) {printThis(this);}

Object::~Object() {
    printThis(this);
}

Object::Object(ObjectPrivate &d) : d_ptr(&d)
{
}

void *ObjectPrivate::operator new(size_t sz)
{
    auto ret = ::operator new(sz);
    printNew(sz, ret);
    return ret;
}

void ObjectPrivate::operator delete(void *p, size_t sz)
{
    printDelete(p, sz)
    ::operator delete(p);
}
