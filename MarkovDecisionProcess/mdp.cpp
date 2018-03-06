/*
Name & Lastname:   Mehmet SARIOÐLU
ID:                1807262
*/

/*Libs*/
#include <cstdio>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <vector>
#include <fstream>
#include <cmath>

using namespace std;
static const string fileName="mdp.inp";
static int maxActionNumber;
class Action;
Action *actions;

float discountFactor;
class Node;
enum nodeType {Vortex='V',Teleport='T',Star='S',Goal='G'};
Node *skynetNodes;
vector< vector<float> > Value;
int GoalNodeValue=500;
vector<int> Policy;

class Node{
	private:
		int nodeNumber;
		nodeType type;
		//bool *actionTaken;
		vector<int> actionTaken;
	public:
		Node(){};
		void Init(const int nodeNumber,const nodeType type){
			this->nodeNumber=nodeNumber;
			this->type=type;
		}
		
		void SetAction(int actionNumber){
			actionTaken.push_back(actionNumber);
		}
		
		bool isActionCanTaken(int actionID){
			return (actionTaken[actionID]);
		}
		
		vector<int> getActionsTaken(){
			return actionTaken;
		}
		
		bool isGoal(){
			return((char)this->type=='G');	
		};
		
		void Print(){
			cout<<endl<<"Node Number: "<<this->nodeNumber<<" Type: "<<(char)this->type<<endl;
			cout<<"Posible Actions' IDs:           ";
			
			for(int i=0;i<actionTaken.size();i++){
				cout<<actionTaken[i]<<"  ";
			}
			cout<<endl<<endl;
		}
};

class Result{
	public:
	int possibleTransitionNode;
	int possibleTransitionPercentage;
	Result(){};
	Result(int possibleTransitionNode,int possibleTransitionPercentage){
		this->possibleTransitionNode=possibleTransitionNode;
		this->possibleTransitionPercentage=possibleTransitionPercentage;
	};
	void Print(){
		cout<<"node: "<<possibleTransitionNode<<" percentage: "<<possibleTransitionPercentage;
	}
};

class Effect{
	int nodeId;
	int reward;
	vector<Result> possibleTransitionResults;
	public:
	Effect(){};
	Effect(int nodeId,int reward){
		this->nodeId=nodeId;
		this->reward=reward;
	}
	
	addResult(const Result possibleTransitionResult){
		possibleTransitionResults.push_back (possibleTransitionResult);
	};
	void Print(){
		cout<<"  target Node: "<<nodeId<<" reward: "<<reward<<endl;
		for (int i = 0; i != possibleTransitionResults.size(); ++i) {
			cout<<"    possible result "<<i<<": ";
			possibleTransitionResults[i].Print();
			cout<<endl;
		}
	};
	int getNodeID(){
		return nodeId;
	}
	int getReward(){
		return reward;
	}
	vector<Result> getTransitions(){
		return possibleTransitionResults;
	}
	Result getPossibleTransition(int targetNode){
		for(int i=0;i<possibleTransitionResults.size();i++){
			if(possibleTransitionResults[i].possibleTransitionNode==targetNode)
				return possibleTransitionResults[i];
		}
	}
};

class Action{
	vector<Effect> effects;
	public:
		int id;
		Action(){};
		void addEffect(Effect *efc){
			effects.push_back(*efc);
		};
		void addResult(int efectId,Result *res){
			effects[efectId].addResult(*res);
		};
		void Print(){
			for (int i = 0; i != effects.size(); ++i) {
			cout<<" effect "<<i<<": "<<endl;
			effects[i].Print();
			cout<<endl;
			}
		}
		int getProbablity(int sourceNodeID,int targetNodeID){
			for(int i=0;i<effects.size();i++){
				if(effects[i].getNodeID()==sourceNodeID)
				return effects[i].getPossibleTransition(targetNodeID).possibleTransitionPercentage;
			}
			return 0;
		}
		vector<Result> getTransitions(int sourceNodeID){
			
			for(int i=0;i<effects.size();i++){
				if(effects[i].getNodeID()==sourceNodeID)
				return effects[i].getTransitions();
			}
			return vector<Result>();
		}
		int getReward(int sourceNodeID,int targetNodeID){
			for(int i=0;i<effects.size();i++){
				if(effects[i].getNodeID()==sourceNodeID)
				return effects[i].getReward();
			}
			return 0;
		}
};

bool getInput(){
	int lineNumber=0;
	char * tokenPtr;
	string line;
	ifstream sourceFile;
	sourceFile.open(fileName.c_str());
	
	if (sourceFile.is_open())
	  {
	  	int actID;int efcID=0;
	    while ( getline (sourceFile,line,'\n') )
	    {
	    	if(line[0]=='E')
	    	cout<<"File Read! "<<endl;
	    	else{
		      	//cout << "line begins" << '\n';
		      	if(lineNumber==0){//first line is for node types
		      		skynetNodes=new Node[line.length()];//Allocate all nodes
		      		
		      		for (int i=0;i<line.length();i++){//Init skynet nodes
		      			skynetNodes[i].Init(i,(nodeType)line[i]);
		      			//skynetNodes[i].ListValues();
					  }
				}
				else if(lineNumber==1){//second line is for discount factor
				  	discountFactor=atof(line.c_str());
				  	//cout <<"\n\nDiscount Factor\n\n"<< setprecision(2) << fixed << discountFactor << endl;
				}
				else if(lineNumber==2){//third line is for number of actions
				  	maxActionNumber=atoi(line.c_str());
				  	actions=new Action[maxActionNumber];//allocate actions
					for(int i=0;i<maxActionNumber;i++){
						actions[i].id=i;
					}
				  	//cout <<"\n\nDiscount Factor\n\n"<< setprecision(2) << fixed << discountFactor << endl;
				}
				else if(lineNumber<=8){//number of actions (7 node's actions given because no action in skynet)
				  	char strInLine[1024];
			      	strncpy(strInLine, line.c_str(), sizeof(strInLine));
					strInLine[sizeof(strInLine) - 1] = 0;//For safety
			      	
			      	tokenPtr=strtok (strInLine," ");
			      	int nodeID=atoi(tokenPtr);//get node id 
			      	tokenPtr = strtok (NULL, " ");
			      	while (tokenPtr != NULL)
					  {
					  	skynetNodes[nodeID].SetAction(atoi(tokenPtr));
					    //cout <<"token Ptr: "<<atoi(tokenPtr)<<"   "<<endl;
					    tokenPtr = strtok (NULL, " ");
					  }
					}
				else{//actions
					char strInLine[1024];
			      	strncpy(strInLine, line.c_str(), sizeof(strInLine));
					strInLine[sizeof(strInLine) - 1] = 0;//For safety
					
			      	bool takeAction;
			      	tokenPtr=strtok (strInLine," ");
			      	while (tokenPtr != NULL)//handle line elements
					{		
					  		
					    if(strcmp(tokenPtr,"action")==0){
					    	
					    	tokenPtr = strtok (NULL, " ");// is : character
					    	tokenPtr = strtok (NULL, " ");// is action id
					    	actID=atoi(tokenPtr);//action id given
					    	efcID=0;
					    	takeAction=true;
						}
						else if(strcmp(tokenPtr,"#")==0){
						 }
						else if(strcmp(tokenPtr,"$")==0){
						 	efcID++;
						 	takeAction=true;
						 }
						 else{
						 	if(takeAction){
							 	int efcNodeID=atoi(tokenPtr);
							 	lineNumber++;
							 	getline (sourceFile,line,'\n');
							 	int efcRewID=atoi(line.c_str());
							 	actions[actID].addEffect(new Effect(efcNodeID,efcRewID));
							 	takeAction=false;
							 }
							 else{
							 	int resultNodeID=atoi(tokenPtr);
							 	tokenPtr = strtok (NULL, " ");
							 	int sucRate=atoi(tokenPtr);
							 	actions[actID].addResult(efcID,new Result(resultNodeID,sucRate));
							 }
						 }
						 
						 
					    tokenPtr = strtok (NULL, " ");
					}
				}
				lineNumber++;
			}
	    }
	    sourceFile.close();
	    return true;
	  }
	  else return false;
}

void seeActions(){
	for(int i=0;i<maxActionNumber;i++){
		cout<<"Action "<<i<<": "<<endl;
		actions[i].Print();
	}
}
void seeNodes(){
		
	for(int i=0;i<(sizeof(skynetNodes));i++){
		cout<<"Node: "<<i<<": "<<endl;
		skynetNodes[i].Print();
	}
}

template<typename T>
ostream& operator<< (ostream& out, const vector<T>& v) {
    out <<setw(2)<< "[";
    size_t last = v.size() - 1;
    for (size_t i = 0; i < v.size(); ++i) {
        out <<setw(5)<< v[i];
        if (i != last)
            out << ", ";
    }
    out<< "]" <<endl;
    return out;
}


void printValueMatrix(){
	cout<<setw(16)<< "";
	for(int j=0;j<Value[0].size();j++){
		cout<<setw(11)<<"Node "<<j;
	}
	
	cout<< endl;
	for(int i=0;i<Value.size();i++){
		for(int j=0;j<Value[i].size();j++){
			if(j==0)cout<<setw(12)<<"k "<<setw(3)<<i<<":";
			cout<<setw(12)<<Value[i][j];
		}
		cout<<endl;
	}
	cout<<endl;
}

int getProbablity(int sourceNodeID,int targetNodeID,int actionID){
	
	return actions[actionID].getProbablity(sourceNodeID,targetNodeID);
}

int getReward(int sourceNodeID,int targetNodeID,int actionID){
	return actions[actionID].getReward(sourceNodeID,targetNodeID);
}

int getTotalP(int sourceNodeID,int actionID){
	int sumofProb=0;
	vector<Result> possibleTransitions=actions[actionID].getTransitions(sourceNodeID);
	for(int i=0;i<possibleTransitions.size();i++){
		sumofProb+=possibleTransitions[i].possibleTransitionPercentage;
	}
	return sumofProb;
}

float getMaxRewardedAction(int NodeId,int k){
	float maxReward=-100;
	if(skynetNodes[NodeId].isGoal())maxReward=GoalNodeValue;
	vector<int> actPossible(skynetNodes[NodeId].getActionsTaken());
	for(int a=0;a<actPossible.size();a++)//traversing target nodes
	{
		int actID=actPossible[a];//is available action id
		//cout<<"NodeId: "<<NodeId<<" actID: "<<actID<<endl;
		vector<Result> results(actions[actID].getTransitions(NodeId));
		float newReward=0;
			for(int r=0;r<results.size();r++){//traversing target nodes
				int sp=results[r].possibleTransitionNode;//s prime
				newReward+=(float)getProbablity(NodeId,sp,actID)/(float)100*(getReward(NodeId,sp,actID)+discountFactor*Value[k][sp]);
			}
		if (maxReward<newReward)
		maxReward=newReward;
	}
	return maxReward;
}

int getMaxRewardedActionArg(int NodeId,int k){
	int bestActionID=-1;
	float maxReward=-100;
	vector<int> actPossible(skynetNodes[NodeId].getActionsTaken());
	for(int a=0;a<actPossible.size();a++)//traversing target nodes
	{
		int actID=actPossible[a];//is available action id
		//cout<<"NodeId: "<<NodeId<<" actID: "<<actID<<endl;
		vector<Result> results(actions[actID].getTransitions(NodeId));
		float newReward=0;
			for(int r=0;r<results.size();r++){//traversing target nodes
				int sp=results[r].possibleTransitionNode;//s prime
				newReward+=(float)getProbablity(NodeId,sp,actID)/(float)100*(getReward(NodeId,sp,actID)+discountFactor*Value[k][sp]);
			}
		if (maxReward<newReward)
			maxReward=newReward;
			bestActionID=actID;
	}	

	return bestActionID;
}

int getMaxValuedNode(){
	int nodeID;
	float max=-100;
	for(int i=0;i<Value.size();i++){
		for(int j=0;j<Value[i].size();j++){
			if(Value[i][j]>max)
			{
				nodeID=j;
				max=Value[i][j];
			}
		}
	}
	return nodeID;
}

void printPolicy(){
cout<<"\nPolicy:\n"<<endl;
	for(int i=0;i<sizeof(skynetNodes);i++){
		if(!skynetNodes[i].isGoal()){
			vector<Result> results=actions[Policy[i]].getTransitions(i);
			cout<<"Node "<<i<<": ";
			if(results.size()>0)
			for(int j=0;j<results.size();j++){
				cout<<results[j].possibleTransitionNode;
				if(j<results.size()-1)
				cout<<", ";
			}
			else
			cout<<"No action can be taken..";
		cout<<endl;
		}
	}
}


void ValueIteration(int treshold,float cutDownValue){
	bool cutIterationFlag=false;
	cout<<endl<<"------------ Now Traversing Skynet.. --------------"<<endl;
	Policy.clear();
	
	float arr[(sizeof(skynetNodes))] = {};
	vector<float> row (arr, arr + sizeof(arr) / sizeof(arr[0]) );
	Value.push_back(row);
	
	for(int s=0;s<sizeof(skynetNodes);s++){
		Value[0][s]=0;
	}
	char c;
	int k=0;
	do{
		k++;
		float arr[(sizeof(skynetNodes))] = {};
		vector<float> row (arr, arr + sizeof(arr) / sizeof(arr[0]) );
		Value.push_back(row);
	
		for(int s=0;s<sizeof(skynetNodes);s++){
			Value[k][s]=getMaxRewardedAction(s,k-1);
			//cout<<"cutDown"<<abs(Value[k][s]-Value[k-1][s])<<endl;
			if(abs(Value[k][s]-Value[k-1][s])>0&&(abs(Value[k][s]-Value[k-1][s])<cutDownValue)&&k>1){
				cutIterationFlag=true;
			}
		}
		printValueMatrix();
		
		//cout<<"getMaxValuedNode: "<<getMaxValuedNode()<<endl;
		cout<<"\n________________________________________________________________________________________________________________________\n"<<endl;
		if(cutIterationFlag)break;
		
		cout<<endl<<"Enter c to continue: ";
		cin>>c;
		if(c=='$')break;
	}
	while(k<treshold && c=='c');
	while(k<treshold&&!cutIterationFlag){
		k++;
		float arr[(sizeof(skynetNodes))] = {};
		vector<float> row (arr, arr + sizeof(arr) / sizeof(arr[0]) );
		Value.push_back(row);
	
		for(int s=0;s<sizeof(skynetNodes);s++){
			Value[k][s]=getMaxRewardedAction(s,k-1);
			if(abs(Value[k][s]-Value[k-1][s])>0&&(abs(Value[k][s]-Value[k-1][s])<cutDownValue)&&k>1){
				cutIterationFlag=true;
			}
		}
		printValueMatrix();
		//cout<<"getMaxValuedNode: "<<getMaxValuedNode()<<endl;
		cout<<"\n________________________________________________________________________________________________________________________\n"<<endl;
	}
	
		for(int s=0;s<sizeof(skynetNodes);s++){
			Policy.push_back(getMaxRewardedActionArg(s,k));
		}
		
		//print Policy
		cout<<"\nPolicy Actions:\n"<<Policy<<endl;
		printPolicy();
}


int main(){
	bool inputRead;
	char c;
	
	inputRead=getInput();
	if(inputRead){
		seeNodes();
		seeActions();		
		cout<<"All base information generated.."<<endl;
	}
	else{
		cout << "Unable to open file"; 
	}
	ValueIteration(10,0.000001);	
	
	system("PAUSE");
}
