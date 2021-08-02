class Trie {
public:
  struct node {
    bool is_teminal = false;
    vector<int> table;
    int set_occurence;
    bool visited; // use this for tree traversal
    int depth;
    node(int _n) { table = vector<int>(_n); set_occurence = 0; visited = false; depth = 0; }
  };

  int table_size;
  vector<node> nxt;
  int node_id;
  Trie(int _size) : table_size(_size) {
    nxt.emplace_back(table_size);
    node_id = 0;
  }

  void insert(const string &s) {
    int now = 0;
    for (char ch : s) {
      int prev = now;
      if (nxt[now].table[ch - 'A'] == 0) {
        nxt.push_back(node(26));
        nxt[now].table[ch - 'A'] = ++node_id;
      }
      now = nxt[now].table[ch - 'A'];
      nxt[now].set_occurence++;
      nxt[now].depth = nxt[prev].depth + 1;
      ans = max(ans, nxt[now].depth * 1ll * nxt[now].set_occurence);
    }
    nxt[now].is_teminal = true;
  }

  void debug_node() {
    // for (auto nd : nxt) { dbg(nd.is_teminal); dbg(nd.table); dbg(nd.set_occurence); dbg(nd.visited); dbg('\n', '\n'); }
  }
};