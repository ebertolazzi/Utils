/**
 */

///
/// file: ThreadPool4.hxx
///

namespace Utils {

  class ThreadPool4 : public ThreadPoolBase {

    typedef double real_type;

    typedef tp::Queue::TaskData TaskData;

    std::atomic<bool>        m_done;
    std::atomic<unsigned>    m_running_task;
    std::atomic<unsigned>    m_running_thread;
    std::vector<std::thread> m_worker_threads;
    tp::Queue                m_work_queue; // not thread safe
    UTILS_MUTEX              m_work_on_queue_mutex;

    TicToc                   m_tm;
    std::vector<real_type>   m_job_ms;
    std::vector<real_type>   m_pop_ms;
    std::vector<unsigned>    m_n_job;
    real_type                m_push_ms;

    inline
    void
    nano_sleep() const
    #ifdef UTILS_OS_WINDOWS
    { Sleep(0); }
    #else
    { sleep_for_nanoseconds(1); }
    #endif

    TaskData *
    pop_task() {
      m_work_on_queue_mutex.lock();
      while ( m_work_queue.empty() ) {
        m_work_on_queue_mutex.unlock();
        nano_sleep();
        m_work_on_queue_mutex.lock();
      }
      TaskData * task = m_work_queue.pop();
      ++m_running_task; // must be incremented in the locked part
      m_work_on_queue_mutex.unlock();
      return task;
    }

    void
    push_task( TaskData * task ) {
      m_tm.tic();
      m_work_on_queue_mutex.lock();
      while ( m_work_queue.is_full() ) {
        m_work_on_queue_mutex.unlock();
        nano_sleep();
        m_work_on_queue_mutex.lock();
      }
      m_work_queue.push( task );
      m_work_on_queue_mutex.unlock();
      m_tm.toc();
      m_push_ms += m_tm.elapsed_ms();
    }

    void
    worker_thread(
      real_type & pop_ms,
      real_type & job_ms,
      unsigned  & n_job
    ) {
      TicToc m_tm;
      ++m_running_thread;
      while ( !m_done ) {
        // ----------------------------
        m_tm.tic();
        TaskData * task = pop_task();
        m_tm.toc();
        pop_ms += m_tm.elapsed_ms();
        // ----------------------------
        m_tm.tic();
        (*task)(); // run and delete task;
        m_tm.toc();
        job_ms += m_tm.elapsed_ms();
        // ----------------------------
        --m_running_task;
        ++n_job;
        // ----------------------------
        std::this_thread::yield();
      }
      --m_running_thread;
    }

    void
    create_workers( unsigned thread_count ) {
      m_worker_threads.clear();
      m_worker_threads.reserve(thread_count);
      m_job_ms.resize( std::size_t(thread_count) );
      m_pop_ms.resize( std::size_t(thread_count) );
      m_n_job.resize( std::size_t(thread_count) );
      std::fill( m_job_ms.begin(), m_job_ms.end(), 0 );
      std::fill( m_pop_ms.begin(), m_pop_ms.end(), 0 );
      std::fill( m_n_job.begin(), m_n_job.end(), 0 );
      m_push_ms = 0;
      m_done    = false;
      try {
        for ( unsigned i=0; i<thread_count; ++i )
          m_worker_threads.push_back(
            std::thread(
              &ThreadPool4::worker_thread, this,
              std::ref(m_pop_ms[i]),
              std::ref(m_job_ms[i]),
              std::ref(m_n_job[i])
            )
          );
      } catch(...) {
        m_done = true;
        throw;
      }
    }

  public:

    explicit
    ThreadPool4(
      unsigned thread_count   = std::thread::hardware_concurrency(),
      unsigned queue_capacity = 0
    )
    : m_done(false)
    , m_running_task(0)
    , m_running_thread(0)
    , m_work_queue( queue_capacity == 0 ? 4 * (thread_count+1) : queue_capacity )
    {
      create_workers( thread_count );
    }

    virtual ~ThreadPool4() { join(); }

    void resize( unsigned thread_count ) override { resize( thread_count, 0 ); }

    void
    resize( unsigned thread_count, unsigned queue_capacity = 0 ) {
      join();
      if ( queue_capacity == 0 ) queue_capacity = 4 * (thread_count+1);
      m_work_queue.resize( queue_capacity );
      create_workers( thread_count );
    }

    void
    exec( std::function<void()> && fun ) override
    { push_task( new TaskData(std::move(fun)) ); }

    void
    wait() override
    { while ( !m_work_queue.empty() || m_running_task > 0 ) nano_sleep(); }

    void
    join() {
      wait(); // finish all the running task
      m_done = true;
      unsigned i = m_running_thread;
      while ( i-- > 0 ) push_task( new TaskData([](){}) );
      while ( m_running_thread > 0 ) nano_sleep();
      m_work_queue.clear(); // remove spurious (null task) remained
      for ( std::thread & w : m_worker_threads ) { if (w.joinable()) w.join(); }
      m_worker_threads.clear(); // destroy the workers threads vector
    }

    void
    info( ostream_type & s ) const override {
      unsigned nw = unsigned(m_pop_ms.size());
      for ( unsigned i = 0; i < nw; ++i ) {
        unsigned njob = m_n_job[i];
        fmt::print( s,
          "Worker {:2}, #job = {:4}, "
          "[job {:.6} mus, POP {:.6} mus]\n",
          i, njob,
          1000*m_job_ms[i]/njob,
          1000*m_pop_ms[i]/njob
        );
      }
      fmt::print( s, "PUSH {:10.6} ms\n\n", m_push_ms );
    }

    unsigned thread_count()   const override { return unsigned(m_worker_threads.size()); }
    unsigned queue_capacity() const          { return m_work_queue.capacity(); }

    char const * name() const override { return "ThreadPool4"; }
  };

}

///
/// eof: ThreadPool4.hxx
///
