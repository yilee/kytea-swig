// mykytea.cpp
#include <iostream>
#include "mykytea.hpp"

const int MAX_LEN = 256;

int split_argv(char* input, const char* configs[]){
  int len;
  char *cp;
  const char *delim = " ";
  
  cp = input;
  for(len = 0; len < MAX_LEN; len++){
    if((configs[len] = strtok(cp, delim)) == NULL )
      break;
    cp = NULL;
  }
  return len;
}

Mykytea::Mykytea(char* str)
{
  const char* configs[MAX_LEN];
  int len = split_argv(str, configs);
  
  config = new KyteaConfig();
  config->parseRunCommandLine(len, configs);
  
  kytea = new Kytea(config);
  kytea->readModel(config->getModelFile().c_str());  
  util = kytea->getStringUtil();
  
}

Mykytea::~Mykytea()
{
  if(kytea != NULL) delete kytea;
  // chanted the order, so that there is no double-freeing problem; 
  // it should not, but given the JNI and C++ ownership, this could happen.
  if(config) delete config;
  if(util)  delete util;
}

/** the inference local to each thread */
vector<string>* Mykytea::getWS(string str){
    vector<string>* vec = new vector<string>;

    KyteaChar spaceChar = util->mapChar(" ");
    KyteaString      ks = util->mapString(str), buff(ks.length());

    int len = ks.length();
    KyteaSentence * pSentence = new KyteaSentence();
    int charLen = 0;
    for(int j = 0; j < len; j++) {
      int bpos = 0;
      for( ; j < len && ks[j] != spaceChar; j++)
        buff[bpos++] = ks[j];
      if(bpos == 0) {
        if(ks[j] == spaceChar)
          continue;
      }
      KyteaString word_str = buff.substr(0,bpos);
      KyteaWord word(word_str, util->normalize(word_str));
      charLen += bpos;
      pSentence->words.push_back(word);
    }

    pSentence->surface = KyteaString(charLen);
    pSentence->norm    = KyteaString(charLen);
    unsigned pos = 0;
    for(KyteaSentence::Words::const_iterator tit = pSentence->words.begin(); tit != pSentence->words.end(); tit++) {
      pSentence->surface.splice(tit->surface, pos);
      pSentence->norm.splice(tit->norm, pos);
      unsigned nextPos = pos + tit->surface.length() - 1;
      while(pos++ < nextPos)
        pSentence->wsConfs.push_back(0.0);
      pSentence->wsConfs.push_back(100.0);
    }
    if(pSentence->wsConfs.size() > 0)
      pSentence->wsConfs.pop_back();


    ///compute word segmentation, not read only
    kytea->calculateWS(*pSentence);
    const KyteaSentence::Words & words =  pSentence->words;

    for(int i = 0; i < (int) words.size(); i++) {
         (*vec).push_back(util->showString(words[i].surface));
    }

    if(pSentence) delete pSentence;
    return vec;
}

vector<Tags>* Mykytea::getTags(string str){
    vector<Tags>* ret_words = new vector<Tags>;

    KyteaString surface_string = util->mapString(str);
    KyteaSentence sentence(surface_string, util->normalize(surface_string));
    kytea->calculateWS(sentence);

    for(int i = 0; i < config->getNumTags(); i++)
        kytea->calculateTags(sentence,i);

    const KyteaSentence::Words & words =  sentence.words;

    for(int i = 0; i < (int)words.size(); i++) {
        tags vec_tag;
        for(int j = 0; j < (int)words[i].tags.size(); j++) {
            vector< pair<string, double> > vec_tmp;
            for(int k = 0; k < 1; k++) {
                vec_tmp.push_back( make_pair(util->showString(words[i].tags[j][k].first), words[i].tags[j][k].second) );
            }
            vec_tag.push_back( vec_tmp );
        }
        struct Tags t = { util->showString(words[i].surface), vec_tag };
        (*ret_words).push_back( t );
    }
    return ret_words;
}

vector<Tags>* Mykytea::getAllTags(string str){
    vector<Tags>* ret_words = new vector<Tags>;

    KyteaString surface_string = util->mapString(str);
    KyteaSentence sentence(surface_string, util->normalize(surface_string));
    kytea->calculateWS(sentence);

    for(int i = 0; i < config->getNumTags(); i++)
        kytea->calculateTags(sentence,i);

    const KyteaSentence::Words & words =  sentence.words;

    for(int i = 0; i < (int)words.size(); i++) {
        tags vec_tag;
        for(int j = 0; j < (int)words[i].tags.size(); j++) {
            vector< pair<string, double> > vec_tmp;
            for(int k = 0; k < (int)words[i].tags[j].size(); k++) {
                vec_tmp.push_back( make_pair(util->showString(words[i].tags[j][k].first), words[i].tags[j][k].second) );
            }
            vec_tag.push_back( vec_tmp );
        }
        struct Tags t = { util->showString(words[i].surface), vec_tag };
        (*ret_words).push_back( t );
    }
    return ret_words;
}

string Mykytea::getTagsToString(string str)
{
    KyteaString surface_string = util->mapString(str);
    KyteaSentence sentence(surface_string, util->normalize(surface_string));
    kytea->calculateWS(sentence);

    for(int i = 0; i < config->getNumTags(); i++)
        kytea->calculateTags(sentence,i);

    const KyteaSentence::Words & words =  sentence.words;

    string ret_str;
    for(int i = 0; i < (int)words.size(); i++) {
        ret_str += util->showString(words[i].surface);
        for(int j = 0; j < (int)words[i].tags.size(); j++) {
            for(int k = 0; k < 1; k++) {
                ret_str += "/";
                ret_str += util->showString(words[i].tags[j][k].first);
            }
        }
        ret_str += " ";
    }
    return ret_str;
  
}
