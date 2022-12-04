// Copyright
#include <iostream>
#include <mutex>
#include <queue>
#include <chrono>
#include <sstream>
#include <thread>
using namespace std;
mutex mtx;
mutex mtx_c;
template <typename T>
class ConcurrentQueue {
 public:
  void push(const T &data) { 
    //queue_mutex_.lock();
    queue_.push(data);
    //queue_mutex_.unlock();
     }

  T pop() {
    queue_mutex_.lock();
    T result = queue_.front();
    queue_.pop();
    queue_mutex_.unlock();
    return result;
  }

  int size() const {
    return queue_.size();
  }

 private:
  std::queue<T> queue_;
  std::mutex queue_mutex_;
};

class Producer {
 public:
  Producer(unsigned int id, ConcurrentQueue<std::string> *queue)
      : id_(id), queue_(queue) {}

  void operator()() {
    int data = 0;
    while (true) {
      std::stringstream stream;
      stream<<data++<< " Producer: " << id_ ;
      queue_->push(stream.str());
      mtx.lock();
      cout << "Producer: " << id_ << " Data: " << data-1 <<" QUEUE_SIZE: " << queue_->size()<< std::endl;
      mtx.unlock();
    }
  }

 private:
  unsigned int id_;
  ConcurrentQueue<std::string> *queue_;
};

class Consumer {
 public:
  Consumer(unsigned int id, ConcurrentQueue<std::string> *queue)
      : id_(id), queue_(queue) {}

  void operator()() {
    while (true) {
      if (queue_->size()>0&&mtx_c.try_lock()) {
      std::stringstream stream;
      stream << "Consumer: " << id_ << " Data: " << queue_->pop().c_str();
      mtx_c.unlock();
      mtx.lock();
      std::cout << stream.str()<< " QUEUE_SIZE: " << queue_->size()<<endl;
      mtx.unlock();}
      else {
        this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }
  }

 private:
  unsigned int id_;
  ConcurrentQueue<std::string> *queue_;
};

int main(int argc, char *argv[]) {
  if (argc != 3) {
    return 1;
  }
  int number_producers = std::stoi(argv[1]);
  int number_consumers = std::stoi(argv[2]);

  ConcurrentQueue<std::string> queue;

  std::vector<std::thread *> producers;
  for (unsigned int i = 0; i < number_producers; ++i) {
    std::thread *producer_thread = new std::thread(Producer(i, &queue));
    producers.push_back(producer_thread);
  }

  std::vector<std::thread *> consumers;
  for (unsigned int i = 0; i < number_consumers; ++i) {
    std::thread *consumer_thread = new std::thread(Consumer(i, &queue));
    consumers.push_back(consumer_thread);
  }

  int stop;
  std::cin >> stop;
  // join

  return 0;
}