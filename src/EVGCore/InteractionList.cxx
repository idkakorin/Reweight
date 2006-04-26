//____________________________________________________________________________
/*
 Copyright (c) 2003-2006, GENIE Neutrino MC Generator Collaboration
 All rights reserved.
 For the licensing terms see $GENIE/USER_LICENSE.

 Author: Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
         CCLRC, Rutherford Appleton Laboratory - May 13, 2005

 For the class documentation see the corresponding header file.

 Important revisions after version 2.0.0 :

*/
//____________________________________________________________________________

#include "EVGCore/InteractionList.h"
#include "Interaction/Interaction.h"
#include "Messenger/Messenger.h"

using std::endl;
using namespace genie;

//____________________________________________________________________________
namespace genie {
 ostream & operator << (ostream & stream, const InteractionList & intl)
 {
   intl.Print(stream);

   return stream;
 }
}
//___________________________________________________________________________
InteractionList::InteractionList() :
vector<Interaction *>()
{

}
//___________________________________________________________________________
InteractionList::InteractionList(const InteractionList & intl) :
vector<Interaction *>()
{
  this->Copy(intl);
}
//___________________________________________________________________________
InteractionList::~InteractionList()
{
  this->Reset();
}
//___________________________________________________________________________
void InteractionList::Reset(void)
{
  InteractionList::const_iterator iter;

  for(iter = this->begin(); iter != this->end(); ++iter) {
    Interaction * interaction = *iter;
    delete interaction;
    interaction = 0;
  }
  this->clear();
}
//___________________________________________________________________________
void InteractionList::Append(const InteractionList & intl)
{
  InteractionList::const_iterator iter;
  for(iter = intl.begin(); iter != intl.end(); ++iter) {
    Interaction * interaction = *iter;
    this->push_back(new Interaction(*interaction));
  }
}
//___________________________________________________________________________
void InteractionList::Copy(const InteractionList & intl)
{
  this->Reset();

  InteractionList::const_iterator iter;
  for(iter = intl.begin(); iter != intl.end(); ++iter) {
    Interaction * interaction = *iter;
    this->push_back(new Interaction(*interaction));
  }
}
//___________________________________________________________________________
void InteractionList::Print(ostream & stream) const
{
  InteractionList::const_iterator iter;

  for(iter = this->begin(); iter != this->end(); ++iter) {
    Interaction * interaction = *iter;
    if(interaction) stream << *interaction;
    else            stream << "\n******** NULL INTERACTION ********" << endl;
  }
}
//___________________________________________________________________________
InteractionList & InteractionList::operator = (const InteractionList & intl)
{
  this->Copy(intl);
  return (*this);
}
//___________________________________________________________________________

