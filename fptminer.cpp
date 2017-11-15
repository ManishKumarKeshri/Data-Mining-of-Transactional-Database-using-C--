#include <bits/stdc++.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <exception>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iterator>
#include <bits/stdc++.h>
#include <unordered_map>
using namespace std;
typedef vector<string> temp;
class FP_Node{
public:
    string item;
    double frequency;
    FP_Node* node;
    FP_Node* parent;
    std::vector<FP_Node*> children;
    FP_Node(string item,FP_Node* parent):children(){
      this->item=item;
      this->frequency=1;
      this->node=NULL;
      this->parent=parent;
    }
};
class FPTree{
public:
    FP_Node* root;
    map<string,FP_Node*> head_table;
    double minsup;
    double minconf;
    std::map<std::string,double> itemfrequency;
    bool IsSubset(std::vector<string> A, std::vector<string> B)
    {
      std::sort(A.begin(), A.end());
      std::sort(B.begin(), B.end());
      return std::includes(A.begin(), A.end(), B.begin(), B.end());
    }
    FPTree(vector<vector<string>> transactions,double minsup,double minconf, string outputfile)
    {
      ofstream myfile;
      myfile.open(outputfile);
      root=new FP_Node("",NULL);
      this->minconf=minconf;
      this->minsup=minsup;
        for(vector<string> transaction:transactions){
            for(string& item:transaction){
                ++itemfrequency[item];
            }
        }
    std::map<std::string,double>::iterator it= itemfrequency.begin();
    while(it!=itemfrequency.end()){
      if(it->second<minsup){
          itemfrequency.erase(it++);
        }
      else
        it++;
    }
    struct cmpStruct {
        bool operator() (pair<double,string> one,pair<double,string> two) const
        {
            return (one.first >two.first) or (not(one.first > two.first) and one.second < two.second);
        }
    };
    std::set<pair<double,string>,cmpStruct> itemsByFrequency;
    for(auto& pair:itemfrequency) {
        string item = pair.first;
        double frequency = pair.second;
        itemsByFrequency.insert( { frequency, item } );
    }
     for ( vector<string>& transaction : transactions ) {
        FP_Node* cnode = root;
        for (const auto& pair : itemsByFrequency ) {
            string item = pair.second;
            if ( std::find( transaction.begin(), transaction.end(), item ) != transaction.end() ) {
                const auto t = std::find_if(
                    cnode->children.cbegin(), cnode->children.cend(),  [item](FP_Node* fpnode) {
                        return fpnode->item == item;
                } );
                if ( t == cnode->children.end() ) {
                    FP_Node* cnode_new_child = new FP_Node( item, cnode );
                    cnode->children.push_back( cnode_new_child );
                    if ( head_table.count( cnode_new_child->item ) ) {
                        FP_Node* prev_fpnode = head_table[cnode_new_child->item];
                        while ( prev_fpnode->node ) { prev_fpnode = prev_fpnode->node; }
                        prev_fpnode->node = cnode_new_child;
                    }
                    else {
                        head_table[cnode_new_child->item] = cnode_new_child;
                    }
                    cnode = cnode_new_child;
                }
                else {
                    FP_Node* curr_fpnode_child = *t;
                    ++curr_fpnode_child->frequency;
                    cnode = curr_fpnode_child;
                }
            }
        }
    }
    std::map<std::vector<string>,double> frequentpatterns;
    std::map<std::string,FP_Node*>::iterator itx= head_table.begin();
    while(itx!=head_table.end()){
      std::map<std::string,double> conditionalbasepattern;
      FP_Node* itm=itx->second;
      double supofcpattern=0;
      while(itm->node!=NULL){
        supofcpattern+=itm->frequency;
        FP_Node* conditem=itm->parent;
        while(conditem!=root)
        {
          conditionalbasepattern[conditem->item]+=itm->frequency;
          conditem=conditem->parent;
        }
        itm=itm->node;
       }
        std::map<std::string,double>::iterator itmm= conditionalbasepattern.begin();
        while(itmm!=conditionalbasepattern.end()){
          if(itmm->second<minsup){
              conditionalbasepattern.erase(itmm++);
            }
          else
            itmm++;
        }
      vector<string> temp;
      std::map<std::string,double>::iterator itmn= conditionalbasepattern.begin();
      while(itmn!=conditionalbasepattern.end()){
          temp.push_back(itmn->first);
          itmn++;
      	}
      int n=temp.size();
      vector<string> firstpattern;
      firstpattern.push_back(itm->item);
      frequentpatterns[firstpattern]=supofcpattern;
      for (int len = 1; len <= n; len++)
      {
        for (int i = 0; i <= n - len; i++)
        {
            int j = i + len - 1;
            double supoffreqpattern=DBL_MAX;
            vector<string> freqpattern;
            freqpattern.push_back(itm->item);
            for (int k = i; k <= j; k++)
                {
                supoffreqpattern=min(supoffreqpattern,conditionalbasepattern[temp[k]]);
                freqpattern.push_back(temp[k]);
                }
            frequentpatterns[freqpattern]+=supoffreqpattern;
        }
      }
      itx++;
    }

    vector<vector<string>> temp2;
    std::map<std::vector<string>,double>::iterator itmno= frequentpatterns.begin();
    while(itmno!=frequentpatterns.end()){
        temp2.push_back(itmno->first);
        itmno++;
      }
    int count=0;
    int n2=temp2.size();
    if(minsup>=25)
    {
    std::sort(temp2.begin(), temp2.end(), [](const vector<string> & a, const vector<string> & b){ return a.size() < b.size(); });
    int sze=temp2.size();
    int size2=temp2[sze-1].size();
    for (int j2 = 0; j2 <n2; j2++)
    {
      bool boolA[sze] = { false };
      for (int i2 = j2+1;(temp2[j2].size()+temp2[i2].size())<size2; i2++)
      {
        vector<vector<string>> tx;
        vector <string> t;
        t.insert( t.end(), temp2[j2].begin(), temp2[j2].end() );
        t.insert( t.end(), temp2[i2].begin(), temp2[i2].end() );
        if((frequentpatterns.find(t)!=frequentpatterns.end()) && !(IsSubset(temp2[j2],temp2[i2])) && (!boolA[i2]))
        {
          double tmp=frequentpatterns[t]/frequentpatterns[temp2[j2]];
          if((tmp>=minconf))
          {
          for (vector<string>::const_iterator iter = temp2[j2].begin();
          iter !=temp2[j2].end(); ++iter)
          myfile << *iter <<" ";
          myfile<<" | ";
          for (vector<string>::const_iterator iter2 = temp2[i2].begin();
          iter2 !=temp2[i2].end(); ++iter2)
          myfile << *iter2 <<" ";
          myfile<<" | ";
          myfile<<frequentpatterns[t];
          myfile<<" | ";
          myfile<<tmp;
          myfile<<endl;
          count++;
          }
          else{
            tx.push_back(temp2[j2]);
          }
        }
        for (int k2 = 0; k2 <tx.size(); k2++)
        {
          for(int m2=0;m2<n2;m2++)
          {
            if((IsSubset(temp2[m2],tx[k2])) && (!boolA[m2]))
            boolA[m2]=true;
          }
        }
      }
    }
  }
  else{
    for(int ii=0;ii<temp2.size();ii++)
    {
      for (vector<string>::const_iterator iter = temp2[ii].begin();
      iter !=temp2[ii].end(); ++iter)
      myfile << *iter <<" ";
      myfile<<" | ";
      myfile<<"{}";
      myfile<<" | ";
      myfile<<frequentpatterns[temp2[ii]];
      myfile<<" | ";
      myfile<<"-1";
      myfile<<endl;
    }
  }
  }
};

int main(int argc,char* argv[])
{
  if(argc!=5)
  return 0;
  clock_t tStart = clock();
  std::map<std::string,vector<std::string>> transcationinMap;
  std::ifstream infile(argv[3]);
  double x1;
  std::string x;
  std::string y;
  while(!infile.eof()){
    infile>>x;
    infile>>y;
    if(transcationinMap.find(x) != transcationinMap.end() )
    {
      vector<string> itemlist=transcationinMap[x];
      itemlist.push_back(y);
      transcationinMap[x]=itemlist;
    }
    else
    {
      vector<string> firstitem;
      firstitem.push_back(y);
    transcationinMap[x]=firstitem;
    }
  }
  std::map<std::string,vector<string>>::iterator itx= transcationinMap.begin();
  vector<vector<string>> transactions;
  while(itx!=transcationinMap.end()){
    transactions.push_back(itx->second);
    itx++;
  }
  double minsup = atof(argv[1]);
  double minconf = atof(argv[2]);
  FPTree fp_tree{ transactions, minsup ,minconf,argv[4]};
  printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
  return 0;
}
