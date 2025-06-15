#include <iostream>
#include <stdexcept>

struct Foo {
};

template<size_t N, typename T>
struct NoMoreThenNCopy {
    NoMoreThenNCopy(T* bare_ptr)
     : ptr_(bare_ptr), count_ptr_(new size_t(1uz))
    {}
    NoMoreThenNCopy(const NoMoreThenNCopy& other) 
     : ptr_(other.ptr_), count_ptr_(other.count_ptr_) 
    {
        ++(*count_ptr_);
        if (*count_ptr_ > N) {
            throw std::runtime_error("more than N copies of NoMoreThenNCopy");
        }
    }
    NoMoreThenNCopy(NoMoreThenNCopy&& other) 
    {
        ptr_ = std::exchange(other.ptr_, nullptr);
        count_ptr_ = std::exchange(other.count_ptr_, nullptr);
    }
    NoMoreThenNCopy& operator=(const NoMoreThenNCopy& other) 
    {
        if ((--*count_ptr_) == 0uz) {
            DeleteResourses();
        }
        count_ptr_ = ++(*other.count_ptr_);
        ptr_ = other.ptr_;
        if (*count_ptr_ > N) {
            throw std::runtime_error("more than N copies of NoMoreThenNCopy");
        }
    }
    NoMoreThenNCopy& operator=(NoMoreThenNCopy&& other) 
    {
        if ((--*count_ptr_) == 0uz) {
            DeleteResourses();
        }
        ptr_ = std::exchange(other.ptr_, nullptr);
        count_ptr_ = std::exchange(other.count_ptr_, nullptr);
    }
    ~NoMoreThenNCopy() {
        if ((--*count_ptr_) == 0uz) {
            DeleteResourses();
        }
    }
    T& operator*(){
        return *ptr_;
    }
    T* operator->() {
        return ptr_;
    }

private:
    void DeleteResourses() {
        delete ptr_;
        delete count_ptr_;
    }
    T* ptr_;
    size_t* count_ptr_;
};

int main() {
    NoMoreThenNCopy<2, Foo> p{new Foo{}}; 
    auto p2 = p;    
    try {
        auto p3 = p; // some exception
    }
    catch (const std::exception& ex) {
        std::cout << "exception: " << ex.what() << "\n";
    }
}
