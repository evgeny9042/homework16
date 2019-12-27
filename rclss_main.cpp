
#include <dlib/clustering.h>
#include <dlib/rand.h>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <map>

using namespace std;
using namespace dlib;

// из строки в число
std::string value(std::string s, double &v)
{
  if ( s.empty() ) return s;
  std::size_t pos = s.find(';');
  if ( pos != std::string::npos ) {
    if ( pos == 0 ) {
      v = 0;
    } else {
      v = stod(s.substr(0, pos));
    }    
    return s.substr(pos + 1, s.length());
  }
  if ( !s.empty() )
    v = stod(s);
  else
    v = 0;
  return std::string();
}

typedef matrix<double, 8, 1> sample_type;
typedef radial_basis_kernel<sample_type> kernel_type;

// из строки в числа
sample_type parse(const std::string &s, unsigned long *c = nullptr)
{
  sample_type d;
  std::string str = s;
  for ( int i = 0; i < 8; i++ ) {
    double v;
    str = value(str, v);
    if ( c == nullptr ) {
      // 4-тый параметр уменьшаем, а то сильно влияет на кластеризацию
      if ( i == 3 && v > 100000 )  // уменьшаем, тока если он еще не был уменьшен до этого
        v = v / 100000;
    }
    d(i) = v;
  }
  if (c)
    *c = stoi(str);
  return d;
}

/// рассчитать расстояние между двумя точками в 8 метном пространстве
double length(const sample_type& left, const sample_type& right) 
{
  double d = 0;
  for ( int i = 0; i < 8; i++ ) {
    double delta = (left(i) - right(i));
    d += delta*delta;
  }
  return std::pow(d, 0.5);
}

struct my_data_t {
  sample_type data;     // точка в многомерном пространстве
  unsigned long klass;  // класс, к которому она относится
};

void print(const sample_type&d)
{
  for ( int i = 0; i < 8; i++ )
    std::cout << d(i) << ";";
}

// вывести в стандартный выход
void print(const my_data_t&d, double l)
{  
  print(d.data);
  std::cout << " klass = " << d.klass;
  std::cout << " length = " << l << std::endl;
}

int main(int argc, char **argv)
{
  if ( argc != 2 ) return 0;
  std::string file_name(argv[1]);    

  // считываем структуру кластеризатора
  kcentroid<kernel_type> kc(kernel_type(0.1), 0.01, 8);
  kkmeans<kernel_type> test(kc);
  try {
    deserialize(file_name) >> test;
  } catch ( const std::exception&e ) {
    cout << "exception thrown!" << endl;
    cout << e.what() << endl;
  }

  // считываем исходные данные 
  ifstream myfile;
  myfile.open(file_name + ".txt");
  if ( !myfile.is_open() ) {
    cout << "open file error!" << endl;
    return 0;
  }

  std::vector<my_data_t> initial_values;
  std::string line;
  while ( std::getline(myfile, line) ) {
    unsigned long c;
    sample_type d = parse(line, &c);
    initial_values.push_back({d, c});
  }
  myfile.close();

  // классифицируем
  std::vector<sample_type> values_to_predict;
  std::string s;
  while ( std::getline(std::cin, s) ) {
    values_to_predict.push_back(parse(s));
  }
  // выводим в порядке возрастания расстояния между точками в многомерном пространстве
  for ( const sample_type& value : values_to_predict ) {
    unsigned long predict = test(value);
    std::multimap<double, unsigned long> candidates;
    for (size_t i = 0; i < initial_values.size(); i++) {
      if ( initial_values[i].klass == predict ) {
        candidates.insert(std::make_pair(length(value, initial_values[i].data), i));
      }
    }
    std::cout << "Candidates for "; print(value); std::cout << endl;
    for ( auto it = candidates.cbegin(); it != candidates.cend(); it++ ) {
      print(initial_values[it->second], it->first);
    }
    std::cout << std::endl;
  }
  return 0;
}