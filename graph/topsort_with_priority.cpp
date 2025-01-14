template <typename T>
vector<int> find_topsort(const digraph<T> &g) {
  vector<int> deg(g.n, 0);
  for (int id = 0; id < (int) g.edges.size(); id++) {
    if (g.ignore != nullptr && g.ignore(id)) {
      continue;
    }
    deg[g.edges[id].to]++;
  }
  vector<int> x;
  set<int> st;
  for (int i = 0; i < g.n; i++) {
    if (deg[i] == 0) {
      st.insert(i);
    }
  }
  while (!st.empty()) {
    int i = *st.begin();
    st.erase(st.begin());
    x.push_back(i);
    for (int id : g.g[i]) {
      if (g.ignore != nullptr && g.ignore(id)) {
        continue;
      }
      auto &e = g.edges[id];
      int to = e.to;
      if (--deg[to] == 0) {
        st.insert(to);
      }
    }
  }
  if ((int) x.size() != g.n) {
    return vector<int>();
  }
  return x;
}