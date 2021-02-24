#include "../../include/types/ProfileUpdater.h"

#include <cstdlib>
#include <map>
#include <string>

ProfileUpdater::ProfileUpdater(AlternativesPerformance &altPerf_data,
                               float delta)
    : altPerf_data(altPerf_data), delta_(delta) {}

ProfileUpdater::ProfileUpdater(const ProfileUpdater &profUp)
    : altPerf_data(profUp.altPerf_data), delta_(profUp.delta_) {}

ProfileUpdater::~ProfileUpdater() {}

std::unordered_map<float, float> ProfileUpdater::computeAboveDesirability(
    MRSortModel &model, std::string critId, Perf &b, Perf &b_above,
    Category &cat, Category &cat_above,
    std::unordered_map<std::string, float> &ct_prof) {
  // Data from the problem
  float lambda = model.lambda;
  float weight = model.criteria[critId].getWeight();
  int direction = model.criteria[critId].getDirection();
  float delta = direction * this->delta_;
  std::vector<std::vector<Perf>> altPerfTable_data =
      this->altPerf_data.getPerformanceTable();
  Profiles profiles_data = model.profiles;

  std::cout << model << std::endl;

  // Data calculated from the model
  AlternativesPerformance altPerf_model =
      model.categoryAssignments(altPerf_data);

  // Alternatives between given profile and above profile
  std::vector<Perf> alt_between =
      altPerf_model.getAltBetween(critId, b.getValue(), b_above.getValue());

  std::cout << "Alts between " << b << " and " << b_above << ": " << std::endl;
  std::cout << alt_between << std::endl;

  // Updating profile value with profile + delta*direction
  Profiles profiles_model_updated = Profiles(profiles_data);

  // Initializing the map of desirability indexes
  std::unordered_map<float, float> desirability_above;
  float numerator = 0;
  float denominator = 0;

  for (Perf alt : alt_between) {
    // Need to check if delta to high?
    // Checking if the move is not already handled by the below_des of b_above
    if ((alt.getValue() + delta) * direction < b_above.getValue() * direction) {
      // if ((b.getValue() + delta) * direction > alt.getValue() * direction) {
      std::cout << "IN THE LOOP FOR " << alt.getName() << std::endl;
      std::string altName = alt.getName();
      float conc = ct_prof[altName];
      float diff = conc - weight;
      std::string aa_data =
          altPerf_data.getAlternativeAssignment(altName).getCategoryId();
      std::string aa_model =
          altPerf_model.getAlternativeAssignment(altName).getCategoryId();

      std::cout << altName << " = data: " << aa_data << " - model: " << aa_model
                << std::endl;
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
          desirability_above[alt.getValue() + delta] = numerator / denominator;
          std::cout << "W" << std::endl;
        }
        // Wrong classification
        // Moving the profile results in correct classification -> V
        else {
          numerator += 2;
          denominator += 1;
          desirability_above[alt.getValue() + delta] = numerator / denominator;
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
        desirability_above[alt.getValue() + delta] = numerator / denominator;
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
