#include <bits/stdc++.h>
#include <ctime>
#include <ratio>
#include <chrono>
using namespace std;
using namespace std::chrono;

#pragma GCC optimize("Ofast")
#pragma GCC target("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")
#pragma GCC optimize("unroll-loops")

const bool VERBOSE = false; // TODO false
const int MAX_TIME = 280000; // TODO 280000

auto startTime = high_resolution_clock::now();

int n, m;

vector<set<int>> black_edges, red_edges;
vector<set<int>> backup_black_edges, backup_red_edges;
vector<vector<int>> gr;
vector<vector<int>> small_candidates;
vector<pair<int, int>> sorted_deg;

int maxdeg = 1e9;
vector<pair<int, int>> seq;

double ALPHA, BETA;

void endProgram(bool force = false){
    auto stopTime = high_resolution_clock::now();
    if (force || duration_cast<milliseconds>(stopTime - startTime).count() >= MAX_TIME){
        for(int i = 0; i < seq.size(); i++) {
            cout << seq[i].first << ' ' << seq[i].second << '\n';
        }

        if (VERBOSE){
            cerr<<"BEST SOL : "<<maxdeg<<'\n';

            auto stopTime = high_resolution_clock::now();
            cerr<<"Time : "<<duration_cast<milliseconds>(stopTime - startTime).count()<<'\n';
        }
        exit(0);
    }
}

void addBlackEdge(int x, int y) {
    if(x != y) {
        black_edges[x].insert(y);
        black_edges[y].insert(x);
    }
}

void removeBlackEdge(int x, int y) {
    black_edges[x].erase(y);
    black_edges[y].erase(x);
}

void addRedEdge(int x, int y) {
    if(x != y) {
        red_edges[x].insert(y);
        red_edges[y].insert(x);
    }
}

void removeRedEdge(int x, int y) {
    red_edges[x].erase(y);
    red_edges[y].erase(x);
}

bool cmp(int a, int b){
    return gr[a].size() > gr[b].size();
}

void readGraph() {
    char c;
    string s;
    cin >> c >> s >> n >> m;

    black_edges.resize(n+1);
    red_edges.resize(n+1);
    backup_black_edges.resize(n+1);
    backup_red_edges.resize(n+1);
    gr.resize(n+1);
    small_candidates.resize(n+1);

    for(int i = 1; i <= m; i++) {
        int x, y;
        cin >> x >> y;
        addBlackEdge(x, y);
        gr[x].push_back(y);
        gr[y].push_back(x);
    }

    backup_black_edges = black_edges;
    backup_red_edges = red_edges;

    for (int i=1; i<=n; i++){
        sort(gr[i].begin(), gr[i].end(), cmp);
        if (gr[i].size() <= 5){
            for (auto &x : gr[i]){
                small_candidates[x].push_back(i);
            }
        }
    }

    for (int i=1; i<=n; i++){
        sorted_deg.push_back({black_edges[i].size(), i});
    }
    sort(sorted_deg.begin(), sorted_deg.end(), greater<>());
}

pair<int,int> checkValue(int v, int w){
    int maxx = 0;
    int cont = red_edges[v].size();

    if (red_edges[v].find(w) != red_edges[v].end()){
        cont--;
    }

    // all edges from x to vertices that are not adjacent to y become red
    auto pnt_black = black_edges[w].begin();
    auto pnt_red = red_edges[w].begin();
    for (auto &x : black_edges[v]){
        endProgram();
        if (x == w) continue;

        while (pnt_black != black_edges[w].end() && *pnt_black < x){
            pnt_black++;
        }
        while (pnt_red != red_edges[w].end() && *pnt_red < x){
            pnt_red++;
        }

        if ((pnt_black == black_edges[w].end() || *pnt_black != x) && (pnt_red == red_edges[w].end() || *pnt_red != x)){
            cont ++;
            maxx = max(maxx, (int)red_edges[x].size() + 1);
        }
    }

    // x is connected with a red edge to all vertices that are connected to y but not to x
    pnt_black = black_edges[v].begin();
    pnt_red = red_edges[v].begin();
    for (auto &x : black_edges[w]){
        endProgram();
        if (x == v) continue;

        while (pnt_black != black_edges[v].end() && *pnt_black < x){
            pnt_black++;
        }
        while (pnt_red != red_edges[v].end() && *pnt_red < x){
            pnt_red++;
        }

        if ((pnt_black == black_edges[v].end() || *pnt_black != x) && (pnt_red == red_edges[v].end() || *pnt_red != x)){
            cont ++;
            maxx = max(maxx, (int)red_edges[x].size() + 1);
        }
    }

    pnt_red = red_edges[v].begin();
    for (auto &x : red_edges[w]){
        endProgram();
        if (x == v) continue;
        while (pnt_red != red_edges[v].end() && *pnt_red < x){
            pnt_red++;
        }

        if (pnt_red == red_edges[v].end() || *pnt_red != x){
            cont ++;
            maxx = max(maxx, (int)red_edges[x].size() + 1);
        }
    }

    return {max(maxx, cont), cont};
}

void contract(int v, int w) {
    endProgram();
    removeBlackEdge(v, w);
    removeRedEdge(v, w);

    vector<pair<int, int>> remove_black;
    vector<pair<int, int>> add_red;

    // all edges from x to vertices that are not adjacent to y become red
    auto pnt_black = black_edges[w].begin();
    auto pnt_red = red_edges[w].begin();
    for(auto x : black_edges[v]) {
        endProgram();
        if (x == w) continue;

        while (pnt_black != black_edges[w].end() && *pnt_black < x){
            pnt_black++;
        }
        while (pnt_red != red_edges[w].end() && *pnt_red < x){
            pnt_red++;
        }

        if ((pnt_black == black_edges[w].end() || *pnt_black != x) && (pnt_red == red_edges[w].end() || *pnt_red != x)){
            add_red.push_back({v, x});
            remove_black.push_back({v, x});
        }
    }

    // x is connected with a red edge to all vertices that are connected to y but not to x
    pnt_black = black_edges[v].begin();
    pnt_red = red_edges[v].begin();
    for (auto &x : black_edges[w]){
        endProgram();
        if (x == v) continue;

        while (pnt_black != black_edges[v].end() && *pnt_black < x){
            pnt_black++;
        }
        while (pnt_red != red_edges[v].end() && *pnt_red < x){
            pnt_red++;
        }

        if ((pnt_black == black_edges[v].end() || *pnt_black != x) && (pnt_red == red_edges[v].end() || *pnt_red != x)){
            add_red.push_back({v, x});
            remove_black.push_back({v, x});
        }
    }

    pnt_red = red_edges[v].begin();
    for (auto &x : red_edges[w]){
        endProgram();
        if (x == v) continue;
        while (pnt_red != red_edges[v].end() && *pnt_red < x){
            pnt_red++;
        }

        if (pnt_red == red_edges[v].end() || *pnt_red != x){
            add_red.push_back({v, x});
            remove_black.push_back({v, x});
        }
    }

    // add edges
    for (auto &x : add_red){
        endProgram();
        addRedEdge(x.first, x.second);
    }
    for (auto &x : remove_black){
        endProgram();
        removeBlackEdge(x.first, x.second);
    }

    // delete y
    for (auto &x : black_edges[w]){
        endProgram();
        black_edges[x].erase(w);
    }
    for (auto &x : red_edges[w]){
        endProgram();
        red_edges[x].erase(w);
    }
    black_edges[w].clear();
    red_edges[w].clear();
}

vector<pair<int, int>> generateRandomSolution(){
    vector<pair<int, int>> seq;

    vector<bool> used(n + 1, false);
    vector<int> nodes;

    for (int i=1; i<=n; i++){
        nodes.push_back(i);
    }

    while(nodes.size() > 1) {
        vector<int> t;
        int sz = nodes.size();

        for(int i = 0; i < sz - 1; i += 2) {
            seq.push_back({nodes[i], nodes[i + 1]});
            t.push_back(nodes[i]);
        }
        if(sz % 2) {
            seq.push_back({nodes[sz - 3], nodes[sz - 1]});
        }
        nodes = t;
    }

    return seq;
}

int getOption(set<pair<int,int>> &options, vector<bool> &used){
    if (options.empty())
        return -1;
    auto node = *options.begin();
    options.erase(node);
    while (used[node.second] || node.first != int(ALPHA * (double)red_edges[node.second].size()+ BETA * (double)black_edges[node.second].size())){
        endProgram();
        if (!used[node.second]){
            options.insert({int(ALPHA * (double)red_edges[node.second].size()+ BETA * (double)black_edges[node.second].size()), node.second});
        }
        if (options.empty())
            return -1;
        node = *options.begin();
        options.erase(node);
    }

    // cerr<<"red "<<node.second<<" "<<node.first<<" "<<red_edges[node.second].size()<<" : "<<small_red.size()<<'\n';

    return node.second;
}

pair<int,int> chooseNodes(set<pair<int,int>> &options, int &max_red_deg, vector<bool> &used, int INCREMENT, int ADD){
    set<int> set_nodes;
    vector<int> all_nodes;

    int CHOICES = 0;
    int LOOK_FOR = max_red_deg;
    int node1 = -1, node2 = -1, minn = 1e9, add = 1e9;
    int LIMIT = 5*INCREMENT;

    while (!options.empty() && CHOICES <= LIMIT){
        CHOICES += INCREMENT;
        while (!options.empty() && (int)set_nodes.size() < CHOICES){
            endProgram();
            int node = getOption(options, used);
            if (node == -1){
                continue;
            }
            if (set_nodes.find(node) != set_nodes.end()){
                continue;
            }

            all_nodes.push_back(node);

            if (red_edges[node].size() > add){
                continue;
            }
            for (auto &other : set_nodes){
                if (red_edges[other].size() > add){
                    continue;
                }
                pair<int,int> p = checkValue(other, node);
                int val = p.first + p.second;
                if (val < minn){
                    minn = val;
                    add = p.first;
                    node1 = other;
                    node2 = node;
                }
                else{
                    p = checkValue(node, other);
                    val = p.first + p.second;
                    if (val < minn){
                        minn = val;
                        add = p.first;
                        node1 = node;
                        node2 = other;
                    }
                }

                if (add <= LOOK_FOR){
                    break;
                }
            }
            if (add <= LOOK_FOR){
                break;
            }

            set<int> tested;
            int neighs = 0;
            for (auto &x : red_edges[node]){
                if (neighs >= INCREMENT){
                    break;
                }
                neighs ++;

                int tests = 0;
                for (auto &y : small_candidates[x]){
                    if (tests >= INCREMENT){
                        break;
                    }
                    if (node == y){
                        continue;
                    }
                    if (set_nodes.find(y) != set_nodes.end()){
                        continue;
                    }
                    if (tested.find(y) != set_nodes.end()){
                        continue;
                    }
                    if (red_edges[y].size() > add){
                        continue;
                    }
                    if (black_edges[y].size() > add){
                        continue;
                    }
                    if (used[y]){
                        continue;
                    }

                    tested.insert(y);

                    tests ++;
                    pair<int,int> p = checkValue(y, node);
                    int val = p.first + p.second;
                    if (val < minn){
                        minn = val;
                        add = p.first;
                        node1 = y;
                        node2 = node;
                    }
                    else{
                        p = checkValue(node, y);
                        val = p.first + p.second;
                        if (val < minn){
                            minn = val;
                            add = p.first;
                            node1 = node;
                            node2 = y;
                        }
                    }

                    if (add <= LOOK_FOR){
                        break;
                    }
                }

                if (add <= LOOK_FOR){
                    break;
                }
            }
            if (add <= LOOK_FOR){
                break;
            }

            set_nodes.insert(node);
        }

        if (add <= LOOK_FOR){
            break;
        }
        LOOK_FOR += ADD;
    }

    contract(node1, node2);
    used[node2] = true;

    for (auto &x : all_nodes){
        if (!used[x]){
            options.insert({int(ALPHA * (double)red_edges[x].size()+ BETA * (double)black_edges[x].size()), x});
        }
    }

    max_red_deg = max(max_red_deg, add);

    return {node1, node2};
}

void restore(){
    black_edges = backup_black_edges;
    red_edges = backup_red_edges;
}

pair<int, vector<pair<int, int>>> solutionBigSmall(bool is_checker = false, int last_value = 0){
    int max_red_deg = 0;
    vector<pair<int, int>> seq;

    // get big and small
    vector<int> big;
    vector<int> small;

    int limit = sqrt(n);
    for (int i=0; i<limit; i++){
        big.push_back(sorted_deg[i].second);
    }
    for (int i=limit; i<sorted_deg.size(); i++){
        small.push_back(sorted_deg[i].second);
    }

    vector<bool> used(n + 1, false);
    set<pair<int,int>> options;

    for(auto &x : small) {
        options.insert({int(ALPHA * (double)red_edges[x].size()+ BETA * (double)black_edges[x].size()), x});
    }

    int steps = n - 1;

    double TOTAL_TIME = 150000.0;

    double total_steps = n-1;
    double INCREMENT = 20;
    int ADD = 1;
    auto startTime = high_resolution_clock::now();

    double MAXVAL = 50.0, MINVAL = 5.0;
    int VELOCITY_MIN = 0, VELOCITY_MAX = 0;

    int ADD_MOMENT = int(sqrt(n) * sqrt(sqrt(n)));

    while(steps--) {
        if (is_checker && last_value <= max_red_deg){
            return {2e9, {}};
        }
        if (is_checker && (n - steps) == min(int(n/10), int(5e4))){
            return {max_red_deg, {}};
        }
        
        endProgram();
        pair<int,int> nodes = chooseNodes(options, max_red_deg, used, ceil(INCREMENT), ADD);
        seq.push_back(nodes);

        if (steps == ADD_MOMENT){
            for(auto &x : big) {
                if (!used[x]){
                    options.insert({int(ALPHA * (double)red_edges[x].size()+ BETA * (double)black_edges[x].size()), x});
                }
            }
        }

        auto stopTime = high_resolution_clock::now();
        double duration = duration_cast<milliseconds>(stopTime - startTime).count();
        double curr_steps = total_steps - steps;

        // cerr<<duration<<" "<<curr_steps<<" "<<duration / curr_steps * total_steps<<" / "<<TOTAL_TIME<<" : "<<INCREMENT<<" "<<ADD<<'\n';

        double diff = TOTAL_TIME - duration / curr_steps * total_steps;
        diff /= steps + 1 + sqrt(n);

        double changed_increment = INCREMENT + diff;
        changed_increment = min(MAXVAL, max(MINVAL, changed_increment));

        if (curr_steps > int(sqrt(n))){
            INCREMENT = changed_increment;
        }

        if (INCREMENT <= MINVAL + 0.0001){
            VELOCITY_MIN ++;
        }
        else{
            VELOCITY_MIN = 0;
        }

        if (INCREMENT >= MAXVAL - 0.0001){
            VELOCITY_MAX ++;
        }
        else{
            VELOCITY_MAX = 0;
        }

        if (VELOCITY_MIN >= 100){
            MAXVAL = 30.0;
        }

        if (VELOCITY_MIN >= int(sqrt(n) * sqrt(sqrt(n))) && MINVAL > 1.0){
            MINVAL -= 1.0;
            VELOCITY_MIN = 0;
        }

        if (VELOCITY_MAX >= int(sqrt(n) * sqrt(sqrt(n))) && MAXVAL < 70.0){
            MAXVAL += 1.0;
            VELOCITY_MAX = 0;
        }

        if (VERBOSE && !is_checker){
            if (steps % 10000 == 0 || steps <= 10000){
                cerr<<steps<<" : "<<max_red_deg<<" "<<INCREMENT<<" "<<ADD<<" : "<<duration / curr_steps * total_steps<<'\n';
            }
        }
    }

    return {max_red_deg, seq};
}

pair<int, vector<pair<int, int>>> solutionRandomRandom(int last_value){
    int max_red_deg = 0;
    vector<pair<int, int>> seq;

    vector<bool> used(n + 1, false);

    vector<int> nodes;
    for (int i=1; i<=n; i++){
        nodes.push_back(i);
    }

    random_shuffle(nodes.begin(), nodes.end());

    while(nodes.size() > 1) {
        if (max_red_deg >= last_value){
            return {2e9, {}};
        }
        endProgram();
        vector<int> t;
        int sz = nodes.size();

        for(int i = 0; i < sz - 1; i += 2) {
            endProgram();
            seq.push_back({nodes[i], nodes[i + 1]});
            contract(nodes[i], nodes[i + 1]);
            max_red_deg = max(max_red_deg, (int)red_edges[nodes[i]].size());
            t.push_back(nodes[i]);
        }
        if(sz % 2) {
            contract(nodes[sz - 3], nodes[sz - 1]);
            max_red_deg = max(max_red_deg, (int)red_edges[nodes[sz - 3]].size());
            seq.push_back({nodes[sz - 3], nodes[sz - 1]});
        }
        nodes = t;
    }

    return {max_red_deg, seq};
}

pair<int, vector<pair<int, int>>> solutionRandomSorted(int last_value){
    int max_red_deg = 0;
    vector<pair<int, int>> seq;

    vector<bool> used(n + 1, false);
    
    vector<int> nodes;
    for (int i=(int)sorted_deg.size()-1; i>=0; i--){
        nodes.push_back(sorted_deg[i].second);
    }

    while(nodes.size() > 1) {
        if (max_red_deg >= last_value){
            return {2e9, {}};
        }
        endProgram();
        vector<int> t;
        int sz = nodes.size();
        
        for(int i = 0; i < sz - 1; i += 2) {
            endProgram();
            seq.push_back({nodes[i], nodes[i + 1]});
            contract(nodes[i], nodes[i + 1]);
            max_red_deg = max(max_red_deg, (int)red_edges[nodes[i]].size());
            t.push_back(nodes[i]);
        }
        if(sz % 2) {
            contract(nodes[sz - 3], nodes[sz - 1]);
            max_red_deg = max(max_red_deg, (int)red_edges[nodes[sz - 3]].size());
            seq.push_back({nodes[sz - 3], nodes[sz - 1]});
        }
        nodes = t;
    }

    return {max_red_deg, seq};
}

pair<int, vector<pair<int, int>>> solutionBfsTree(){
    int max_red_deg = 0;
    vector<pair<int, int>> seq;

    vector<bool> used(n + 1, false);

    vector<int> roots;
    vector<vector<int>> edges(n+1);

    for (auto &x : sorted_deg){
        endProgram();
        if (!used[x.second]){
            used[x.second] = true;
            queue<int> q;
            q.push(x.second);
            vector<int> ord;

            while (!q.empty()){
                endProgram();
                int node = q.front();
                q.pop();

                // cerr<<"BFS "<<node<<'\n';

                ord.push_back(node);

                for (auto &neigh : gr[node]){
                    if (!used[neigh]){
                        edges[node].push_back(neigh);
                        used[neigh] = true;
                        q.push(neigh);
                    }
                }
            }

            reverse(ord.begin(), ord.end());
            for (auto &node : ord){
                endProgram();
                vector<int> nodes = edges[node];
                while(nodes.size() > 1) {
                    endProgram();
                    vector<int> t;
                    int sz = nodes.size();

                    for(int i = 0; i < sz - 1; i += 2) {
                        endProgram();
                        seq.push_back({nodes[i], nodes[i + 1]});
                        pair<int,int> p = checkValue(nodes[i], nodes[i + 1]);
                        int val = p.first;
                        contract(nodes[i], nodes[i + 1]);
                        max_red_deg = max(max_red_deg, val);
                        t.push_back(nodes[i]);
                    }
                    if(sz % 2) {
                        pair<int,int> p = checkValue(nodes[sz - 3], nodes[sz - 1]);
                        int val = p.first;
                        contract(nodes[sz - 3], nodes[sz - 1]);
                        max_red_deg = max(max_red_deg, val);
                        seq.push_back({nodes[sz - 3], nodes[sz - 1]});
                    }
                    nodes = t;
                }

                if (!nodes.empty()){
                    pair<int,int> p = checkValue(node, nodes[0]);
                    int val = p.first;
                    contract(node, nodes[0]);
                    seq.push_back({node, nodes[0]});
                    max_red_deg = max(max_red_deg, val);
                }
            }

            roots.push_back(x.second);
        }
    }

    for (int i=0; i+1<roots.size(); i++){
        endProgram();
        pair<int,int> p = checkValue(roots[i+1], roots[i]);
        int val = p.first;
        contract(roots[i+1], roots[i]);
        seq.push_back({roots[i+1], roots[i]});
        max_red_deg = max(max_red_deg, val);
    }

    return {max_red_deg, seq};
}

pair<int, vector<pair<int, int>>> solutionPriorityBfsTree(int last_value){
    int max_red_deg = 0;
    vector<pair<int, int>> seq;

    vector<bool> used(n + 1, false);

    vector<int> roots;
    vector<vector<int>> edges(n+1);

    for (auto &x : sorted_deg){
        endProgram();
        if (!used[x.second]){
            used[x.second] = true;
            priority_queue<pair<int,int>, vector<pair<int,int>>> q;
            q.push({gr[x.second].size(), x.second});
            vector<int> ord;

            while (!q.empty()){
                if (max_red_deg >= last_value){
                    return {2e9, {}};
                }
                endProgram();
                pair<int,int> aux = q.top();
                int node = aux.second;
                q.pop();

                // cerr<<"BFS "<<node<<'\n';

                ord.push_back(node);

                for (auto &neigh : gr[node]){
                    endProgram();
                    if (!used[neigh]){
                        edges[node].push_back(neigh);
                        used[neigh] = true;
                        q.push({gr[neigh].size(), neigh});
                    }
                }
            }

            reverse(ord.begin(), ord.end());
            for (auto &node : ord){
                if (max_red_deg >= last_value){
                    return {2e9, {}};
                }
                vector<int> nodes = edges[node];
                while(nodes.size() > 1) {
                    endProgram();
                    vector<int> t;
                    int sz = nodes.size();

                    for(int i = 0; i < sz - 1; i += 2) {
                        endProgram();
                        seq.push_back({nodes[i], nodes[i + 1]});
                        pair<int,int> p = checkValue(nodes[i], nodes[i + 1]);
                        int val = p.first;
                        contract(nodes[i], nodes[i + 1]);
                        max_red_deg = max(max_red_deg, val);
                        t.push_back(nodes[i]);
                    }
                    if(sz % 2) {
                        pair<int,int> p = checkValue(nodes[sz - 3], nodes[sz - 1]);
                        int val = p.first;
                        contract(nodes[sz - 3], nodes[sz - 1]);
                        max_red_deg = max(max_red_deg, val);
                        seq.push_back({nodes[sz - 3], nodes[sz - 1]});
                    }
                    nodes = t;
                }

                if (!nodes.empty()){
                    pair<int,int> p = checkValue(node, nodes[0]);
                    int val = p.first;
                    contract(node, nodes[0]);
                    seq.push_back({node, nodes[0]});
                    max_red_deg = max(max_red_deg, val);
                }
            }

            roots.push_back(x.second);
        }
    }

    for (int i=0; i+1<roots.size(); i++){
        if (max_red_deg >= last_value){
            return {2e9, {}};
        }
        endProgram();
        pair<int,int> p = checkValue(roots[i+1], roots[i]);
        int val = p.first;
        contract(roots[i+1], roots[i]);
        seq.push_back({roots[i+1], roots[i]});
        max_red_deg = max(max_red_deg, val);
    }

    return {max_red_deg, seq};
}

int checkCommon(int v, int w){
    int cont = 0;

    auto pnt_black = black_edges[w].begin();
    auto pnt_red = red_edges[w].begin();
    for (auto &x : black_edges[v]){
        endProgram();
        if (x == w) continue;

        while (pnt_black != black_edges[w].end() && *pnt_black < x){
            pnt_black++;
        }
        while (pnt_red != red_edges[w].end() && *pnt_red < x){
            pnt_red++;
        }

        if ((pnt_black == black_edges[w].end() || *pnt_black == x) || (pnt_red == red_edges[w].end() || *pnt_red == x)){
            cont ++;
        }
    }

    return cont;
}

pair<int, vector<pair<int, int>>> solutionDegreeOrder(int last_value){
    int max_red_deg = 0;
    vector<pair<int, int>> seq;

    vector<bool> used(n + 1, false);

    vector<pair<int, int>> nodes;
    for (int i=1; i<=n; i++){
        nodes.push_back({black_edges[i].size(), i});
    }
    sort(nodes.begin(), nodes.end());
    int choices = int(1e7) / n;

    while (nodes.size() > 1){
        endProgram();
        vector<pair<int, int>> aux;

        for (int i=0; i<nodes.size(); i++){
            if (max_red_deg >= last_value){
                return {2e9, {}};
            }
            endProgram();
            if (used[nodes[i].second])
                continue;

            int node1 = nodes[i].second, node2 = -1, maxx = -1;
            int j = i+1;
            int tested = 0;
            while (j < nodes.size() && tested < choices){
                endProgram();
                if (!used[nodes[j].second]){
                    tested++;
                    int val = checkCommon(nodes[i].second, nodes[j].second);
                    if (val > maxx){
                        maxx = val;
                        node2 = nodes[j].second;
                    }
                }
                j++;
            }
            if (node2 != -1){
                seq.push_back({node1, node2});
                int val = checkValue(node1, node2).first;
                contract(node1, node2);
                max_red_deg = max(max_red_deg, val);
                used[node2] = true;
            }

            aux.push_back({black_edges[node1].size(), node1});
        }

        nodes = aux;
    }

    return {max_red_deg, seq};
}

int main() {

    // freopen("heuristic_002.gr", "r", stdin); freopen("output", "w", stdout);

    ios_base::sync_with_stdio(false);
    cin.tie(nullptr); cout.tie(nullptr);
    srand(0);

    readGraph();

    pair<int,vector<pair<int, int>>> curr_sol;

    // INIT RANDOM
    seq = generateRandomSolution();

    if (m <= 4*n) {
        // SOLUTION BFS TREE
        curr_sol = solutionBfsTree();
        if (VERBOSE) {
            cerr << "SOL BFS TREE : " << curr_sol.first << '\n';

            auto stopTime = high_resolution_clock::now();
            cerr << "Time : " << duration_cast<milliseconds>(stopTime - startTime).count() << '\n';
        }
        if (curr_sol.first < maxdeg) {
            maxdeg = curr_sol.first;
            seq = curr_sol.second;
        }
        restore();
    }

    // CHOOSE BEST ALPHA AND BETA
    double best_alpha = 2.0, best_beta = 0.5;
    int best_val = 1e9;

    vector<pair<double,double>> alpha_beta = {{2.0, 0.5},
                                              {2.0, 0.0001}};

    if (m > 4*n){
        for (auto &x : alpha_beta){
            ALPHA = x.first;
            BETA = x.second;
            curr_sol = solutionBigSmall(true, best_val);
            if (VERBOSE){
                cerr<<"CHOOSE ALPHA = "<<x.first<<" , BETA = "<<x.second<<" : "<<curr_sol.first<<'\n';
                auto stopTime = high_resolution_clock::now();
                cerr<<"Time : "<<duration_cast<milliseconds>(stopTime - startTime).count()<<'\n';
            }
            if (curr_sol.first < best_val){
                best_val = curr_sol.first;
                best_alpha = x.first;
                best_beta = x.second;
            }
            restore();
        }
    }

    ALPHA = best_alpha;
    BETA = best_beta;
    if (VERBOSE){
        cerr<<"BEST ALPHA = "<<best_alpha<<" , BETA = "<<best_beta<<" : "<<best_val<<'\n';
        auto stopTime = high_resolution_clock::now();
        cerr<<"Time : "<<duration_cast<milliseconds>(stopTime - startTime).count()<<'\n';
    }

    // SOLUTION BIG SMALL
    curr_sol = solutionBigSmall();
    if (VERBOSE){
        cerr<<"SOL BIG SMALL : "<<curr_sol.first<<'\n';

        auto stopTime = high_resolution_clock::now();
        cerr<<"Time : "<<duration_cast<milliseconds>(stopTime - startTime).count()<<'\n';
    }
    if (curr_sol.first < maxdeg){
        maxdeg = curr_sol.first;
        seq = curr_sol.second;
    }
    restore();

    if (m <= 4*n) {
        // SOLUTION PRIORITY BFS TREE
        curr_sol = solutionPriorityBfsTree(maxdeg);
        if (VERBOSE) {
            cerr << "SOL PRIORITY BFS TREE : " << curr_sol.first << '\n';

            auto stopTime = high_resolution_clock::now();
            cerr << "Time : " << duration_cast<milliseconds>(stopTime - startTime).count() << '\n';
        }
        if (curr_sol.first < maxdeg) {
            maxdeg = curr_sol.first;
            seq = curr_sol.second;
        }
        restore();
    }

    // SOLUTION DEGREE ORDER
    curr_sol = solutionDegreeOrder(maxdeg);
    if (VERBOSE){
        cerr<<"SOL DEGREE ORDER : "<<curr_sol.first<<'\n';

        auto stopTime = high_resolution_clock::now();
        cerr<<"Time : "<<duration_cast<milliseconds>(stopTime - startTime).count()<<'\n';
    }
    if (curr_sol.first < maxdeg){
        maxdeg = curr_sol.first;
        seq = curr_sol.second;
    }
    restore();

    // SOLUTION RANDOM SORTED
    curr_sol = solutionRandomSorted(maxdeg);
    if (VERBOSE){
        cerr<<"SOL RANDOM SORTED : "<<curr_sol.first<<'\n';

        auto stopTime = high_resolution_clock::now();
        cerr<<"Time : "<<duration_cast<milliseconds>(stopTime - startTime).count()<<'\n';
    }
    if (curr_sol.first < maxdeg){
        maxdeg = curr_sol.first;
        seq = curr_sol.second;
    }
    restore();

    // SOLUTION RANDOM RANDOM
    curr_sol = solutionRandomRandom(maxdeg);
    if (VERBOSE){
        cerr<<"SOL RANDOM RANDOM : "<<curr_sol.first<<'\n';

        auto stopTime = high_resolution_clock::now();
        cerr<<"Time : "<<duration_cast<milliseconds>(stopTime - startTime).count()<<'\n';
    }
    if (curr_sol.first < maxdeg){
        maxdeg = curr_sol.first;
        seq = curr_sol.second;
    }
    restore();


    // answer
    endProgram(true);

    return 0;
}