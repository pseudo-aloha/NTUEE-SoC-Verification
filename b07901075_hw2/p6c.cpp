/****************************************************************************
  FileName     [ testBdd.cpp ]
  PackageName  [ ]
  Synopsis     [ Define main() ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "bddNode.h"
#include "bddMgr.h"

using namespace std;

/**************************************************************************/
/*                        Define Global BDD Manager                       */
/**************************************************************************/
BddMgr bm;

/**************************************************************************/
/*                    Define Static Function Prototypes                   */
/**************************************************************************/
static void initBdd(size_t nSupports, size_t hashSize, size_t cacheSize);


/**************************************************************************/
/*                             Define main()                              */
/**************************************************************************/
int
main()
{
   initBdd(6, 127, 61);

   /*-------- THIS IS JUST A TEST CODE ---------*/
   BddNode a(bm.getSupport(1));
   BddNode b(bm.getSupport(2));
   BddNode c(bm.getSupport(3));
   BddNode d(bm.getSupport(4));
   BddNode e(bm.getSupport(5));
   BddNode f(bm.getSupport(6));

   BddNode g1 = b & d;
   // cout << "bdd g1" << endl;
   // cout << g1 << endl;

   BddNode g2 = e & ~c;
   // cout << "bdd g2" << endl;
   // cout << g2 << endl;

   BddNode g3 = a & b;
   // cout << "bdd g3" << endl;
   // cout << g3 << endl;

   BddNode g4 = c & g1;
   // cout << "bdd g4" << endl;
   // cout << g4 << endl;

   BddNode g5 = g1 | g2;
   // cout << "bdd g5" << endl;
   // cout << g5 << endl;

   BddNode g6 = g2 | d;
   // cout << "bdd g6" << endl;
   // cout << g6 << endl;

   BddNode g7 = g6 & g3;
   // cout << "bdd g7" << endl;
   // cout << g7 << endl;

   BddNode g8 = g4 | g7;
   // cout << "bdd g8" << endl;
   // cout << g8 << endl;

   BddNode g9 = f & g8;
   // cout << "bdd g9" << endl;
   // cout << g9 << endl;

   BddNode g9_g5 = g9 & g5;
   cout << "g9(g5 = 1)" << endl;
   cout << g9_g5 << endl;

   BddNode g9_ng5 = g9 & ~g5;
   cout << "g9(g5 = 0)" << endl;
   cout << g9_ng5 << endl;

   BddNode diff = g9_g5 ^ g9_ng5;
   cout << "bdd diff" << endl;
   cout << diff << endl;


   /*----------- END OF TEST CODE ------------*/
}


/**************************************************************************/
/*                          Define Static Functions                       */
/**************************************************************************/
static void
initBdd(size_t nin, size_t h, size_t c)
{
   BddNode::_debugBddAddr = true;
   BddNode::_debugRefCount = true;

//   bm.reset();
   bm.init(nin, h, c);
}

