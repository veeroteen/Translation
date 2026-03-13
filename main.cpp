#include <iostream>
#include "Tables.h"
#include <stdexcept>
#include <fstream>
int main()
{
   std::string testFolder = "test1/";
   StaticTable keywords;
   {
      std::ifstream stream(testFolder + "Keywords.txt");
      std::string buff = "";
      std::vector<std::string> bArr;
      while (std::getline(stream, buff))
      {
         bArr.push_back(buff);
      }
      keywords.load(std::move(bArr));
   }
   StaticTable operators;
   {
      std::ifstream stream(testFolder + "Operators.txt");
      std::string buff = "";
      std::vector<std::string> bArr;
      while (std::getline(stream, buff))
      {
         bArr.push_back(buff);
      }
      operators.load(std::move(bArr));
   }
   StaticTable separators;
   {
      std::ifstream stream(testFolder + "Separators.txt");
      std::string buff = "";
      std::vector<std::string> bArr;
      while (std::getline(stream, buff))
      {
         bArr.push_back(buff);
      }
      separators.load(std::move(bArr));
   }
   DynamicTable constants;
   DynamicTable identificators;
   std::vector<Lexeme> lexems;
   {
      std::ifstream stream(testFolder + "Program.txt");
      std::string buff = "";
      int lspace = 0,rspace = 0;
      while (std::getline(stream, buff))
      {
         rspace = buff.find(" ", lspace);
         while (lspace != -1)
         {
            std::string substr = buff.substr(lspace, rspace == -1 ? buff.size() : rspace);
            


            lspace = rspace;
            rspace = buff.find(" ", lspace);
         }

      }

   }




   return 0;
}

