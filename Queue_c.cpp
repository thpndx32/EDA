// Copyright
#include <iostream>
#include <mutex>
#include <queue>
#include <chrono>
#include <sstream>
#include <thread>
using namespace std;
mutex mtx;//mutex para output
template <typename T>
class ConcurrentQueue {
 public:
  void push(const T &data) { 
    //queue_mutex_.lock(); // se puede activar estos locks pero acabaria convirtiendo el proceso en lineal
    queue_.push(data);
    //queue_mutex_.unlock();
     }

  T pop() {
    T result = queue_.front();
    queue_.pop();
    return result;
  }

  int size() const {
    return queue_.size();
  }
  std::mutex queue_mutex_;
 private:
  std::queue<T> queue_;
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
      mtx.lock();//mutex para que lo que se imprime este ordenado
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
      if (queue_->size()>0&&queue_->queue_mutex_.try_lock()) {//se tiene que verificar que haya al menos un elemento y que no haya otro thread realizando una operacion
      std::stringstream stream;
      stream << "Consumer: " << id_ << " Data: " << queue_->pop().c_str();
      queue_->queue_mutex_.unlock();
      mtx.lock();//mutex para que lo que se imprime este ordenado
      std::cout << stream.str()<< " QUEUE_SIZE: " << queue_->size()<<endl;
      mtx.unlock();}
      else {
        this_thread::sleep_for(std::chrono::milliseconds(10));// si no hay mas elementos en la cola esperar por 10 milisegundos
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
  std::cin >> stop;//ingresar un valor para detener todo el proceso, cuando se detiene por este metodo y no Ctrl+c en las ultimas impresiones se encuentran multiples
                  //datos basura, al hacerlo con Ctrl+c se puede encontrar a lo mucho un par de datos basura

  return 0;
}
