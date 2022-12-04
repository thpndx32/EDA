#include <iostream>
#include <mutex>
#include <thread>
#include <fstream>
#include <random>

using namespace std;
mutex mtx;//mutex para que los output se hagan de forma ordenada
ofstream ofs,deleted;

template <class T> class nodo {
public:
    mutex mtx_n;//mutex por cada nodo
  T valor;
  nodo<T> *next;
  nodo(T v, nodo<T> *n = nullptr) {
    valor = v;
    next = n;
  }
};

template <class T> struct LE {
  nodo<T> *head = nullptr;
  mutex mtx_h;//mutex para que las operaciones add,del y find no causen problemas entre si
  void print();
  bool find(T v, nodo<T> *pos);
  bool find_withoutm(T v, nodo<T>*& pos);
  bool add(T v);
  bool del(T v);
};


int get_random(int low, int high) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distribution(low, high);
  return distribution(gen);
}

template <class T> struct Add{
  private:
    int min,max;
    LE<T>* ptr;
  public:
    Add<T>(int m, int M, LE<T>* p) {
      min = m;
      max = M;
      ptr = p;
    }
    void operator() (int operaciones){
      for(int i = 0; i<operaciones; i++){
          int number = get_random(min, max);
          //cout<<"Inserting "<<number<<endl;
          if(!ptr->add(number)){ 
            mtx.lock();
            ofs <<"NO añadio "<<number<<endl;
            mtx.unlock();
        } else {
          mtx.lock();
          ofs <<"Añadio  "<<number<<endl;
          mtx.unlock();
        }
      }
    }
};

template <class T> struct Find{
  private:
    int min,max;
    LE<T>* ptr;
  public:
    Find<T>(int m, int M, LE<T>* p) {
      min = m;
      max = M;
      ptr = p;
    }
    void operator() (int operaciones){
      for(int i = 0; i<operaciones; i++){
          //cout <<"find iteration->"<< i << endl;
          //cout<<"here "<<i<<" "<<operaciones<<endl;
          int x=get_random(min,max);
          if(!ptr->find(x,ptr->head)) {
            mtx.lock();
            ofs <<"NO encontro "<<x<<endl;
            mtx.unlock();
        } else {
          mtx.lock();
          ofs <<"Encontro  "<<x<<endl;
          mtx.unlock();
        }
      }
    }
};

template <class T> struct Remove{
  private:
    int min,max;
    LE<T>* ptr;
  public:
    Remove<T>(int m, int M, LE<T>* p) {
      min = m;
      max = M;
      ptr = p;
    }
    void operator() (int operaciones){
      for(int i = 0; i<operaciones; i++){
          //cout<<"here "<<i<<" "<<operaciones<<endl;
          int x = get_random(min,max);
          if(!ptr->del(x)){
             mtx.lock();
             ofs <<"NO borro "<<x<<endl;
             mtx.unlock();
            }else{
              mtx.lock();
              deleted<<"Borro  "<<x<<endl;
              ofs<<"ELEMENT DELETED"<<endl;
              mtx.unlock();
            }
      }
    }
};

template <class T> void LE<T>::print() {
  ofs << "Head->";
  for (nodo<T> *p = head; p; p = p->next)
    ofs << p->valor << "->";
  ofs << "NULL" << endl;
}

template <class T> bool LE<T>::find(T v, nodo<T> *pos) {
  bool encontrado = false;
  pos = nullptr;
  mtx_h.lock();//lock para no iniciar una busqueda en caso se este realizando una modificacion en la LE mediante add o delete, no afecta a la busqueda que realizan estas 2 operaciones
  for (nodo<T>* p = head; p && p->valor <= v && p->mtx_n.try_lock(); p = p->next) {//lock mientras se realiza la busqueda
        if (p->valor == v) {
            p->mtx_n.unlock();//unlock cuando se encuentra el elemento
            encontrado = true;
            break;
        }
        p->mtx_n.unlock();//unlock si se tiene que continuar la busqueda;
        pos = p;
    }
  mtx_h.unlock();//unlock
  return encontrado;
}

template <class T> bool LE<T>::find_withoutm(T v, nodo<T>*& pos) {
    bool encontrado = false;
    pos = nullptr;

    for (nodo<T>* p = head; p && p->valor <= v && p->mtx_n.try_lock(); p = p->next) {//lock mientras se realiza la busqueda
        if (p->valor == v) {
            p->mtx_n.unlock();//unlock cuando se encuentra el elemento
            encontrado = true;
            break;
        }
        p->mtx_n.unlock();//unlock si se tiene que continuar la busqueda;
        pos = p;
    }
    return encontrado;
}

template <class T> bool LE<T>::add(T v) {
  nodo<T> *pos_ant;
  if (!find_withoutm(v, pos_ant)) {
    mtx_h.lock();//lock para no modificar la lista si otra modificacion esta ocurriendo.
    if (pos_ant)
      pos_ant->next = new nodo<T>(v, pos_ant->next);
    else
      head = new nodo<T>(v, head);
    mtx_h.unlock();//unlock
    return true;
  }
  return false;
}

template <class T> bool LE<T>::del(T v) {
  nodo<T> *pos_ant, *pos_del;
  T uwu;
  if (find_withoutm(v, pos_ant)) {
    mtx_h.lock();//lock para no modificar la lista si otra modificacion esta ocurriendo.
    if (pos_ant) {
      pos_del = pos_ant->next;
      pos_ant->next = pos_del->next; 
      delete pos_del;
    } else {
      pos_del = head;
      head = pos_del->next; 
      delete pos_del;
    }
    mtx_h.unlock();//unlock
    return true;
  }
  return false;
}
int main() {
    ofs.open("output.txt");//output resultante menos los elementos eliminados de la lista
    deleted.open("eliminados.txt");//output elementos eliminados de la lista
  LE<int> a;
  Remove<int> rmv(0,1000,&a);
  Find<int> fnd(0,1000,&a);
  Add<int> add(0,1000,&a);

  thread threads[9];
  for(int i = 0; i<3; i++){
    threads[i*3]= thread(Add<int>(0,1000,&a),100);
    threads[i*3+1] = thread(Remove<int>(0, 1000, &a), 100);
    threads[i*3+2] = thread(Find<int>(0, 1000, &a), 100);
  }
  for (int i = 0; i < 9; i++){
      (threads[i]).join();
  }
  a.print();
  deleted.close();
  ofs.close();
}