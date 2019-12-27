
#include <dlib/clustering.h>
#include <dlib/rand.h>

#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace dlib;

std::string value(std::string s, double &v)
{
  if ( s.empty() ) return s;
  std::size_t pos = s.find(';');
  if ( pos != std::string::npos ) {
    if ( pos == 0 )
      v = 0;
    else
      v = stod(s.substr(0, pos));    
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

sample_type parse(const std::string &s)
{
  sample_type d;
  std::string str = s;
  for ( int i = 0; i < 8; i++ ) {
    double v;
    str = value(str, v);
    // 4-тый параметр уменьшаем, а то сильно влияет на кластеризацию
    if ( i == 3 && v > 100000 ) 
      v = v / 100000;
    d(i) = v;
  }
  return d;
}

int main(int argc, char **argv)
{
  if ( argc != 3 ) return 0;
  int number_klasters = atoi(argv[1]);
  std::string file_name(argv[2]);    

  kcentroid<kernel_type> kc(kernel_type(0.1), 0.01, 8);
  kkmeans<kernel_type> test(kc);

  std::vector<sample_type> samples;
  std::string s;
  while ( std::getline(std::cin, s) ) {
    samples.push_back(parse(s));
  }

  test.set_number_of_centers(number_klasters);

  std::vector<sample_type> initial_centers;
  pick_initial_centers(number_klasters, initial_centers, samples, test.get_kernel());

  test.train(samples, initial_centers);

  try {
    serialize(file_name) << test;
  } catch ( const std::exception&e ) {
    cout << "exception thrown!" << endl;
    cout << e.what() << endl;
  }

  // выводим 
  ofstream myfile;
  myfile.open(file_name + ".txt");  
  for ( const sample_type& value : samples ) {
    for ( int i = 0; i < 8; i++ ) {
      myfile << value(i) << ";";
    }
    myfile << test(value) << "\n";
  }
  myfile.close();
  return 0;
}