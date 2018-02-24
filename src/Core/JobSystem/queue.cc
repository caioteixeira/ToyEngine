// jobxx - C++ lightweight task library.
//
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non - commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// For more information, please refer to <http://unlicense.org/>
//
// Authors:
//   Sean Middleditch <sean.middleditch@gmail.com>

#include "queue.h"
#include "job.h"
#include "_detail/job_impl.h"
#include "_detail/queue_impl.h"
#include "_detail/task.h"

jobxx::queue::queue() : _impl(new _detail::queue_impl)
{
}

jobxx::queue::~queue()
{
    close();
    delete _impl;
}

void jobxx::queue::wait_job_actively(job const& awaited)
{
    if (awaited.complete())
    {
        return;
    }

    while (!awaited.complete())
    {
        work_one();

        _detail::task* item = nullptr;
        park_result const result = park::park_until(
            _impl->waiting, [&awaited] { return awaited.complete(); },
            awaited._impl->waiting, [this, &item] { return (item = _impl->pull_task()) != nullptr; });

        // if we were unparked by the task queue, that means that there is work
        // available. we will only have acquired the task already if it was ready
        // when the predicate was invoked, not if we actually slept. if we are
        // unparked by the task queue, that means a task is available and that no
        // other thread was unparked, so we either must process it or unpark another
        // thread in order to ensure that the work gets done in a timely manner.
        // FIXME: this addresses a race condition, but I'm really not happy with the
        // general design or interface here.
        if (result == park_result::second && item == nullptr)
        {
            item = _impl->pull_task();
        }

        // we don't want to execute work inside the
        // parkable condition, but we have to act
        // on anything polled by it.
        if (item != nullptr)
        {
            _impl->execute(item);
        }
    }
}

bool jobxx::queue::work_one()
{
    _detail::task* item = _impl->pull_task();
    if (item != nullptr)
    {
        _impl->execute(item);
        return true;
    }
    else
    {
        return false;
    }
}

void jobxx::queue::work_all()
{
    while (work_one())
    {
        // keep looping while there's work
    }
}

void jobxx::queue::work_forever()
{
    while (!_impl->closed.load(std::memory_order_relaxed))
    {
        work_all();

        _detail::task* item = nullptr;
        _impl->waiting.park_until([this, &item]
        {
            return _impl->closed.load(std::memory_order_relaxed) || (item = _impl->pull_task()) != nullptr;
        });

        // we don't want to execute work inside the
        // parkable condition, but we have to act
        // on anything polled by it.
        if (item != nullptr)
        {
            _impl->execute(item);
        }
    }
}

void jobxx::queue::close()
{
    // before closing _try_ to empty the task queue
    work_all();

    // close the park, which ensures nobody is
    // blocked on this queue, by unparking all threads
    // after we mark the queue as closed (which
    // prevents reparking).
    _impl->closed.store(true);
    _impl->waiting.unpark_all();

    // actually finish any work remaining, knowing
    // that no new work can be added to the queue
    // after closing the park.
    work_all();
}

jobxx::_detail::job_impl* jobxx::queue::_create_job()
{
    return new _detail::job_impl;
}

auto jobxx::queue::spawn_task(delegate&& work) -> spawn_result
{
    return _impl->spawn_task(std::move(work), nullptr);
}

auto jobxx::_detail::queue_impl::spawn_task(delegate work, _detail::job_impl* parent) -> spawn_result
{
    // task with no work is not allowed/useful
    if (!work)
    {
        return spawn_result::empty_function;
    }

    // we can't spawn tasks on closed queue
    if (closed.load(std::memory_order_acquire))
    {
        return spawn_result::queue_full;
    }

    if (parent != nullptr)
    {
        // increment the number of pending tasks
        // and if this is the first task, add a
        // reference so the job isn't deleted
        // before the task completes. we only do
        // this count on the first/last task to
        // avoid excessive reference counting.
        if (0 == parent->tasks++)
        {
            ++parent->refs;
        }
    }

    _detail::task* item = new _detail::task{std::move(work), parent};
    tasks.push_back(item);
    waiting.unpark_one();

    return spawn_result::success;
}

jobxx::_detail::task* jobxx::_detail::queue_impl::pull_task()
{
    jobxx::_detail::task* item = nullptr;
    tasks.pop_front(item); // on failure, item is left unmodified, e.g. nullptr
    return item;
}

void jobxx::_detail::queue_impl::execute(_detail::task* item)
{
    if (item->work)
    {
        context ctx(*this, item->parent);
        item->work(ctx);
    }

    if (item->parent != nullptr)
    {
        // decrement the number of outstanding
        // tasks. if this is the last task that
        // was pending, also remove the reference
        // count we added when the first task was
        // added, since there are no longer any
        // tasks referencing the job.
        if (0 == --item->parent->tasks)
        {
            // awaken any parked threads awaiting the job
            item->parent->waiting.unpark_all();

            if (0 == --item->parent->refs)
            {
                delete item->parent;
            }
        }
    }

    // the task is no longer needed
    delete item;
}
