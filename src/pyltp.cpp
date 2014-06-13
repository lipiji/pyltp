/**
 * pyltp - A python extension for Language Technology Platform
 *
 *  @author: HuangFJ <biohfj@gmail.com>
 *
 * This project forks from https://github.com/HuangFJ/pyltp. The basic structure of
 * the project is perserved. But interface is adopted from XML level to library level
 * to allow more flexible usage.
 *
 *  @author: LIU, Yijia <yjliu@ir.hit.edu.cn>
 */
#include <iostream>
#include <vector>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "ltp/segment_dll.h"
#include "ltp/postag_dll.h"
#include "ltp/parser_dll.h"
#include "ltp/ner_dll.h"
#include "ltp/SRL_DLL.h"


struct Segmentor {
  Segmentor()
    : model(NULL) {}

  void load(std::string model_path) {
    if (model == NULL) {
      model = segmentor_create_segmentor(model_path.c_str());
    } else {
      std::cerr << "Model reloaded!" << std::endl;
    }
  }

  void load_with_lexicon(std::string model_path, std::string lexicon_path) {
    if (model == NULL) {
      model = segmentor_create_segmentor(model_path.c_str(), lexicon_path.c_str());
    } else {
      std::cerr << "Model reloaded!" << std::endl;
    }
  }

  std::vector<std::string> segment(std::string sentence) {
    std::vector<std::string> ret;
    if (model == NULL) {
      std::cerr << "Segmentor: Model not loaded!" << std::endl;
    } else {
      segmentor_segment(model, sentence.c_str(), ret);
    }
    return ret;
  }

  void release() {
    if (model != NULL) {
      segmentor_release_segmentor(model);
      model = NULL;
    }
  }

  void * model;
};


struct Postagger {
  Postagger()
    : model(NULL) {}

  void load(std::string model_path) {
    if (model == NULL) {
      model = postagger_create_postagger(model_path.c_str());
    } else {
      std::cerr << "Postagger: Model reloaded!" << std::endl;
    }
  }

  std::vector<std::string> postag(std::vector<std::string> words) {
    std::vector<std::string> ret;
    if (model == NULL) {
      std::cerr << "Postagger: Model not loaded!" << std::endl;
    } else {
      postagger_postag(model, words, ret);
    }
    return ret;
  }

  void release() {
    if (model != NULL) {
      postagger_release_postagger(model);
      model = NULL;
    }
  }

  void * model;
};


typedef std::pair<int, std::string> ParseResult;

struct Parser {
  Parser()
    : model(NULL) {}

  void load(std::string model_path) {
    if (model == NULL) {
      model = parser_create_parser(model_path.c_str());
    } else {
      std::cerr << "Parser: Model reloaded!" << std::endl;
    }
  }

  std::vector<ParseResult > parse(std::vector<std::string> words,
      std::vector<std::string> postags) {
    std::vector<ParseResult > ret;
    std::vector<int>          heads;
    std::vector<std::string>  relations;

    if (model == NULL) {
      std::cerr << "Parser: Model not loaded!" << std::endl;
    } else {
      parser_parse(model, words, postags, heads, relations);
    }

    for (int i = 0; i < heads.size(); ++ i) {
      ret.push_back(ParseResult(heads[i], relations[i]));
    }
    return ret;
  }

  void release() {
    if (model != NULL) {
      parser_release_parser(model);
      model = NULL;
    }
  }

  void * model;
};


BOOST_PYTHON_MODULE(pyltp)
{
  using namespace boost::python;

  class_<ParseResult >("ParseResult")
    .def_readwrite("head",     &ParseResult::first)
    .def_readwrite("relation", &ParseResult::second);

  class_<std::vector<std::string> >("VectorOfString")
    .def(vector_indexing_suite<std::vector<std::string> >() );

  class_<std::vector<ParseResult > >("VectorOfParseResult")
    .def(vector_indexing_suite<std::vector<ParseResult> >() );

  class_<Segmentor>("Segmentor")
    .def("load", &Segmentor::load)
    .def("load_with_lexicon", &Segmentor::load_with_lexicon)
    .def("segment", &Segmentor::segment)
    .def("release", &Segmentor::release)
    ;

  class_<Postagger>("Postagger")
    .def("load", &Postagger::load)
    .def("postag", &Postagger::postag)
    .def("release", &Postagger::release)
    ;

  class_<Parser>("Parser")
    .def("load", &Parser::load)
    .def("parse", &Parser::parse)
    .def("release", &Parser::release)
    ;
}