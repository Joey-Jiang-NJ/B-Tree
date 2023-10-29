#include <iostream>
#include <iomanip>
#include <array>
#include <algorithm>
#include "set.h"
using namespace std;
using namespace main_savitch_11;


template<class Item>
set<Item>::set( )
{
  data_count=0;
  child_count=0;
  for(size_t i = 0; i < MAXIMUM+2; i++)
  {
      subset[i] = NULL;
  }
}

template<class Item>
set<Item>::set(const set& source)
{
  data_count=source.data_count;
  child_count=source.child_count;
  for(int i=0; i<data_count; i++){
    data[i]=source.data[i];
  }

  for(int i=0; i<child_count;i++){
    subset[i]=source->subset[i];
  }
}

// MODIFICATION MEMBER FUNCTIONS
template<class Item>
void set<Item>::operator =(const set& source)
{
  if(this == &source){
    return;
  }
  clear();
  this=&source;
}

template <class Item>
void set<Item>::b_tree_clear(set<Item>*& root_ptr) {
    if (root_ptr != NULL) {
        for (auto& v : root_ptr->data) {
            v = Item();
        }
        for (int i = 0; i < root_ptr->child_count; i++) {
            b_tree_clear(root_ptr->subset[i]);
        }
        delete root_ptr;
        root_ptr = NULL;
    }
}

template<class Item>
void set<Item>::clear( )
{
  for (auto& v : data) {
        v = Item();
    }
    for (auto& p : subset) {
        b_tree_clear(p);
    }
    data_count = 0;
    child_count = 0;
}

template<class Item>
bool set<Item>::insert(const Item& entry)
{
  if(!loose_insert(entry)){
    return false;
  }
  if(data_count>MAXIMUM){
    size_t ogCount = child_count;
    //shifts the root subsets to the right by 1
    for(size_t i = child_count ; i > 0; i--)
    {
        subset[i] = subset[i-1];
    }
    child_count++;
    //sets the data in subset[0] as a copy of the root data
    subset[0] = new set();
    for(size_t i = 0; i < MAXIMUM+1; i++)
    {
        subset[0]->data[i] = data[i];
    }
    subset[0]->data_count = data_count;
    //sets subset[0]'s subsets equal to the original subsets of the root
    //updates the child_count for the root and subset[0]
    for(size_t i = 1; i < child_count; i++)
    {
        subset[0]->subset[i-1] = subset[i];
    }
    child_count = 1;
    subset[0]->child_count = ogCount;
    data_count = 0;

    //fixes the excess entry problem in subtree[0]
    fix_excess(0);
  }
  return true;
}

template<class Item>
std::size_t set<Item>::erase(const Item& target)
{
  return 1;
}

// CONSTANT MEMBER FUNCTIONS
template<class Item>
std::size_t set<Item>::count(const Item& target) const
{
  size_t count=0;
  for(auto d : data){
    if(target=d){
      count++;
    }
  }

  if(is_leaf()){
    return count;
  }

  for(auto s : *subset){
    return count+s->count(target);
  }
}



// SUGGESTED FUNCTION FOR DEBUGGING
template<class Item>
void set<Item>::print(int indent) const
{
  const int EXTRA_INDENTATION = 4;
  size_t i;

  cout << setw(indent) << "";

  for(i = 0; i < data_count; ++i)
      cout << data[i] << " ";
  cout << endl;

  for(i = 0; i < child_count; ++i)
      subset[i]->print(indent+EXTRA_INDENTATION);
}

template<class Item>
size_t set<Item>::get_index(const Item& entry)
{
  for(size_t i=0; i<data_count; i++){
    if(entry<data[i]){
      return i;
    }
  }
  return data_count;
}


template<class Item>
bool set<Item>::loose_insert(const Item& entry)
{
  bool b;
  if(is_leaf()){
    for(int i=0; i<data_count; i++){
      if(data[i]<entry){
        continue;
      }
      else if(data[i]==entry){
        return false;
      }

      for(int j=data_count-1; j>=i+1; j--){
        data[j+1]=data[j];
      }
      data[i+1]=entry;
      data_count++;
      return true;
    }
  }

  for(int i=0; i<data_count+1; i++){ //compare entry to data[datacount+1] which might be empty
    if(data[i]==entry){
      return false;
    }
    else if(data[i]=NULL){ //compare entry to data[datacount+1] which might be empty
      b = subset[i]->loose_insert(entry);
      if(subset[i]->data_count>MAXIMUM){
        fix_excess(i);
      }
    }
    else if(entry<data[i]){
      b = subset[i]->loose_insert(entry);
      if(subset[i]->data_count>MAXIMUM){
        fix_excess(i);
      }
    }
  }
  return b;
}

template<class Item>
bool set<Item>::loose_erase(const Item& target)
{
  size_t index=0;
  bool found;
  bool b;
  while((index<data_count)&&(target>data[index])){
    index++;
  }

  if(target==data[index]){
    found=true;
  }
  else{
    found=false;
  }


  if(is_leaf()&& !found){
    return false;
  }
  if(is_leaf()&& found){
    for(size_t i=index; i<data_count; i++){
      data[i]=data[i+1];
    }
    data_count--;
    return true;
  }

  if(!(is_leaf)){
    for(size_t i=0; i<child_count; i++){
      b=subset[i]->loose_erase(target);
      if(subset[i]->data_count<MINIMUM){
        fix_shortage(i);
      }
    }
  }
  return b;
}

template<class Item>
void set<Item>::remove_biggest(Item& removed_entry)
{
  //remove the biggest item from the btree;
  if(is_leaf()){
    remove_entry = data[data_count-1];
    data_count--;
  }
  else{
    subset[child_count-1]->remove_biggest(removed_entry);
    if((subset[child_count-1]->data_count)<MINIMUM){
      fix_shortage(child_count-1);
    }
  }
}

template<class Item>
void set<Item>::fix_excess(std::size_t i)
{
  for(int ix=child_count-1; ix>i; i--){
    subset[ix+1] = subset[ix];
  }

  subset[i+1]  = new set<Item>;
  child_count++;

  for(int ix=MINIMUM+1; ix<MAXIMUM+2; ix++){
    subset[i+1]->subset[ix-MINIMUM-1]=subset[i]->subset[ix];
  }
  for(int ix=MINIMUM+1; ix<MAXIMUM+1; ix++){
    subset[i+1]->data[ix-MINIMUM-1]=subset[i]->subset[ix];
  }

  subset[i]->data_count=MINIMUM;
  subset[i]->child_count=MINIMUM+1;
  subset[i+1]->data_count=MINIMUM;
  subset[i+1]->child_count=MINIMUM+1;

  for(int ix=data_count-1; ix>=i; ix--){
    data[ix+1]=data[ix];
  }
  data[i]=subset[i]->data[MINIMUM];
  data_count++;
}

template<class Item>
void set<Item>::fix_shortage(std::size_t i)
{
  if(i > 0)
  {
      if(subset[i-1]->data_count > MINIMUM)
      {
          caseOne(i);
      }
      else if(subset[i-1]->data_count == MINIMUM)
      {
          caseThree(i);
      }
  }

  else if(i < child_count-1)
  {
      if(subset[i+1]->data_count > MINIMUM)
      {
          caseTwo(i);
      }
      else if(subset[i+1]->data_count == MINIMUM)
      {
          caseFour(i);
      }
  }

}

template<class Item>
void set<Item>::caseOne(size_t i)
{
  for(size_t j=data_count-1; j>=0; j--){
    subset[i]->data[j+1]=subset[i]->data[j];
  }

  subset[i]->data[0]=data[i-1];
  subset[i]->data_count++;

  data[i-1]=subset[i-1]->data[data_count-1];
  subset[i-1]->data_count--;

  if(!subset[i-1]->if_leaf()){
    for(size_t k=child_count-1; k>=0; k--){
      subset[i]->subset[k+1]=subset[i]->subset[k];
    }
    subset[i]->subset[0]=subset[i-1]->subset[child_count-1];
    subset[i]->child_count++;
    subset[i-1]->child_count--;
  }
  return;
}



template<class Item>
void set<Item>::caseTwo(size_t i)
{
  subset[i]->data[data_count]=data[i];
  subset[i]->data_count++;

  data[i]=subset[i+1]->data[0];

  for(size_t j=1; j<subset[i+1]->data_count; j++){
    subset[i+1]->data[j-1]=subset[i+1]->data[j];
  }
  subset[i+1]->data_count--;

  if(!subset->is_leaf()){
    subset[i]->subset[child_count]=subset[i+1]->subset[0];

    for(size_t k=1; k<subset[i]->child_count; k++){
      subset[i+1]->subset[k-1]=subset[i+1]->subset[k];
    }
    subset[i]->child_count++;
    subset[i+1]->child_count--;
  }
}

template<class Item>
void set<Item>::caseThree(size_t i)
{
  subset[i-1]->data[data_count]=data[i-1];
  subset[i-1]->data_count++;
  for(size_t j=i; j<data_count; j++){
    data[j-1]=data[j];
  }
  data_count--;
//transfer
  for(size_t k=0; k<subset[i]->child_count; k++){
    subset[i-1]->subset[child_count]=subset[i]->subset[k];
    subset[i-1]->child_count++;
  }
  for(size_t l=0; l<subset[i]->data_count; l++){
    subset[i-1]->data[data_count]=subset[i]->data[l];
    subset[i-1]->data_count++;
  }

  subset[i]->data_count=0;
  subset[i]->child_count=0;

  delete subset[i];
  for(size_t m=i; m<child_count-1; m++){
    subset[m]=subset[m+1];
  }
  child_count--;
  return;
}

template<class Item>
void set<Item>::caseFour(size_t i)
{
  //step 1- move down
  for(size_t j=subset[i+1]->data_count; j>=0; j--){
    subset[i+1]->data[j+1]=subset[i+1]->data[j];
  }
  subset[i+1]->data[0]=data[i];
  subset[i+1]->data_count++;

  for(size_t k=i; k<data_count-1; k++){
    data[k]=data[k+1];
  }
  data_count--;
  //step 2-transfer
  for(size_t l=subset[i+1]->child_count; l>=0; l--){
    subset[i+1]->subset[l+subset[i]->child_count]=subset[i+1]->subset[l];
  }
  for(size_t m=0; m<subset[i]->child_count; m++){
    subset[i+1]->subset[m]=subset[i]->subset[m];
  }
  subset[i+1]->child_count += subset[i]->child_count;

  for(size_t n=subset[i+1]->data_count; n>=0; n--){
    subset[i+1]->data[n+subset[i]->data_count]=subset[i+1]->data[n];
  }
  for(size_t p=0; p<subset[i]->data_count; p++){
    subset[i+1]->data[p]=subset[i]->data[p];
  }
  subset[i+1]->data_count += subset[i]->data_count;

  subset[i]->data_count=0;
  subset[i]->child_count=0;
  //delete and shift 
  delete subset[i];

  for(size_t q=i+1; q<child_count; q++){
    subset[q-1]=subset[q];
  }
  child_count--;
  return;
}
