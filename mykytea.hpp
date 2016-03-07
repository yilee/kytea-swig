// mykytea.hpp
#ifndef MYKYTEA_HPP
#define MYKYTEA_HPP

#include <vector>
#include <string>
#include <map>

#include <kytea/kytea.h>
#include <kytea/kytea-struct.h>
#include <kytea/string-util.h>


using namespace std;
using namespace kytea;

typedef vector< vector< pair<string, double> > > tags;

struct Tags{
  string surface;
  tags tag;
};

class Mykytea
{
public:

  Mykytea(char* str);
  ~Mykytea();

  vector<string>* getWS(string str);
  vector<Tags>* getTags(string str);
  vector<Tags>* getAllTags(string str);
  string getTagsToString(string str);

private:
  /** candidate for the memory mapping */
  Kytea* kytea;

  /** local to each thread */
  StringUtil* util;  
  KyteaConfig* config;
};
#endif
