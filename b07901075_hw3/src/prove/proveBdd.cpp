/****************************************************************************
  FileName     [ proveBdd.cpp ]
  PackageName  [ prove ]
  Synopsis     [ For BDD-based verification ]
  Author       [ ]
  Copyright    [ Copyleft(c) 2010-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include "v3NtkUtil.h"
#include "v3Msg.h"
#include "bddMgrV.h"
#include <math.h>

void
BddMgrV::buildPInitialState()
{
   // TODO : remember to set _initState
   // Set Initial State to All Zero
   V3NtkHandler* const handler = v3Handler.getCurHandler();
   V3Ntk* const ntk = handler -> getNtk();
   // initialize _initState to be tautology
   _initState = BddNodeV::_one;
   // use & operation to make each Latch to be 0
   for(size_t i = 0; i < ntk -> getLatchSize(); ++i)
   {
     _initState = _initState & (~getBddNodeV(ntk -> getLatch(i).id));
   }
   // make sure that the _reachStates is empty
   _reachStates.clear();
   assert(_reachStates.empty());
   // store the _initstate to _reachStates
   _reachStates.push_back(_initState);
}

void
BddMgrV::buildPTransRelation()
{
   // TODO : remember to set _tr, _tri
   V3NtkHandler* const handler = v3Handler.getCurHandler();
   V3Ntk* const ntk = handler -> getNtk();
   BddNodeV D, Y;
   _tri = BddNodeV::_one;
   for(size_t i = 0; i < ntk -> getLatchSize(); ++i)
   {
      Y = bddMgrV -> getBddNodeV(handler -> getNetNameOrFormedWithId(ntk -> getLatch(i))+"_ns");
      D = getBddNodeV(ntk -> getInputNetId(ntk -> getLatch(i), 0).id);
      if(isV3NetInverted(ntk -> getInputNetId(ntk -> getLatch(i), 0))) D = ~D;
      _tri = (_tri & (~(Y ^ D))); 
   }
   _tr = getPTri();
   for(size_t i = 0; i < ntk -> getInputSize(); ++i)
   {
      _tr = _tr.exist(ntk -> getInput(i).id);
   }
}

void
BddMgrV::buildPImage( int level )
{
   // TODO : remember to add _reachStates and set _isFixed
   // Note:: _reachStates record the set of reachable states
   V3NtkHandler* const handler = v3Handler.getCurHandler();
   V3Ntk* const ntk = handler -> getNtk();
   bool ismoved;
   BddNodeV S, R, R_prime;
   for(int i = 0; i < level; ++i)
   {
      // if the reachable state is reached, don't bother to compute the following
      if(i == 0 && _isFixed)
      {
         cout << "Fixed point is reached (time : " << _reachStates.size() - 1 << ")" << endl;
         break;
      }
      cout << "Computing reachable state (time : " << _reachStates.size() - 1 << " )..." << endl;

      // compute current state
      if(_reachStates.size() < 2)
      {
         S = (getPTr() & getPReachState()); // tr & reachable_states
      }
      else
      {
         S = (getPTr() & bddMgrV -> restrict(getPReachState(), ~(_reachStates[_reachStates.size() - 2])));
      }
      

      // conduct existential quantifier on X
      for(size_t j = 0; j < ntk -> getLatchSize(); ++j)
      {
         S = S.exist(ntk -> getLatch(j).id);
      }

      // move Y -> X
      S = S.nodeMove(bddMgrV -> getBddNodeV(handler->getNetNameOrFormedWithId(ntk -> getLatch(0))+"_ns").getLevel(), ntk -> getLatch(0).id, ismoved);
      
      // make sure that the moce operation is success
      assert(ismoved == true);

      // check if is fixed
      R = getPReachState(); // previous reach states
      R_prime = (getPReachState() | S); // the present reach states
      if(R == R_prime)
      {
         cout << "Fixed point is reached (time : " << _reachStates.size() - 1 << ")" << endl;
         _isFixed = true;
         break;
      }
      else
      {
         _reachStates.push_back(R_prime);
      }
   }
}

void 
BddMgrV::runPCheckProperty( const string &name, BddNodeV monitor )
{
   // TODO : prove the correctness of AG(~monitor)
   V3NtkHandler* const handler = v3Handler.getCurHandler();
   V3Ntk* const ntk = handler -> getNtk();
   if(_isFixed && (monitor & getPReachState()) == BddNodeV::_zero)
   {
      cout << "Monitor \"" << name << "\" is safe." << endl;
   }
   else
   {
      if((monitor & getPReachState()) == BddNodeV::_zero)
      {
         cout << "Monitor \"" << name << "\" is safe up to time " << _reachStates.size() - 1 << "." << endl;
      }
      else
      {
         cout << "Monitor \"" << name << "\" is violated." << endl;
         cout << "Counter Example:" << endl;
         computeCounter(monitor);
      }
   }
}

void 
BddMgrV::computeCounter( BddNodeV monitor )
{
   V3NtkHandler* const handler = v3Handler.getCurHandler();
   V3Ntk* const ntk = handler -> getNtk();
   size_t violate_step =  _reachStates.size() - 1;
   size_t find_left_bracket = 0, find_neg = 0, find_right_bracket = 0;
   for(violate_step; ; --violate_step)
   {
      if((_reachStates[violate_step] & monitor) == BddNodeV::_zero) break;
   }
   violate_step += 1;
   BddNodeV curr_state = (_reachStates[violate_step] & monitor).getCube(), I;
   BddNodeV previous_state;
   bool counterExmaple[violate_step + 1][ntk -> getInputSize()];
   bool ismoved = false;
   string tmp_str;

   
   for(int i = violate_step - 1; i >= 0; --i)
   {
      curr_state = curr_state.nodeMove(ntk -> getLatch(0).id, bddMgrV -> getBddNodeV(handler->getNetNameOrFormedWithId(ntk -> getLatch(0))+"_ns").getLevel(), ismoved);
      assert(ismoved);
      I = curr_state & _tri;
      if( i > 0)
      {
         for(size_t j = 0; j < ntk -> getLatchSize(); ++j)
         {
            I = I.exist(ntk -> getLatch(j).id);
            I = I.exist(ntk -> getLatch(j).id + ntk -> getLatchSize());
         }
      }
      else
      {
        for(size_t j = 0; j < ntk -> getLatchSize(); ++j)
         {
            I = I.exist(ntk -> getLatch(j).id + ntk -> getLatchSize());
         }
      }
      tmp_str = I.getCube().toString();
      for(size_t j = 0; j < ntk -> getInputSize(); ++j)
      {
         if(!tmp_str.empty())
         {
            find_left_bracket = tmp_str.find("(");
            find_neg = tmp_str.substr(0, find_left_bracket).find('!');
            find_right_bracket = tmp_str.find(")");
            if(find_neg == string::npos)
            {
               counterExmaple[i][j] = 1;
            }
            else
            {
               counterExmaple[i][j] = 0;
            }
            tmp_str = tmp_str.substr(find_right_bracket + 1);
         }
         else
         {
            counterExmaple[i][j] = 1;
         }
      }
      
      previous_state = curr_state & _tri & I;
      for(size_t j = 0; j < ntk -> getLatchSize(); ++j)
      {
         previous_state = previous_state.exist(ntk -> getLatch(j).id + ntk -> getLatchSize());
      }
      for(size_t j = 0; j < ntk -> getInputSize(); ++j)
      {
         previous_state = previous_state.exist(ntk -> getInput(j).id);
      }
      curr_state = previous_state;
   }

   for(size_t j = 0; j < ntk -> getInputSize(); ++j)
   {
      counterExmaple[violate_step][j] = 1;
   }

   
   for(size_t i = 0; i <= violate_step; ++i)
   {
      cout << i << ": ";
      for(size_t j = 0; j < ntk -> getInputSize(); ++j)
      {
         cout << counterExmaple[i][j];
      }
      cout << endl;
   }
}