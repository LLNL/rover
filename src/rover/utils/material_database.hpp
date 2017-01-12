#ifndef rover_material_database_h
#define rover_material_database_h


#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <stdio.h>
#include <vector>
#include <vtkm/cont/ArrayHandle.h>

namespace rover {

struct EnergyBin
{
  double energy; //in eV
  double opacity;//cm^2/gm
};
struct Element
{
  std::string symbol;
  int atomic_number;    // -1 if compound
  double atomic_weight;
  double density;       // units (g/cc)
  std::vector<EnergyBin> energy_bins; 
  Element()
  {}
  ~Element()
  {}
  Element(const Element &other)
  {
    symbol = other.symbol;
    atomic_number = other.atomic_number;
    atomic_weight = other.atomic_weight;
    density = other.density;
    energy_bins = other.energy_bins;

  }

  Element& operator=(const Element &other)
  {
    symbol = other.symbol;
    atomic_number = other.atomic_number;
    atomic_weight = other.atomic_weight;
    density = other.density;
    energy_bins = other.energy_bins;
    return *this;
  }

  void PrintSelf()
  {
    printf("Element: %s\n", symbol.c_str());
    printf("   Atomic number: %d\n", atomic_number);
    printf("   %d Bins:\n", (int)energy_bins.size());
    for(int i = 0; i < energy_bins.size();++i)
    {
      printf("        %e %e\n", energy_bins[i].energy, energy_bins[i].opacity);
    } 
  }

};

class MaterialDatabase
{
public:
  MaterialDatabase();
  template<typename T>
  void get_elements(const std::vector<std::string> &symbols,
                   const int &num_bins,
                   vtkm::cont::ArrayHandle<T> &output);
  ~MaterialDatabase(){};
protected:
  void read_db();
  bool contains_keyword(const std::string &line) const;
  void parse_element(const std::string &, Element &, std::ifstream &);
  std::map<std::string,Element> Elements;
  double sample_bin(const Element &, const double &x); 
  double minMeV;    // min photon eneregy
  double maxMeV;    // max photon eneregy
};



}// namespace rover
#endif
