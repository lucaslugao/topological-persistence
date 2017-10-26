#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Struct types and definitions

typedef std::set<int> vertices;
typedef std::vector<std::set<int>> sparse_matrix;

struct simplex {
  int dim;
  float val;
  vertices vert;
};

struct bar {
  int index;
  int dim;
  float birth;
  float death;
};

typedef std::vector<bar> barcode;

// Returns a vector of simplexes representing the filtration read from
// the file specified by the filename
std::vector<simplex> read_filtration(std::string filename) {
  std::vector<simplex> F;
  FILE *input = fopen(filename.c_str(), "r");

  if (input != nullptr) {
    int temp;

    while (true) {
      simplex s;
      if (fscanf(input, "%f %d", &s.val, &s.dim) != 2)
        break;
      for (int i = 0; i <= s.dim; i++) {
        fscanf(input, "%d", &temp);
        s.vert.insert(temp);
      }
      F.push_back(s);
    }
  } else {
    std::cout << "Failed to read file " << filename << std::endl;
  }
  return F;
}

// Sort the filtration inplace by looking first at the f-value and
// untying by the dimention of the given simplex
void sort_filtration(std::vector<simplex> &filtration) {
  std::sort(filtration.begin(), filtration.end(),
            [](const simplex &a, const simplex &b) -> bool {
              if (a.val == b.val)
                return a.dim < b.dim;
              return a.val < b.val;
            });
}

// Create the boundary matrix from the given sorted filtration F.
// The implementation builds the matrix in O(n log n) time by creating
// a indexer mapper to keep track of the simplicies indexes.
sparse_matrix build_boundary_matrix(std::vector<simplex> F, bool debug = false) {
  sparse_matrix M(F.size());
  std::map<vertices, int> indexer;
  for (std::size_t j = 0; j < F.size(); ++j) {
    if (debug && j % ((int)(F.size() / 10)) == 0) {
      printf("%d / %d\n", j, F.size());
    }
    indexer.insert(std::pair<vertices, int>(F[j].vert, j));
    if (F[j].vert.size() > 1) {
      for (auto &v : F[j].vert) {
        vertices boundary = F[j].vert;
        boundary.erase(v);
        int index = indexer[boundary];
        M[j].insert(index);
      }
    }
  }
  return M;
}

void pivote_column(sparse_matrix &R, int pivot, int j) {
  std::set<int> result;
  std::set_symmetric_difference(R[pivot].begin(), R[pivot].end(), R[j].begin(),
                                R[j].end(),
                                std::inserter(result, result.begin()));
  R[j] = result;
}

void print_matrix(sparse_matrix M) {
  std::vector<std::vector<bool>> D(M.size());
  for (int j = 0; j < M.size(); ++j) {
    D[j].resize(M.size(), false);
    for (auto i : M[j])
      D[j][i] = true;
  }
  for (int j = 0; j < M.size(); ++j) {
    for (int i = 0; i < M.size(); ++i) {
      printf("%c", D[i][j] ? '#' : '_');
    }
    printf("\n");
  }
}

template <typename A> void print_iter(A s) {
  std::cout << "[";
  for (auto &v : s) {
    if (&v != &(*s.begin()))
      std::cout << ", ";
    std::cout << v;
  }
  std::cout << "]\n";
}
sparse_matrix reduce_boundary_matrix(sparse_matrix R, bool debug = false) {
  std::vector<int> pivots(R.size(), -1);
  for (std::size_t j = 0; j < R.size(); ++j) {
    if (debug && j % ((int)(R.size() / 10)) == 0) {
      printf("%d / %d\n", j, R.size());
    }
    while (!R[j].empty()) {
      int pivot = *R[j].rbegin();
      if (pivots[pivot] < 0)
        break;
      pivote_column(R, pivots[pivot], (int)j);
    }
    if (!R[j].empty()) {
      pivots[*R[j].rbegin()] = (int)j;
      // Boost by pre pivotating the column
      for (auto semi_pivot = ++R[j].rbegin(); semi_pivot != R[j].rend();
           ++semi_pivot) {
        if (pivots[*semi_pivot] > 0) {
          pivote_column(R, pivots[*semi_pivot], (int)j);
          semi_pivot = R[j].rbegin();
        }
      }
    }
  }
  return R;
}

barcode create_barcode(sparse_matrix R, std::vector<simplex> F) {
  barcode C;
  std::vector<bool> pivots(R.size(), false);
  for (std::size_t j = 0; j < R.size(); ++j)
    if (!R[j].empty())
      pivots[*R[j].rbegin()] = true;

  for (std::size_t j = 0; j < R.size(); ++j) {
    if (!R[j].empty()) {
      int birth = *R[j].rbegin();
      int death = (int)j;
      bar b;
      b.index = birth;
      b.dim = F[birth].dim;
      b.birth = F[birth].val;
      b.death = F[death].val;

      C.push_back(b);
    } else {
      if (!pivots[j]) {
        bar b;
        b.index = (int)j;
        b.dim = F[j].dim;
        b.birth = F[j].val;
        b.death = std::numeric_limits<float>::infinity();

        C.push_back(b);
      }
    }
  }
  std::sort(C.begin(), C.end(), [](const bar &a, const bar &b) -> bool {
    return a.birth < b.birth;
  });
  return C;
}

void print_float(float f) {
  if (std::isinf(f)) {
    std::cout << "inf";
  } else {
    std::ios init(NULL);
    init.copyfmt(std::cout);
    if (std::roundf(f) == f) {
      std::cout << std::fixed << std::setprecision(1) << f;
    } else {
      std::cout << std::defaultfloat << f;
    }
    std::cout.copyfmt(init);
  }
}

void print_barcode(barcode C, float threshold = -1.0f) {
  for (auto b : C) {
    if (threshold < 0 || b.death - b.birth > threshold) {
      std::cout << b.dim << " ";
      print_float(b.birth);
      std::cout << " ";
      print_float(b.death);
      std::cout << "\n";
    }
  }
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Syntax: " << argv[0] << " <filtration_file> [<optional threshold>]" << std::endl;
    return 0;
  }
  float threshold = -1;
  if (argc >= 3) {
    threshold = std::stof(argv[2]);
  }
  std::string name = argv[1];

  auto F = read_filtration(name);
  sort_filtration(F);
  auto B = build_boundary_matrix(F);
  auto R = reduce_boundary_matrix(B);
  auto barcode_s = create_barcode(R, F);
  print_barcode(barcode_s, threshold);
  return 0;
}