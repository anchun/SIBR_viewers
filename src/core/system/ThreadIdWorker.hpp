
#ifndef __SIBR_SYSTEM_THREADIDWORKER_HPP__
# define __SIBR_SYSTEM_THREADIDWORKER_HPP__

# include <atomic>
# include <thread>
# include <mutex>
# include <queue>

# include "core/system/Config.hpp"

namespace sibr
{
	///
	/// Class used to work concurrently on multiple tasks/
	///
	/// INSTRUCTION
	/// The only shared object is a queue (TaskIds) that
	/// contains ids of remaining tasks to perform.
	///
	/// Here is an example of 'pattern' where you could use
	/// it:
	///
	/// Typically, you use this id to access a const array
	/// (input) and write results to another array using once
	/// again this id. The output array is already resized
	/// at the begin so that you can freely modify its
	/// element without hurt other threads.
	///
	/// \ingroup sibr_system
	///
	class /*SIBR_SYSTEM_EXPORT*/ ThreadIdWorker : public std::thread
	{
		/**
		Code Example:

		std::vector<ThreadIdWorker>	workers(MASKPATCH_NBTHREADS);

		// Launch all threads
		for (ThreadIdWorker& t: workers)
		t = std::move(ThreadIdWorker(taskId, workFunc));

		// Wait for all threads
		for (ThreadIdWorker& t: workers)
		if (t.joinable())
		t.join();
		**/
	public:
		typedef	std::queue<uint>	TaskIds;
	public:
		/// Build an empty worker (placeholder)
		ThreadIdWorker( void );
		/// Move constructor
		ThreadIdWorker( ThreadIdWorker&& other );

		/// Build from a list of task ids and a function to perform
		/// in parallel. 'func' takes a task index in parameter and
		/// return either FALSE for signaling the worker to stop or
		/// TRUE for keep going.
		ThreadIdWorker( TaskIds& ids, std::function<bool(uint)> func );

		/// Move assign
		ThreadIdWorker& operator =( ThreadIdWorker&& other );

	private:
		ThreadIdWorker( const ThreadIdWorker& ); // deleted

		/// Will pull next tasks or automatically stop
		void		taskPuller( TaskIds& ids, std::function<bool(uint)> func );

		SIBR_SYSTEM_EXPORT static std::mutex	g_mutex;	///< used to protect the common shared TaskIds list
	};

	///// INLINES /////
	ThreadIdWorker::ThreadIdWorker( void ) {
	}

	ThreadIdWorker::ThreadIdWorker( ThreadIdWorker&& other ) :
		std::thread(std::move((std::thread&)other)) {
	}

	ThreadIdWorker::ThreadIdWorker( TaskIds& ids, std::function<bool(uint)> func )
		: std::thread( [this, &ids, &func]() { taskPuller(ids, std::move(func)); } ) {
	}

	ThreadIdWorker& ThreadIdWorker::operator =( ThreadIdWorker&& other ) {
		((std::thread*)this)->operator=(std::move(other)); return *this;
	}

	void		ThreadIdWorker::taskPuller( TaskIds& ids, std::function<bool(uint)> func ) {
		uint id = 0;
		bool stop = false;
		while (!stop)
		{
			{ // Pop next id
				std::lock_guard<std::mutex>	lock(g_mutex);
				stop = ids.empty();

				if (!stop)
				{
					id = ids.front();
					ids.pop();
				}
			}

			if (!stop)
				stop = !func(id);
		}
	}

} // namespace sibr

#endif // __SIBR_SYSTEM_THREADIDWORKER_HPP__
