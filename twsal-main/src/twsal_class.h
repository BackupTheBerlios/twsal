#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdlib.h>

using namespace std;

#include <twsal.h>


class twsal_class : public twsal
{
     public:
          twsal_memory * mem; 
          inline twsal_class()
          {
               mem = new twsal_memory(0);
          }
          inline void initialize()
          {
               execute(mem);
          }
};

