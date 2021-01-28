#include "../../include/types/Profiles.h"
#include "../../include/types/PerformanceTable.h"
#include "../../include/utils.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

Profiles::Profiles(std::vector<Performance> &perf_vect)
    : PerformanceTable(perf_vect) {
  if (!this->isProfileOrdered()) {
    throw std::invalid_argument(
        "The given performance vector cannot be used as a profiles performance "
        "table. Each row must be ranked such that for each row i we have on "
        "each criterion j : v_i - 1_j > v_i_j > v_i + 1_j");
  }
}

Profiles::Profiles(int nb_of_prof, Criteria &crits, std::string prefix)
    : PerformanceTable(nb_of_prof, crits, prefix) {
  this->generateRandomPerfValues();
}

Profiles::Profiles(const Profiles &profiles) : PerformanceTable(profiles) {}

Profiles::~Profiles() {}

std::ostream &operator<<(std::ostream &out, const Profiles &profs) {
  out << "Profiles[ ";
  for (std::vector<Perf> p : profs.pt_) {
    out << "Profile: ";
    for (Perf perf : p) {
      out << perf << " ";
    }
    out << "| ";
  }
  out << "]";
  return out;
}

void Profiles::generateRandomPerfValues(unsigned long int seed, int lower_bound,
                                        int upper_bound) {
  if (lower_bound > upper_bound) {
    throw std::invalid_argument(
        "Lower bound must be lower than the upper bound.");
  }
  srand(seed);
  for (int j = 0; j < pt_[0].size(); j++) {
    std::vector<float> r_vect;
    for (int i = 0; i < pt_.size(); i++) {
      r_vect.push_back(getRandomUniformFloat(seed, lower_bound, upper_bound));
    }
    std::sort(r_vect.begin(), r_vect.end());
    std::reverse(r_vect.begin(), r_vect.end());
    for (int i = 0; i < pt_.size(); i++) {
      pt_[i][j].setValue(r_vect[i]);
    }
  }
}

bool Profiles::isProfileOrdered() {
  for (int i = 0; i < pt_.size() - 1; i++) {
    for (int j = 0; j < pt_[i].size(); j++) {
      if (pt_[i][j].getValue() < pt_[i + 1][j].getValue()) {
        return false;
      }
    }
  }
  return true;
}

std::pair<std::vector<Perf>, std::vector<Perf>>
Profiles::getBelowAndAboveProfile(std::string profName) {
  std::vector<Perf> below;
  std::vector<Perf> above;
  if (mode_ == "alt") {
    for (int h = 0; h < pt_.size(); h++) {
      if (pt_[h][0].getName() == profName) {
        if (h == 0) {
          below = pt_[h];
          above = pt_[h + 1];
          return std::make_pair(below, above);
        } else if (h == pt_.size() - 1) {
          below = pt_[h - 1];
          above = pt_[h];
          return std::make_pair(below, above);
        } else {
          below = pt_[h - 1];
          above = pt_[h + 1];
          return std::make_pair(below, above);
        }
      }
    }
    throw std::invalid_argument("Profile not found.");
  } else {
    throw std::domain_error("Profiles perftable mode corrupted.");
  }
}

void Profiles::setPerf(std::string name, std::string crit, float value) {
  // supposing the pt is consistent:
  // if in mode alt, each row contains 1 and only 1 (alternative or profile)
  // if in mode crit, each row contains 1 and only 1 criterion
  bool crit_found = false;
  bool alt_found = false;
  if (mode_ == "alt") {
    int i = 0; // alt index
    int j = 0; // crit index
    for (std::vector<Perf> p : pt_) {
      if (p[0].getName() == name) {
        alt_found = true;
        for (Perf perf : p) {
          if (perf.getCrit() == crit) {
            crit_found = true;
            pt_[i][j].setValue(value);
            break;
          }
          j = j + 1;
        }
        if (!crit_found) {
          throw std::invalid_argument(
              "Criterion not found in performance table");
        }
      }
      i = i + 1;
    }
    if (!alt_found) {
      throw std::invalid_argument("Name not found in performance table");
    }
  } else if (mode_ == "crit") {
    int i = 0; // crit index
    int j = 0; // alt index
    for (std::vector<Perf> p : pt_) {
      if (p[0].getCrit() == crit) {
        crit_found = true;
        for (Perf perf : p) {
          if (perf.getName() == name) {
            alt_found = true;
            pt_[i][j].setValue(value);
            break;
          }
          j = j + 1;
        }
        if (!alt_found) {
          throw std::invalid_argument("Name not found in performance table");
        }
      }
      i = i + 1;
    }
    if (!crit_found) {
      throw std::invalid_argument("Criterion not found in performance table");
    }
  } else {
    throw std::domain_error("Performance table mode corrupted.");
  }
}
