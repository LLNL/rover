//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2018, Lawrence Livermore National Security, LLC.
// 
// Produced at the Lawrence Livermore National Laboratory
// 
// LLNL-CODE-749865
// 
// All rights reserved.
// 
// This file is part of Rover. 
// 
// Please also read rover/LICENSE
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the disclaimer below.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the disclaimer (as noted below) in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of the LLNS/LLNL nor the names of its contributors may
//   be used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
// LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
#ifndef rover_material_database_h
#define rover_material_database_h


#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <stdio.h>
#include <vector>
#include <vtkm/cont/ArrayHandle.h>
#include "test_config.h"
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

  void PrintSelf() const
  {
    printf("Element: %s\n", symbol.c_str());
    printf("   Atomic number: %d\n", atomic_number);
    printf("   %d Bins:\n", (int)energy_bins.size());
    for(int i = 0; i < (int)energy_bins.size();++i)
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
                    vtkm::cont::ArrayHandle<T> absorption,
                    int &element_count);
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


bool contains(const std::string haystack, std::string needle)
{
    std::size_t found = haystack.find(needle);
      return (found != std::string::npos);
}

std::string replace(std::string &str,
                        const std::string &toReplace,
                                            const std::string &replaceWith)
{
      std::string s = str;
          return(s.replace(s.find(toReplace), toReplace.length(), replaceWith));
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
{  
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    { 
       //strip white space
       if(item != "") elems.push_back(item);
    }
    return elems;
}
std::vector<std::string> split(const std::string &s, char delim)
{  
    std::vector<std::string> elems;
      split(s, delim, elems);
        return elems;
}

MaterialDatabase::MaterialDatabase()
{
  minMeV = 1e100;
  maxMeV = -1e100;
  read_db();
};

bool 
MaterialDatabase::contains_keyword(const std::string &line) const
{
  if(contains(line, "Version")) return true;
  if(contains(line, "Element")) return true;
  if(contains(line, "Edge")) return true;
  if(contains(line, "Lines")) return true;
  if(contains(line, "CK")) return true;
  if(contains(line, "CKtotal")) return true;
  if(contains(line, "Photo")) return true;
  if(contains(line, "Scatter")) return true;
  if(contains(line, "EndElement")) return true;
  if(contains(line, "End")) return true;
  
  return false;
}

void 
MaterialDatabase::parse_element(const std::string &line, Element &elem, std::ifstream &reader)
{
  std::vector<std::string> values = split(line,' ');
  elem.symbol = values.at(1);
  elem.atomic_number = atoi(values.at(2).c_str());
  elem.atomic_weight = atof(values.at(3).c_str());
  elem.density = atof(values.at(4).c_str());
  bool elementClose = false;
  while(!elementClose) 
  {
    char buff[1024];
    reader.getline(buff, 1024);
    std::string next_line(buff);
    
    
    if(contains(next_line, "Absorb"))
    {
      while(1)
      {
        reader.getline(buff,1024);
        std::string bin_line(buff);
        if(this->contains_keyword(bin_line))
        {
          break;
        }
        //std::cout<<bin_line<<"\n";
        std::vector<std::string> s_bin = split(bin_line,' ');
        EnergyBin bin;
        bin.energy = atof(s_bin.at(0).c_str());
        bin.opacity = atof(s_bin.at(4).c_str());
        minMeV = std::min(bin.energy, minMeV);
        maxMeV = std::max(bin.energy, maxMeV);
        elem.energy_bins.push_back(bin);
      }
    }
    
    if(contains(std::string(buff), "EndElement"))
    {
      elementClose = true;
      continue;
    }

  }

  //elem.PrintSelf();
} 

void MaterialDatabase::read_db()
{
  std::ifstream db_reader;
  std::string data_dir(DATA_DIR);
  std::string file("opacities.dat");
  std::string file_name = data_dir + file;
  db_reader.open(file_name.c_str());
  if(!db_reader.is_open())
  {
    printf("Could not open the db\n");
    return;
  }
  else printf("Opened db\n");
  while(!db_reader.eof())
  {
    const int max_line = 1024;
    char buffer[max_line];
    db_reader.getline(buffer, max_line);
    std::string line(buffer);
    Element currentElement;
    // skip lines containing comments
    //std::cout<<line<<"\n";
    if(contains(line, "//")) {printf("Skipping\n"); continue;}
    if(contains(line, "Element ")) parse_element(line, currentElement, db_reader);
    Elements[currentElement.symbol] = currentElement;
  }
  //std::cout<<"Read "<<Elements.size()<<" elements\n";
}

double 
MaterialDatabase::sample_bin(const Element &elem, const double &x)
{
  int n = (int)elem.energy_bins.size();
  if(n == 0) 
  {
    printf("No EngeryBins to sample\n"); 
    return 0.f;
  }
  if(n == 1 || x <= elem.energy_bins[0].energy)
    return elem.energy_bins[0].opacity;
  if(x >= elem.energy_bins[n-1].energy)
   return elem.energy_bins[n-1].opacity;
  int upperBin;
  for (upperBin=1; upperBin<n-1; upperBin++)
  {
    if(x < elem.energy_bins[upperBin].energy)
      break;
  }
  int lowerBin = upperBin - 1;
  double seg = elem.energy_bins[upperBin].energy - elem.energy_bins[lowerBin].energy;
  double delta;
  if (seg == 0.)
     delta = .5;
  else
   delta = (x - elem.energy_bins[lowerBin].energy) / seg;
  return (elem.energy_bins[lowerBin].opacity * (1. - delta) + 
          elem.energy_bins[upperBin].opacity * delta);
}

struct GetFieldSizeFunctor
{
  vtkm::Id *m_size;

  GetFieldSizeFunctor(vtkm::Id *size)
    : m_size(size)
  {}

  template<typename T, typename Storage>
  void operator()(const vtkm::cont::ArrayHandle<T, Storage> &array) const
  {
      *m_size = array.GetPortalConstControl().GetNumberOfValues();
  }
};

template<typename T>
void
MaterialDatabase::get_elements(const std::vector<std::string> &symbols,
                               const int &num_bins,
                               vtkm::cont::ArrayHandle<T> absorption,
                               int &element_count)
{
  element_count = 0;
  double spectrumLength = maxMeV - minMeV;
  vtkm::cont::ArrayHandle<T> look_up;
  absorption.Allocate(symbols.size() * num_bins);
  if(spectrumLength <= 0)
  {
    std::cout<<"Bad spectrum length: "<<minMeV<<" - "<<maxMeV<<"\n";
  }
  double seg = spectrumLength / (double) num_bins;

  const int numEls = symbols.size();
  for(int i = 0; i < numEls; ++i)
  {
    std::map<std::string,Element>::iterator it = Elements.find(symbols[i]);
    if(it == Elements.end())
    {
      std::cout<<"Warning: symbol "<<symbols[i]<<" not found. Skipping\n";
      continue;
    }
    element_count++; 
    Element elem = it->second;
    
    //std::cout<<"\nElem : "<<i<<"\n";
    for(int j = 0; j < num_bins; ++j)
    {
      double position = minMeV + (double)j * seg;
      double value = this->sample_bin(elem,position);
      // Devide by element density
      // This value will have units if 1/cm
      // and will be multiplied by distance (cm)
      // to obtain the actual extinction coeff
      value = value / elem.density;
      //std::cout<<" "<<value;
      absorption.GetPortalControl().Set( i * num_bins + j,value);
    }

  }
}

}// namespace rover
#endif
