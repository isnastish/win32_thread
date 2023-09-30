#pragma once

#include <Windows.h>
#include <stdint.h>
#include <tuple>
#include <functional>
#include <utility>
#include <variant>
#include <memory>

class Win32Thread
{
private:
	typedef int32_t bool32;

	struct Thread
	{
		DWORD id;
		HANDLE handle;
	};

	template<class TupleType, size_t...Indices>
	static DWORD __stdcall thread_proc_(LPVOID arg_list)
	{
		TupleType& tuple = *static_cast<TupleType*>(arg_list);
		std::invoke(std::move(std::get<Indices>(tuple))...);
		return 0;
	}

	template<class TupleType, size_t...Indices>
	static constexpr auto create_thread_proc_(std::index_sequence<Indices...>)
	{
		return &thread_proc_<TupleType, Indices...>;
	}
		
	template<typename Proc, typename... Args>
	void create_thread(Proc&& proc, Args&&... arg_list)
	{
		typedef std::tuple<Proc, Args...> TupleType;
		std::unique_ptr<TupleType> tuple_ptr = 
			std::make_unique<TupleType>(TupleType(std::forward<Proc>(proc), std::forward<Args>(arg_list)...));

		auto thread_proc = create_thread_proc_<TupleType>(std::make_index_sequence<1 + sizeof...(Args)>());
		thread.handle = CreateThread(NULL, 0, thread_proc, tuple_ptr.get(), 0, &thread.id);
		if (!thread.handle)
			throw;
		tuple_ptr.release();
	}

private:
	bool32 is_finished() const
	{
		return(thread.id == 0);
	}

public:
	Win32Thread() {}

	template<typename Proc, typename... Args>
	Win32Thread(Proc&& proc, Args&&... arg_list)
	{
		create_thread(std::forward<Proc>(proc), std::forward<Args>(arg_list)...);
	}

	~Win32Thread()
	{
		if (!is_finished())
			throw;
	}

	Win32Thread(const Win32Thread&) = delete;
	Win32Thread& operator=(const Win32Thread&) = delete;

	Win32Thread(Win32Thread&& from)
	{
		thread = from.thread;
		from.thread = {};
	}

	Win32Thread& operator=(Win32Thread&& from)
	{
		if (!is_finished())
			throw;

		thread = from.thread;
		from.thread = {};

		return *this;
	}

	void wait_for_thread()
	{
		if (!is_finished())
		{
			WaitForMultipleObjects(1, &thread.handle, FALSE, INFINITE);
			CloseHandle(thread.handle);

			thread = {};
		}
	}

	uint32_t system_cores() const
	{
		SYSTEM_INFO sys_info = {};
		GetSystemInfo(&sys_info);
		uint32_t result = sys_info.dwNumberOfProcessors;
		return result;
	}

	static uint32_t current_thread_id()
	{
		uint32_t result = GetCurrentThreadId();
		return result;
	}
	
private:
	Thread thread{};
};