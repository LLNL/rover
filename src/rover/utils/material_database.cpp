#include <utils/material_database.hpp>
#include <sstream>

namespace rover {

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
  db_reader.open("/nfs/tmp2larsen30/rover_api/src/rover/utils/opacities.dat");
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
  std::cout<<"Read "<<Elements.size()<<" elements\n";
}

double 
MaterialDatabase::sample_bin(const Element &elem, const double &x)
{
  int n = elem.energy_bins.size();
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

template<typename T>
void
MaterialDatabase::get_elements(const std::vector<std::string> &symbols,
                              const int &num_bins,
                              vtkm::cont::ArrayHandle<T> &output)
{
  double spectrumLength = maxMeV - minMeV;
  output.Allocate(symbols.size() * num_bins);
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
    Element elem = it->second;
    
    for(int j = 0; j < num_bins; ++j)
    {
      double position = minMeV + (double)j * seg;
      double value = this->sample_bin(elem,position);
      // Devide by element density
      // This value will have units if 1/cm
      // and will be multiplied by distance (cm)
      // to obtain the actual extinction coeff
      value = value / elem.density;
      output.GetPortalControl().Set( i * num_bins + j,value);
    }

  }
}

//Explicit instantiations
template void MaterialDatabase::get_elements<vtkm::Float32>(const std::vector<std::string> &symbols,
                                                            const int &num_bins,
                                                            vtkm::cont::ArrayHandle<vtkm::Float32> &output);

} // namespace rover
