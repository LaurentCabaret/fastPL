#include "../../include/types/ProfileUpdater.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

ProfileUpdater::ProfileUpdater(AlternativesPerformance &altPerf_data,
                               float epsilon)
    : altPerf_data(altPerf_data), epsilon_(epsilon) {}

ProfileUpdater::ProfileUpdater(const ProfileUpdater &profUp)
    : altPerf_data(profUp.altPerf_data), epsilon_(profUp.epsilon_) {}

ProfileUpdater::~ProfileUpdater() {}

std::unordered_map<float, float> ProfileUpdater::computeAboveDesirability(
    MRSortModel &model, std::string critId, Perf &b, Perf &b_above,
    Category &cat, Category &cat_above,
    std::unordered_map<std::string, float> &ct_prof,
    AlternativesPerformance &altPerf_model) {
  // Data from the problem
  float lambda = model.lambda;
  float weight = model.criteria[critId].getWeight();
  float epsilon = this->epsilon_;

  // Alternatives between given profile and above profile. AltPerf_model needs
  // to be sorted and in mode crit to have a meaniful iteration in the loop
  altPerf_model.sort();
  std::vector<Perf> alt_between =
      altPerf_model.getAltBetween(critId, b.getValue(), b_above.getValue());

  // Initializing the map of desirability indexes
  std::unordered_map<float, float> desirability_above;
  float numerator = 0;
  float denominator = 0;

  for (Perf alt : alt_between) {
    // Checking if the move will not go above b_above
    if (alt.getValue() + epsilon < b_above.getValue()) {
      std::cout << "IN THE LOOP FOR " << alt.getName() << std::endl;
      std::string altName = alt.getName();
      float conc = ct_prof[altName];
      float diff = conc - weight;
      std::string aa_data =
          altPerf_data.getAlternativeAssignment(altName).getCategoryId();
      std::string aa_model =
          altPerf_model.getAlternativeAssignment(altName).getCategoryId();

      // Here we are checking if the move of profile b right above/under the
      // performance of alt is going to help the model
      if (aa_data == cat_above.getCategoryId()) {
        // Correct classification
        // Moving the profile results in misclassification -> Q
        if (aa_model == cat_above.getCategoryId() and diff < lambda) {
          denominator += 5;
          std::cout << "Q" << std::endl;
        }
        // Wrong classification
        // Moving the profile is in favor of wrong classification -> R
        else if (aa_model == cat.getCategoryId()) {
          denominator += 1;
          std::cout << "R" << std::endl;
        }
      } else if (aa_data == cat.getCategoryId() and
                 aa_model == cat_above.getCategoryId()) {
        // Wrong classification
        // Moving the profile is in favor of right classification -> W
        if (diff >= lambda) {
          numerator += 0.5;
          denominator += 1;
          desirability_above[alt.getValue() + epsilon] =
              numerator / denominator;
          std::cout << "W" << std::endl;
        }
        // Wrong classification
        // Moving the profile results in correct classification -> V
        else {
          numerator += 2;
          denominator += 1;
          desirability_above[alt.getValue() + epsilon] =
              numerator / denominator;
          std::cout << "V" << std::endl;
        }
      }
      // Wrong classification
      // Moving the profile is in favor of right classification -> T
      else if (aa_data != aa_model and
               altPerf_model.getAlternativeAssignment(altName)
                       .getCategoryRank() >= cat_above.getCategoryRank() and
               altPerf_data.getAlternativeAssignment(altName)
                       .getCategoryRank() < cat.getCategoryRank()) {
        numerator += 0.1;
        denominator += 1;
        desirability_above[alt.getValue() + epsilon] = numerator / denominator;
        std::cout << "T" << std::endl;
      }
    }
  }
  auto it = desirability_above.begin();
  while (it != desirability_above.end()) {
    std::cout << it->first << "->" << it->second << " " << std::endl;
    it++;
  }
  return desirability_above;
}

std::unordered_map<float, float> ProfileUpdater::computeBelowDesirability(
    MRSortModel &model, std::string critId, Perf &b, Perf &b_below,
    Category &cat, Category &cat_above,
    std::unordered_map<std::string, float> &ct_prof,
    AlternativesPerformance &altPerf_model) {
  // Data from the problem
  float lambda = model.lambda;
  float weight = model.criteria[critId].getWeight();
  // Direction & epsilon not in original algorithm
  float epsilon = this->epsilon_;

  // Alternatives between given profile and above profile. AltPerf_model needs
  // to be sorted and in mode crit to have a meaniful iteration in the loop
  altPerf_model.sort();
  std::vector<Perf> alt_between =
      altPerf_model.getAltBetween(critId, b_below.getValue(), b.getValue());

  // Initializing the map of desirability indexes
  std::unordered_map<float, float> desirability_below;
  float numerator = 0;
  float denominator = 0;

  for (Perf alt : alt_between) {
    // Checking if the move will not go below b_below
    if ((alt.getValue() - epsilon) > b_below.getValue()) {
      std::cout << "IN THE LOOP FOR " << alt.getName() << std::endl;
      std::string altName = alt.getName();
      float conc = ct_prof[altName];
      float diff = conc + weight;
      std::string aa_data =
          altPerf_data.getAlternativeAssignment(altName).getCategoryId();
      std::string aa_model =
          altPerf_model.getAlternativeAssignment(altName).getCategoryId();

      // Here we are checking if the move of profile b at the level of the
      // performance of alt is going to help the model
      if (aa_data == cat_above.getCategoryId() and
          aa_model == cat.getCategoryId()) {
        // Wrong classification
        // Moving the profile results in correct classification -> V
        if (diff >= lambda) {
          numerator += 2;
          denominator += 1;
          desirability_below[alt.getValue() - epsilon] =
              numerator / denominator;
          std::cout << "V" << std::endl;
        }
        // Wrong classification
        // Moving the profile is in favor of right classification -> W
        else {
          numerator += 0.5;
          denominator += 1;
          desirability_below[alt.getValue() - epsilon] =
              numerator / denominator;
          std::cout << "W" << std::endl;
        }
      } else if (aa_data == cat.getCategoryId()) {
        // Correct classification
        // Moving the profile results in misclassification -> Q
        if (aa_model == cat.getCategoryId() and diff >= lambda) {
          denominator += 5;
          std::cout << "Q" << std::endl;
        }
        // Wrong classification
        // Moving the profile is in favor of wrong classification -> R
        else if (aa_model == cat_above.getCategoryId()) {
          denominator += 1;
          std::cout << "R" << std::endl;
        }
      }
      // Wrong classification
      // Moving the profile is in favor of right classification -> T
      else if (aa_data != aa_model and
               altPerf_model.getAlternativeAssignment(altName)
                       .getCategoryRank() > cat.getCategoryRank() and
               altPerf_data.getAlternativeAssignment(altName)
                       .getCategoryRank() > cat.getCategoryRank()) {
        numerator += 0.1;
        denominator += 1;
        desirability_below[alt.getValue() - epsilon] = numerator / denominator;
        std::cout << "T" << std::endl;
      }
    }
  }
  auto it = desirability_below.begin();
  while (it != desirability_below.end()) {
    std::cout << it->first << "->" << it->second << " " << std::endl;
    it++;
  }
  return desirability_below;
}

// NOT WORKING YET
float chooseMaxDesirability(std::unordered_map<float, float> &desirability,
                            Perf &b) {
  std::cout << "hello" << std::endl;
  auto key_selector = [](auto pair) { return pair.first; };
  auto value_selector = [](auto pair) { return pair.second; };

  std::vector<float> keys(desirability.size());
  std::vector<float> values(desirability.size());
  std::transform(desirability.begin(), desirability.end(), keys.begin(),
                 key_selector);
  std::transform(desirability.begin(), desirability.end(), values.begin(),
                 value_selector);
  float key_max = keys[0];
  float val_max = values[0];
  float diff = std::abs(b.getValue() - key_max);
  for (auto element : keys) {
    float k = element;
    float v = desirability[element];
    std::cout << "key_max: " << key_max << " val_max: " << val_max << std::endl;
    std::cout << "k: " << k << " v: " << v << std::endl;
    if (v >= val_max) {
      float tmp = std::abs(b.getValue() - k);
      std::cout << "tmp: " << tmp << std::endl;
      if (tmp > diff) {
        std::cout << "test3" << std::endl;
        key_max = k;
        val_max = v;
        diff = tmp;
      }
    }
  }
  return key_max;
}

void ProfileUpdater::updateTables(
    MRSortModel &model, std::string critId, Perf &b_old, Perf &b_new,
    std::unordered_map<std::string, std::unordered_map<std::string, float>> &ct,
    int &good, AlternativesPerformance &altPerf_model) {
  if (b_old.getName() != b_new.getName() ||
      b_old.getCrit() != b_new.getCrit()) {
    throw std::invalid_argument("Profile perfs must have same name and crit");
  }

  std::cout << "update for profile " << b_old.getName() << " on criterion "
            << b_old.getCrit() << " from " << b_old.getValue() << " to "
            << b_new.getValue() << std::endl;

  float w = 0;
  // Determine if the profile is moving up or down to know how to adjust the
  // weight for the concordance and calculate alternatives between old profile
  // perf and new profile perf.
  std::vector<Perf> alt_between;
  if (b_old.getValue() > b_new.getValue()) {
    w = model.criteria[critId].getWeight();
    alt_between =
        altPerf_model.getAltBetween(critId, b_new.getValue(), b_old.getValue());
  } else {
    w = -model.criteria[critId].getWeight();
    alt_between =
        altPerf_model.getAltBetween(critId, b_old.getValue(), b_new.getValue());
  }

  for (Perf alt : alt_between) {
    // Data assignment
    std::string aa_data =
        altPerf_data.getAlternativeAssignment(alt.getName()).getCategoryId();
    // Old assignmment
    std::string aa_old =
        altPerf_model.getAlternativeAssignment(alt.getName()).getCategoryId();

    // Update concordance table
    float c = ct[b_old.getName()][alt.getName()] + w;
    ct[b_old.getName()][alt.getName()] = c;
    // Update profile
    model.profiles.setPerf(b_new.getName(), b_new.getCrit(), b_new.getValue());

    // New assignment
    auto alternative = altPerf_model.operator[](alt.getName());
    std::vector<std::vector<Perf>> pt = model.profiles.getPerformanceTable();
    Category cat_new = model.categoryAssignment(alternative, pt);
    std::string aa_new = cat_new.getCategoryId();

    // Update alternative assignment
    altPerf_model.setAlternativeAssignment(alt.getName(), cat_new);

    std::cout << alt.getName() << "=> aa_data: " << aa_data
              << " aa_old: " << aa_old << " aa_new: " << aa_new << std::endl;
    std::cout << "c: " << c << std::endl;

    // Update good assignment count
    if (aa_old == aa_new) {
      std::cout << "no change" << std::endl;
      break;
    } else if (aa_old == aa_data) {
      good = good - 1;
      std::cout << "one less :( " << std::endl;
    } else if (aa_new == aa_data) {
      good = good + 1;
      std::cout << "one more :D " << std::endl;
    }
  }
}

void ProfileUpdater::optimizeProfile(std::vect<Perf> &prof, Category &cat_below,
                                     Category &cat_above, MRSortModel &model) {
  std::pair<std::vect<Perf>, std::vect<Perf>> below_above =
      model.profiles.getBelowAndAboveProfile(prof[0].getName());
  std::vect<Perf> prof_below = below_above[0];
  std::vect<Perf> prof_above = below_above[1];
  for (Criterion crit : model.criteria.getCriterionVect()) {
    // Perf b = prof
  }
}

// Concordance table as an argument of optimize
// Idem #good
// Idem aa_data
// Idem aa_model