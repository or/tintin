#ifndef DRACIS_SMARTPOINTER_H
#define DRACIS_SMARTPOINTER_H

#include <iostream>

using namespace std;

class SmartPointerBase {
public:
  inline SmartPointerBase() :referenceCounter_(0) { /* nothing to do */ };
  inline ~SmartPointerBase() { /* nothing to do */ };

  inline void incrRefCounter() { ++referenceCounter_; };
  inline void decrRefCounter() { --referenceCounter_; };
  inline int getRefCounter() const { return referenceCounter_; };
  inline bool hasReferences() const { return referenceCounter_ > 0; };

private:
  int referenceCounter_;
};



template <class T>
class SmartPointer {
public:
  SmartPointer();
  SmartPointer(T *target);
  SmartPointer(const SmartPointer<T> &pointer);
  ~SmartPointer();

  SmartPointer &operator=(const SmartPointer<T> &pointer);
  SmartPointer &operator=(const T *target);

  bool operator==(const SmartPointer<T> &pointer) const;
  bool operator==(const T *target) const;
  bool operator!=(const SmartPointer<T> &pointer) const;
  bool operator!=(const T *target) const;
  bool operator<(const SmartPointer<T> &pointer) const;

  bool isNull() const;

  void decrTargetRefCounter();

  T *operator->() const;
  T *getTarget() const;

private:
  void setTarget(const T *target);

  T *target_;
};



template<class T>
inline SmartPointer<T>::SmartPointer()
                       :target_(NULL)
{
  // nothing to do
}



template<class T>
inline SmartPointer<T>::SmartPointer(T *target)
                       :target_(NULL)
{
  setTarget(target);
}



template<class T>
inline SmartPointer<T>::SmartPointer(const SmartPointer<T> &pointer)
                       :target_(NULL)
{
  setTarget(pointer.getTarget());
}



template<class T>
inline SmartPointer<T>::~SmartPointer()
{
  decrTargetRefCounter();
}



template<class T>
inline SmartPointer<T> &SmartPointer<T>::operator=(const SmartPointer<T> &pointer)
{
  setTarget(pointer.getTarget());
  return *this;
}



template<class T>
inline SmartPointer<T> &SmartPointer<T>::operator=(const T *target)
{
  setTarget(target);
  return *this;
}



template<class T>
inline void SmartPointer<T>::setTarget(const T *target)
{
  decrTargetRefCounter();
  target_ = (T*)target;
  if (target_ != NULL) {
    target_->incrRefCounter();
  }
}



template<class T>
inline bool SmartPointer<T>::operator==(const SmartPointer<T> &pointer) const
{
  return pointer.getTarget() == target_;
}



template<class T>
inline bool SmartPointer<T>::operator==(const T *target) const
{
  return target == target_;
}



template<class T>
inline bool SmartPointer<T>::operator!=(const SmartPointer<T> &pointer) const
{
  return !(*this == pointer);
}



template<class T>
inline bool SmartPointer<T>::operator!=(const T *target) const
{
  return !(*this == target);
}



template<class T>
bool SmartPointer<T>::operator<(const SmartPointer<T> &pointer) const
{
  return (*getTarget()) < (*pointer.getTarget());
}



template<class T>
inline T *SmartPointer<T>::operator->() const
{
  return target_;
}



template<class T>
inline bool SmartPointer<T>::isNull() const
{
  return target_ == NULL;
}



template<class T>
inline T *SmartPointer<T>::getTarget() const
{
  return target_;
}



template<class T>
inline void SmartPointer<T>::decrTargetRefCounter()
{
  //cout << "dereferencing " << target_ << endl;
  if (target_ != NULL) {
    target_->decrRefCounter();
    if (!target_->hasReferences()) {
      delete target_;
      target_ = NULL;
    }
  }
}

#endif
