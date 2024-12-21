#include <iostream>
#include <algorithm>
#include<vector>
#include<map>
#include<queue>
#include<string>
#include <cstdlib>  
#include <fstream>
#include <sstream>
#include <regex>
using namespace std;

class Node;
class Broadcast;
int numProcesses;

class Broadcast{
public:
    string msg_id;  // "m1"
    Node* send_event;
    vector<int> recv_B, recv_A;

    Broadcast(string msg_id){
        this->msg_id = msg_id;
    }
};

class Process{
public:
    string process; // "p1"
    int index;
    vector<int> vector_clock;
    queue<Broadcast*> q;
    vector<string> output;
    Process(string process){
        this->process = process;
        this->output.push_back("begin process "+process);
        if(process.size()>1){
            this->index = (int)process[1] - '1';
        }
    }
    void initClock(){
        this->vector_clock.resize(numProcesses);
    }
};

class Node{
public:
    Process* process;
    string event; // "send" or "receive"
    vector<Node*> children;
    Broadcast* msg; // can be NULL
    vector<int> vector_clock;

    Node(Process* process, string type){
        this->process = process;
        this->event = type;
        this->msg = nullptr;
    }
    void initClock(){
        this->vector_clock.resize(numProcesses);
    }

    void visualize(){
        if(this->event=="root"){
            cout << "root:\n";
            for(auto c: this->children){
                cout << "    " << c->event << " " << c->msg->msg_id << " (" << c->process->process << ")\n";
            }
            for(auto c: this->children){
                c->visualize();
            }
            return;
        }
        cout << this->event << " " << this->msg->msg_id << " (" << this->process->process << "):\n";
        for(auto c: this->children){
            cout << "    " << c->event << " " << c->msg->msg_id << " (" << c->process->process << ")\n";
        }
        for(auto c: this->children){
            c->visualize();
        }
    }
};


vector<string> split_string(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;

    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void raise_error() {
    cout << "\n";
    std::exit(0);
}

// Variadic template function to handle any number of arguments
template<typename T, typename... Args>
void raise_error(T first, Args... args) {
    std::cout << first << " ";  // Print the first argument
    raise_error(args...);    // Recursive call for the rest of the arguments
}

bool dfs(Node* v, map<Node*, int>& vis, vector<Node*>& endTime){
    vis[v] = 1;
    for(auto u: v->children){
        if(vis[u] == 0){
            if(dfs(u, vis, endTime)) return true;
        }
        else if(vis[u] == 1){
            return true;
        }
    }
    endTime.push_back(v);
    vis[v] = 2;
    return false;
}

vector<Node*> TopoSort(Node* root) {
    map<Node*, int> vis;
    vector<Node*> endTime;
    bool hasCycle = dfs(root, vis, endTime);
    if(hasCycle){
        cout<<"Wrong causal order of receive and send events"<<endl;  
        raise_error();
    }
    endTime.pop_back();
    reverse(endTime.begin(), endTime.end());
    return endTime;
}

bool checkMessage(Node* node, Broadcast* message){
    bool flag = true;
    int send_event_pid = message->send_event->process->index;
    if(node->process->vector_clock[send_event_pid] + 1 != message->send_event->vector_clock[send_event_pid]){
        return false;
    }

    for(int i=0; i<node->process->vector_clock.size(); i++){
        if(i != send_event_pid){
            if(node->process->vector_clock[i] < message->send_event->vector_clock[i]){
                flag = false;
                break;
            }
        }
    }
    return flag;
}

int main(){
    ifstream file("input.txt");
    string line;
    regex Begin("^begin process (\\w+)$");
    regex Send("^send (\\w+)$");
    regex Recv("^recv_B (\\w+) (\\w+)$");
    regex End("^end process (\\w+)$");

    smatch match;  // To hold match results
    vector<string> args;
    string process="";
    string msg_id="";
    int num_processes = 0;
    int process_closed = 0;

    Node* root = new Node(new Process("root"), "root");
    Node* prev = root;

    map<string, Broadcast*> message_store;
    map<string, Process*> process_store;
    vector<pair<Node*, Broadcast*>> postponed_edges;

    if (file.is_open()) {
        int lineno = 0;
        while (getline(file, line)) {
            // cout << line << endl;
            if (regex_search(line, match, Begin)) {
                vector<string> args = split_string(match[0], ' ');
                process = args[2];
                num_processes++;
            }
            else if (regex_search(line, match, Send)) {
                vector<string> args = split_string(match[0], ' ');
                msg_id = args[1];
                if(process==""){
                    // cout << "about to send error2" << endl;
                    raise_error("Wrong Input Format in Line", lineno, ": send line cannot be before begin line.");
                }
                Process* process_obj;
                if(process_store.find(process)!=process_store.end())
                    process_obj = process_store[process];
                else{
                    process_obj = new Process(process);
                    process_store[process] = process_obj;
                }
                Node* event = new Node(process_obj, "send");
                Broadcast* msg;
                if(message_store.find(msg_id)!=message_store.end())
                    msg = message_store[msg_id];
                else{
                    msg = new Broadcast(msg_id);
                    message_store[msg_id] = msg;
                }
                msg->send_event = event;
                event->msg = msg;
                prev->children.push_back(event);
                prev = event;
            }
            else if (regex_search(line, match, Recv)) {
                vector<string> args = split_string(match[0], ' ');
                string sender = args[1];
                msg_id = args[2];
                if(process==""){
                    // cout << "about to send error3" << endl;
                    raise_error("Wrong Input Format in Line", lineno, ": recv_B line cannot be before begin line.");
                }
                Process* process_obj;
                if(process_store.find(process)!=process_store.end())
                    process_obj = process_store[process];
                else{
                    process_obj = new Process(process);
                    process_store[process] = process_obj;
                }
                Node* event = new Node(process_obj, "recv");
                Broadcast* msg;
                if(message_store.find(msg_id)!=message_store.end()){
                    msg = message_store[msg_id];
                    if(!(msg->send_event)){
                        postponed_edges.push_back({event, msg});
                    }
                    else if(msg->send_event->process->process!=sender){
                        // cout << "about to send error4" << endl;
                        raise_error("Wrong Input in Line ",lineno, ": sender and message id do not match.");
                    }
                    else{
                        msg->send_event->children.push_back(event);
                    }
                    // cout << "adsfjl" << msg->send_event->children.size() << endl;
                }
                else{
                    msg = new Broadcast(msg_id);
                    message_store[msg_id] = msg;
                    postponed_edges.push_back({event, msg});
                }
                event->msg = msg;
                prev->children.push_back(event);
                prev = event;
            }
            else if (regex_search(line, match, End)) {
                vector<string> args = split_string(match[0], ' ');
                if(args[2]!=process){
                    // cout << "about to send error5" << endl;
                    raise_error("Wrong Input Format in Line ", lineno, ": trying to end a process which is not currently active.");
                }
                process = "";
                prev = root;
                process_closed++;
            }
            lineno++;
        }
        file.close();
        if(num_processes != process_closed){
            cerr<<"All processes not ended."<<endl;
            raise_error();
        }
        numProcesses = num_processes;
        for(auto p: postponed_edges){
            // cout << "posted " << p.second->msg_id << " " << p.first->process << endl;
            Broadcast* msg = p.second;
            Node* receiver = p.first;
            if(!(msg->send_event)){
                // cout << "about to send error6" << endl;
                raise_error("Wrong Input: No process sends the message ", msg->msg_id);
            }
            msg->send_event->children.push_back(receiver);
        }
        // cout << "test\n";
        // root->visualize();
    } else {
        raise_error("Unable to open file.");
    }
    vector<Node*> toposort = TopoSort(root);

    for(auto it: process_store){
        it.second->initClock();
    }

    for(auto node: toposort){
        node->initClock();
    }

    for(auto node: toposort){
        if(node->event == "send"){
            node->process->vector_clock[node->process->index]++;
            // cerr<<"Check4 "<<node->event<<endl;
            node->vector_clock = node->process->vector_clock;
            string op = "send "+ node->msg->msg_id+  " (";
            for(int i=0; i<node->vector_clock.size(); i++){
                op += to_string(node->vector_clock[i]);
            }
            op += ")";

            // cerr<<op<<endl;

            node->process->output.push_back(op);
        }
        else if(node->event == "recv"){
            // cerr<<"Check5 "<<node->event<<endl;

            bool flag = checkMessage(node, node->msg);
            // cerr<<"Check 6 "<<node->event<<" "<<flag<<endl;
            node->msg->recv_B = node->process->vector_clock;
            int j = node->msg->send_event->process->index;
            if(flag){
                // cerr<<"Check5 "<<node->event<<endl;
                string op = "recv_B "+node->msg->send_event->process->process+" "+node->msg->msg_id + " (";
                for(int i=0; i<node->vector_clock.size(); i++){
                    op += to_string(node->process->vector_clock[i]);
                }
                op += ")";

                // cerr<<op<<endl;
                node->process->output.push_back(op);

                node->msg->recv_B = node->process->vector_clock;
                node->process->vector_clock[j]++;
                node->vector_clock = node->process->vector_clock;
                node->msg->recv_A = node->process->vector_clock;

                op = "recv_A "+node->msg->send_event->process->process+" "+node->msg->msg_id + " (";
                for(int i=0; i<node->vector_clock.size(); i++){
                    op += to_string(node->vector_clock[i]);
                }
                op += ")";

                // cerr<<op<<endl;

                node->process->output.push_back(op);
            }
            else{
                string op = "recv_B "+node->msg->send_event->process->process+" "+node->msg->msg_id + " (";
                for(int i=0; i<node->vector_clock.size(); i++){
                    op += to_string(node->process->vector_clock[i]);
                }
                op += ")";

                // cerr<<op<<endl;
                node->process->output.push_back(op);

                node->process->q.push(node->msg);
                continue;
            } 
        }
        else continue;

        cerr<<"Check3 "<<node->event<<endl;

        int curr=0;
        while(!node->process->q.empty()){
            // cerr<<"Check67 "<<node->event<<endl;
            Broadcast* message = node->process->q.front();
            node->process->q.pop();
            bool flag = checkMessage(node, message);
            int j = message->send_event->process->index;
            // cerr<<"Check677 "<<flag<< " "<< node->event<<endl;
            if(curr > node->process->q.size()) break;    
            if(flag){
                node->process->vector_clock[j]++;
                node->vector_clock = node->process->vector_clock;
                message->recv_A = node->process->vector_clock;
                curr=0;
                string op = "recv_A "+message->send_event->process->process+" "+message->msg_id + " (";
                for(int i=0; i<node->vector_clock.size(); i++){
                    op += to_string(node->vector_clock[i]);
                }
                op += ")";
            
                // cerr<<op<<endl;

                node->process->output.push_back(op);
            }
            else {
                curr++;
                node->process->q.push(message);
            }
        }
    }
    
    ofstream ofile("output2.txt");
    if (!ofile) {
        cerr << "Error: Could not open output.txt for writing." << endl;
        exit(0); // Handle the error as needed
    }
    for(auto p : process_store){
        for(auto event: p.second->output){
            ofile<<event<<endl;
        }
        ofile<<"end process "<<p.first<<endl<<endl;
    }
    ofile.close();
    return 0;
}