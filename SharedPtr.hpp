#ifndef SHARE_P
#define SHARE_P

#include <mutex>
#include <iostream>
#include <functional>

namespace cs540{

	template<typename T>
	class SharedPtr{
		public:
			SharedPtr();
			template <typename U> explicit SharedPtr(U *);
			SharedPtr(const SharedPtr &p);
			template <typename U> SharedPtr(const SharedPtr<U> &p);

			SharedPtr(SharedPtr &&p);
			template <typename U> SharedPtr(SharedPtr<U> &&p);

			SharedPtr &operator=(const SharedPtr &p);
			template <typename U> SharedPtr<T> &operator=(const SharedPtr<U> &p);

			SharedPtr &operator=(SharedPtr &&p);
			template <typename U> SharedPtr &operator=(SharedPtr<U> &&p);

			~SharedPtr();

			void reset();
			template <typename U> void reset(U *p);

			T *get() const;
			T &operator*() const;
			T *operator->() const;
			explicit operator bool() const;

			template<typename U>
			friend class SharedPtr;

			template <typename T1, typename U1>
			friend SharedPtr<T1> static_pointer_cast(const SharedPtr<U1> &sp);

			template <typename T1, typename U1>
			friend SharedPtr<T1> dynamic_pointer_cast(const SharedPtr<U1> &sp);

		private:
			T* internalPointer;
			std::mutex* mutex;
			int* numRef;
			std::function<void()> del;
			template<typename U> SharedPtr(const SharedPtr<U> &p, T* pointerCasted);
	};

	template<typename T>
	template<typename U>
	SharedPtr<T>::SharedPtr(const SharedPtr<U> &p, T* pointerCasted){
		std::cout << "CAST const" << std::endl;
		internalPointer = nullptr;
		if(p != nullptr){
			p.mutex->lock();
			mutex = p.mutex;
			numRef = p.numRef;
			++(*numRef);
			internalPointer = pointerCasted;
			del = p.del;
			mutex->unlock();
		}
	}

	template<typename T>
	SharedPtr<T>::SharedPtr(){
		std::cout << "null const" << std::endl;
		internalPointer = nullptr;
		numRef = nullptr;
		mutex = nullptr;
		del = nullptr;
	}

	template<typename T>
	template<typename U>
	SharedPtr<T>::SharedPtr(U* p){
		std::cout << "raw pointer const" << std::endl;
		mutex = new std::mutex();
		internalPointer = p;
		numRef = new int(1);
		del = [p](){
			delete static_cast<U*>(p);
		};
	}

	template<typename T>
	SharedPtr<T>::SharedPtr(const SharedPtr &p){
		std::cout << "no member template copy const" << std::endl;
		internalPointer = nullptr;
		if(p != nullptr){
			p.mutex->lock();
			mutex = p.mutex;
			numRef = p.numRef;
			++(*numRef);
			internalPointer = p.internalPointer;
			del = p.del;
			mutex->unlock();
		}
	}

	template<typename T>
	template<typename U>
	SharedPtr<T>::SharedPtr(const SharedPtr<U> &p){
		std::cout << "yes member template copy const" << std::endl;
		internalPointer = nullptr;
		if(p != nullptr){
			p.mutex->lock();
			mutex = p.mutex;
			numRef = p.numRef;
			++(*numRef);
			internalPointer = p.internalPointer;
			del = p.del;
			mutex->unlock();
		}
	}

	template<typename T>
	SharedPtr<T>::SharedPtr(SharedPtr &&p){
		std::cout << "no member template move const" << std::endl;
		mutex = p.mutex;
		p.mutex->lock();
		internalPointer = p.internalPointer;
		numRef = p.numRef;
		del = p.del;
		p.mutex->unlock();

		p.mutex = nullptr;
		p.internalPointer = nullptr;
		p.numRef = nullptr;
		p.del = nullptr;
	}

	template<typename T>
	template<typename U>
	SharedPtr<T>::SharedPtr(SharedPtr<U> &&p){
		std::cout << "yes member template move const" << std::endl;
		mutex = p.mutex;
		p.mutex->lock();
		internalPointer = p.internalPointer;
		numRef = p.numRef;
		del = p.del;
		p.mutex->unlock();

		p.mutex = nullptr;
		p.internalPointer = nullptr;
		p.numRef = nullptr;
		p.del = nullptr;
	}

	template<typename T>
	SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr &p){
		std::cout << "no member template op=" << std::endl;
		if(this->internalPointer == p.internalPointer) {return (*this);}
		
		if((*this) != nullptr){
			mutex->lock();
			--(*numRef);
			if(*numRef == 0){
				mutex->unlock();
				std::cout << "destroying" << std::endl;
				delete numRef;
				delete mutex;
				del();
			}
			else{
				mutex->unlock();
			}
		}

		mutex = p.mutex;
		numRef = p.numRef;
		internalPointer = p.internalPointer;
		del = p.del;
		if(p != nullptr){
			mutex->lock();
			++(*numRef);
			mutex->unlock();
		}
		return (*this);
	}

	template<typename T>
	template<typename U> 
	SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<U> &p){
		std::cout << "yes member template op=" << std::endl;
		if(this->internalPointer == p.internalPointer) {return (*this);}
		
		if((*this) != nullptr){
			mutex->lock();
			--(*numRef);
			if(*numRef == 0){
				mutex->unlock();
				std::cout << "destroying" << std::endl;
				delete numRef;
				delete mutex;
				del();
			}
			else{
				mutex->unlock();
			}
		}

		mutex = p.mutex;
		numRef = p.numRef;
		internalPointer = p.internalPointer;
		del = p.del;
		if(p != nullptr){
			mutex->lock();
			++(*numRef);
			mutex->unlock();
		}
		return (*this);
	}

	template<typename T>
	SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr &&p){
		std::cout << "no member template move op=" << std::endl;

		if((*this) != nullptr){
			mutex->lock();
			--(*numRef);
			if(*numRef == 0){
				mutex->unlock();
				std::cout << "destroying" << std::endl;
				delete mutex;
				delete numRef;
				del();
				del = nullptr;
			}
			else{
				mutex->unlock();
			}	
		}


		mutex = p.mutex;
		mutex->lock();
		internalPointer = p.internalPointer;
		numRef = p.numRef;
		del = p.del;
		mutex->unlock();

		p.mutex = nullptr;
		p.internalPointer = nullptr;
		p.numRef = nullptr;
		p.del = nullptr;
		return (*this);
	}

	template<typename T>
	template<typename U>
	SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U> &&p){
		std::cout << "yes member template move op=" << std::endl;

		if((*this) != nullptr){
			mutex->lock();
			--(*numRef);
			if(*numRef == 0){
				mutex->unlock();
				std::cout << "destroying" << std::endl;
				delete mutex;
				delete numRef;
				del();
				del = nullptr;
			}
			else{
				mutex->unlock();
			}	
		}

		mutex = p.mutex;
		mutex->lock();
		internalPointer = p.internalPointer;
		numRef = p.numRef;
		del = p.del;
		mutex->unlock();

		p.mutex = nullptr;
		p.internalPointer = nullptr;
		p.numRef = nullptr;
		p.del = nullptr;
		return (*this);
	}



	template<typename T>
	SharedPtr<T>::~SharedPtr(){
		std::cout << "destructor" << std::endl;
		if((*this) != nullptr){
			mutex->lock();
			--(*numRef);
			if(*numRef == 0){
				mutex->unlock();
				std::cout << "destroying" << std::endl;
				delete mutex;
				delete numRef;
				del();
				del = nullptr;
			}
			else{
				mutex->unlock();
			}
		}
	}

	template<typename T>
	void SharedPtr<T>::reset(){
		if((*this) != nullptr){
			mutex->lock();
			--(*numRef);
			if(*numRef == 0){
				mutex->unlock();
				std::cout << "destroying" << std::endl;
				delete mutex;
				delete numRef;
				del();
			}
			else{
				mutex->unlock();
			}
		}
		mutex = nullptr;
		internalPointer = nullptr;
		numRef = nullptr;
		del = nullptr;
	}

	template<typename T>
	template<typename U>
	void SharedPtr<T>::reset(U *p){
		if((*this) != nullptr){
			mutex->lock();
			--(*numRef);
			if(*numRef == 0){
				mutex->unlock();
				std::cout << "destroying" << std::endl;
				delete mutex;
				delete numRef;
				del();
			}
			else{
				mutex->unlock();
			}
		}
		mutex = new std::mutex();
		internalPointer = p;
		numRef = new int(1);
		del = [p](){ //capture it
			delete static_cast<U*>(p);
		};
	}

	template<typename T>
	T* SharedPtr<T>::get() const{
		return internalPointer;
	}

	template<typename T>
	T& SharedPtr<T>::operator*() const{
		return *internalPointer;
	}

	template<typename T>
	T* SharedPtr<T>::operator->() const{
		return internalPointer;
	}

	template<typename T>
	SharedPtr<T>::operator bool() const{
		return (internalPointer != nullptr);
	}

	template <typename T1, typename T2>
	bool operator==(const SharedPtr<T1> &p1, const SharedPtr<T2> &p2){
		return p1.get() == p2.get();
	}

	template <typename T>
	bool operator==(const SharedPtr<T> &p1, std::nullptr_t){
		return p1.get() == nullptr;
	}

	template <typename T>
	bool operator==(std::nullptr_t, const SharedPtr<T> &p1){
		return p1.get() == nullptr;
	}

	template <typename T1, typename T2>
	bool operator!=(const SharedPtr<T1> &p1, const SharedPtr<T2> &p2){
		return !(p1 == p2);
	}

	template <typename T>
	bool operator!=(const SharedPtr<T> &p1, std::nullptr_t){
		return !(p1 == nullptr);
	}

	template <typename T>
	bool operator!=(std::nullptr_t, const SharedPtr<T> &p1){
		return !(p1 == nullptr);
	}

	template <typename T1, typename U1>
	SharedPtr<T1> static_pointer_cast(const SharedPtr<U1> &sp){
		auto p = static_cast<T1*>(sp.get());
    	return SharedPtr<T1>(sp, p);
	}

	template <typename T1, typename U1>
	SharedPtr<T1> dynamic_pointer_cast(const SharedPtr<U1> &sp){
		if (auto p = dynamic_cast<T1*>(sp.get())) {
	        return SharedPtr<T1>(sp, p);
	    } else {
	        return SharedPtr<T1>();
	    }
	}
}

#endif