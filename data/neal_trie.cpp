// This version of aho_corasick uses a bitmask of size ALPHABET, so it must be modified for ALPHABET > 26.
template<char MIN_CHAR = 'a'>
struct aho_corasick {
	struct node {
		// suff = the index of the node of the longest strict suffix of the current node that's also in the tree.
		//   Also see "blue arcs" on Wikipedia: https://en.wikipedia.org/wiki/Aho%E2%80%93Corasick_algorithm
		// dict = the index of the node of the longest strict suffix of the current node that's in the word list.
		//   Also see "green arcs" on Wikipedia.
		// depth = normal trie depth (root is 0). Can be removed to save memory.
		// word_index = the index of the *first* word ending at this node. -1 if none.
		// word_count = the total number of words ending at this node. Used in count_total_matches().
		// first_child = the first child of this node (children are sequential due to BFS order), -1 if none.
		// child_mask = the bitmask of child keys available from this node. If ALPHABET > 26, change the type.
		int suff = -1, dict = -1, depth = 0;
		int word_index = -1, word_count = 0;
		int first_child = -1;
		unsigned child_mask = 0;

		int get_child(char c) const {
			int bit = c - MIN_CHAR;

			if ((child_mask >> bit & 1) == 0)
				return -1;

			assert(first_child >= 0);
			return first_child + __builtin_popcount(child_mask & ((1 << bit) - 1));
		}
	};

	vector<node> nodes;
	int W;
	vector<string> words;
	vector<int> word_location;
	vector<int> word_indices_by_depth;
	vector<int> defer;

	aho_corasick(const vector<string> &_words = {}) {
		if (!_words.empty())
			build(_words);
	}

	// Builds the adj list based on suffix parents. Often we want to perform DP and/or queries on this tree.
	vector<vector<int>> build_suffix_adj() const {
		vector<vector<int>> adj(nodes.size());

		for (int i = 1; i < int(nodes.size()); i++)
			adj[nodes[i].suff].push_back(i);

		return adj;
	}

	int get_or_add_child(int current, char c) {
		int bit = c - MIN_CHAR;

		if (nodes[current].child_mask >> bit & 1)
			return nodes[current].get_child(c);

		assert(nodes[current].child_mask >> bit == 0);
		int index = int(nodes.size());
		nodes[current].child_mask |= 1 << bit;

		if (nodes[current].first_child < 0)
			nodes[current].first_child = index;

		nodes.emplace_back();
		nodes.back().depth = nodes[current].depth + 1;
		return index;
	}

	// Returns where in the trie we should end up after starting at `location` and adding char `c`.
	// Runs in worst case O(depth) but amortizes to O(1) in most situations.
	int get_suffix_link(int location, char c) const {
		int child;

		while (location >= 0 && (child = nodes[location].get_child(c)) < 0)
			location = nodes[location].suff;

		return location < 0 ? 0 : child;
	}

	void build(const vector<string> &_words) {
		nodes = {node()};
		words = _words;
		W = int(words.size());
		stable_sort(words.begin(), words.end());
		word_location.assign(W, 0);
		vector<int> remaining(W);
		int n_rem = W;

		for (int i = 0; i < W; i++)
			remaining[i] = i;

		for (int depth = 0; n_rem > 0; depth++) {
			int next_n_rem = 0;

			for (int i = 0; i < n_rem; i++) {
				int word = remaining[i];
				int &location = word_location[word];
				location = get_or_add_child(location, words[word][depth]);

				if (depth == int(words[word].size()) - 1) {
					if (nodes[location].word_index < 0)
						nodes[location].word_index = word;

					nodes[location].word_count++;
				} else {
					remaining[next_n_rem++] = word;
				}
			}

			n_rem = next_n_rem;
		}

		int max_depth = 0;
		defer.resize(W);

		for (int i = 0; i < W; i++) {
			max_depth = max(max_depth, int(words[i].size()));
			defer[i] = nodes[word_location[i]].word_index;
		}

		// Create a list of word indices in decreasing order of depth, in linear time via counting sort.
		word_indices_by_depth.resize(W);
		vector<int> depth_freq(max_depth + 1, 0);

		for (int i = 0; i < W; i++)
			depth_freq[words[i].size()]++;

		for (int i = max_depth - 1; i >= 0; i--)
			depth_freq[i] += depth_freq[i + 1];

		for (int i = 0; i < W; i++)
			word_indices_by_depth[--depth_freq[words[i].size()]] = i;

		// Solve suffix parents by traversing in order of depth (BFS order).
		for (int i = 0; i < int(nodes.size()); i++) {
			unsigned child_mask = nodes[i].child_mask;

			while (child_mask != 0) {
				int bit = __builtin_ctz(child_mask);
				char c = char(MIN_CHAR + bit);
				int index = nodes[i].get_child(c);
				child_mask ^= 1 << bit;

				// Find index's suffix parent by traversing suffix parents of i until one of them has a child c.
				int suffix_parent = get_suffix_link(nodes[i].suff, c);
				nodes[index].suff = suffix_parent;
				nodes[index].word_count += nodes[suffix_parent].word_count;
				nodes[index].dict = nodes[suffix_parent].word_index < 0 ? nodes[suffix_parent].dict : suffix_parent;
			}
		}
	}

	// Counts the number of matches of each word in O(text length + num words).
	vector<int> count_matches(const string &text) const {
		vector<int> matches(W, 0);
		int current = 0;

		for (char c : text) {
			current = get_suffix_link(current, c);
			int dict_node = nodes[current].word_index < 0 ? nodes[current].dict : current;

			if (dict_node >= 0)
				matches[nodes[dict_node].word_index]++;
		}

		// Iterate in decreasing order of depth.
		for (int word_index : word_indices_by_depth) {
			int location = word_location[word_index];
			int dict_node = nodes[location].dict;

			if (dict_node >= 0)
				matches[nodes[dict_node].word_index] += matches[word_index];
		}

		for (int i = 0; i < W; i++)
			matches[i] = matches[defer[i]];

		return matches;
	}

	// Counts the number of matches over all words at each ending position in `text` in O(text length).
	vector<int> count_matches_by_position(const string &text) const {
		vector<int> matches(text.size());
		int current = 0;

		for (int i = 0; i < int(text.size()); i++) {
			current = get_suffix_link(current, text[i]);
			matches[i] = nodes[current].word_count;
		}

		return matches;
	}

	// Counts the total number of matches of all words within `text` in O(text length).
	int64_t count_total_matches(const string &text) const {
		int64_t matches = 0;
		int current = 0;

		for (char c : text) {
			current = get_suffix_link(current, c);
			matches += nodes[current].word_count;
		}

		return matches;
	}
};