#pragma once

// Thread-safe sinleton
template <typename T>
struct Singleton
{
	struct object_creator
	{
		object_creator() { Singleton<T>::instance(); }
		inline void do_nothing()const {}
	};

	static object_creator create_object;

public:
	static T& instance()
	{
		static T obj;
		//fool compiler: ensure the constructor will be called
		create_object.do_nothing();
		return obj;
	}

};
template <typename T> typename Singleton<T>::object_creator Singleton<T>::create_object;