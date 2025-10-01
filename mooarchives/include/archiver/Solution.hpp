#pragma  once
#include <cstdio>
#include <vector>

#define point_printf_format "%-16.15g"

using namespace std;

enum dominance_t { IS_DOMINATED_BY, DOMINATES, NONDOMINATED, EQUALS };
// Function implementation
void
vector_fprintf(FILE *stream, const char * format,
               const std::vector<double> & vec, const char *sep = " ");

class Solution {
  
public:

  vector<double> o;

  static void Initialise (unsigned d) {
    Solution::_num_objs = d;
  }

  Solution (void) 
    : o ()
  { 
    o.reserve(_num_objs); 
  }
  
  void setObjectives(vector<double> src)
  {
    this->o = src;
  }

  unsigned int num_objs(void) const { return Solution::_num_objs; }

  Solution * clone(void) { return new Solution(*this); }

  void print(FILE *stream=stdout) const
  {
    vector<double>::const_iterator iter = o.begin();
    fprintf (stream, point_printf_format, *iter);
    for (++iter; iter != o.end(); ++iter) {         
      fprintf (stream, "\t" point_printf_format, *iter);
    }
    fprintf (stream, "\n");
  }

  dominance_t dominance (const Solution &b) const
  {
    bool a_leq_b, b_leq_a;
    a_leq_b = b_leq_a = true;
    unsigned n_objs = this->o.size();
    for (unsigned int d = 0; d < n_objs; d++) {
      a_leq_b = a_leq_b && (this->o[d] <= b.o[d]);
      b_leq_a = b_leq_a && (b.o[d] <= this->o[d]);
    }
    
    // FIXME: This could use bit-wise operations to be much faster.
    if (!a_leq_b && !b_leq_a) {
      return NONDOMINATED;
    } else if (!a_leq_b) {
      return IS_DOMINATED_BY;
    } else if (!b_leq_a) {
      return DOMINATES;
    } else {
      return EQUALS;
    }
  }

  bool is_equal (const Solution &b) const
  {
    bool equal = true;
    unsigned i = 0;
    unsigned n_objs = this->o.size();
    while (equal && i < n_objs) {
      // FIXME: This should use some epsilon difference.
      equal = (this->o[i] == b.o[i]);
      ++i;
    }
    return equal;
  }

  bool dominates (const Solution &b) const
  {
    dominance_t result = this->dominance(b);
    return result == DOMINATES;
  }

  bool weaklydominates (const Solution &b) const
  {
    dominance_t result = this->dominance(b);
    return (result == DOMINATES || result == EQUALS);
  }


  static unsigned int _num_objs;
};

// // Static member definition
// unsigned int Solution::_num_objs = 0;

