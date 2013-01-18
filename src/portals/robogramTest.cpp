#include "RoboGrams.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

// ***************************************
// *                                     *
// *  RoboGram Testing and Example Code  *
// *                                     *
// ***************************************

// *************************************
// *                                   *
// *  Message<double> Specializations  *
// *                                   *
// *************************************

template<> void MessageHolder<double>::initialize() { message = 0;}

template<> std::string MessageHolder<double>::describe() const
{ return strprintf("%8.3f", message);}

// ***************************
// *                         *
// *  Vector Template Class  *
// *                         *
// ***************************

template<int N>
class Vect
{
  double x_[N];

public:
  Vect()
  {
    Clear();
  }

  void Clear()
  {
    for (int i = 0; i < N; ++i)
      x_[i] = 0;
  }

  double& operator[](int i)
  {
    assert(0 <= i && i < N);
    return x_[i];
  }

  double operator[](int i) const
  {
    assert(0 <= i && i < N);
    return x_[i];
  }

  Vect<N>& operator+=(const Vect<N>& y)
  {
    for (int i = 0; i < N; ++i)
      x_[i] += y.x_[i];
    return *this;
  }

  Vect<N> operator+(const Vect<N>& y) const
  {
    Vect<N> x(*this);
    x += y;
    return x;
  }

  double operator*(const Vect<N>& y) const
  {
    double s = 0;
    for (int i = 0; i < N; ++i)
      s += x_[i] * y.x_[i];
    return s;
  }

  std::string DebugString() const;
};

template<> SET_POOL_SIZE(Vect<2>, 10);

template<int N>
std::string Vect<N>::DebugString() const
{
  std::string s;
  for (int i = 0; i < N; ++i)
    s += strprintf("%8.3f", x_[i]);
  return s;
}


// **********************************
// *                                *
// *  Square Matrix Template Class  *
// *                                *
// **********************************

template<int N>
class SqMat
{
  Vect<N> a_[N];

public:
  SqMat() { Clear();}

  void Clear();

  Vect<N>& operator[](int i)
  {
    assert(0 <= i && i < N);
    return a_[i];
  }

  Vect<N> operator[](int i) const
  {
    assert(0 <= i && i < N);
    return a_[i];
  }

  Vect<N> operator*(const Vect<N>&) const;

  SqMat<N> operator*(const SqMat<N>&) const;

  std::string DebugString() const;
};

template<int N>
void SqMat<N>::Clear()
{
  for (int i = 0; i < N; ++i)
  {
    a_[i].Clear();
    a_[i][i] = 1;
  }
}

template<int N>
Vect<N> SqMat<N>::operator*(const Vect<N>& v) const
{
  Vect<N> x;
  for (int i = 0; i < N; ++i)
    x[i] = a_[i] * v;
  return x;
}

template<int N>
SqMat<N> SqMat<N>::operator*(const SqMat<N>& b) const
{
  SqMat<N> m;
  for (int r = 0; r < N; ++r)
    for (int c = 0; c < N; ++c)
    {
      double s = 0;
      for (int i = 0; i < N; ++i)
        s += a_[r][i] * b.a_[i][c];
      m.a_[r][c] = s;
    }
  return m;
}

template<int N>
std::string SqMat<N>::DebugString() const
{
  std::string s;
  for (int i = 0; i < N; ++i)
    s += a_[i].DebugString() + '\n';
  return s;
}


// ***********************
// *                     *
// *  Vector Sum Module  *
// *                     *
// ***********************

class VSumModule : public Module
{
public:
  VSumModule() : out(base()) {}

  InPortal<Vect<2> > aIn;
  InPortal<Vect<2> > bIn;
  OutPortal<Vect<2> > out;

protected:
  virtual void run_();
};

void VSumModule::run_()
{
  aIn.latch();
  bIn.latch();

  Message<Vect<2> > sum(0);
  *sum.get() = aIn.message() + bIn.message();

  out.setMessage(sum);
}

// ************************
// *                      *
// *  Dot Product Module  *
// *                      *
// ************************

class DotModule : public Module
{
public:
  DotModule() : out(base()) {}

  InPortal<Vect<2> > aIn;
  InPortal<Vect<2> > bIn;
  OutPortal<double> out;

protected:
  virtual void run_();
};

void DotModule::run_()
{
  aIn.latch();
  bIn.latch();

  Message<double> dot(0);
  *dot.get() = aIn.message() * bIn.message();

  out.setMessage(dot);
}

// ****************************
// *                          *
// *  Matrix * Vector Module  *
// *                          *
// ****************************

class SqMatXVectModule : public Module
{
public:
  SqMatXVectModule() : out(base()) {}

  InPortal<SqMat<2> > mIn;
  InPortal<Vect<2> >  vIn;
  OutPortal<Vect<2> > out;

protected:
  virtual void run_();
};

void SqMatXVectModule::run_()
{
  mIn.latch();
  vIn.latch();

  Vect<2> v = mIn.message() * vIn.message();
  Message<Vect<2> > prod(&v);

  out.setMessage(prod);
}

// ********************
// *                  *
// *  Display Module  *
// *                  *
// ********************

class DisplayModule : public Module
{
public:
  InPortal<double> in;

protected:
  virtual void run_();
};

void DisplayModule::run_()
{
  in.latch();
  int n = (int)in.message();
  if (n > 80)
    n = 80;

  std::cout << std::string(n, ' ') << "*\n";
}

// ******************
// *                *
// *  Main Program  *
// *                *
// ******************

int main(int argc, char* argv[])
{
  {
    DotModule        dotMod;
    SqMatXVectModule rotMod;
    VSumModule       shiftMod;
    DisplayModule    dispMod;

    Vect<2> unit, shift, spin;
    unit[0] = unit[1] = M_SQRT1_2;
    spin[0] = 32;
    shift[0] = shift[1] = 32;

    double a = 5. * M_PI / 180., cs = cos(a), sn = sin(a);
    SqMat<2> rotK;
    rotK[0][0] = rotK[1][1] = cs;
    rotK[0][1] = sn;
    rotK[1][0] = -sn;

    rotMod.mIn.setMessage(&rotK);
    rotMod.out.setMessage(&spin);
    rotMod.vIn.wireTo(&rotMod.out, true);

    shiftMod.aIn.setMessage(&shift);
    shiftMod.bIn.wireTo(&rotMod.out);

    dotMod.aIn.setMessage(&unit);
    dotMod.bIn.wireTo(&shiftMod.out);

    dispMod.in.wireTo(&dotMod.out);

    RoboGram rg;
    rg.addModule(dispMod);
    rg.addModule(rotMod);
    rg.addModule(dotMod);
    rg.addModule(shiftMod);

    for (int i = 0; i < 72; ++i)
      rg.run();

    std::cout << Message<double>  ::describe("double"  );
    std::cout << Message<Vect<2> > ::describe("Vect<2>" );
    std::cout << Message<SqMat<2> >::describe("SqMat<2>");
  }

  std::cout << Message<double>  ::describe("double"  );
  std::cout << Message<Vect<2> > ::describe("Vect<2>" );
  std::cout << Message<SqMat<2> >::describe("SqMat<2>");

  return 0;
}

