const int SZ = 2;
template<typename T, typename K>
struct Trie {
  struct node {
    K value;
    bool is_terminal;
    int vis_count;
    vector<int> children;

    node(K val) : value(val) {
      is_terminal = false;
      children = vector<int>(SZ);
      vis_count = 0;
    }
  };

  int cast(K val) {
    int ret = val;  // change your casting function here
    assert(ret < SZ and ret >= 0);
    return ret;
  }

  vector<node> tree;

  Trie(K val) { tree.push_back(node(val)); }

  void insert(const T &sequence, bool remove = false) {
    int cur = 0;
    for (int i = 0; i < (int) sequence.size(); i++) {
      K value = sequence[i];
      if (tree[cur].children[cast(value)] == 0) {
        tree[cur].children[cast(value)] = (int) tree.size();
        tree.emplace_back(value);
      }
      cur = tree[cur].children[cast(value)];
      if (remove) {
        tree[cur].vis_count -= 1;
      } else {
        tree[cur].vis_count += 1;
      }
    }
    tree[cur].is_terminal = true;
  }

  void remove(const T& sequence) { insert(sequence, true); }

  int get_node(const T& sequence) {
    int cur = 0;
    for (int i = 0; i < (int) sequence.size(); i++) {
      K value = sequence[i];
      if (tree[cur].children[cast(value)] == 0) return 0;
      cur = tree[cur].children[cast(value)];
    }
    return cur;
  }

  bool search(const T& sequence) {
    int n = get_node(sequence);
    return n != 0 and tree[n].is_terminal;
  }

  bool starts_with(const T& sequence) { return get_node(sequence) != 0; }

  void make_all_unvisited() { for (auto& node : tree) node.visited = false; }

  void dfs(int cur) {
    tree[cur].visited = true;
    for (int i = 0; i < SZ; i++) {
      if (tree[cur].children[i] == 0) continue;
      int child_node = tree[cur].children[i];
      if (tree[child_node].visited) continue;
      dfs(child_node);
    }
  }

  void debug() {
    int nc = 0;
    for (auto& nd : tree) {
      dbg(nc++);
      dbg(nd.visited);
      dbg(nd.children);
      dbg(nd.vis_count);
      dbg(nd.value);
      dbg(nd.is_terminal);
      cout << '\n';
    }
  }
}; 