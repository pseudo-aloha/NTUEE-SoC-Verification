#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string.h>
#include <vector>
#include "sat.h"
#include "Solver.h"

using namespace std;

class Gate
{
public:
   Gate(unsigned i = 0): _gid(i) {}
   ~Gate() {}

   Var getVar() const { return _var; }
   void setVar(const Var& v) { _var = v; }

private:
   unsigned   _gid;  // for debugging purpose...
   Var        _var;
};

int main(int argc, char **argv)
{
   ifstream ifs;
   char buffer[256];
   bool is_firsr_line = true;
   int num_men;
   vector<int> Assign;
   vector<int> AssignNot;
   vector<int> LessThan;
   vector<int> Adjasent;
   vector<int> AdjasentNot;
   if (argc != 2) {
      cerr << "Error: Missing input file!!" << endl;
      exit(-1);
   }
   else
   {
      ifs.open(argv[1]);

      if(!ifs.is_open())
      {
         cerr << "Error: Wrong input file!!" << endl;
         exit(-1);
      }
      else
      {
         while(!ifs.eof())
         {
            ifs.getline(buffer, sizeof(buffer));
            string tmp_str = buffer;
            if(is_firsr_line)
            {
               num_men = stoi(buffer);
               is_firsr_line = false;
            }
            else
            {
               tmp_str = tmp_str.substr(0, tmp_str.find("("));
               if(tmp_str == "Assign")
               {
                  tmp_str = buffer;
                  Assign.push_back(stoi(tmp_str.substr(tmp_str.find("(") + 1, tmp_str.find(","))));
                  Assign.push_back(stoi(tmp_str.substr(tmp_str.find(",") + 2, tmp_str.find(")"))));
               }
               else if(tmp_str == "AssignNot")
               {
                  tmp_str = buffer;
                  AssignNot.push_back(stoi(tmp_str.substr(tmp_str.find("(") + 1, tmp_str.find(","))));
                  AssignNot.push_back(stoi(tmp_str.substr(tmp_str.find(",") + 2, tmp_str.find(")"))));
               }
               else if(tmp_str == "LessThan")
               {
                  tmp_str = buffer;
                  LessThan.push_back(stoi(tmp_str.substr(tmp_str.find("(") + 1, tmp_str.find(","))));
                  LessThan.push_back(stoi(tmp_str.substr(tmp_str.find(",") + 2, tmp_str.find(")"))));
               }
               else if(tmp_str == "Adjacent")
               {
                  tmp_str = buffer;
                  Adjasent.push_back(stoi(tmp_str.substr(tmp_str.find("(") + 1, tmp_str.find(","))));
                  Adjasent.push_back(stoi(tmp_str.substr(tmp_str.find(",") + 2, tmp_str.find(")"))));
               }
               else if(tmp_str == "AdjacentNot")
               {
                  tmp_str = buffer;
                  AdjasentNot.push_back(stoi(tmp_str.substr(tmp_str.find("(") + 1, tmp_str.find(","))));
                  AdjasentNot.push_back(stoi(tmp_str.substr(tmp_str.find(",") + 2, tmp_str.find(")"))));
               }
               
            }
        }
        ifs.close();
      }
   }
   vector<Gate *> gates;
   vector<vector<Gate *> > gate_arr;


   // Init gates
   for(size_t i = 0; i < num_men; ++i)
   {
      gates.clear();
      for(size_t j = 0; j < num_men; ++j)
      {
         gates.push_back(new Gate(num_men * i + j + 1));
      }
      gate_arr.push_back(gates);
   }

   SatSolver solver;
   solver.initialize();
   solver.assumeRelease();  // Clear assumptions
   // Allocate and record variables; No Var ID for POs
   for(size_t i = 0; i < num_men; ++i)
   {
      for(size_t j = 0; j < num_men; ++j)
      {
         Var v = solver.newVar();
         gate_arr[i][j]->setVar(v);
      }
   }

   // for(size_t i = 0; i < num_men; ++i)
   // {
   //    for(size_t j = 0; j < num_men; ++j)
   //    {
   //       cout << i << " " << j << " "  << gate_arr[i][j] -> getVar() << endl;
   //    }
   // }
   vec<Lit> lits;
   
   // 1. a person must appear in exactly a seat and a seat can only contain one person
   // 1. (1) a person must appear in a seat
   for(size_t i = 0; i < num_men; ++i)
   {
      lits.clear();
      for(size_t j = 0; j < num_men; ++j)
      {
         lits.push(Lit(gate_arr[j][i] -> getVar()));
         // cout << "lits size " << lits.size() << endl;
      }
      solver.addCNF(lits);
      
   }
   // 1. (2) a person cannot appear in two seats simultaneously
   for(size_t i = 0; i < num_men; ++i)
   {
      
      for(size_t j = i; j < num_men - 1; ++j)
      {
         lits.clear();
         lits.push(~Lit(gate_arr[j][i] -> getVar()));
         lits.push(~Lit(gate_arr[j + 1][i] -> getVar()));
         solver.addCNF(lits);
      }
      // cout << "lits " << lits[0] <<endl;
      
   }


   // 1. (3) a seat can only contain one person
   for(size_t i = 0; i < num_men; ++i)
   {
      for(size_t j = 0; j < num_men ; ++j)
      {
         for(size_t k = 0; k < num_men; ++k)
            if(k != j)
            {
               lits.clear();
               lits.push(~Lit(gate_arr[i][j] -> getVar()));
               lits.push(~Lit(gate_arr[i][k] -> getVar()));
               solver.addCNF(lits);
            }
      }
      // cout << "lits " << lits[0] <<endl;
      
   }

   // 2. Assign
   if(!Assign.empty())
   {
      for(size_t i = 0; i < Assign.size(); i += 2)
      {
         solver.assumeProperty(gate_arr[Assign[i + 1]][Assign[i]] -> getVar(), 1);
      }
   }
   
   // 3. AssignNot
   if(!AssignNot.empty())
   {
      for(size_t i = 0; i < AssignNot.size(); i += 2)
      {
         solver.assumeProperty(gate_arr[AssignNot[i + 1]][AssignNot[i]] -> getVar(), 0);
      }
   }

   // 4. LessThan
   if(!LessThan.empty())
   {
      for(size_t i = 0; i < LessThan.size(); i += 2)
      {
         solver.assumeProperty(gate_arr[0][LessThan[i + 1]] -> getVar(), 0);
         for(size_t j = 1; j < num_men; ++j)
         {
            lits.clear();
            lits.push(~Lit(gate_arr[j][LessThan[i + 1]] -> getVar()));
            for(size_t k = 0; k < j; ++k)
            {
               lits.push(Lit(gate_arr[k][LessThan[i]] -> getVar()));
            }
            solver.addCNF(lits);
         }
      }
   }

   // 5. Adjasent
   if(!Adjasent.empty())
   {
      
      for(size_t i = 0; i < Adjasent.size(); i += 2)
      {
         lits.clear();
         lits.push(~Lit(gate_arr[0][Adjasent[i]] -> getVar()));
         lits.push(Lit(gate_arr[1][Adjasent[i + 1]] -> getVar()));
         solver.addCNF(lits);
         lits.clear();
         lits.push(~Lit(gate_arr[num_men - 1][Adjasent[i]] -> getVar()));
         lits.push(Lit(gate_arr[num_men - 2][Adjasent[i + 1]] -> getVar()));
         solver.addCNF(lits);
         for(size_t j = 1; j < num_men - 1; ++j)
         {
            lits.clear();
            lits.push(~Lit(gate_arr[j][Adjasent[i]] -> getVar()));
            lits.push(Lit(gate_arr[j - 1][Adjasent[i + 1]] -> getVar()));
            lits.push(Lit(gate_arr[j + 1][Adjasent[i + 1]] -> getVar()));
            solver.addCNF(lits);
         }
      }
   }

   // 6. AdjasentNot
   if(!AdjasentNot.empty())
   {
      
      for(size_t i = 0; i < AdjasentNot.size(); i += 2)
      {
         lits.clear();
         lits.push(~Lit(gate_arr[0][AdjasentNot[i]] -> getVar()));
         lits.push(~Lit(gate_arr[1][AdjasentNot[i + 1]] -> getVar()));
         solver.addCNF(lits);
         lits.clear();
         lits.push(~Lit(gate_arr[num_men - 1][AdjasentNot[i]] -> getVar()));
         lits.push(~Lit(gate_arr[num_men - 2][AdjasentNot[i + 1]] -> getVar()));
         solver.addCNF(lits);
         for(size_t j = 1; j < num_men - 1; ++j)
         {
            lits.clear();
            lits.push(~Lit(gate_arr[j][AdjasentNot[i]] -> getVar()));
            lits.push(~Lit(gate_arr[j - 1][AdjasentNot[i + 1]] -> getVar()));
            solver.addCNF(lits);
            lits.clear();
            lits.push(~Lit(gate_arr[j][AdjasentNot[i]] -> getVar()));
            lits.push(~Lit(gate_arr[j + 1][AdjasentNot[i + 1]] -> getVar()));
            solver.addCNF(lits);
         }
      }
   }

   // solve
   bool result;
   result = solver.assumpSolve();
   if (result) {
      cout << "Satisfiable assignment:" << endl;
      for (size_t i = 0, n = gate_arr.size(); i < n; ++i)
      {
         for (size_t j = 0, n = gate_arr[0].size(); j < n; ++j)
         {
            // cout << solver.getValue(gate_arr[i][j]->getVar());
            if(solver.getValue(gate_arr[i][j]->getVar()))
            {
               cout << i << "(" << j << ")";
               if(i != n-1)
               {
                  cout << ", ";
               }
               else
               {
                  cout << endl;
               }
            }
         }
         // cout << endl;
      }
         
   }
   else{
      cout << "No satisfiable assignment can be found." << endl;
   }
}
