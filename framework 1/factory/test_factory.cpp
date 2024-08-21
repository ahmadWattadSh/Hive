#include "factory.hpp"
#include "singleton.hpp"

#include <iostream>

enum KEYS{KEY1, KEY2, KEY3};


struct ARGS
{
  int x;
  float y;
  int z;
};


class ITask
{
public:
  
  ITask() = default;
  virtual ~ITask() = default;

  virtual void Execute() = 0;

  ITask(const ITask& o_) = delete;
  const ITask& operator=(const ITask& o_) = delete;
};


class Read: public ITask
{
public:
  Read(ARGS args)
  {
    std::cout << "Read args" << std::endl;
  }
  static std::shared_ptr<ITask> Create(ARGS args)
  {
    return std::make_shared<Read>(args);
  }

  void Execute() override {std::cout << "Reading task" << std::endl;} 
};


class Write: public ITask
{
public:  
  Write(ARGS args)
  {
    std::cout << "Write args" << std::endl;
  }

  static std::shared_ptr<ITask> Create(ARGS args)
  {
    return std::make_shared<Write>(args);
  }

  void Execute() override {std::cout << "Writing task" << std::endl;} 
};


using namespace ilrd;

int main()
{
    struct ARGS args1 = {1,2,3};
    struct ARGS args2 = {1,2,3};

    Factory<ITask, KEYS, ARGS> * factory = Singleton<Factory<ITask, KEYS, ARGS>>::GetInstance();

    factory->Register(KEY1, Read::Create);
    factory->Register(KEY2, Write::Create);

    std::shared_ptr<ITask> ptr1 = factory->Create(KEY1, args1);
    ptr1->Execute();

    std::shared_ptr<ITask> ptr2 = factory->Create(KEY2, args2);
    ptr2->Execute();

}


