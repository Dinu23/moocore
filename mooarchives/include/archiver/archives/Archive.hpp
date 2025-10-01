#pragma once
#include "../Solution.hpp"
#include <vector>
#include <limits>
#include <cstdio>
#include "../utils/debug.h"
#include <cassert>

using std::vector;

class BaseArchiveElementData {
public:
  virtual ~BaseArchiveElementData() { }
};

template<class T>
class ArchiveElement : public T {
public:
  ArchiveElement (const T &a) : T(a), data(NULL) {}
  ~ArchiveElement() { if (data) delete data; }
  class BaseArchiveElementData *data;

private:
  ArchiveElement () {};
};

template<class T>
class BaseArchive : public std::vector<ArchiveElement<T> *>
{
public:
  virtual ~BaseArchive();

protected:
  typedef ArchiveElement<T> element_type;
  typedef typename vector<element_type *>::size_type size_type;
  typedef typename vector<element_type *>::const_iterator const_iterator;
  typedef typename vector<element_type *>::iterator iterator ;

  BaseArchive(size_type dimension)
    : _max_size(std::numeric_limits<size_type>::max()),
      _dimension(dimension),
      sequence(0),
      ubound(dimension, -std::numeric_limits<double>::max()),
      lbound(dimension, std::numeric_limits<double>::max())
  { }  

  BaseArchive(size_type maxsize, size_type dimension)
    : _max_size(maxsize),
      _dimension(dimension),
      sequence(0),
      ubound(dimension, -std::numeric_limits<double>::max()),
      lbound(dimension, std::numeric_limits<double>::max()),
      uev (maxsize, 0)
  { 
    this->reserve(maxsize); 
  }

  int calculate_uev ()
  {
    int i, j, tmp;
    int dim = dimension();
    int arcsize = this->size();

    for (j = 0; j < arcsize; j++)
      uev[j] = 0;

    for (i = 0; i < dim; i++) { 
      int unique = 0; 
      assert (ubound[i] > -std::numeric_limits<double>::max());
      assert (lbound[i] < std::numeric_limits<double>::max());
      for (j = 0; j < arcsize; j++) {
        if ((*this)[j]->o[i] == ubound[i]) {
          unique++;
          if (unique == 1)
            tmp = j;
        }
      }
      if (unique >= 1) {
        uev[tmp] = 1;
      }
        
      unique = 0;
      for (j = 0; j < arcsize; j++) {
        if ((*this)[j]->o[i] == lbound[i]) {
          unique++;
          if(unique == 1)
            tmp = j;
        }
      }
      if(unique >= 1)
        {
          uev[tmp] = 1;
        }
    }
    
    int nue = 0;
    for(j = 0;j < arcsize; j++)
      {
        if(uev[j] == 1)
          {
            nue++;
          }
      }
    return nue;
  }

  void calculate_bounds()
  {
    int i, j;
    int arcsize = this->size();
    int dim = this->dimension();

    for (i = 0; i < dim; i++) {
      ubound[i] = -std::numeric_limits<double>::max();
      lbound[i] = std::numeric_limits<double>::max();
      for (j = 0; j < arcsize; j++)  {
        if ( (*this)[j]->o[i] < lbound[i] )
          lbound[i] = (*this)[j]->o[i];
        if ( (*this)[j]->o[i] > ubound[i] )
          ubound[i] = (*this)[j]->o[i];
      }
    }

    DEBUG2_FUNPRINT ("lbound = ");
    DEBUG2 (vector_fprintf (stderr, "%.6f", lbound); fprintf(stderr, "\n"));
    DEBUG2_PRINT ("\tubound = ");
    DEBUG2 (vector_fprintf (stderr, "%.6f", ubound); fprintf(stderr, "\n"));
  }

  bool update_bounds(const element_type &s)
  {
    int k;
    int dim = _dimension;
    bool bounds_changed_p = false;

    for (k = 0; k < dim; k++) {
      if (ubound[k] < s.o[k]) {
        ubound[k] = s.o[k];
        bounds_changed_p = true;
      }
      if (lbound[k] > s.o[k]) {
        lbound[k] = s.o[k];
        bounds_changed_p = true;
      }
    }
    return bounds_changed_p;
  }

public:
  size_type max_size(void) const { return _max_size; }
  size_type dimension(void) const { return _dimension; }

  bool overfull (void)
  {
    return this->size() > this->max_size();
  }

  void max_size (size_type maxsize)
  { 
    this->reserve (maxsize);
    _max_size = maxsize;
  }

  void print(FILE *stream = stdout)
  {
    for (const_iterator iter = this->begin();
         iter != this->end(); ++iter) {
      (*iter)->print (stream);
    }
  }

  iterator erase (iterator i)
  { 
    delete *i;
    return this->vector<element_type *>::erase(i);
  }

  void pop_back ()
  { 
    element_type * s = this->back();
    delete s;
    this->vector<element_type *>::pop_back();
  }

  virtual dominance_t add(const T &s) = 0;

  dominance_t update(const element_type &s)
  {
    bool dominates_p = false;
    iterator iter = this->begin();
    while (iter != this->end()) {
      const element_type &a = **iter;
      switch (s.dominance(a))
        {
        case IS_DOMINATED_BY:
          return IS_DOMINATED_BY;
        
        case EQUALS:
          return EQUALS;

        case DOMINATES:
          iter = this->erase(iter);
          dominates_p = true;
          break;
          
        case NONDOMINATED:
          ++iter;
          break;
        }
    }
    return (dominates_p) ? DOMINATES : NONDOMINATED;
  }

private:
  size_type _max_size;
  size_type _dimension;

protected:
  unsigned sequence; // Number of points ever added to the archive.
  // min and max vectors in the objective space i.e. ideal and anti-ideal points
  vector<double> ubound;
  vector<double> lbound;
  vector<int> uev;   // list of indexes (into the archive) of uniquely extremal vectors
};

// Template implementation
template<class T>
BaseArchive<T>::~BaseArchive()
{
  for (iterator k = this->begin(); k != this->end(); k++)
    delete *k;
}

template<class T>
class UnboundedArchive : public BaseArchive<T>
{
public:
  UnboundedArchive(unsigned dimension)
  : BaseArchive<T>(dimension) {}

  dominance_t add(const T &s)
  {
    dominance_t result = this->update(s);
    if (result == DOMINATES || result == NONDOMINATED)
      this->push_back(new ArchiveElement<T>(s));
    return result;
  }
};

template<class T>
class DominatingArchive : public BaseArchive<T>
{
public:
  typedef typename BaseArchive<T>::size_type size_type;
  typedef typename BaseArchive<T>::element_type element_type;
  
  DominatingArchive(size_type maxsize, size_type dimension)
  : BaseArchive<T>(maxsize, dimension) { }

  dominance_t add(const T &s)
  {
    dominance_t result = this->update(s);
    if ((result == DOMINATES || result == NONDOMINATED)
        && this->size() < this->max_size())
      this->push_back(new ArchiveElement<T>(s));

    return result;
  }
};
